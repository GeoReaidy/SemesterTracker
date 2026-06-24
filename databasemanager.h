#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "assignment.h"
#include "course.h"
#include "semester.h"
#include "sqlite3.h"
#include "user.h"

#include <string>
#include <vector>

class DatabaseManager
{
private:
    sqlite3 *database;

public:
    DatabaseManager();
    ~DatabaseManager();

    // Connection and setup
    bool openDatabase(const std::string &databasePath);
    void closeDatabase();
    bool createTables();

    // Utility
    int getLastInsertID() const;
    std::string getPasswordHashByUsername(const std::string &username);

    bool saveLoginSession(
        int userID,
        const std::string &tokenHash
    );

    int getUserIDBySessionTokenHash(
        const std::string &tokenHash
    );

    bool deleteLoginSession(
        const std::string &tokenHash
    );

    bool deleteLoginSessionsForUser(int userID);

    // User operations
    bool addUser(const std::string &username,
                 const std::string &passwordHash,
                 int maxCredits);

    bool deleteUser(int userID);
    bool userExists(const std::string &username);
    int getUserIDByUsername(const std::string &username);

    User loadUserByUsername(const std::string &username);
    User loadFullUserByUsername(const std::string &username);
    User loadFullUserByID(int userID);

    bool updateUsername(int userID,
                        const std::string &newUsername);

    bool updateUserProfile(int userID,
                           const std::string &newUsername,
                           int maxCredits);

    bool updatePasswordHash(int userID,
                            const std::string &newPasswordHash);

    // Semester operations
    bool addSemester(int userID,
                     const std::string &name,
                     int year,
                     bool inProgress = false);

    bool addCompletedSemester(int userID,
                              const std::string &name,
                              int year,
                              int completedCredits,
                              double semesterGPA);

    bool updateSemester(int semesterID,
                        const std::string &name,
                        int year,
                        bool inProgress);

    bool updateSemesterDetails(
        int semesterID,
        const std::string &name,
        int year,
        bool inProgress,
        bool summaryOnly,
        int summaryCredits,
        double summaryGPA,
        SemesterStatus status = SemesterStatus::Planned
    );

    bool setSemesterStatus(
        int semesterID,
        SemesterStatus status
    );

    bool deleteSemester(int semesterID);
    bool clearCurrentSemesterForUser(int userID);

    std::vector<Semester> loadSemestersForUser(int userID);

    // Course operations
    bool addCourse(int semesterID,
                   const std::string &courseCode,
                   const std::string &courseName,
                   int credits);

    bool addCompletedCourse(
        int semesterID,
        const std::string &courseCode,
        const std::string &courseName,
        int credits,
        const std::string &letterGrade,
        double finalPercentage
    );


    int findCourseByCodeForUser(
        int userID,
        const std::string &courseCode
    );

    bool retakeCourse(
        int existingCourseID,
        int newSemesterID,
        const std::string &courseCode,
        const std::string &courseName,
        int credits,
        bool hasFinalGrade,
        const std::string &letterGrade,
        double finalPercentage
    );

    bool updateCourse(int courseID,
                      const std::string &courseCode,
                      const std::string &courseName,
                      int credits);

    bool updateCourseDetails(
        int courseID,
        int semesterID,
        const std::string &courseCode,
        const std::string &courseName,
        int credits
    );

    bool setCourseStatus(
        int courseID,
        CourseStatus status
    );

    bool deleteCourse(int courseID);

    std::vector<Course> loadCoursesForSemester(int semesterID);

    // Assignment operations
    bool addAssignment(int courseID,
                       const std::string &name,
                       double grade,
                       int weight,
                       const std::string &dueDate = "");

    bool updateAssignment(int assignmentID,
                          const std::string &name,
                          double grade,
                          int weight,
                          const std::string &dueDate = "");

    bool deleteAssignment(int assignmentID);

    bool setAssignmentCompleted(
        int assignmentID,
        bool completed
    );

    std::vector<Assignment> loadAssignmentsForCourse(int courseID);
};

#endif
