#include "databasemanager.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>


namespace
{
std::string trimCopy(const std::string &value)
{
    const auto first = std::find_if_not(
        value.begin(),
        value.end(),
        [](unsigned char character)
        {
            return std::isspace(character);
        }
    );

    if (first == value.end())
    {
        return {};
    }

    const auto last = std::find_if_not(
        value.rbegin(),
        value.rend(),
        [](unsigned char character)
        {
            return std::isspace(character);
        }
    ).base();

    return std::string(first, last);
}

bool isValidUsername(const std::string &value)
{
    const std::string username = trimCopy(value);

    if (username.size() < 3 || username.size() > 40)
    {
        return false;
    }

    return std::all_of(
        username.begin(),
        username.end(),
        [](unsigned char character)
        {
            return std::isalnum(character) ||
                   character == '_' ||
                   character == '-' ||
                   character == '.';
        }
    );
}

bool rowExists(
    sqlite3 *database,
    const char *sql,
    int integerValue,
    const std::string &textValue,
    int secondIntegerValue,
    int ignoredID = -1)
{
    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return true;
    }

    int bindIndex = 1;
    sqlite3_bind_int(statement, bindIndex++, integerValue);

    if (!textValue.empty())
    {
        sqlite3_bind_text(
            statement,
            bindIndex++,
            textValue.c_str(),
            -1,
            SQLITE_TRANSIENT
        );
    }

    if (secondIntegerValue != -1)
    {
        sqlite3_bind_int(
            statement,
            bindIndex++,
            secondIntegerValue
        );
    }

    if (ignoredID >= 0)
    {
        sqlite3_bind_int(
            statement,
            bindIndex,
            ignoredID
        );
    }

    const bool exists =
        sqlite3_step(statement) == SQLITE_ROW;

    sqlite3_finalize(statement);
    return exists;
}


bool semesterAcceptsCourses(sqlite3 *database, int semesterID)
{
    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT summary_only FROM semesters WHERE id = ?;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, semesterID);
    const bool accepts =
        sqlite3_step(statement) == SQLITE_ROW &&
        sqlite3_column_int(statement, 0) == 0;

    sqlite3_finalize(statement);
    return accepts;
}

CourseStatus defaultCourseStatusForSemester(
    sqlite3 *database,
    int semesterID)
{
    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT in_progress FROM semesters WHERE id = ?;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return CourseStatus::Planned;
    }

    sqlite3_bind_int(statement, 1, semesterID);

    CourseStatus status = CourseStatus::Planned;

    if (sqlite3_step(statement) == SQLITE_ROW &&
        sqlite3_column_int(statement, 0) == 1)
    {
        status = CourseStatus::InProgress;
    }

    sqlite3_finalize(statement);
    return status;
}

bool courseHasCompleteGrade(
    sqlite3 *database,
    int courseID)
{
    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT COALESCE(SUM(weight), 0) "
            "FROM assignments "
            "WHERE course_id = ? "
            "AND grade >= 0;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, courseID);

    bool hasCompleteGrade = false;

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        hasCompleteGrade =
            sqlite3_column_int(statement, 0) == 100;
    }

    sqlite3_finalize(statement);
    return hasCompleteGrade;
}

int assignmentWeightTotal(
    sqlite3 *database,
    int courseID,
    int ignoredAssignmentID = -1)
{
    const char *sql =
        ignoredAssignmentID >= 0
            ? "SELECT COALESCE(SUM(weight), 0) "
              "FROM assignments "
              "WHERE course_id = ? AND id != ?;"
            : "SELECT COALESCE(SUM(weight), 0) "
              "FROM assignments "
              "WHERE course_id = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return 101;
    }

    sqlite3_bind_int(statement, 1, courseID);

    if (ignoredAssignmentID >= 0)
    {
        sqlite3_bind_int(
            statement,
            2,
            ignoredAssignmentID
        );
    }

    int total = 101;

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        total = sqlite3_column_int(statement, 0);
    }

    sqlite3_finalize(statement);
    return total;
}
}

// ============================================================
// Constructor / Destructor
// ============================================================

DatabaseManager::DatabaseManager()
    : database(nullptr)
{
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

// ============================================================
// Connection Management
// ============================================================

bool DatabaseManager::openDatabase(const std::string &databasePath)
{
    std::cout << "Opening database file: " << databasePath << std::endl;

    int result = sqlite3_open(databasePath.c_str(), &database);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to open database: "
                  << sqlite3_errmsg(database) << std::endl;

        closeDatabase();
        return false;
    }

    char *errorMessage = nullptr;

    result = sqlite3_exec(database,
                          "PRAGMA foreign_keys = ON;",
                          nullptr,
                          nullptr,
                          &errorMessage);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to enable foreign keys: "
                  << errorMessage << std::endl;

        sqlite3_free(errorMessage);
        closeDatabase();
        return false;
    }

    std::cout << "Database opened successfully." << std::endl;
    std::cout << "Foreign keys enabled." << std::endl;

    return true;
}

void DatabaseManager::closeDatabase()
{
    if (database != nullptr)
    {
        sqlite3_close(database);
        database = nullptr;
        std::cout << "Database closed." << std::endl;
    }
}

// ============================================================
// Table Setup
// ============================================================

bool DatabaseManager::createTables()
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE, "
        "password_hash TEXT NOT NULL, "
        "max_credits INTEGER NOT NULL DEFAULT 120"
        ");"

        "CREATE TABLE IF NOT EXISTS login_sessions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "token_hash TEXT NOT NULL UNIQUE, "
        "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS semesters ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "name TEXT NOT NULL, "
        "year INTEGER NOT NULL, "
        "in_progress INTEGER NOT NULL DEFAULT 0, "
        "status TEXT NOT NULL DEFAULT 'planned', "
        "summary_only INTEGER NOT NULL DEFAULT 0, "
        "summary_credits INTEGER NOT NULL DEFAULT 0, "
        "summary_gpa REAL NOT NULL DEFAULT 0.0, "
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS courses ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "semester_id INTEGER NOT NULL, "
        "course_code TEXT NOT NULL, "
        "course_name TEXT NOT NULL, "
        "credits INTEGER NOT NULL, "
        "status TEXT NOT NULL DEFAULT 'planned', "
        "retaken INTEGER NOT NULL DEFAULT 0, "
        "FOREIGN KEY (semester_id) REFERENCES semesters(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS assignments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "course_id INTEGER NOT NULL, "
        "name TEXT NOT NULL, "
        "grade REAL NOT NULL, "
        "weight INTEGER NOT NULL, "
        "due_date TEXT NOT NULL DEFAULT '', "
        "completed INTEGER NOT NULL DEFAULT 0, "
        "FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE"
        ");";

    char *errorMessage = nullptr;

    int result = sqlite3_exec(database, sql, nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to create tables: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    // Migrate older databases that were created before due dates existed.
    bool dueDateColumnExists = false;
    sqlite3_stmt *columnStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "PRAGMA table_info(assignments);",
            -1,
            &columnStatement,
            nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(columnStatement) == SQLITE_ROW)
        {
            const unsigned char *columnName =
                sqlite3_column_text(columnStatement, 1);

            if (columnName != nullptr &&
                std::string(
                    reinterpret_cast<const char *>(columnName)
                ) == "due_date")
            {
                dueDateColumnExists = true;
                break;
            }
        }
    }

    sqlite3_finalize(columnStatement);

    if (!dueDateColumnExists)
    {
        char *migrationError = nullptr;

        const int migrationResult = sqlite3_exec(
            database,
            "ALTER TABLE assignments "
            "ADD COLUMN due_date TEXT NOT NULL DEFAULT '';",
            nullptr,
            nullptr,
            &migrationError
        );

        if (migrationResult != SQLITE_OK)
        {
            std::cout << "Failed to add assignment due_date column: "
                      << (migrationError ? migrationError : "Unknown error")
                      << std::endl;

            sqlite3_free(migrationError);
            return false;
        }
    }

    bool completedColumnExists = false;
    sqlite3_stmt *completedColumnStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "PRAGMA table_info(assignments);",
            -1,
            &completedColumnStatement,
            nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(completedColumnStatement) == SQLITE_ROW)
        {
            const unsigned char *columnName =
                sqlite3_column_text(
                    completedColumnStatement,
                    1
                );

            if (columnName != nullptr &&
                std::string(
                    reinterpret_cast<const char *>(columnName)
                ) == "completed")
            {
                completedColumnExists = true;
                break;
            }
        }
    }

    sqlite3_finalize(completedColumnStatement);

    if (!completedColumnExists)
    {
        char *migrationError = nullptr;

        const int migrationResult = sqlite3_exec(
            database,
            "ALTER TABLE assignments "
            "ADD COLUMN completed INTEGER NOT NULL DEFAULT 0;",
            nullptr,
            nullptr,
            &migrationError
        );

        if (migrationResult != SQLITE_OK)
        {
            std::cout
                << "Failed to add assignment completed column: "
                << (migrationError
                        ? migrationError
                        : "Unknown error")
                << std::endl;

            sqlite3_free(migrationError);
            return false;
        }
    }

    if (sqlite3_exec(
            database,
            "UPDATE assignments "
            "SET completed = 1 "
            "WHERE completed = 0 "
            "AND weight = 100 "
            "AND name LIKE 'Final Course Grade (%';",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        std::cout
            << "Failed to migrate completed-course assignments: "
            << sqlite3_errmsg(database)
            << std::endl;
        return false;
    }

    bool semesterStatusColumnAdded = false;

    const struct SemesterColumnMigration
    {
        const char *name;
        const char *sql;
    } semesterMigrations[] = {
        {
            "status",
            "ALTER TABLE semesters "
            "ADD COLUMN status TEXT NOT NULL DEFAULT 'planned';"
        },
        {
            "summary_only",
            "ALTER TABLE semesters "
            "ADD COLUMN summary_only INTEGER NOT NULL DEFAULT 0;"
        },
        {
            "summary_credits",
            "ALTER TABLE semesters "
            "ADD COLUMN summary_credits INTEGER NOT NULL DEFAULT 0;"
        },
        {
            "summary_gpa",
            "ALTER TABLE semesters "
            "ADD COLUMN summary_gpa REAL NOT NULL DEFAULT 0.0;"
        }
    };

    for (const SemesterColumnMigration &migration : semesterMigrations)
    {
        bool exists = false;
        sqlite3_stmt *semesterColumnStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "PRAGMA table_info(semesters);",
                -1,
                &semesterColumnStatement,
                nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(semesterColumnStatement) == SQLITE_ROW)
            {
                const unsigned char *columnName =
                    sqlite3_column_text(semesterColumnStatement, 1);

                if (columnName != nullptr &&
                    std::string(
                        reinterpret_cast<const char *>(columnName)
                    ) == migration.name)
                {
                    exists = true;
                    break;
                }
            }
        }

        sqlite3_finalize(semesterColumnStatement);

        if (!exists)
        {
            if (sqlite3_exec(
                    database,
                    migration.sql,
                    nullptr,
                    nullptr,
                    nullptr) != SQLITE_OK)
            {
                std::cout
                    << "Failed to add semester column "
                    << migration.name
                    << ": "
                    << sqlite3_errmsg(database)
                    << std::endl;
                return false;
            }

            if (std::string(migration.name) == "status")
            {
                semesterStatusColumnAdded = true;
            }
        }
    }

    if (semesterStatusColumnAdded)
    {
        const char *semesterStatusMigrationSql =
            "UPDATE semesters "
            "SET status = CASE "
            "WHEN summary_only = 1 THEN 'completed' "
            "WHEN in_progress = 1 THEN 'active' "
            "ELSE 'planned' "
            "END;";

        if (sqlite3_exec(
                database,
                semesterStatusMigrationSql,
                nullptr,
                nullptr,
                nullptr) != SQLITE_OK)
        {
            std::cout
                << "Failed to migrate existing semester statuses: "
                << sqlite3_errmsg(database)
                << std::endl;
            return false;
        }
    }

    bool courseStatusColumnAdded = false;

    const struct CourseColumnMigration
    {
        const char *name;
        const char *sql;
    } courseMigrations[] = {
        {
            "status",
            "ALTER TABLE courses "
            "ADD COLUMN status TEXT NOT NULL DEFAULT 'planned';"
        },
        {
            "retaken",
            "ALTER TABLE courses "
            "ADD COLUMN retaken INTEGER NOT NULL DEFAULT 0;"
        }
    };

    for (const CourseColumnMigration &migration : courseMigrations)
    {
        bool exists = false;
        sqlite3_stmt *courseColumnStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "PRAGMA table_info(courses);",
                -1,
                &courseColumnStatement,
                nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(courseColumnStatement) == SQLITE_ROW)
            {
                const unsigned char *columnName =
                    sqlite3_column_text(courseColumnStatement, 1);

                if (columnName != nullptr &&
                    std::string(
                        reinterpret_cast<const char *>(columnName)
                    ) == migration.name)
                {
                    exists = true;
                    break;
                }
            }
        }

        sqlite3_finalize(courseColumnStatement);

        if (!exists)
        {
            if (sqlite3_exec(
                    database,
                    migration.sql,
                    nullptr,
                    nullptr,
                    nullptr) != SQLITE_OK)
            {
                std::cout
                    << "Failed to add course column "
                    << migration.name
                    << ": "
                    << sqlite3_errmsg(database)
                    << std::endl;
                return false;
            }

            if (std::string(migration.name) == "status")
            {
                courseStatusColumnAdded = true;
            }
        }
    }

    if (courseStatusColumnAdded)
    {
        const char *statusMigrationSql =
            "UPDATE courses "
            "SET status = CASE "
            "WHEN EXISTS ("
                "SELECT 1 FROM assignments a "
                "WHERE a.course_id = courses.id "
                "AND a.weight = 100 "
                "AND a.name LIKE 'Final Course Grade (%'"
            ") THEN 'completed' "
            "WHEN EXISTS ("
                "SELECT 1 FROM semesters s "
                "WHERE s.id = courses.semester_id "
                "AND s.in_progress = 1"
            ") THEN 'in_progress' "
            "ELSE 'completed' "
            "END;";

        if (sqlite3_exec(
                database,
                statusMigrationSql,
                nullptr,
                nullptr,
                nullptr) != SQLITE_OK)
        {
            std::cout
                << "Failed to migrate existing course statuses: "
                << sqlite3_errmsg(database)
                << std::endl;
            return false;
        }
    }

    std::cout << "Tables created successfully." << std::endl;
    return true;
}

// ============================================================
// Utility
// ============================================================

int DatabaseManager::getLastInsertID() const
{
    return static_cast<int>(sqlite3_last_insert_rowid(database));
}

std::string DatabaseManager::getPasswordHashByUsername(const std::string &username)
{
    const char *sql =
        "SELECT password_hash FROM users "
        "WHERE username = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare getPasswordHashByUsername statement: "
                  << sqlite3_errmsg(database) << std::endl;
        return "";
    }

    sqlite3_bind_text(statement, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    result = sqlite3_step(statement);

    std::string passwordHash;

    if (result == SQLITE_ROW)
    {
        const unsigned char *hashText = sqlite3_column_text(statement, 0);
        passwordHash = hashText ? reinterpret_cast<const char *>(hashText) : "";
    }

    sqlite3_finalize(statement);

    return passwordHash;
}

bool DatabaseManager::saveLoginSession(
    int userID,
    const std::string &tokenHash)
{
    if (database == nullptr ||
        userID <= 0 ||
        tokenHash.empty())
    {
        return false;
    }

    // Keep a single remembered login per user on this installation.
    deleteLoginSessionsForUser(userID);

    const char *sql =
        "INSERT INTO login_sessions "
        "(user_id, token_hash) "
        "VALUES (?, ?);";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, userID);
    sqlite3_bind_text(
        statement,
        2,
        tokenHash.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

int DatabaseManager::getUserIDBySessionTokenHash(
    const std::string &tokenHash)
{
    if (database == nullptr || tokenHash.empty())
    {
        return -1;
    }

    const char *sql =
        "SELECT user_id "
        "FROM login_sessions "
        "WHERE token_hash = ? "
        "LIMIT 1;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return -1;
    }

    sqlite3_bind_text(
        statement,
        1,
        tokenHash.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    int userID = -1;

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        userID = sqlite3_column_int(statement, 0);
    }

    sqlite3_finalize(statement);
    return userID;
}

bool DatabaseManager::deleteLoginSession(
    const std::string &tokenHash)
{
    if (database == nullptr || tokenHash.empty())
    {
        return false;
    }

    const char *sql =
        "DELETE FROM login_sessions "
        "WHERE token_hash = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        tokenHash.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::deleteLoginSessionsForUser(
    int userID)
{
    if (database == nullptr || userID <= 0)
    {
        return false;
    }

    const char *sql =
        "DELETE FROM login_sessions "
        "WHERE user_id = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, userID);

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

// ============================================================
// User Operations
// ============================================================

bool DatabaseManager::addUser(
    const std::string &username,
    const std::string &passwordHash,
    int maxCredits)
{
    if (database == nullptr)
    {
        return false;
    }

    const std::string normalizedUsername =
        trimCopy(username);

    if (!isValidUsername(normalizedUsername) ||
        passwordHash.empty() ||
        maxCredits < 1 ||
        maxCredits > 1000 ||
        userExists(normalizedUsername))
    {
        return false;
    }

    try
    {
        const User candidate(
            -1,
            normalizedUsername,
            maxCredits
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid user data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    const char *sql =
        "INSERT INTO users "
        "(username, password_hash, max_credits) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedUsername.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        passwordHash.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(statement, 3, maxCredits);

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::deleteUser(int userID)
{
    if (database == nullptr || userID <= 0)
    {
        return false;
    }

    char *errorMessage = nullptr;

    if (sqlite3_exec(
            database,
            "BEGIN IMMEDIATE TRANSACTION;",
            nullptr,
            nullptr,
            &errorMessage) != SQLITE_OK)
    {
        std::cout << "Failed to begin account deletion: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;

        sqlite3_free(errorMessage);
        return false;
    }

    const char *statements[] = {
        "DELETE FROM assignments "
        "WHERE course_id IN ("
        "SELECT c.id FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE s.user_id = ?"
        ");",

        "DELETE FROM courses "
        "WHERE semester_id IN ("
        "SELECT id FROM semesters WHERE user_id = ?"
        ");",

        "DELETE FROM semesters WHERE user_id = ?;",

        "DELETE FROM users WHERE id = ?;"
    };

    for (int index = 0; index < 4; ++index)
    {
        sqlite3_stmt *statement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                statements[index],
                -1,
                &statement,
                nullptr) != SQLITE_OK)
        {
            std::cout << "Failed to prepare account deletion statement: "
                      << sqlite3_errmsg(database)
                      << std::endl;

            sqlite3_finalize(statement);
            sqlite3_exec(
                database,
                "ROLLBACK;",
                nullptr,
                nullptr,
                nullptr
            );
            return false;
        }

        sqlite3_bind_int(statement, 1, userID);

        if (sqlite3_step(statement) != SQLITE_DONE)
        {
            std::cout << "Failed during account deletion: "
                      << sqlite3_errmsg(database)
                      << std::endl;

            sqlite3_finalize(statement);
            sqlite3_exec(
                database,
                "ROLLBACK;",
                nullptr,
                nullptr,
                nullptr
            );
            return false;
        }

        const int changedRows = sqlite3_changes(database);
        sqlite3_finalize(statement);

        // The final DELETE must remove exactly one user row.
        if (index == 3 && changedRows != 1)
        {
            std::cout << "Account deletion failed: user row was not removed."
                      << std::endl;

            sqlite3_exec(
                database,
                "ROLLBACK;",
                nullptr,
                nullptr,
                nullptr
            );
            return false;
        }
    }

    if (sqlite3_exec(
            database,
            "COMMIT;",
            nullptr,
            nullptr,
            &errorMessage) != SQLITE_OK)
    {
        std::cout << "Failed to commit account deletion: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;

        sqlite3_free(errorMessage);
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    std::cout << "User and all related data deleted successfully. ID: "
              << userID
              << std::endl;

    return true;
}

bool DatabaseManager::userExists(const std::string &username)
{
    const char *sql =
        "SELECT id FROM users "
        "WHERE username = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare userExists statement: "
                  << sqlite3_errmsg(database) << std::endl;
        return false;
    }

    sqlite3_bind_text(statement, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    result = sqlite3_step(statement);

    bool exists = (result == SQLITE_ROW);

    if (result != SQLITE_ROW && result != SQLITE_DONE)
    {
        std::cout << "Failed to check user existence: "
                  << sqlite3_errmsg(database) << std::endl;
    }

    sqlite3_finalize(statement);

    return exists;
}

int DatabaseManager::getUserIDByUsername(const std::string &username)
{
    const char *sql =
        "SELECT id FROM users "
        "WHERE username = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare getUserIDByUsername statement: "
                  << sqlite3_errmsg(database) << std::endl;
        return -1;
    }

    sqlite3_bind_text(statement, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    result = sqlite3_step(statement);

    int userID = -1;

    if (result == SQLITE_ROW)
    {
        userID = sqlite3_column_int(statement, 0);
    }
    else if (result == SQLITE_DONE)
    {
        userID = -1;
    }
    else
    {
        std::cout << "Failed to get user ID: "
                  << sqlite3_errmsg(database) << std::endl;
    }

    sqlite3_finalize(statement);

    return userID;
}

User DatabaseManager::loadUserByUsername(const std::string &username)
{
    const char *sql =
        "SELECT id, username, max_credits FROM users "
        "WHERE username = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        throw std::runtime_error(
            std::string("Failed to prepare loadUserByUsername statement: ") +
            sqlite3_errmsg(database));
    }

    sqlite3_bind_text(statement, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        int userID = sqlite3_column_int(statement, 0);

        const unsigned char *usernameText = sqlite3_column_text(statement, 1);
        std::string loadedUsername =
            usernameText ? reinterpret_cast<const char *>(usernameText) : "";

        int maxCredits = sqlite3_column_int(statement, 2);

        sqlite3_finalize(statement);

        return User(userID, loadedUsername, maxCredits);
    }

    sqlite3_finalize(statement);

    throw std::runtime_error("User not found: " + username);
}

User DatabaseManager::loadFullUserByUsername(const std::string &username)
{
    User user = loadUserByUsername(username);

    std::vector<Semester> semesters = loadSemestersForUser(user.getID());

    for (auto &semester : semesters)
    {
        std::vector<Course> courses = loadCoursesForSemester(semester.getID());

        for (auto &course : courses)
        {
            std::vector<Assignment> assignments = loadAssignmentsForCourse(course.getID());

            for (const auto &assignment : assignments)
            {
                course.addAssignment(assignment);
            }

            semester.addCourse(course);
        }

        user.addSemester(semester);
    }

    return user;
}

// ============================================================
// Semester Operations
// ============================================================

bool DatabaseManager::addSemester(
    int userID,
    const std::string &name,
    int year,
    bool inProgress)
{
    if (database == nullptr || userID <= 0)
    {
        return false;
    }

    const std::string normalizedName = trimCopy(name);

    try
    {
        const Semester candidate(
            -1,
            normalizedName,
            year,
            inProgress,
            false,
            0,
            0.0,
            inProgress
                ? SemesterStatus::Active
                : SemesterStatus::Planned
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid semester data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    if (rowExists(
            database,
            "SELECT 1 FROM semesters "
            "WHERE user_id = ? "
            "AND lower(trim(name)) = lower(trim(?)) "
            "AND year = ? LIMIT 1;",
            userID,
            normalizedName,
            year))
    {
        return false;
    }

    if (inProgress &&
        !clearCurrentSemesterForUser(userID))
    {
        return false;
    }

    const char *sql =
        "INSERT INTO semesters "
        "(user_id, name, year, in_progress, status) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, userID);
    sqlite3_bind_text(
        statement,
        2,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 3, year);
    sqlite3_bind_int(statement, 4, inProgress ? 1 : 0);

    const std::string storedStatus =
        semesterStatusToStorage(
            inProgress
                ? SemesterStatus::Active
                : SemesterStatus::Planned
        );

    sqlite3_bind_text(
        statement,
        5,
        storedStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::addCompletedSemester(
    int userID,
    const std::string &name,
    int year,
    int completedCredits,
    double semesterGPA)
{
    if (database == nullptr || userID <= 0)
    {
        return false;
    }

    const std::string normalizedName = trimCopy(name);

    try
    {
        const Semester candidate(-1, normalizedName, year, false, true,
                                 completedCredits, semesterGPA);
        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid completed semester data: " << error.what() << std::endl;
        return false;
    }

    if (rowExists(database,
                  "SELECT 1 FROM semesters WHERE user_id = ? "
                  "AND lower(trim(name)) = lower(trim(?)) AND year = ? LIMIT 1;",
                  userID, normalizedName, year))
    {
        return false;
    }

    const char *sql =
        "INSERT INTO semesters "
        "(user_id, name, year, in_progress, status, "
        "summary_only, summary_credits, summary_gpa) "
        "VALUES (?, ?, ?, 0, 'completed', 1, ?, ?);";

    sqlite3_stmt *statement = nullptr;
    if (sqlite3_prepare_v2(database, sql, -1, &statement, nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, userID);
    sqlite3_bind_text(statement, 2, normalizedName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 3, year);
    sqlite3_bind_int(statement, 4, completedCredits);
    sqlite3_bind_double(statement, 5, semesterGPA);

    const bool success = sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::updateSemester(
    int semesterID,
    const std::string &name,
    int year,
    bool inProgress)
{
    if (database == nullptr || semesterID <= 0)
    {
        return false;
    }

    const std::string normalizedName = trimCopy(name);

    try
    {
        const Semester candidate(
            semesterID,
            normalizedName,
            year,
            inProgress,
            false,
            0,
            0.0,
            inProgress
                ? SemesterStatus::Active
                : SemesterStatus::Planned
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid semester data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    sqlite3_stmt *ownerStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT user_id FROM semesters WHERE id = ?;",
            -1,
            &ownerStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(ownerStatement, 1, semesterID);

    if (sqlite3_step(ownerStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(ownerStatement);
        return false;
    }

    const int userID =
        sqlite3_column_int(ownerStatement, 0);

    sqlite3_finalize(ownerStatement);

    if (rowExists(
            database,
            "SELECT 1 FROM semesters "
            "WHERE user_id = ? "
            "AND lower(trim(name)) = lower(trim(?)) "
            "AND year = ? AND id != ? LIMIT 1;",
            userID,
            normalizedName,
            year,
            semesterID))
    {
        return false;
    }

    if (inProgress &&
        !clearCurrentSemesterForUser(userID))
    {
        return false;
    }

    const char *sql =
        "UPDATE semesters "
        "SET name = ?, year = ?, in_progress = ?, status = ? "
        "WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 2, year);
    sqlite3_bind_int(statement, 3, inProgress ? 1 : 0);

    const std::string storedStatus =
        semesterStatusToStorage(
            inProgress
                ? SemesterStatus::Active
                : SemesterStatus::Planned
        );

    sqlite3_bind_text(
        statement,
        4,
        storedStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 5, semesterID);

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::updateSemesterDetails(
    int semesterID,
    const std::string &name,
    int year,
    bool inProgress,
    bool summaryOnly,
    int summaryCredits,
    double summaryGPA,
    SemesterStatus semesterStatus)
{
    if (database == nullptr || semesterID <= 0)
    {
        return false;
    }

    const std::string normalizedName = trimCopy(name);

    SemesterStatus effectiveStatus =
        summaryOnly
            ? SemesterStatus::Completed
            : semesterStatus;

    if (!summaryOnly && inProgress)
    {
        effectiveStatus = SemesterStatus::Active;
    }

    const bool effectiveInProgress =
        effectiveStatus == SemesterStatus::Active;

    const int effectiveSummaryCredits =
        summaryOnly ? summaryCredits : 0;
    const double effectiveSummaryGPA =
        summaryOnly ? summaryGPA : 0.0;

    try
    {
        const Semester candidate(
            semesterID,
            normalizedName,
            year,
            effectiveInProgress,
            summaryOnly,
            effectiveSummaryCredits,
            effectiveSummaryGPA,
            effectiveStatus
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid semester data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    sqlite3_stmt *ownerStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT user_id FROM semesters WHERE id = ?;",
            -1,
            &ownerStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(ownerStatement, 1, semesterID);

    if (sqlite3_step(ownerStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(ownerStatement);
        return false;
    }

    const int userID = sqlite3_column_int(ownerStatement, 0);
    sqlite3_finalize(ownerStatement);

    if (rowExists(
            database,
            "SELECT 1 FROM semesters "
            "WHERE user_id = ? "
            "AND lower(trim(name)) = lower(trim(?)) "
            "AND year = ? AND id != ? LIMIT 1;",
            userID,
            normalizedName,
            year,
            semesterID))
    {
        return false;
    }

    if (summaryOnly)
    {
        sqlite3_stmt *courseStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "SELECT 1 FROM courses "
                "WHERE semester_id = ? LIMIT 1;",
                -1,
                &courseStatement,
                nullptr) != SQLITE_OK)
        {
            return false;
        }

        sqlite3_bind_int(courseStatement, 1, semesterID);
        const bool hasCourses =
            sqlite3_step(courseStatement) == SQLITE_ROW;
        sqlite3_finalize(courseStatement);

        if (hasCourses)
        {
            return false;
        }
    }

    if (sqlite3_exec(
            database,
            "BEGIN IMMEDIATE TRANSACTION;",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    if (effectiveInProgress)
    {
        sqlite3_stmt *clearStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "UPDATE semesters "
                "SET in_progress = 0, "
                "status = CASE "
                "WHEN status = 'active' THEN 'planned' "
                "ELSE status END "
                "WHERE user_id = ?;",
                -1,
                &clearStatement,
                nullptr) != SQLITE_OK)
        {
            sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        sqlite3_bind_int(clearStatement, 1, userID);
        const bool cleared =
            sqlite3_step(clearStatement) == SQLITE_DONE;
        sqlite3_finalize(clearStatement);

        if (!cleared)
        {
            sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
    }

    const char *sql =
        "UPDATE semesters "
        "SET name = ?, year = ?, in_progress = ?, status = ?, "
        "summary_only = ?, summary_credits = ?, summary_gpa = ? "
        "WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 2, year);
    sqlite3_bind_int(statement, 3, effectiveInProgress ? 1 : 0);

    const std::string storedStatus =
        semesterStatusToStorage(effectiveStatus);

    sqlite3_bind_text(
        statement,
        4,
        storedStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 5, summaryOnly ? 1 : 0);
    sqlite3_bind_int(statement, 6, effectiveSummaryCredits);
    sqlite3_bind_double(statement, 7, effectiveSummaryGPA);
    sqlite3_bind_int(statement, 8, semesterID);

    const bool updated =
        sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_finalize(statement);

    if (!updated || sqlite3_changes(database) != 1)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    if (sqlite3_exec(
            database,
            "COMMIT;",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    return true;
}

bool DatabaseManager::setSemesterStatus(
    int semesterID,
    SemesterStatus status)
{
    if (database == nullptr || semesterID <= 0)
    {
        return false;
    }

    sqlite3_stmt *semesterStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT user_id, summary_only, status "
            "FROM semesters WHERE id = ?;",
            -1,
            &semesterStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        semesterStatement,
        1,
        semesterID
    );

    if (sqlite3_step(semesterStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(semesterStatement);
        return false;
    }

    const int userID =
        sqlite3_column_int(semesterStatement, 0);

    const bool summaryOnly =
        sqlite3_column_int(semesterStatement, 1) == 1;

    const unsigned char *currentStatusText =
        sqlite3_column_text(semesterStatement, 2);

    const SemesterStatus currentStatus =
        semesterStatusFromStorage(
            currentStatusText
                ? reinterpret_cast<const char *>(
                      currentStatusText
                  )
                : "planned"
        );

    sqlite3_finalize(semesterStatement);

    if (summaryOnly &&
        status != SemesterStatus::Completed)
    {
        return false;
    }

    if (status == currentStatus)
    {
        return true;
    }

    if (status == SemesterStatus::Completed)
    {
        sqlite3_stmt *unfinishedStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "SELECT 1 FROM courses "
                "WHERE semester_id = ? "
                "AND status NOT IN ('completed', 'withdrawn') "
                "LIMIT 1;",
                -1,
                &unfinishedStatement,
                nullptr) != SQLITE_OK)
        {
            return false;
        }

        sqlite3_bind_int(
            unfinishedStatement,
            1,
            semesterID
        );

        const bool hasUnfinishedCourses =
            sqlite3_step(unfinishedStatement) == SQLITE_ROW;

        sqlite3_finalize(unfinishedStatement);

        if (hasUnfinishedCourses)
        {
            return false;
        }
    }

    if (sqlite3_exec(
            database,
            "BEGIN IMMEDIATE TRANSACTION;",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    const auto rollback = [this]()
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
    };

    if (status == SemesterStatus::Active)
    {
        sqlite3_stmt *oldCourseStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "UPDATE courses "
                "SET status = 'planned' "
                "WHERE status = 'in_progress' "
                "AND semester_id IN ("
                    "SELECT id FROM semesters "
                    "WHERE user_id = ? "
                    "AND status = 'active' "
                    "AND id != ?"
                ");",
                -1,
                &oldCourseStatement,
                nullptr) != SQLITE_OK)
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            oldCourseStatement,
            1,
            userID
        );
        sqlite3_bind_int(
            oldCourseStatement,
            2,
            semesterID
        );

        const bool oldCoursesUpdated =
            sqlite3_step(oldCourseStatement) == SQLITE_DONE;

        sqlite3_finalize(oldCourseStatement);

        if (!oldCoursesUpdated)
        {
            rollback();
            return false;
        }

        sqlite3_stmt *clearStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "UPDATE semesters "
                "SET in_progress = 0, "
                "status = CASE "
                    "WHEN status = 'active' THEN 'planned' "
                    "ELSE status "
                "END "
                "WHERE user_id = ? AND id != ?;",
                -1,
                &clearStatement,
                nullptr) != SQLITE_OK)
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            clearStatement,
            1,
            userID
        );
        sqlite3_bind_int(
            clearStatement,
            2,
            semesterID
        );

        const bool cleared =
            sqlite3_step(clearStatement) == SQLITE_DONE;

        sqlite3_finalize(clearStatement);

        if (!cleared)
        {
            rollback();
            return false;
        }

        sqlite3_stmt *activateCoursesStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "UPDATE courses "
                "SET status = 'in_progress' "
                "WHERE semester_id = ? "
                "AND status = 'planned';",
                -1,
                &activateCoursesStatement,
                nullptr) != SQLITE_OK)
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            activateCoursesStatement,
            1,
            semesterID
        );

        const bool coursesActivated =
            sqlite3_step(activateCoursesStatement) == SQLITE_DONE;

        sqlite3_finalize(activateCoursesStatement);

        if (!coursesActivated)
        {
            rollback();
            return false;
        }
    }
    else if (status == SemesterStatus::Planned)
    {
        sqlite3_stmt *planCoursesStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "UPDATE courses "
                "SET status = 'planned' "
                "WHERE semester_id = ? "
                "AND status = 'in_progress';",
                -1,
                &planCoursesStatement,
                nullptr) != SQLITE_OK)
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            planCoursesStatement,
            1,
            semesterID
        );

        const bool coursesPlanned =
            sqlite3_step(planCoursesStatement) == SQLITE_DONE;

        sqlite3_finalize(planCoursesStatement);

        if (!coursesPlanned)
        {
            rollback();
            return false;
        }
    }

    sqlite3_stmt *updateStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE semesters "
            "SET status = ?, in_progress = ? "
            "WHERE id = ?;",
            -1,
            &updateStatement,
            nullptr) != SQLITE_OK)
    {
        rollback();
        return false;
    }

    const std::string storedStatus =
        semesterStatusToStorage(status);

    sqlite3_bind_text(
        updateStatement,
        1,
        storedStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        updateStatement,
        2,
        status == SemesterStatus::Active ? 1 : 0
    );
    sqlite3_bind_int(
        updateStatement,
        3,
        semesterID
    );

    const bool updated =
        sqlite3_step(updateStatement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(updateStatement);

    if (!updated)
    {
        rollback();
        return false;
    }

    if (sqlite3_exec(
            database,
            "COMMIT;",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        rollback();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteSemester(int semesterID)
{
    const char *sql =
        "DELETE FROM semesters WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare deleteSemester statement: "
                  << sqlite3_errmsg(database) << std::endl;
        return false;
    }

    sqlite3_bind_int(statement, 1, semesterID);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        std::cout << "Failed to delete semester: "
                  << sqlite3_errmsg(database) << std::endl;

        sqlite3_finalize(statement);
        return false;
    }

    sqlite3_finalize(statement);

    std::cout << "Semester deleted successfully. ID: " << semesterID << std::endl;
    return true;
}

std::vector<Semester> DatabaseManager::loadSemestersForUser(int userID)
{
    std::vector<Semester> semesters;

    const char *sql =
        "SELECT id, name, year, in_progress, status, "
        "summary_only, summary_credits, summary_gpa FROM semesters "
        "WHERE user_id = ? "
        "ORDER BY year ASC, id ASC;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare loadSemestersForUser statement: "
                  << sqlite3_errmsg(database) << std::endl;
        return semesters;
    }

    sqlite3_bind_int(statement, 1, userID);

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        int semesterID = sqlite3_column_int(statement, 0);

        const unsigned char *nameText = sqlite3_column_text(statement, 1);
        std::string name =
            nameText ? reinterpret_cast<const char *>(nameText) : "";

        int year = sqlite3_column_int(statement, 2);

        bool inProgress = sqlite3_column_int(statement, 3) == 1;

        const unsigned char *statusText =
            sqlite3_column_text(statement, 4);

        const std::string storedStatus =
            statusText
                ? reinterpret_cast<const char *>(statusText)
                : (inProgress ? "active" : "planned");

        bool summaryOnly = sqlite3_column_int(statement, 5) == 1;
        int summaryCredits = sqlite3_column_int(statement, 6);
        double summaryGPA = sqlite3_column_double(statement, 7);

        Semester semester(
            semesterID,
            name,
            year,
            inProgress,
            summaryOnly,
            summaryCredits,
            summaryGPA,
            semesterStatusFromStorage(storedStatus)
        );
        semesters.push_back(semester);
    }

    if (result != SQLITE_DONE)
    {
        std::cout << "Error while loading semesters: "
                  << sqlite3_errmsg(database) << std::endl;
    }

    sqlite3_finalize(statement);

    return semesters;
}

bool DatabaseManager::clearCurrentSemesterForUser(int userID)
{
    if (database == nullptr || userID <= 0)
    {
        return false;
    }

    if (sqlite3_exec(
            database,
            "BEGIN IMMEDIATE TRANSACTION;",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_stmt *courseStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE courses "
            "SET status = 'planned' "
            "WHERE status = 'in_progress' "
            "AND semester_id IN ("
                "SELECT id FROM semesters "
                "WHERE user_id = ? "
                "AND status = 'active'"
            ");",
            -1,
            &courseStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    sqlite3_bind_int(
        courseStatement,
        1,
        userID
    );

    const bool coursesUpdated =
        sqlite3_step(courseStatement) == SQLITE_DONE;

    sqlite3_finalize(courseStatement);

    if (!coursesUpdated)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    sqlite3_stmt *semesterStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE semesters "
            "SET in_progress = 0, "
            "status = CASE "
                "WHEN status = 'active' THEN 'planned' "
                "ELSE status "
            "END "
            "WHERE user_id = ?;",
            -1,
            &semesterStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    sqlite3_bind_int(
        semesterStatement,
        1,
        userID
    );

    const bool semestersUpdated =
        sqlite3_step(semesterStatement) == SQLITE_DONE;

    sqlite3_finalize(semesterStatement);

    if (!semestersUpdated)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    if (sqlite3_exec(
            database,
            "COMMIT;",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    return true;
}

// ============================================================
// Course Operations
// ============================================================

bool DatabaseManager::addCourse(
    int semesterID,
    const std::string &courseCode,
    const std::string &courseName,
    int credits)
{
    if (database == nullptr || semesterID <= 0 ||
        !semesterAcceptsCourses(database, semesterID))
    {
        return false;
    }

    const std::string normalizedCode =
        trimCopy(courseCode);

    const std::string normalizedName =
        trimCopy(courseName);

    try
    {
        const Course candidate(
            -1,
            normalizedName,
            normalizedCode,
            credits
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid course data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    if (rowExists(
            database,
            "SELECT 1 FROM courses "
            "WHERE semester_id = ? "
            "AND lower(trim(course_code)) = "
            "lower(trim(?)) LIMIT 1;",
            semesterID,
            normalizedCode,
            -1))
    {
        return false;
    }

    const CourseStatus initialStatus =
        defaultCourseStatusForSemester(
            database,
            semesterID
        );

    const std::string storedStatus =
        courseStatusToStorage(initialStatus);

    const char *sql =
        "INSERT INTO courses "
        "(semester_id, course_code, course_name, credits, status, retaken) "
        "VALUES (?, ?, ?, ?, ?, 0);";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, semesterID);
    sqlite3_bind_text(
        statement,
        2,
        normalizedCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        statement,
        3,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 4, credits);
    sqlite3_bind_text(
        statement,
        5,
        storedStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}


bool DatabaseManager::addCompletedCourse(
    int semesterID,
    const std::string &courseCode,
    const std::string &courseName,
    int credits,
    const std::string &letterGrade,
    double finalPercentage)
{
    if (database == nullptr ||
        semesterID <= 0 ||
        letterGrade.empty() ||
        finalPercentage < 0.0 ||
        finalPercentage > 100.0)
    {
        return false;
    }

    char *errorMessage = nullptr;

    if (sqlite3_exec(
            database,
            "BEGIN IMMEDIATE TRANSACTION;",
            nullptr,
            nullptr,
            &errorMessage) != SQLITE_OK)
    {
        std::cout << "Failed to begin completed-course transaction: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    if (!addCourse(
            semesterID,
            courseCode,
            courseName,
            credits))
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    const int courseID = getLastInsertID();

    sqlite3_stmt *statusStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE courses "
            "SET status = 'completed' "
            "WHERE id = ?;",
            -1,
            &statusStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    sqlite3_bind_int(
        statusStatement,
        1,
        courseID
    );

    const bool statusSaved =
        sqlite3_step(statusStatement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(statusStatement);

    if (!statusSaved)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    const std::string assignmentName =
        "Final Course Grade (" +
        letterGrade +
        ")";

    sqlite3_stmt *assignmentStatement = nullptr;

    const char *assignmentSql =
        "INSERT INTO assignments "
        "(course_id, name, grade, weight, completed) "
        "VALUES (?, ?, ?, 100, 1);";

    if (sqlite3_prepare_v2(
            database,
            assignmentSql,
            -1,
            &assignmentStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    sqlite3_bind_int(
        assignmentStatement,
        1,
        courseID
    );
    sqlite3_bind_text(
        assignmentStatement,
        2,
        assignmentName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_double(
        assignmentStatement,
        3,
        finalPercentage
    );

    const bool assignmentSaved =
        sqlite3_step(assignmentStatement) ==
        SQLITE_DONE;

    sqlite3_finalize(assignmentStatement);

    if (!assignmentSaved)
    {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    if (sqlite3_exec(
            database,
            "COMMIT;",
            nullptr,
            nullptr,
            &errorMessage) != SQLITE_OK)
    {
        std::cout << "Failed to commit completed-course transaction: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;
        sqlite3_free(errorMessage);

        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    return true;
}

int DatabaseManager::findCourseByCodeForUser(
    int userID,
    const std::string &courseCode)
{
    if (database == nullptr ||
        userID <= 0 ||
        courseCode.empty())
    {
        return -1;
    }

    const char *sql =
        "SELECT c.id "
        "FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE s.user_id = ? "
        "AND lower(trim(c.course_code)) = lower(trim(?)) "
        "ORDER BY s.year DESC, c.id DESC "
        "LIMIT 1;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return -1;
    }

    sqlite3_bind_int(statement, 1, userID);
    sqlite3_bind_text(
        statement,
        2,
        courseCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    int courseID = -1;

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        courseID = sqlite3_column_int(
            statement,
            0
        );
    }

    sqlite3_finalize(statement);
    return courseID;
}

bool DatabaseManager::retakeCourse(
    int existingCourseID,
    int newSemesterID,
    const std::string &courseCode,
    const std::string &courseName,
    int credits,
    bool hasFinalGrade,
    const std::string &letterGrade,
    double finalPercentage)
{
    if (database == nullptr ||
        existingCourseID <= 0 ||
        newSemesterID <= 0 ||
        courseCode.empty() ||
        courseName.empty() ||
        credits < 1 ||
        credits > 30 ||
        !semesterAcceptsCourses(database, newSemesterID))
    {
        return false;
    }

    if (hasFinalGrade &&
        (letterGrade.empty() ||
         finalPercentage < 0.0 ||
         finalPercentage > 100.0))
    {
        return false;
    }

    char *errorMessage = nullptr;

    if (sqlite3_exec(
            database,
            "BEGIN IMMEDIATE TRANSACTION;",
            nullptr,
            nullptr,
            &errorMessage) != SQLITE_OK)
    {
        std::cout << "Failed to begin retake transaction: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    sqlite3_stmt *deleteAssignmentsStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "DELETE FROM assignments WHERE course_id = ?;",
            -1,
            &deleteAssignmentsStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(
        deleteAssignmentsStatement,
        1,
        existingCourseID
    );

    const bool assignmentsDeleted =
        sqlite3_step(deleteAssignmentsStatement) ==
        SQLITE_DONE;

    sqlite3_finalize(deleteAssignmentsStatement);

    if (!assignmentsDeleted)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    const CourseStatus retakeStatus =
        hasFinalGrade
            ? CourseStatus::Completed
            : defaultCourseStatusForSemester(
                  database,
                  newSemesterID
              );

    const std::string storedRetakeStatus =
        courseStatusToStorage(retakeStatus);

    sqlite3_stmt *updateCourseStatement = nullptr;

    const char *updateSql =
        "UPDATE courses "
        "SET semester_id = ?, course_code = ?, "
        "course_name = ?, credits = ?, "
        "status = ?, retaken = 1 "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(
            database,
            updateSql,
            -1,
            &updateCourseStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(
        updateCourseStatement,
        1,
        newSemesterID
    );
    sqlite3_bind_text(
        updateCourseStatement,
        2,
        courseCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        updateCourseStatement,
        3,
        courseName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        updateCourseStatement,
        4,
        credits
    );
    sqlite3_bind_text(
        updateCourseStatement,
        5,
        storedRetakeStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        updateCourseStatement,
        6,
        existingCourseID
    );

    const bool courseUpdated =
        sqlite3_step(updateCourseStatement) ==
            SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(updateCourseStatement);

    if (!courseUpdated)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    if (hasFinalGrade)
    {
        const std::string assignmentName =
            "Final Course Grade (" +
            letterGrade +
            ")";

        sqlite3_stmt *gradeStatement = nullptr;

        const char *gradeSql =
            "INSERT INTO assignments "
            "(course_id, name, grade, weight, completed) "
            "VALUES (?, ?, ?, 100, 1);";

        if (sqlite3_prepare_v2(
                database,
                gradeSql,
                -1,
                &gradeStatement,
                nullptr) != SQLITE_OK)
        {
            sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        sqlite3_bind_int(
            gradeStatement,
            1,
            existingCourseID
        );
        sqlite3_bind_text(
            gradeStatement,
            2,
            assignmentName.c_str(),
            -1,
            SQLITE_TRANSIENT
        );
        sqlite3_bind_double(
            gradeStatement,
            3,
            finalPercentage
        );

        const bool gradeSaved =
            sqlite3_step(gradeStatement) ==
            SQLITE_DONE;

        sqlite3_finalize(gradeStatement);

        if (!gradeSaved)
        {
            sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
    }

    if (sqlite3_exec(
            database,
            "COMMIT;",
            nullptr,
            nullptr,
            &errorMessage) != SQLITE_OK)
    {
        std::cout << "Failed to commit retake transaction: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;
        sqlite3_free(errorMessage);

        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );
        return false;
    }

    return true;
}

bool DatabaseManager::updateCourse(
    int courseID,
    const std::string &courseCode,
    const std::string &courseName,
    int credits)
{
    if (database == nullptr || courseID <= 0)
    {
        return false;
    }

    const std::string normalizedCode =
        trimCopy(courseCode);

    const std::string normalizedName =
        trimCopy(courseName);

    try
    {
        const Course candidate(
            courseID,
            normalizedName,
            normalizedCode,
            credits
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid course data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    sqlite3_stmt *ownerStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT semester_id FROM courses WHERE id = ?;",
            -1,
            &ownerStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(ownerStatement, 1, courseID);

    if (sqlite3_step(ownerStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(ownerStatement);
        return false;
    }

    const int semesterID =
        sqlite3_column_int(ownerStatement, 0);

    sqlite3_finalize(ownerStatement);

    sqlite3_stmt *duplicateStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT 1 FROM courses "
            "WHERE semester_id = ? "
            "AND lower(trim(course_code)) = lower(trim(?)) "
            "AND id != ? LIMIT 1;",
            -1,
            &duplicateStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        duplicateStatement,
        1,
        semesterID
    );
    sqlite3_bind_text(
        duplicateStatement,
        2,
        normalizedCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        duplicateStatement,
        3,
        courseID
    );

    const bool duplicateExists =
        sqlite3_step(duplicateStatement) == SQLITE_ROW;

    sqlite3_finalize(duplicateStatement);

    if (duplicateExists)
    {
        return false;
    }

    const char *sql =
        "UPDATE courses "
        "SET course_code = ?, course_name = ?, credits = ? "
        "WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        statement,
        2,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 3, credits);
    sqlite3_bind_int(statement, 4, courseID);

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}


bool DatabaseManager::updateCourseDetails(
    int courseID,
    int semesterID,
    const std::string &courseCode,
    const std::string &courseName,
    int credits)
{
    if (database == nullptr ||
        courseID <= 0 ||
        semesterID <= 0 ||
        !semesterAcceptsCourses(database, semesterID))
    {
        return false;
    }

    const std::string normalizedCode =
        trimCopy(courseCode);

    const std::string normalizedName =
        trimCopy(courseName);

    try
    {
        const Course candidate(
            courseID,
            normalizedName,
            normalizedCode,
            credits
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid course data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    sqlite3_stmt *userStatement = nullptr;

    const char *userSql =
        "SELECT user_id FROM semesters WHERE id = ?;";

    if (sqlite3_prepare_v2(
            database,
            userSql,
            -1,
            &userStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(userStatement, 1, semesterID);

    if (sqlite3_step(userStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(userStatement);
        return false;
    }

    const int userID =
        sqlite3_column_int(userStatement, 0);

    sqlite3_finalize(userStatement);

    sqlite3_stmt *duplicateStatement = nullptr;

    const char *duplicateSql =
        "SELECT 1 "
        "FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE s.user_id = ? "
        "AND lower(trim(c.course_code)) = lower(trim(?)) "
        "AND c.id != ? "
        "LIMIT 1;";

    if (sqlite3_prepare_v2(
            database,
            duplicateSql,
            -1,
            &duplicateStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        duplicateStatement,
        1,
        userID
    );
    sqlite3_bind_text(
        duplicateStatement,
        2,
        normalizedCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        duplicateStatement,
        3,
        courseID
    );

    const bool duplicateExists =
        sqlite3_step(duplicateStatement) == SQLITE_ROW;

    sqlite3_finalize(duplicateStatement);

    if (duplicateExists)
    {
        return false;
    }

    sqlite3_stmt *statement = nullptr;

    const char *sql =
        "UPDATE courses "
        "SET semester_id = ?, "
        "course_code = ?, "
        "course_name = ?, "
        "credits = ? "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        statement,
        1,
        semesterID
    );
    sqlite3_bind_text(
        statement,
        2,
        normalizedCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        statement,
        3,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        statement,
        4,
        credits
    );
    sqlite3_bind_int(
        statement,
        5,
        courseID
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::setCourseStatus(
    int courseID,
    CourseStatus status)
{
    if (database == nullptr || courseID <= 0)
    {
        return false;
    }

    if (status == CourseStatus::Completed &&
        !courseHasCompleteGrade(
            database,
            courseID
        ))
    {
        return false;
    }

    const std::string storedStatus =
        courseStatusToStorage(status);

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE courses "
            "SET status = ? "
            "WHERE id = ?;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        storedStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        statement,
        2,
        courseID
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::deleteCourse(int courseID)
{
    const char *sql =
        "DELETE FROM courses WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare deleteCourse statement: "
                  << sqlite3_errmsg(database) << std::endl;
        return false;
    }

    sqlite3_bind_int(statement, 1, courseID);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        std::cout << "Failed to delete course: "
                  << sqlite3_errmsg(database) << std::endl;

        sqlite3_finalize(statement);
        return false;
    }

    sqlite3_finalize(statement);

    std::cout << "Course deleted successfully. ID: " << courseID << std::endl;
    return true;
}

std::vector<Course> DatabaseManager::loadCoursesForSemester(int semesterID)
{
    std::vector<Course> courses;

    const char *sql =
        "SELECT id, course_name, course_code, credits, status, retaken "
        "FROM courses "
        "WHERE semester_id = ? "
        "ORDER BY id ASC;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare loadCoursesForSemester statement: "
                  << sqlite3_errmsg(database) << std::endl;
        return courses;
    }

    sqlite3_bind_int(statement, 1, semesterID);

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        int courseID = sqlite3_column_int(statement, 0);

        const unsigned char *nameText = sqlite3_column_text(statement, 1);
        std::string courseName =
            nameText ? reinterpret_cast<const char *>(nameText) : "";

        const unsigned char *codeText = sqlite3_column_text(statement, 2);
        std::string courseCode =
            codeText ? reinterpret_cast<const char *>(codeText) : "";

        int credits = sqlite3_column_int(statement, 3);

        const unsigned char *statusText =
            sqlite3_column_text(statement, 4);

        const std::string storedStatus =
            statusText
                ? reinterpret_cast<const char *>(statusText)
                : "in_progress";

        const bool retaken =
            sqlite3_column_int(statement, 5) == 1;

        Course course(
            courseID,
            courseName,
            courseCode,
            credits,
            courseStatusFromStorage(storedStatus),
            retaken
        );

        courses.push_back(course);
    }

    if (result != SQLITE_DONE)
    {
        std::cout << "Error while loading courses: "
                  << sqlite3_errmsg(database) << std::endl;
    }

    sqlite3_finalize(statement);

    return courses;
}

// ============================================================
// Assignment Operations
// ============================================================

bool DatabaseManager::addAssignment(
    int courseID,
    const std::string &name,
    double grade,
    int weight,
    const std::string &dueDate)
{
    if (database == nullptr || courseID <= 0)
    {
        return false;
    }

    const std::string normalizedName = trimCopy(name);
    const std::string normalizedDueDate = trimCopy(dueDate);

    try
    {
        const Assignment candidate(
            -1,
            normalizedName,
            weight,
            grade,
            normalizedDueDate
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid assignment data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    if (rowExists(
            database,
            "SELECT 1 FROM assignments "
            "WHERE course_id = ? "
            "AND lower(trim(name)) = lower(trim(?)) "
            "LIMIT 1;",
            courseID,
            normalizedName,
            -1))
    {
        return false;
    }

    const int currentWeight =
        assignmentWeightTotal(database, courseID);

    if (currentWeight + weight > 100)
    {
        return false;
    }

    const char *sql =
        "INSERT INTO assignments "
        "(course_id, name, grade, weight, due_date) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, courseID);
    sqlite3_bind_text(
        statement,
        2,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_double(statement, 3, grade);
    sqlite3_bind_int(statement, 4, weight);
    sqlite3_bind_text(
        statement,
        5,
        normalizedDueDate.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::updateAssignment(
    int assignmentID,
    const std::string &name,
    double grade,
    int weight,
    const std::string &dueDate)
{
    if (database == nullptr || assignmentID <= 0)
    {
        return false;
    }

    const std::string normalizedName = trimCopy(name);
    const std::string normalizedDueDate = trimCopy(dueDate);

    try
    {
        const Assignment candidate(
            assignmentID,
            normalizedName,
            weight,
            grade,
            normalizedDueDate
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        std::cout << "Invalid assignment data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    sqlite3_stmt *ownerStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT course_id FROM assignments WHERE id = ?;",
            -1,
            &ownerStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(ownerStatement, 1, assignmentID);

    if (sqlite3_step(ownerStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(ownerStatement);
        return false;
    }

    const int courseID =
        sqlite3_column_int(ownerStatement, 0);

    sqlite3_finalize(ownerStatement);

    sqlite3_stmt *duplicateStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT 1 FROM assignments "
            "WHERE course_id = ? "
            "AND lower(trim(name)) = lower(trim(?)) "
            "AND id != ? LIMIT 1;",
            -1,
            &duplicateStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        duplicateStatement,
        1,
        courseID
    );
    sqlite3_bind_text(
        duplicateStatement,
        2,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        duplicateStatement,
        3,
        assignmentID
    );

    const bool duplicateExists =
        sqlite3_step(duplicateStatement) == SQLITE_ROW;

    sqlite3_finalize(duplicateStatement);

    if (duplicateExists)
    {
        return false;
    }

    const int otherWeight =
        assignmentWeightTotal(
            database,
            courseID,
            assignmentID
        );

    if (otherWeight + weight > 100)
    {
        return false;
    }

    const char *sql =
        "UPDATE assignments "
        "SET name = ?, grade = ?, weight = ?, due_date = ? "
        "WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_double(statement, 2, grade);
    sqlite3_bind_int(statement, 3, weight);
    sqlite3_bind_text(
        statement,
        4,
        normalizedDueDate.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 5, assignmentID);

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::deleteAssignment(int assignmentID)
{
    const char *sql =
        "DELETE FROM assignments WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(
        database,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare deleteAssignment statement: "
                  << sqlite3_errmsg(database)
                  << std::endl;
        return false;
    }

    sqlite3_bind_int(statement, 1, assignmentID);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        std::cout << "Failed to delete assignment: "
                  << sqlite3_errmsg(database)
                  << std::endl;

        sqlite3_finalize(statement);
        return false;
    }

    sqlite3_finalize(statement);
    return true;
}

bool DatabaseManager::setAssignmentCompleted(
    int assignmentID,
    bool completed)
{
    if (database == nullptr || assignmentID <= 0)
    {
        return false;
    }

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE assignments "
            "SET completed = ? "
            "WHERE id = ?;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        statement,
        1,
        completed ? 1 : 0
    );
    sqlite3_bind_int(
        statement,
        2,
        assignmentID
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

std::vector<Assignment>
DatabaseManager::loadAssignmentsForCourse(int courseID)
{
    std::vector<Assignment> assignments;

    const char *sql =
        "SELECT id, name, weight, grade, due_date, completed "
        "FROM assignments "
        "WHERE course_id = ? "
        "ORDER BY "
        "completed ASC, "
        "CASE WHEN due_date = '' THEN 1 ELSE 0 END, "
        "due_date ASC, id ASC;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(
        database,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::cout
            << "Failed to prepare loadAssignmentsForCourse statement: "
            << sqlite3_errmsg(database)
            << std::endl;

        return assignments;
    }

    sqlite3_bind_int(statement, 1, courseID);

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        const int assignmentID =
            sqlite3_column_int(statement, 0);

        const unsigned char *nameText =
            sqlite3_column_text(statement, 1);

        const std::string assignmentName =
            nameText
                ? reinterpret_cast<const char *>(nameText)
                : "";

        const int weight =
            sqlite3_column_int(statement, 2);

        const double grade =
            sqlite3_column_double(statement, 3);

        const unsigned char *dueDateText =
            sqlite3_column_text(statement, 4);

        const std::string dueDate =
            dueDateText
                ? reinterpret_cast<const char *>(dueDateText)
                : "";

        const bool completed =
            sqlite3_column_int(statement, 5) != 0;

        assignments.emplace_back(
            assignmentID,
            assignmentName,
            weight,
            grade,
            dueDate,
            completed
        );
    }

    if (result != SQLITE_DONE)
    {
        std::cout << "Error while loading assignments: "
                  << sqlite3_errmsg(database)
                  << std::endl;
    }

    sqlite3_finalize(statement);
    return assignments;
}

// ============================================================
// Account Updates
// ============================================================

User DatabaseManager::loadFullUserByID(
    int userID)
{
    if (database == nullptr || userID <= 0)
    {
        throw std::runtime_error(
            "Invalid user session."
        );
    }

    const char *sql =
        "SELECT username "
        "FROM users "
        "WHERE id = ? "
        "LIMIT 1;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        throw std::runtime_error(
            "Could not prepare the saved-login query."
        );
    }

    sqlite3_bind_int(statement, 1, userID);

    std::string username;

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char *text =
            sqlite3_column_text(statement, 0);

        if (text != nullptr)
        {
            username =
                reinterpret_cast<const char *>(text);
        }
    }

    sqlite3_finalize(statement);

    if (username.empty())
    {
        throw std::runtime_error(
            "The saved account no longer exists."
        );
    }

    return loadFullUserByUsername(username);
}

bool DatabaseManager::updateUsername(
    int userID,
    const std::string &newUsername)
{
    if (database == nullptr || userID <= 0)
    {
        return false;
    }

    const std::string normalizedUsername =
        trimCopy(newUsername);

    if (!isValidUsername(normalizedUsername))
    {
        return false;
    }

    sqlite3_stmt *duplicateStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT 1 FROM users "
            "WHERE lower(username) = lower(?) "
            "AND id != ? LIMIT 1;",
            -1,
            &duplicateStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        duplicateStatement,
        1,
        normalizedUsername.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        duplicateStatement,
        2,
        userID
    );

    const bool duplicateExists =
        sqlite3_step(duplicateStatement) == SQLITE_ROW;

    sqlite3_finalize(duplicateStatement);

    if (duplicateExists)
    {
        return false;
    }

    const char *sql =
        "UPDATE users SET username = ? WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedUsername.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 2, userID);

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::updateUserProfile(
    int userID,
    const std::string &newUsername,
    int maxCredits)
{
    if (database == nullptr ||
        userID <= 0 ||
        maxCredits < 1 ||
        maxCredits > 1000)
    {
        return false;
    }

    const std::string normalizedUsername =
        trimCopy(newUsername);

    if (!isValidUsername(normalizedUsername))
    {
        return false;
    }

    sqlite3_stmt *creditStatement = nullptr;

    const char *creditSql =
        "SELECT COALESCE(SUM(c.credits), 0) "
        "FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE s.user_id = ?;";

    if (sqlite3_prepare_v2(
            database,
            creditSql,
            -1,
            &creditStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        creditStatement,
        1,
        userID
    );

    int registeredCredits = 0;

    if (sqlite3_step(creditStatement) == SQLITE_ROW)
    {
        registeredCredits =
            sqlite3_column_int(
                creditStatement,
                0
            );
    }
    else
    {
        sqlite3_finalize(creditStatement);
        return false;
    }

    sqlite3_finalize(creditStatement);

    if (maxCredits < registeredCredits)
    {
        return false;
    }

    sqlite3_stmt *duplicateStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT 1 FROM users "
            "WHERE lower(username) = lower(?) "
            "AND id != ? LIMIT 1;",
            -1,
            &duplicateStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        duplicateStatement,
        1,
        normalizedUsername.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        duplicateStatement,
        2,
        userID
    );

    const bool duplicateExists =
        sqlite3_step(duplicateStatement) ==
        SQLITE_ROW;

    sqlite3_finalize(duplicateStatement);

    if (duplicateExists)
    {
        return false;
    }

    sqlite3_stmt *statement = nullptr;

    const char *sql =
        "UPDATE users "
        "SET username = ?, max_credits = ? "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedUsername.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        statement,
        2,
        maxCredits
    );

    sqlite3_bind_int(
        statement,
        3,
        userID
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::updatePasswordHash(
    int userID,
    const std::string &newPasswordHash)
{
    if (database == nullptr || userID <= 0 || newPasswordHash.empty())
    {
        return false;
    }

    const char *sql =
        "UPDATE users "
        "SET password_hash = ? "
        "WHERE id = ?;";

    sqlite3_stmt *statement = nullptr;

    int result = sqlite3_prepare_v2(
        database,
        sql,
        -1,
        &statement,
        nullptr);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare password update: "
                  << sqlite3_errmsg(database)
                  << std::endl;
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        newPasswordHash.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_int(statement, 2, userID);

    result = sqlite3_step(statement);

    bool success =
        result == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    if (!success)
    {
        std::cout << "Failed to update password: "
                  << sqlite3_errmsg(database)
                  << std::endl;
    }

    sqlite3_finalize(statement);
    return success;
}