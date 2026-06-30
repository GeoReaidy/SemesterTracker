#include "databasemanager.h"

#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

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

bool tableHasColumn(
    sqlite3 *database,
    const char *tableName,
    const char *columnName)
{
    const std::string sql =
        std::string("PRAGMA table_info(") +
        tableName +
        ");";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql.c_str(),
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    bool exists = false;

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char *name =
            sqlite3_column_text(statement, 1);

        if (name != nullptr &&
            std::string(
                reinterpret_cast<const char *>(name)
            ) == columnName)
        {
            exists = true;
            break;
        }
    }

    sqlite3_finalize(statement);
    return exists;
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


std::string normalizeEmail(const std::string &value)
{
    std::string email = trimCopy(value);

    std::transform(
        email.begin(),
        email.end(),
        email.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::tolower(character)
            );
        }
    );

    return email;
}

bool isValidEmail(const std::string &value)
{
    const std::string email =
        normalizeEmail(value);

    if (email.size() < 5 ||
        email.size() > 254)
    {
        return false;
    }

    const std::size_t at =
        email.find('@');

    if (at == std::string::npos ||
        at == 0 ||
        at + 1 >= email.size() ||
        email.find('@', at + 1) !=
            std::string::npos)
    {
        return false;
    }

    const std::string domain =
        email.substr(at + 1);

    return domain.find('.') !=
               std::string::npos &&
           domain.front() != '.' &&
           domain.back() != '.';
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

bool courseIsHistoricalAttempt(
    sqlite3 *database,
    int courseID)
{
    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT excluded_from_cgpa "
            "FROM courses WHERE id = ?;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return true;
    }

    sqlite3_bind_int(statement, 1, courseID);

    const bool historical =
        sqlite3_step(statement) != SQLITE_ROW ||
        sqlite3_column_int(statement, 0) == 1;

    sqlite3_finalize(statement);
    return historical;
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
            "WHERE a.course_id = ? "
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
    : database(nullptr),
      currentDatabasePath()
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

    currentDatabasePath = databasePath;

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


namespace
{
std::string sqliteErrorText(sqlite3 *db, const std::string &fallback)
{
    if (db != nullptr)
    {
        const char *message = sqlite3_errmsg(db);
        if (message != nullptr)
        {
            return message;
        }
    }

    return fallback;
}

std::string timestampForBackup()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t rawTime =
        std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &rawTime);
#else
    localtime_r(&rawTime, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(
        &localTime,
        "%Y-%m-%d_%H-%M-%S"
    );

    return stream.str();
}

bool copyDatabaseWithBackupApi(
    sqlite3 *destination,
    sqlite3 *source,
    std::string *errorMessage)
{
    sqlite3_backup *backup =
        sqlite3_backup_init(
            destination,
            "main",
            source,
            "main"
        );

    if (backup == nullptr)
    {
        if (errorMessage)
        {
            *errorMessage =
                sqliteErrorText(
                    destination,
                    "Could not initialize the SQLite backup."
                );
        }

        return false;
    }

    int result = SQLITE_OK;

    do
    {
        result = sqlite3_backup_step(backup, 128);

        if (result == SQLITE_BUSY ||
            result == SQLITE_LOCKED)
        {
            sqlite3_sleep(25);
        }
    }
    while (result == SQLITE_OK ||
           result == SQLITE_BUSY ||
           result == SQLITE_LOCKED);

    const int finishResult =
        sqlite3_backup_finish(backup);

    if (result != SQLITE_DONE ||
        finishResult != SQLITE_OK)
    {
        if (errorMessage)
        {
            *errorMessage =
                sqliteErrorText(
                    destination,
                    "The SQLite backup operation failed."
                );
        }

        return false;
    }

    return true;
}
}

bool DatabaseManager::validateDatabaseFile(
    const std::string &path,
    std::string *errorMessage) const
{
    if (path.empty() ||
        !std::filesystem::exists(path))
    {
        if (errorMessage)
        {
            *errorMessage =
                "The selected database file does not exist.";
        }

        return false;
    }

    sqlite3 *candidate = nullptr;

    const int openResult =
        sqlite3_open_v2(
            path.c_str(),
            &candidate,
            SQLITE_OPEN_READONLY,
            nullptr
        );

    if (openResult != SQLITE_OK)
    {
        if (errorMessage)
        {
            *errorMessage =
                sqliteErrorText(
                    candidate,
                    "The selected file is not a readable SQLite database."
                );
        }

        if (candidate)
        {
            sqlite3_close(candidate);
        }

        return false;
    }

    sqlite3_stmt *statement = nullptr;

    const int prepareResult =
        sqlite3_prepare_v2(
            candidate,
            "PRAGMA quick_check;",
            -1,
            &statement,
            nullptr
        );

    bool valid = false;

    if (prepareResult == SQLITE_OK &&
        sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char *resultText =
            sqlite3_column_text(statement, 0);

        valid =
            resultText != nullptr &&
            std::string(
                reinterpret_cast<const char *>(
                    resultText
                )
            ) == "ok";
    }

    if (statement)
    {
        sqlite3_finalize(statement);
    }

    if (!valid && errorMessage)
    {
        *errorMessage =
            "The selected database failed SQLite's integrity check.";
    }

    sqlite3_close(candidate);
    return valid;
}

bool DatabaseManager::backupDatabase(
    const std::string &destinationPath,
    std::string *errorMessage)
{
    if (database == nullptr)
    {
        if (errorMessage)
        {
            *errorMessage =
                "The application database is not open.";
        }

        return false;
    }

    if (destinationPath.empty())
    {
        if (errorMessage)
        {
            *errorMessage =
                "No backup destination was selected.";
        }

        return false;
    }

    sqlite3 *destination = nullptr;

    const int openResult =
        sqlite3_open(
            destinationPath.c_str(),
            &destination
        );

    if (openResult != SQLITE_OK)
    {
        if (errorMessage)
        {
            *errorMessage =
                sqliteErrorText(
                    destination,
                    "Could not create the backup file."
                );
        }

        if (destination)
        {
            sqlite3_close(destination);
        }

        return false;
    }

    const bool copied =
        copyDatabaseWithBackupApi(
            destination,
            database,
            errorMessage
        );

    sqlite3_close(destination);

    if (!copied)
    {
        std::error_code removeError;
        std::filesystem::remove(
            destinationPath,
            removeError
        );
    }

    return copied;
}

bool DatabaseManager::restoreDatabase(
    const std::string &sourcePath,
    std::string *safetyBackupPath,
    std::string *errorMessage)
{
    if (database == nullptr ||
        currentDatabasePath.empty())
    {
        if (errorMessage)
        {
            *errorMessage =
                "The application database is not open.";
        }

        return false;
    }

    std::string validationError;

    if (!validateDatabaseFile(
            sourcePath,
            &validationError))
    {
        if (errorMessage)
        {
            *errorMessage = validationError;
        }

        return false;
    }

    const std::filesystem::path activePath(
        currentDatabasePath
    );

    const std::filesystem::path backupPath =
        activePath.parent_path() /
        (
            activePath.stem().string() +
            "_before_restore_" +
            timestampForBackup() +
            activePath.extension().string() +
            ".bak"
        );

    std::string backupError;

    if (!backupDatabase(
            backupPath.string(),
            &backupError))
    {
        if (errorMessage)
        {
            *errorMessage =
                "A safety backup could not be created: " +
                backupError;
        }

        return false;
    }

    sqlite3 *source = nullptr;

    const int openResult =
        sqlite3_open_v2(
            sourcePath.c_str(),
            &source,
            SQLITE_OPEN_READONLY,
            nullptr
        );

    if (openResult != SQLITE_OK)
    {
        if (errorMessage)
        {
            *errorMessage =
                sqliteErrorText(
                    source,
                    "Could not open the selected backup."
                );
        }

        if (source)
        {
            sqlite3_close(source);
        }

        return false;
    }

    std::string restoreError;

    const bool restored =
        copyDatabaseWithBackupApi(
            database,
            source,
            &restoreError
        );

    sqlite3_close(source);

    if (!restored)
    {
        sqlite3 *safetySource = nullptr;

        if (sqlite3_open_v2(
                backupPath.string().c_str(),
                &safetySource,
                SQLITE_OPEN_READONLY,
                nullptr) == SQLITE_OK)
        {
            std::string rollbackError;
            copyDatabaseWithBackupApi(
                database,
                safetySource,
                &rollbackError
            );
        }

        if (safetySource)
        {
            sqlite3_close(safetySource);
        }

        if (errorMessage)
        {
            *errorMessage =
                "Restore failed and the previous database was recovered. " +
                restoreError;
        }

        return false;
    }

    if (!createTables())
    {
        if (errorMessage)
        {
            *errorMessage =
                "The backup was restored, but its schema could not be prepared.";
        }

        return false;
    }

    if (safetyBackupPath)
    {
        *safetyBackupPath =
            backupPath.string();
    }

    return true;
}

const std::string &DatabaseManager::databasePath() const
{
    return currentDatabasePath;
}



namespace
{
QString sqliteText(sqlite3_stmt *statement, int column)
{
    const unsigned char *text =
        sqlite3_column_text(statement, column);

    return text
        ? QString::fromUtf8(
              reinterpret_cast<const char *>(text)
          )
        : QString();
}

bool executeSql(
    sqlite3 *database,
    const char *sql,
    std::string *errorMessage = nullptr)
{
    char *sqliteError = nullptr;

    const int result =
        sqlite3_exec(
            database,
            sql,
            nullptr,
            nullptr,
            &sqliteError
        );

    if (result != SQLITE_OK)
    {
        if (errorMessage)
        {
            *errorMessage =
                sqliteError
                    ? sqliteError
                    : "SQLite operation failed.";
        }

        sqlite3_free(sqliteError);
        return false;
    }

    return true;
}

bool prepareStatement(
    sqlite3 *database,
    const char *sql,
    sqlite3_stmt **statement,
    std::string *errorMessage)
{
    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            statement,
            nullptr) == SQLITE_OK)
    {
        return true;
    }

    if (errorMessage)
    {
        *errorMessage = sqlite3_errmsg(database);
    }

    return false;
}

bool bindText(
    sqlite3_stmt *statement,
    int index,
    const QString &value)
{
    const QByteArray utf8 = value.toUtf8();

    return sqlite3_bind_text(
               statement,
               index,
               utf8.constData(),
               utf8.size(),
               SQLITE_TRANSIENT
           ) == SQLITE_OK;
}

bool readUserDataDocument(
    const std::string &sourcePath,
    QJsonObject *root,
    std::string *errorMessage)
{
    QFile file(
        QString::fromStdString(sourcePath)
    );

    if (!file.open(QIODevice::ReadOnly))
    {
        if (errorMessage)
        {
            *errorMessage =
                "The selected user-data file could not be opened.";
        }

        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(
            file.readAll(),
            &parseError
        );

    if (parseError.error !=
            QJsonParseError::NoError ||
        !document.isObject())
    {
        if (errorMessage)
        {
            *errorMessage =
                "The selected file is not valid SemesterTracker JSON: " +
                parseError.errorString().toStdString();
        }

        return false;
    }

    const QJsonObject object =
        document.object();

    if (object.value("format").toString() !=
            "SemesterTrackerUserData")
    {
        if (errorMessage)
        {
            *errorMessage =
                "The selected file is not a SemesterTracker user-data export.";
        }

        return false;
    }

    const int formatVersion =
        object.value("formatVersion").toInt(-1);

    if (formatVersion != 1)
    {
        if (errorMessage)
        {
            *errorMessage =
                "This user-data format version is not supported.";
        }

        return false;
    }

    if (!object.value("user").isObject() ||
        !object.value("semesters").isArray() ||
        !object.value("categories").isArray())
    {
        if (errorMessage)
        {
            *errorMessage =
                "The user-data file is missing required sections.";
        }

        return false;
    }

    if (root)
    {
        *root = object;
    }

    return true;
}

int countNestedItems(
    const QJsonArray &semesters,
    const QString &childName,
    const QString &grandchildName = QString())
{
    int count = 0;

    for (const QJsonValue &semesterValue :
         semesters)
    {
        const QJsonObject semester =
            semesterValue.toObject();

        const QJsonArray children =
            semester.value(childName).toArray();

        if (grandchildName.isEmpty())
        {
            count += children.size();
            continue;
        }

        for (const QJsonValue &childValue :
             children)
        {
            count +=
                childValue
                    .toObject()
                    .value(grandchildName)
                    .toArray()
                    .size();
        }
    }

    return count;
}
}

bool DatabaseManager::exportUserData(
    int userID,
    const std::string &destinationPath,
    std::string *errorMessage) const
{
    if (database == nullptr || userID < 0)
    {
        if (errorMessage)
        {
            *errorMessage =
                "No signed-in user is available to export.";
        }

        return false;
    }

    QJsonObject root;
    root["format"] = "SemesterTrackerUserData";
    root["formatVersion"] = 1;
    root["appVersion"] = QStringLiteral(APP_VERSION);
    root["exportedAt"] =
        QDateTime::currentDateTimeUtc().toString(
            Qt::ISODate
        );

    sqlite3_stmt *userStatement = nullptr;

    if (!prepareStatement(
            database,
            "SELECT username, email, max_credits "
            "FROM users WHERE id = ?;",
            &userStatement,
            errorMessage))
    {
        return false;
    }

    sqlite3_bind_int(
        userStatement,
        1,
        userID
    );

    if (sqlite3_step(userStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(userStatement);

        if (errorMessage)
        {
            *errorMessage =
                "The current user could not be found.";
        }

        return false;
    }

    QJsonObject userObject;
    userObject["username"] =
        sqliteText(userStatement, 0);
    userObject["email"] =
        sqliteText(userStatement, 1);
    userObject["maxCredits"] =
        sqlite3_column_int(userStatement, 2);

    sqlite3_finalize(userStatement);
    root["user"] = userObject;

    QJsonArray categories;
    sqlite3_stmt *categoryStatement = nullptr;

    if (!prepareStatement(
            database,
            "SELECT name, is_builtin "
            "FROM assignment_categories "
            "WHERE user_id = ? "
            "ORDER BY is_builtin DESC, name COLLATE NOCASE;",
            &categoryStatement,
            errorMessage))
    {
        return false;
    }

    sqlite3_bind_int(
        categoryStatement,
        1,
        userID
    );

    while (sqlite3_step(categoryStatement) ==
           SQLITE_ROW)
    {
        QJsonObject category;
        category["name"] =
            sqliteText(categoryStatement, 0);
        category["builtIn"] =
            sqlite3_column_int(
                categoryStatement,
                1
            ) != 0;
        categories.append(category);
    }

    sqlite3_finalize(categoryStatement);
    root["categories"] = categories;

    QJsonArray semesters;
    sqlite3_stmt *semesterStatement = nullptr;

    if (!prepareStatement(
            database,
            "SELECT id, name, year, in_progress, status, "
            "summary_only, summary_credits, summary_gpa "
            "FROM semesters WHERE user_id = ? "
            "ORDER BY year, id;",
            &semesterStatement,
            errorMessage))
    {
        return false;
    }

    sqlite3_bind_int(
        semesterStatement,
        1,
        userID
    );

    while (sqlite3_step(semesterStatement) ==
           SQLITE_ROW)
    {
        const int semesterID =
            sqlite3_column_int(
                semesterStatement,
                0
            );

        QJsonObject semester;
        semester["name"] =
            sqliteText(semesterStatement, 1);
        semester["year"] =
            sqlite3_column_int(
                semesterStatement,
                2
            );
        semester["inProgress"] =
            sqlite3_column_int(
                semesterStatement,
                3
            ) != 0;
        semester["status"] =
            sqliteText(semesterStatement, 4);
        semester["summaryOnly"] =
            sqlite3_column_int(
                semesterStatement,
                5
            ) != 0;
        semester["summaryCredits"] =
            sqlite3_column_int(
                semesterStatement,
                6
            );
        semester["summaryGpa"] =
            sqlite3_column_double(
                semesterStatement,
                7
            );

        QJsonArray courses;
        sqlite3_stmt *courseStatement = nullptr;

        if (!prepareStatement(
                database,
                "SELECT id, course_code, course_name, credits, "
                "status, retaken, excluded_from_cgpa, target_grade "
                "FROM courses WHERE semester_id = ? "
                "ORDER BY id;",
                &courseStatement,
                errorMessage))
        {
            sqlite3_finalize(semesterStatement);
            return false;
        }

        sqlite3_bind_int(
            courseStatement,
            1,
            semesterID
        );

        while (sqlite3_step(courseStatement) ==
               SQLITE_ROW)
        {
            const int courseID =
                sqlite3_column_int(
                    courseStatement,
                    0
                );

            QJsonObject course;
            course["code"] =
                sqliteText(courseStatement, 1);
            course["name"] =
                sqliteText(courseStatement, 2);
            course["credits"] =
                sqlite3_column_int(
                    courseStatement,
                    3
                );
            course["status"] =
                sqliteText(courseStatement, 4);
            course["retaken"] =
                sqlite3_column_int(
                    courseStatement,
                    5
                ) != 0;
            course["excludedFromCgpa"] =
                sqlite3_column_int(
                    courseStatement,
                    6
                ) != 0;
            course["targetGrade"] =
                sqlite3_column_double(
                    courseStatement,
                    7
                );

            QJsonArray assignments;
            sqlite3_stmt *assignmentStatement =
                nullptr;

            if (!prepareStatement(
                    database,
                    "SELECT a.name, a.grade, a.weight, "
                    "a.due_date, a.completed, a.projected_grade, "
                    "COALESCE(c.name, 'Other') "
                    "FROM assignments a "
                    "LEFT JOIN assignment_categories c "
                    "ON c.id = a.category_id "
                    "WHERE a.course_id = ? "
                    "ORDER BY a.id;",
                    &assignmentStatement,
                    errorMessage))
            {
                sqlite3_finalize(
                    courseStatement
                );
                sqlite3_finalize(
                    semesterStatement
                );
                return false;
            }

            sqlite3_bind_int(
                assignmentStatement,
                1,
                courseID
            );

            while (sqlite3_step(
                       assignmentStatement) ==
                   SQLITE_ROW)
            {
                QJsonObject assignment;
                assignment["name"] =
                    sqliteText(
                        assignmentStatement,
                        0
                    );
                assignment["grade"] =
                    sqlite3_column_double(
                        assignmentStatement,
                        1
                    );
                assignment["weight"] =
                    sqlite3_column_int(
                        assignmentStatement,
                        2
                    );
                assignment["dueDate"] =
                    sqliteText(
                        assignmentStatement,
                        3
                    );
                assignment["completed"] =
                    sqlite3_column_int(
                        assignmentStatement,
                        4
                    ) != 0;
                assignment["projectedGrade"] =
                    sqlite3_column_double(
                        assignmentStatement,
                        5
                    );
                assignment["category"] =
                    sqliteText(
                        assignmentStatement,
                        6
                    );

                assignments.append(assignment);
            }

            sqlite3_finalize(
                assignmentStatement
            );

            course["assignments"] =
                assignments;
            courses.append(course);
        }

        sqlite3_finalize(courseStatement);
        semester["courses"] = courses;
        semesters.append(semester);
    }

    sqlite3_finalize(semesterStatement);
    root["semesters"] = semesters;

    QSaveFile file(
        QString::fromStdString(
            destinationPath
        )
    );

    if (!file.open(QIODevice::WriteOnly))
    {
        if (errorMessage)
        {
            *errorMessage =
                "The export file could not be created.";
        }

        return false;
    }

    const QJsonDocument document(root);

    if (file.write(
            document.toJson(
                QJsonDocument::Indented
            )) < 0 ||
        !file.commit())
    {
        if (errorMessage)
        {
            *errorMessage =
                "The export file could not be saved.";
        }

        return false;
    }

    return true;
}

bool DatabaseManager::inspectUserDataFile(
    const std::string &sourcePath,
    UserDataImportSummary *summary,
    std::string *errorMessage) const
{
    QJsonObject root;

    if (!readUserDataDocument(
            sourcePath,
            &root,
            errorMessage))
    {
        return false;
    }

    if (summary)
    {
        const QJsonArray semesters =
            root.value("semesters").toArray();

        summary->semesters =
            semesters.size();
        summary->courses =
            countNestedItems(
                semesters,
                "courses"
            );
        summary->assignments =
            countNestedItems(
                semesters,
                "courses",
                "assignments"
            );
        summary->categories =
            root.value("categories")
                .toArray()
                .size();
    }

    return true;
}

bool DatabaseManager::importUserData(
    int userID,
    const std::string &sourcePath,
    UserDataImportMode mode,
    UserDataImportSummary *summary,
    std::string *errorMessage)
{
    if (database == nullptr || userID < 0)
    {
        if (errorMessage)
        {
            *errorMessage =
                "No signed-in user is available for import.";
        }

        return false;
    }

    QJsonObject root;

    if (!readUserDataDocument(
            sourcePath,
            &root,
            errorMessage))
    {
        return false;
    }

    UserDataImportSummary imported;

    if (!executeSql(
            database,
            "BEGIN IMMEDIATE TRANSACTION;",
            errorMessage))
    {
        return false;
    }

    auto rollback = [&]()
    {
        executeSql(
            database,
            "ROLLBACK;"
        );
    };

    if (mode == UserDataImportMode::Replace)
    {
        sqlite3_stmt *deleteSemesters = nullptr;

        if (!prepareStatement(
                database,
                "DELETE FROM semesters "
                "WHERE user_id = ?;",
                &deleteSemesters,
                errorMessage))
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            deleteSemesters,
            1,
            userID
        );

        const bool deleted =
            sqlite3_step(deleteSemesters) ==
            SQLITE_DONE;

        sqlite3_finalize(deleteSemesters);

        if (!deleted)
        {
            if (errorMessage)
            {
                *errorMessage =
                    sqlite3_errmsg(database);
            }

            rollback();
            return false;
        }

        sqlite3_stmt *deleteCategories =
            nullptr;

        if (!prepareStatement(
                database,
                "DELETE FROM assignment_categories "
                "WHERE user_id = ? "
                "AND is_builtin = 0;",
                &deleteCategories,
                errorMessage))
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            deleteCategories,
            1,
            userID
        );

        if (sqlite3_step(deleteCategories) !=
            SQLITE_DONE)
        {
            if (errorMessage)
            {
                *errorMessage =
                    sqlite3_errmsg(database);
            }

            sqlite3_finalize(
                deleteCategories
            );
            rollback();
            return false;
        }

        sqlite3_finalize(deleteCategories);
    }

    const QJsonObject user =
        root.value("user").toObject();

    sqlite3_stmt *profileStatement = nullptr;

    if (!prepareStatement(
            database,
            "UPDATE users SET max_credits = ? "
            "WHERE id = ?;",
            &profileStatement,
            errorMessage))
    {
        rollback();
        return false;
    }

    sqlite3_bind_int(
        profileStatement,
        1,
        qMax(
            1,
            user.value("maxCredits")
                .toInt(120)
        )
    );
    sqlite3_bind_int(
        profileStatement,
        2,
        userID
    );

    if (sqlite3_step(profileStatement) !=
        SQLITE_DONE)
    {
        if (errorMessage)
        {
            *errorMessage =
                sqlite3_errmsg(database);
        }

        sqlite3_finalize(profileStatement);
        rollback();
        return false;
    }

    sqlite3_finalize(profileStatement);

    const QJsonArray categoryArray =
        root.value("categories").toArray();

    for (const QJsonValue &categoryValue :
         categoryArray)
    {
        const QJsonObject category =
            categoryValue.toObject();

        const QString name =
            category.value("name")
                .toString()
                .trimmed();

        if (name.isEmpty())
        {
            continue;
        }

        sqlite3_stmt *categoryInsert = nullptr;

        if (!prepareStatement(
                database,
                "INSERT OR IGNORE INTO assignment_categories "
                "(user_id, name, is_builtin) "
                "VALUES (?, ?, ?);",
                &categoryInsert,
                errorMessage))
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            categoryInsert,
            1,
            userID
        );
        bindText(
            categoryInsert,
            2,
            name
        );
        sqlite3_bind_int(
            categoryInsert,
            3,
            category.value("builtIn")
                .toBool(false)
                ? 1
                : 0
        );

        if (sqlite3_step(categoryInsert) !=
            SQLITE_DONE)
        {
            if (errorMessage)
            {
                *errorMessage =
                    sqlite3_errmsg(database);
            }

            sqlite3_finalize(
                categoryInsert
            );
            rollback();
            return false;
        }

        if (sqlite3_changes(database) > 0)
        {
            ++imported.categories;
        }

        sqlite3_finalize(categoryInsert);
    }

    const QJsonArray semesterArray =
        root.value("semesters").toArray();

    for (const QJsonValue &semesterValue :
         semesterArray)
    {
        const QJsonObject semester =
            semesterValue.toObject();

        const QString semesterName =
            semester.value("name")
                .toString()
                .trimmed();

        if (semesterName.isEmpty())
        {
            continue;
        }

        sqlite3_stmt *semesterInsert = nullptr;

        if (!prepareStatement(
                database,
                "INSERT INTO semesters "
                "(user_id, name, year, in_progress, status, "
                "summary_only, summary_credits, summary_gpa) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?);",
                &semesterInsert,
                errorMessage))
        {
            rollback();
            return false;
        }

        sqlite3_bind_int(
            semesterInsert,
            1,
            userID
        );
        bindText(
            semesterInsert,
            2,
            semesterName
        );
        sqlite3_bind_int(
            semesterInsert,
            3,
            semester.value("year")
                .toInt()
        );
        sqlite3_bind_int(
            semesterInsert,
            4,
            semester.value("inProgress")
                .toBool(false)
                ? 1
                : 0
        );
        bindText(
            semesterInsert,
            5,
            semester.value("status")
                .toString("planned")
        );
        sqlite3_bind_int(
            semesterInsert,
            6,
            semester.value("summaryOnly")
                .toBool(false)
                ? 1
                : 0
        );
        sqlite3_bind_int(
            semesterInsert,
            7,
            semester.value("summaryCredits")
                .toInt()
        );
        sqlite3_bind_double(
            semesterInsert,
            8,
            semester.value("summaryGpa")
                .toDouble()
        );

        if (sqlite3_step(semesterInsert) !=
            SQLITE_DONE)
        {
            if (errorMessage)
            {
                *errorMessage =
                    sqlite3_errmsg(database);
            }

            sqlite3_finalize(
                semesterInsert
            );
            rollback();
            return false;
        }

        sqlite3_finalize(semesterInsert);
        const int newSemesterID =
            static_cast<int>(
                sqlite3_last_insert_rowid(
                    database
                )
            );
        ++imported.semesters;

        const QJsonArray courseArray =
            semester.value("courses")
                .toArray();

        for (const QJsonValue &courseValue :
             courseArray)
        {
            const QJsonObject course =
                courseValue.toObject();

            const QString code =
                course.value("code")
                    .toString()
                    .trimmed();
            const QString name =
                course.value("name")
                    .toString()
                    .trimmed();

            if (code.isEmpty() &&
                name.isEmpty())
            {
                continue;
            }

            sqlite3_stmt *courseInsert =
                nullptr;

            if (!prepareStatement(
                    database,
                    "INSERT INTO courses "
                    "(semester_id, course_code, course_name, credits, "
                    "status, retaken, excluded_from_cgpa, target_grade) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?);",
                    &courseInsert,
                    errorMessage))
            {
                rollback();
                return false;
            }

            sqlite3_bind_int(
                courseInsert,
                1,
                newSemesterID
            );
            bindText(courseInsert, 2, code);
            bindText(courseInsert, 3, name);
            sqlite3_bind_int(
                courseInsert,
                4,
                qMax(
                    0,
                    course.value("credits")
                        .toInt()
                )
            );
            bindText(
                courseInsert,
                5,
                course.value("status")
                    .toString("planned")
            );
            sqlite3_bind_int(
                courseInsert,
                6,
                course.value("retaken")
                    .toBool(false)
                    ? 1
                    : 0
            );
            sqlite3_bind_int(
                courseInsert,
                7,
                course.value(
                    "excludedFromCgpa"
                ).toBool(false)
                    ? 1
                    : 0
            );
            sqlite3_bind_double(
                courseInsert,
                8,
                course.value("targetGrade")
                    .toDouble(-1.0)
            );

            if (sqlite3_step(courseInsert) !=
                SQLITE_DONE)
            {
                if (errorMessage)
                {
                    *errorMessage =
                        sqlite3_errmsg(database);
                }

                sqlite3_finalize(
                    courseInsert
                );
                rollback();
                return false;
            }

            sqlite3_finalize(courseInsert);
            const int newCourseID =
                static_cast<int>(
                    sqlite3_last_insert_rowid(
                        database
                    )
                );
            ++imported.courses;

            const QJsonArray assignmentArray =
                course.value("assignments")
                    .toArray();

            for (const QJsonValue &assignmentValue :
                 assignmentArray)
            {
                const QJsonObject assignment =
                    assignmentValue.toObject();

                const QString assignmentName =
                    assignment.value("name")
                        .toString()
                        .trimmed();

                if (assignmentName.isEmpty())
                {
                    continue;
                }

                const QString categoryName =
                    assignment.value("category")
                        .toString("Other")
                        .trimmed();

                sqlite3_stmt *categoryLookup =
                    nullptr;

                if (!prepareStatement(
                        database,
                        "INSERT OR IGNORE INTO assignment_categories "
                        "(user_id, name, is_builtin) VALUES (?, ?, 0);",
                        &categoryLookup,
                        errorMessage))
                {
                    rollback();
                    return false;
                }

                sqlite3_bind_int(
                    categoryLookup,
                    1,
                    userID
                );
                bindText(
                    categoryLookup,
                    2,
                    categoryName.isEmpty()
                        ? QString("Other")
                        : categoryName
                );

                if (sqlite3_step(categoryLookup) !=
                    SQLITE_DONE)
                {
                    if (errorMessage)
                    {
                        *errorMessage =
                            sqlite3_errmsg(
                                database
                            );
                    }

                    sqlite3_finalize(
                        categoryLookup
                    );
                    rollback();
                    return false;
                }

                sqlite3_finalize(categoryLookup);

                sqlite3_stmt *categoryIDStatement =
                    nullptr;

                if (!prepareStatement(
                        database,
                        "SELECT id FROM assignment_categories "
                        "WHERE user_id = ? AND name = ?;",
                        &categoryIDStatement,
                        errorMessage))
                {
                    rollback();
                    return false;
                }

                sqlite3_bind_int(
                    categoryIDStatement,
                    1,
                    userID
                );
                bindText(
                    categoryIDStatement,
                    2,
                    categoryName.isEmpty()
                        ? QString("Other")
                        : categoryName
                );

                int categoryID = -1;

                if (sqlite3_step(
                        categoryIDStatement) ==
                    SQLITE_ROW)
                {
                    categoryID =
                        sqlite3_column_int(
                            categoryIDStatement,
                            0
                        );
                }

                sqlite3_finalize(
                    categoryIDStatement
                );

                sqlite3_stmt *assignmentInsert =
                    nullptr;

                if (!prepareStatement(
                        database,
                        "INSERT INTO assignments "
                        "(course_id, name, grade, weight, due_date, "
                        "completed, projected_grade, category_id) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);",
                        &assignmentInsert,
                        errorMessage))
                {
                    rollback();
                    return false;
                }

                sqlite3_bind_int(
                    assignmentInsert,
                    1,
                    newCourseID
                );
                bindText(
                    assignmentInsert,
                    2,
                    assignmentName
                );
                sqlite3_bind_double(
                    assignmentInsert,
                    3,
                    assignment.value("grade")
                        .toDouble(-1.0)
                );
                sqlite3_bind_int(
                    assignmentInsert,
                    4,
                    qBound(
                        0,
                        assignment.value("weight")
                            .toInt(),
                        100
                    )
                );
                bindText(
                    assignmentInsert,
                    5,
                    assignment.value("dueDate")
                        .toString()
                );
                sqlite3_bind_int(
                    assignmentInsert,
                    6,
                    assignment.value("completed")
                        .toBool(false)
                        ? 1
                        : 0
                );
                sqlite3_bind_double(
                    assignmentInsert,
                    7,
                    assignment.value(
                        "projectedGrade"
                    ).toDouble(-1.0)
                );

                if (categoryID >= 0)
                {
                    sqlite3_bind_int(
                        assignmentInsert,
                        8,
                        categoryID
                    );
                }
                else
                {
                    sqlite3_bind_null(
                        assignmentInsert,
                        8
                    );
                }

                if (sqlite3_step(assignmentInsert) !=
                    SQLITE_DONE)
                {
                    if (errorMessage)
                    {
                        *errorMessage =
                            sqlite3_errmsg(
                                database
                            );
                    }

                    sqlite3_finalize(
                        assignmentInsert
                    );
                    rollback();
                    return false;
                }

                sqlite3_finalize(
                    assignmentInsert
                );
                ++imported.assignments;
            }
        }
    }

    if (!executeSql(
            database,
            "COMMIT;",
            errorMessage))
    {
        rollback();
        return false;
    }

    if (summary)
    {
        *summary = imported;
    }

    return true;
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
        "email TEXT NOT NULL DEFAULT '', "
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
        "retake_of_course_id INTEGER DEFAULT NULL, "
        "excluded_from_cgpa INTEGER NOT NULL DEFAULT 0, "
        "target_grade REAL NOT NULL DEFAULT -1.0, "
        "FOREIGN KEY (semester_id) REFERENCES semesters(id) ON DELETE CASCADE, "
        "FOREIGN KEY (retake_of_course_id) REFERENCES courses(id) ON DELETE SET NULL"
        ");"

        "CREATE TABLE IF NOT EXISTS assignment_categories ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "name TEXT NOT NULL, "
        "is_builtin INTEGER NOT NULL DEFAULT 0, "
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE, "
        "UNIQUE(user_id, name)"
        ");"

        "CREATE TABLE IF NOT EXISTS assignments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "course_id INTEGER NOT NULL, "
        "name TEXT NOT NULL, "
        "grade REAL NOT NULL, "
        "weight INTEGER NOT NULL, "
        "due_date TEXT NOT NULL DEFAULT '', "
        "completed INTEGER NOT NULL DEFAULT 0, "
        "projected_grade REAL NOT NULL DEFAULT -1.0, "
        "category_id INTEGER DEFAULT NULL, "
        "FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE, "
        "FOREIGN KEY (category_id) REFERENCES assignment_categories(id) ON DELETE SET NULL"
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

    if (!tableHasColumn(
            database,
            "assignments",
            "projected_grade"))
    {
        if (sqlite3_exec(
                database,
                "ALTER TABLE assignments "
                "ADD COLUMN projected_grade "
                "REAL NOT NULL DEFAULT -1.0;",
                nullptr,
                nullptr,
                nullptr) != SQLITE_OK)
        {
            std::cout
                << "Failed to add assignment projected_grade column: "
                << sqlite3_errmsg(database)
                << std::endl;
            return false;
        }
    }

    if (sqlite3_exec(
            database,
            "CREATE TABLE IF NOT EXISTS assignment_categories ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "user_id INTEGER NOT NULL, "
            "name TEXT NOT NULL, "
            "is_builtin INTEGER NOT NULL DEFAULT 0, "
            "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE, "
            "UNIQUE(user_id, name)"
            ");",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        std::cout << "Failed to create assignment categories table: "
                  << sqlite3_errmsg(database)
                  << std::endl;
        return false;
    }

    if (!tableHasColumn(
            database,
            "assignment_categories",
            "is_builtin"))
    {
        if (sqlite3_exec(
                database,
                "ALTER TABLE assignment_categories "
                "ADD COLUMN is_builtin INTEGER NOT NULL DEFAULT 0;",
                nullptr,
                nullptr,
                nullptr) != SQLITE_OK)
        {
            std::cout << "Failed to add assignment category is_builtin column: "
                      << sqlite3_errmsg(database)
                      << std::endl;
            return false;
        }
    }

    if (!tableHasColumn(
            database,
            "assignments",
            "category_id"))
    {
        if (sqlite3_exec(
                database,
                "ALTER TABLE assignments "
                "ADD COLUMN category_id INTEGER DEFAULT NULL;",
                nullptr,
                nullptr,
                nullptr) != SQLITE_OK)
        {
            std::cout << "Failed to add assignment category_id column: "
                      << sqlite3_errmsg(database)
                      << std::endl;
            return false;
        }
    }

    if (!tableHasColumn(
            database,
            "courses",
            "target_grade"))
    {
        if (sqlite3_exec(
                database,
                "ALTER TABLE courses "
                "ADD COLUMN target_grade "
                "REAL NOT NULL DEFAULT -1.0;",
                nullptr,
                nullptr,
                nullptr) != SQLITE_OK)
        {
            std::cout
                << "Failed to add course target_grade column: "
                << sqlite3_errmsg(database)
                << std::endl;
            return false;
        }
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
        },
        {
            "retake_of_course_id",
            "ALTER TABLE courses "
            "ADD COLUMN retake_of_course_id INTEGER DEFAULT NULL;"
        },
        {
            "excluded_from_cgpa",
            "ALTER TABLE courses "
            "ADD COLUMN excluded_from_cgpa INTEGER NOT NULL DEFAULT 0;"
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

    bool emailColumnExists = false;
    sqlite3_stmt *emailColumnStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "PRAGMA table_info(users);",
            -1,
            &emailColumnStatement,
            nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(emailColumnStatement) == SQLITE_ROW)
        {
            const unsigned char *columnName =
                sqlite3_column_text(
                    emailColumnStatement,
                    1
                );

            if (columnName != nullptr &&
                std::string(
                    reinterpret_cast<
                        const char *>(columnName)
                ) == "email")
            {
                emailColumnExists = true;
                break;
            }
        }
    }

    sqlite3_finalize(emailColumnStatement);

    if (!emailColumnExists)
    {
        if (sqlite3_exec(
                database,
                "ALTER TABLE users "
                "ADD COLUMN email TEXT NOT NULL DEFAULT '';",
                nullptr,
                nullptr,
                nullptr) != SQLITE_OK)
        {
            std::cout
                << "Failed to add users.email: "
                << sqlite3_errmsg(database)
                << std::endl;
            return false;
        }
    }

    if (sqlite3_exec(
            database,
            "CREATE UNIQUE INDEX IF NOT EXISTS "
            "idx_users_email_unique "
            "ON users(lower(email)) "
            "WHERE trim(email) <> '';",
            nullptr,
            nullptr,
            nullptr) != SQLITE_OK)
    {
        std::cout
            << "Failed to create the email index: "
            << sqlite3_errmsg(database)
            << std::endl;
        return false;
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
    return addUser(
        username,
        std::string(),
        passwordHash,
        maxCredits
    );
}

bool DatabaseManager::addUser(
    const std::string &username,
    const std::string &email,
    const std::string &passwordHash,
    int maxCredits)
{
    if (database == nullptr)
    {
        return false;
    }

    const std::string normalizedUsername =
        trimCopy(username);

    const std::string normalizedEmail =
        normalizeEmail(email);

    if (!isValidUsername(normalizedUsername) ||
        passwordHash.empty() ||
        maxCredits < 1 ||
        maxCredits > 1000 ||
        userExists(normalizedUsername) ||
        (!normalizedEmail.empty() &&
         (!isValidEmail(normalizedEmail) ||
          emailExists(normalizedEmail))))
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
        "(username, email, password_hash, max_credits) "
        "VALUES (?, ?, ?, ?);";

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
        normalizedEmail.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        3,
        passwordHash.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(statement, 4, maxCredits);

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


bool DatabaseManager::emailExists(
    const std::string &email)
{
    if (database == nullptr)
    {
        return false;
    }

    const std::string normalizedEmail =
        normalizeEmail(email);

    if (!isValidEmail(normalizedEmail))
    {
        return false;
    }

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT 1 FROM users "
            "WHERE lower(email) = lower(?) "
            "LIMIT 1;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedEmail.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    const bool exists =
        sqlite3_step(statement) == SQLITE_ROW;

    sqlite3_finalize(statement);
    return exists;
}

std::string DatabaseManager::getUsernameByEmail(
    const std::string &email)
{
    if (database == nullptr)
    {
        return {};
    }

    const std::string normalizedEmail =
        normalizeEmail(email);

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT username FROM users "
            "WHERE lower(email) = lower(?) "
            "LIMIT 1;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return {};
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedEmail.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    std::string username;

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char *text =
            sqlite3_column_text(statement, 0);

        if (text != nullptr)
        {
            username =
                reinterpret_cast<
                    const char *>(text);
        }
    }

    sqlite3_finalize(statement);
    return username;
}

std::string DatabaseManager::getEmailByUserID(
    int userID)
{
    if (database == nullptr || userID <= 0)
    {
        return {};
    }

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT email FROM users WHERE id = ?;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return {};
    }

    sqlite3_bind_int(statement, 1, userID);

    std::string email;

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char *text =
            sqlite3_column_text(statement, 0);

        if (text != nullptr)
        {
            email =
                reinterpret_cast<
                    const char *>(text);
        }
    }

    sqlite3_finalize(statement);
    return email;
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
        !semesterAcceptsCourses(database, newSemesterID))
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
        std::cout << "Invalid retake course data: "
                  << error.what()
                  << std::endl;
        return false;
    }

    if (hasFinalGrade &&
        (letterGrade.empty() ||
         finalPercentage < 0.0 ||
         finalPercentage > 100.0))
    {
        return false;
    }

    sqlite3_stmt *ownershipStatement = nullptr;

    const char *ownershipSql =
        "SELECT c.semester_id, s.user_id, "
        "c.excluded_from_cgpa "
        "FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE c.id = ?;";

    if (sqlite3_prepare_v2(
            database,
            ownershipSql,
            -1,
            &ownershipStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        ownershipStatement,
        1,
        existingCourseID
    );

    if (sqlite3_step(ownershipStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(ownershipStatement);
        return false;
    }

    const int existingSemesterID =
        sqlite3_column_int(ownershipStatement, 0);

    const int existingUserID =
        sqlite3_column_int(ownershipStatement, 1);

    const bool alreadyHistorical =
        sqlite3_column_int(ownershipStatement, 2) == 1;

    sqlite3_finalize(ownershipStatement);

    if (alreadyHistorical ||
        existingSemesterID == newSemesterID)
    {
        return false;
    }

    sqlite3_stmt *newSemesterStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT user_id FROM semesters WHERE id = ?;",
            -1,
            &newSemesterStatement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        newSemesterStatement,
        1,
        newSemesterID
    );

    const bool newSemesterFound =
        sqlite3_step(newSemesterStatement) == SQLITE_ROW;

    const int newSemesterUserID =
        newSemesterFound
            ? sqlite3_column_int(newSemesterStatement, 0)
            : -1;

    sqlite3_finalize(newSemesterStatement);

    if (!newSemesterFound ||
        newSemesterUserID != existingUserID)
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

    const CourseStatus retakeStatus =
        hasFinalGrade
            ? CourseStatus::Completed
            : defaultCourseStatusForSemester(
                  database,
                  newSemesterID
              );

    const std::string storedRetakeStatus =
        courseStatusToStorage(retakeStatus);

    sqlite3_stmt *insertCourseStatement = nullptr;

    const char *insertSql =
        "INSERT INTO courses "
        "(semester_id, course_code, course_name, credits, "
        "status, retaken, retake_of_course_id, excluded_from_cgpa) "
        "VALUES (?, ?, ?, ?, ?, 1, ?, 0);";

    if (sqlite3_prepare_v2(
            database,
            insertSql,
            -1,
            &insertCourseStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(
        insertCourseStatement,
        1,
        newSemesterID
    );
    sqlite3_bind_text(
        insertCourseStatement,
        2,
        normalizedCode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        insertCourseStatement,
        3,
        normalizedName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        insertCourseStatement,
        4,
        credits
    );
    sqlite3_bind_text(
        insertCourseStatement,
        5,
        storedRetakeStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );
    sqlite3_bind_int(
        insertCourseStatement,
        6,
        existingCourseID
    );

    const bool courseInserted =
        sqlite3_step(insertCourseStatement) == SQLITE_DONE;

    sqlite3_finalize(insertCourseStatement);

    if (!courseInserted)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    const int newCourseID = getLastInsertID();

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
            newCourseID
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
            sqlite3_step(gradeStatement) == SQLITE_DONE;

        sqlite3_finalize(gradeStatement);

        if (!gradeSaved)
        {
            sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
    }

    sqlite3_stmt *excludePreviousStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE courses "
            "SET excluded_from_cgpa = 1 "
            "WHERE id = ?;",
            -1,
            &excludePreviousStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(
        excludePreviousStatement,
        1,
        existingCourseID
    );

    const bool previousExcluded =
        sqlite3_step(excludePreviousStatement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(excludePreviousStatement);

    if (!previousExcluded)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
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
    if (database == nullptr ||
        courseID <= 0 ||
        courseIsHistoricalAttempt(database, courseID))
    {
        return false;
    }

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT semester_id FROM courses WHERE id = ?;",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(statement, 1, courseID);

    const bool found =
        sqlite3_step(statement) == SQLITE_ROW;

    const int semesterID =
        found
            ? sqlite3_column_int(statement, 0)
            : -1;

    sqlite3_finalize(statement);

    if (!found)
    {
        return false;
    }

    return updateCourseDetails(
        courseID,
        semesterID,
        courseCode,
        courseName,
        credits
    );
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
        courseIsHistoricalAttempt(database, courseID) ||
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
        "WITH RECURSIVE retake_chain(id) AS ("
            "SELECT ? "
            "UNION "
            "SELECT c.retake_of_course_id "
            "FROM courses c "
            "JOIN retake_chain r ON c.id = r.id "
            "WHERE c.retake_of_course_id IS NOT NULL "
            "UNION "
            "SELECT c.id "
            "FROM courses c "
            "JOIN retake_chain r "
                "ON c.retake_of_course_id = r.id"
        ") "
        "SELECT 1 "
        "FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE s.user_id = ? "
        "AND lower(trim(c.course_code)) = lower(trim(?)) "
        "AND c.id NOT IN (SELECT id FROM retake_chain) "
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
        courseID
    );
    sqlite3_bind_int(
        duplicateStatement,
        2,
        userID
    );
    sqlite3_bind_text(
        duplicateStatement,
        3,
        normalizedCode.c_str(),
        -1,
        SQLITE_TRANSIENT
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
    if (database == nullptr ||
        courseID <= 0 ||
        courseIsHistoricalAttempt(database, courseID))
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
    if (database == nullptr || courseID <= 0)
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
        sqlite3_free(errorMessage);
        return false;
    }

    sqlite3_stmt *relationStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT retake_of_course_id "
            "FROM courses WHERE id = ?;",
            -1,
            &relationStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(relationStatement, 1, courseID);

    if (sqlite3_step(relationStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(relationStatement);
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    const int sourceCourseID =
        sqlite3_column_type(relationStatement, 0) == SQLITE_NULL
            ? -1
            : sqlite3_column_int(relationStatement, 0);

    sqlite3_finalize(relationStatement);

    sqlite3_stmt *childStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "SELECT 1 FROM courses "
            "WHERE retake_of_course_id = ? "
            "LIMIT 1;",
            -1,
            &childStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(childStatement, 1, courseID);

    const bool hasLaterRetake =
        sqlite3_step(childStatement) == SQLITE_ROW;

    sqlite3_finalize(childStatement);

    if (hasLaterRetake)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_stmt *deleteStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "DELETE FROM courses WHERE id = ?;",
            -1,
            &deleteStatement,
            nullptr) != SQLITE_OK)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(deleteStatement, 1, courseID);

    const bool deleted =
        sqlite3_step(deleteStatement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(deleteStatement);

    if (!deleted)
    {
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    if (sourceCourseID > 0)
    {
        sqlite3_stmt *restoreStatement = nullptr;

        if (sqlite3_prepare_v2(
                database,
                "UPDATE courses "
                "SET excluded_from_cgpa = 0 "
                "WHERE id = ?;",
                -1,
                &restoreStatement,
                nullptr) != SQLITE_OK)
        {
            sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        sqlite3_bind_int(
            restoreStatement,
            1,
            sourceCourseID
        );

        const bool restored =
            sqlite3_step(restoreStatement) == SQLITE_DONE &&
            sqlite3_changes(database) == 1;

        sqlite3_finalize(restoreStatement);

        if (!restored)
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
        sqlite3_free(errorMessage);
        sqlite3_exec(database, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    std::cout << "Course deleted successfully. ID: "
              << courseID
              << std::endl;

    return true;
}

std::vector<Course> DatabaseManager::loadCoursesForSemester(int semesterID)
{
    std::vector<Course> courses;

    const char *sql =
        "SELECT id, course_name, course_code, credits, status, retaken, "
        "retake_of_course_id, excluded_from_cgpa, target_grade "
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

        const int retakeOfCourseID =
            sqlite3_column_type(statement, 6) == SQLITE_NULL
                ? -1
                : sqlite3_column_int(statement, 6);

        const bool excludedFromCGPA =
            sqlite3_column_int(statement, 7) == 1;

        const double targetGrade =
            sqlite3_column_double(statement, 8);

        Course course(
            courseID,
            courseName,
            courseCode,
            credits,
            courseStatusFromStorage(storedStatus),
            retaken,
            retakeOfCourseID,
            excludedFromCGPA,
            targetGrade
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
    const std::string &dueDate,
    int categoryID)
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
        "(course_id, name, grade, weight, due_date, category_id) "
        "VALUES (?, ?, ?, ?, ?, ?);";

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
    if (categoryID > 0)
    {
        sqlite3_bind_int(statement, 6, categoryID);
    }
    else
    {
        sqlite3_bind_null(statement, 6);
    }

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
    const std::string &dueDate,
    int categoryID)
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
        "SET name = ?, grade = ?, weight = ?, due_date = ?, category_id = ? "
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
    if (categoryID > 0)
    {
        sqlite3_bind_int(statement, 5, categoryID);
    }
    else
    {
        sqlite3_bind_null(statement, 5);
    }
    sqlite3_bind_int(statement, 6, assignmentID);

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

bool DatabaseManager::saveCourseProjection(
    int courseID,
    double targetGrade,
    const std::vector<std::pair<int, double>> &assignmentProjections)
{
    if (database == nullptr ||
        courseID <= 0 ||
        targetGrade < -1.0 ||
        targetGrade > 100.0 ||
        courseIsHistoricalAttempt(database, courseID))
    {
        return false;
    }

    for (const auto &projection : assignmentProjections)
    {
        if (projection.first <= 0 ||
            projection.second < -1.0 ||
            projection.second > 100.0)
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

    sqlite3_stmt *courseStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE courses "
            "SET target_grade = ? "
            "WHERE id = ?;",
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

    sqlite3_bind_double(
        courseStatement,
        1,
        targetGrade
    );

    sqlite3_bind_int(
        courseStatement,
        2,
        courseID
    );

    const bool courseUpdated =
        sqlite3_step(courseStatement) ==
        SQLITE_DONE;

    sqlite3_finalize(courseStatement);

    if (!courseUpdated ||
        sqlite3_changes(database) != 1)
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

    sqlite3_stmt *assignmentStatement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE assignments "
            "SET projected_grade = ? "
            "WHERE id = ? AND course_id = ?;",
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

    bool success = true;

    for (const auto &projection :
         assignmentProjections)
    {
        sqlite3_reset(assignmentStatement);
        sqlite3_clear_bindings(
            assignmentStatement
        );

        sqlite3_bind_double(
            assignmentStatement,
            1,
            projection.second
        );

        sqlite3_bind_int(
            assignmentStatement,
            2,
            projection.first
        );

        sqlite3_bind_int(
            assignmentStatement,
            3,
            courseID
        );

        if (sqlite3_step(
                assignmentStatement) !=
            SQLITE_DONE ||
            sqlite3_changes(database) != 1)
        {
            success = false;
            break;
        }
    }

    sqlite3_finalize(assignmentStatement);

    if (!success)
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

std::vector<Assignment>
DatabaseManager::loadAssignmentsForCourse(int courseID)
{
    std::vector<Assignment> assignments;

    const char *sql =
        "SELECT a.id, a.name, a.weight, a.grade, a.due_date, a.completed, "
        "a.projected_grade, COALESCE(a.category_id, -1), "
        "COALESCE(c.name, 'Uncategorized') "
        "FROM assignments a "
        "LEFT JOIN assignment_categories c ON c.id = a.category_id "
        "WHERE a.course_id = ? "
        "ORDER BY "
        "a.completed ASC, "
        "CASE WHEN a.due_date = '' THEN 1 ELSE 0 END, "
        "a.due_date ASC, a.id ASC;";

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

        const double projectedGrade =
            sqlite3_column_double(statement, 6);

        const int categoryID =
            sqlite3_column_int(statement, 7);

        const unsigned char *categoryText =
            sqlite3_column_text(statement, 8);

        const std::string categoryName =
            categoryText
                ? reinterpret_cast<const char *>(categoryText)
                : "Uncategorized";

        assignments.emplace_back(
            assignmentID,
            assignmentName,
            weight,
            grade,
            dueDate,
            completed,
            projectedGrade,
            categoryID,
            categoryName
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

std::vector<AssignmentCategory>
DatabaseManager::loadAssignmentCategoriesForCourse(int courseID)
{
    std::vector<AssignmentCategory> categories;

    if (database == nullptr || courseID <= 0)
    {
        return categories;
    }

    sqlite3_stmt *userStatement = nullptr;
    const char *userSql =
        "SELECT s.user_id "
        "FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE c.id = ?;";

    if (sqlite3_prepare_v2(
            database,
            userSql,
            -1,
            &userStatement,
            nullptr) != SQLITE_OK)
    {
        return categories;
    }

    sqlite3_bind_int(userStatement, 1, courseID);

    if (sqlite3_step(userStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(userStatement);
        return categories;
    }

    const int userID = sqlite3_column_int(userStatement, 0);
    sqlite3_finalize(userStatement);

    static const char *builtInCategories[] = {
        "Quiz",
        "Homework",
        "Test",
        "Midterm",
        "Final",
        "Project",
        "Lab",
        "Presentation",
        "Participation",
        "Other"
    };

    sqlite3_stmt *seedStatement = nullptr;
    const char *seedSql =
        "INSERT OR IGNORE INTO assignment_categories "
        "(user_id, name, is_builtin) VALUES (?, ?, 1);";

    if (sqlite3_prepare_v2(
            database,
            seedSql,
            -1,
            &seedStatement,
            nullptr) == SQLITE_OK)
    {
        for (const char *categoryName : builtInCategories)
        {
            sqlite3_reset(seedStatement);
            sqlite3_clear_bindings(seedStatement);
            sqlite3_bind_int(seedStatement, 1, userID);
            sqlite3_bind_text(
                seedStatement,
                2,
                categoryName,
                -1,
                SQLITE_STATIC
            );
            sqlite3_step(seedStatement);
        }

        sqlite3_finalize(seedStatement);
    }

    const char *sql =
        "SELECT id, name "
        "FROM assignment_categories "
        "WHERE user_id = ? "
        "ORDER BY "
        "is_builtin DESC, "
        "CASE name "
        "WHEN 'Quiz' THEN 1 "
        "WHEN 'Homework' THEN 2 "
        "WHEN 'Test' THEN 3 "
        "WHEN 'Midterm' THEN 4 "
        "WHEN 'Final' THEN 5 "
        "WHEN 'Project' THEN 6 "
        "WHEN 'Lab' THEN 7 "
        "WHEN 'Presentation' THEN 8 "
        "WHEN 'Participation' THEN 9 "
        "WHEN 'Other' THEN 10 "
        "ELSE 100 END, "
        "lower(name), id;";

    sqlite3_stmt *statement = nullptr;
    if (sqlite3_prepare_v2(database, sql, -1, &statement, nullptr) != SQLITE_OK)
    {
        return categories;
    }

    sqlite3_bind_int(statement, 1, userID);

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char *nameText = sqlite3_column_text(statement, 1);
        categories.push_back({
            sqlite3_column_int(statement, 0),
            nameText ? reinterpret_cast<const char *>(nameText) : ""
        });
    }

    sqlite3_finalize(statement);
    return categories;
}

int DatabaseManager::ensureAssignmentCategoryForCourse(
    int courseID,
    const std::string &name)
{
    if (database == nullptr || courseID <= 0)
    {
        return -1;
    }

    const std::string normalizedName = trimCopy(name);
    if (normalizedName.empty())
    {
        return -1;
    }

    sqlite3_stmt *userStatement = nullptr;
    const char *userSql =
        "SELECT s.user_id "
        "FROM courses c "
        "JOIN semesters s ON s.id = c.semester_id "
        "WHERE c.id = ?;";

    if (sqlite3_prepare_v2(database, userSql, -1, &userStatement, nullptr) != SQLITE_OK)
    {
        return -1;
    }

    sqlite3_bind_int(userStatement, 1, courseID);
    if (sqlite3_step(userStatement) != SQLITE_ROW)
    {
        sqlite3_finalize(userStatement);
        return -1;
    }

    const int userID = sqlite3_column_int(userStatement, 0);
    sqlite3_finalize(userStatement);

    sqlite3_stmt *findStatement = nullptr;
    const char *findSql =
        "SELECT id FROM assignment_categories "
        "WHERE user_id = ? AND lower(trim(name)) = lower(trim(?)) "
        "LIMIT 1;";

    if (sqlite3_prepare_v2(database, findSql, -1, &findStatement, nullptr) != SQLITE_OK)
    {
        return -1;
    }

    sqlite3_bind_int(findStatement, 1, userID);
    sqlite3_bind_text(findStatement, 2, normalizedName.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(findStatement) == SQLITE_ROW)
    {
        const int existingID = sqlite3_column_int(findStatement, 0);
        sqlite3_finalize(findStatement);
        return existingID;
    }
    sqlite3_finalize(findStatement);

    sqlite3_stmt *insertStatement = nullptr;
    const char *insertSql =
        "INSERT INTO assignment_categories "
        "(user_id, name, is_builtin) VALUES (?, ?, 0);";

    if (sqlite3_prepare_v2(database, insertSql, -1, &insertStatement, nullptr) != SQLITE_OK)
    {
        return -1;
    }

    sqlite3_bind_int(insertStatement, 1, userID);
    sqlite3_bind_text(insertStatement, 2, normalizedName.c_str(), -1, SQLITE_TRANSIENT);

    const bool inserted = sqlite3_step(insertStatement) == SQLITE_DONE;
    sqlite3_finalize(insertStatement);

    return inserted ? static_cast<int>(sqlite3_last_insert_rowid(database)) : -1;
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
    return updateUserProfile(
        userID,
        newUsername,
        getEmailByUserID(userID),
        maxCredits
    );
}

bool DatabaseManager::updateUserProfile(
    int userID,
    const std::string &newUsername,
    const std::string &newEmail,
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

    const std::string normalizedEmail =
        normalizeEmail(newEmail);

    if (!isValidUsername(normalizedUsername) ||
        !isValidEmail(normalizedEmail))
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
            "WHERE (lower(username) = lower(?) "
            "OR lower(email) = lower(?)) "
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

    sqlite3_bind_text(
        duplicateStatement,
        2,
        normalizedEmail.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        duplicateStatement,
        3,
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
        "SET username = ?, email = ?, "
        "max_credits = ? "
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

    sqlite3_bind_text(
        statement,
        2,
        normalizedEmail.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        statement,
        3,
        maxCredits
    );

    sqlite3_bind_int(
        statement,
        4,
        userID
    );

    const bool success =
        sqlite3_step(statement) == SQLITE_DONE &&
        sqlite3_changes(database) == 1;

    sqlite3_finalize(statement);
    return success;
}

bool DatabaseManager::updateEmail(
    int userID,
    const std::string &newEmail)
{
    if (database == nullptr || userID <= 0)
    {
        return false;
    }

    const std::string normalizedEmail =
        normalizeEmail(newEmail);

    if (!isValidEmail(normalizedEmail))
    {
        return false;
    }

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            "UPDATE users SET email = ? "
            "WHERE id = ? "
            "AND NOT EXISTS ("
                "SELECT 1 FROM users "
                "WHERE lower(email) = lower(?) "
                "AND id != ?"
            ");",
            -1,
            &statement,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        normalizedEmail.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(statement, 2, userID);

    sqlite3_bind_text(
        statement,
        3,
        normalizedEmail.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(statement, 4, userID);

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