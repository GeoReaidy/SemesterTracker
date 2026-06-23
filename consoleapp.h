#ifndef CONSOLEAPP_H
#define CONSOLEAPP_H

#include "authservice.h"
#include "databasemanager.h"
#include "user.h"

#include <string>

class ConsoleApp
{
public:
    ConsoleApp();

    void run();

private:
    // Application state
    DatabaseManager database;
    AuthService authService;
    User currentUser;
    bool running;
    bool loggedIn;

    // Application setup and display
    bool initializeDatabase();
    void clearScreen() const;
    void showStartMenu();
    void showDashboardHeader();
    void showDashboard();

    // Authentication
    void handleRegister();
    void handleLogin();
    void handleLogout();

    // Navigation
    void showSemesterDashboard();
    void showCourseDashboard(int semesterID);
    void showCourseDetailDashboard(int courseID);
    void showManageDataMenu();
    void showSettingsPage();

    // Add operations
    void handleAddSemester();
    void handleAddCourse();
    void handleAddAssignment();

    // Edit operations
    void handleEditSemester();
    void handleEditCourse();
    void handleEditAssignment();

    // Delete operations
    void handleDeleteSemester();
    void handleDeleteCourse();
    void handleDeleteAssignment();
    void handleDeleteAccount();

    // Account settings
    void handleChangeUsername();
    void handleChangePassword();
    void handleSetCurrentSemester();

    // User data and ownership validation
    bool refreshCurrentUser();
    bool semesterBelongsToCurrentUser(int semesterID);
    bool courseBelongsToCurrentUser(int courseID);

    // Input helpers
    int getMenuChoice();
    std::string readNonEmptyLine(const std::string &prompt);
    int readIntInRange(const std::string &prompt, int minValue, int maxValue);
    double readDoubleInRange(const std::string &prompt, double minValue, double maxValue);
    bool askYesNo(const std::string &prompt);
    void waitForEnter();

    // Formatting helpers
    std::string formatDouble(double value);
};

#endif
