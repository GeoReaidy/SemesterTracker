#include "consoleapp.h"

#include <cctype>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#ifndef APP_VERSION
#define APP_VERSION "1.0.0"
#endif

namespace
{
constexpr int kScreenWidth = 80;
constexpr int kColumnWidth = 40;
constexpr int kMinimumId = 1;
constexpr int kMaximumId = 1'000'000;
constexpr int kMinimumYear = 2000;
constexpr int kMaximumYear = 2100;
constexpr int kMinimumCredits = 1;
constexpr int kMaximumCredits = 10;
constexpr double kMinimumGrade = 0.0;
constexpr double kMaximumGrade = 100.0;

const std::string kDivider(kScreenWidth, '-');

bool containsWhitespace(const std::string &value)
{
    for (const char character : value)
    {
        if (std::isspace(static_cast<unsigned char>(character)))
        {
            return true;
        }
    }

    return false;
}

bool isBlank(const std::string &value)
{
    if (value.empty())
    {
        return true;
    }

    for (const char character : value)
    {
        if (!std::isspace(static_cast<unsigned char>(character)))
        {
            return false;
        }
    }

    return true;
}

int toPercentage(double decimalWeight)
{
    return static_cast<int>(decimalWeight * 100.0);
}
} // namespace

ConsoleApp::ConsoleApp()
    : database(), authService(database), currentUser(-1, "temporary"), running(true),
    loggedIn(false)
{
}

void ConsoleApp::clearScreen() const
{
    std::system("cls");
}

bool ConsoleApp::initializeDatabase()
{
    if (!database.openDatabase("semester_tracker.db"))
    {
        std::cout << "Could not open database." << std::endl;
        return false;
    }

    if (!database.createTables())
    {
        std::cout << "Could not create database tables." << std::endl;
        return false;
    }

    return true;
}

int ConsoleApp::getMenuChoice()
{
    int choice{};

    if (!(std::cin >> choice))
    {
        if (std::cin.eof())
        {
            std::cout << "No input stream available. Closing application." << std::endl;
            running = false;
            return -1;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Please enter a number." << std::endl;
        return -1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return choice;
}

void ConsoleApp::showStartMenu()
{
    clearScreen();
    std::cout << "\n===== Semester Tracker v" << APP_VERSION << " =====" << std::endl;
    std::cout << "1. Register" << std::endl;
    std::cout << "2. Login" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Choose option: ";

    int choice = getMenuChoice();

    if (choice == 1)
    {
        handleRegister();
    }
    else if (choice == 2)
    {
        handleLogin();
    }
    else if (choice == 3)
    {
        running = false;
    }
    else if (choice == -1)
    {
        return;
    }
    else
    {
        std::cout << "Invalid choice, try again." << std::endl;
    }
}

void ConsoleApp::showDashboard()
{
    showDashboardHeader();

    const Semester *currentSemester = nullptr;

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.isInProgress())
        {
            currentSemester = &semester;
            break;
        }
    }

    std::cout << "\n===== Current Semester Overview =====" << std::endl;

    if (currentSemester == nullptr)
    {
        std::cout << "No semester is currently marked as in progress." << std::endl;
    }
    else
    {
        std::cout << currentSemester->getName() << " " << currentSemester->getYear()
        << " | Semester GPA: " << formatDouble(currentSemester->calculateSemesterGPA())
        << " | CGPA: " << formatDouble(currentUser.calculateCGPA())
        << " | Courses: " << currentSemester->getCourses().size() << std::endl;

        std::cout << kDivider << std::endl;

        if (currentSemester->getCourses().empty())
        {
            std::cout << "No courses have been added to this semester." << std::endl;
        }
        else
        {
            for (const auto &course : currentSemester->getCourses())
            {
                std::string left = std::to_string(course.getID()) + ". " + course.getCode() +
                                   " - " + course.getName();

                std::string right = "Grade: " + formatDouble(course.calculateCourseGrade()) +
                                    " | GPA: " + formatDouble(course.calculateCourseGPA());

                std::cout << std::left << std::setw(kColumnWidth) << left << right << std::endl;
            }
        }
    }

    std::cout << "\n===== Dashboard Menu =====" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "1. Open current semester"
              << "2. View all semesters" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "3. Add semester"
              << "4. Add course" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "5. Add assignment"
              << "6. Manage data" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "7. Settings"
              << "8. Logout" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "9. Exit" << std::endl;

    std::cout << "\nChoose option: ";

    int choice = getMenuChoice();

    if (choice == 1)
    {
        if (currentSemester == nullptr)
        {
            std::cout << "No current semester is available." << std::endl;
            waitForEnter();
            return;
        }

        showCourseDashboard(currentSemester->getID());
    }
    else if (choice == 2)
    {
        showSemesterDashboard();
    }
    else if (choice == 3)
    {
        handleAddSemester();
        waitForEnter();
    }
    else if (choice == 4)
    {
        handleAddCourse();
        waitForEnter();
    }
    else if (choice == 5)
    {
        handleAddAssignment();
        waitForEnter();
    }
    else if (choice == 6)
    {
        showManageDataMenu();
    }
    else if (choice == 7)
    {
        showSettingsPage();
    }
    else if (choice == 8)
    {
        handleLogout();
    }
    else if (choice == 9)
    {
        running = false;
    }
    else if (choice == -1)
    {
        return;
    }
    else
    {
        std::cout << "Invalid choice. Please try again." << std::endl;
        waitForEnter();
    }
}

void ConsoleApp::handleRegister()
{
    clearScreen();
    std::string username;
    std::string password;

    while (true)
    {
        std::cout << "Create a username: ";
        std::getline(std::cin, username);

        if (username.empty())
        {
            std::cout << "Username cannot be empty." << std::endl;
            continue;
        }

        if (containsWhitespace(username))
        {
            std::cout << "Username cannot contain spaces." << std::endl;
            continue;
        }

        if (database.userExists(username))
        {
            std::cout << "Username already exists." << std::endl;
            continue;
        }

        break;
    }

    while (true)
    {
        std::cout << "Create a password: ";
        std::getline(std::cin, password);

        if (password.empty())
        {
            std::cout << "Password cannot be empty." << std::endl;
            continue;
        }

        break;
    }

    if (authService.registerUser(username, password))
    {
        std::cout << "Registration successful." << std::endl;
    }
    else
    {
        std::cout << "Registration failed." << std::endl;
    }
}

void ConsoleApp::handleLogin()
{
    clearScreen();
    std::string username;
    std::string password;

    while (true)
    {
        std::cout << "Username: ";
        std::getline(std::cin, username);

        if (username.empty())
        {
            std::cout << "Username cannot be empty." << std::endl;
            continue;
        }

        if (containsWhitespace(username))
        {
            std::cout << "Username cannot contain spaces." << std::endl;
            continue;
        }

        break;
    }

    while (true)
    {
        std::cout << "Password: ";
        std::getline(std::cin, password);

        if (password.empty())
        {
            std::cout << "Password cannot be empty." << std::endl;
            continue;
        }

        break;
    }

    if (authService.loginUser(username, password, currentUser))
    {
        loggedIn = true;

        std::cout << "Login successful. Welcome, " << currentUser.getUsername() << "!" << std::endl;
    }
    else
    {
        loggedIn = false;
        std::cout << "Login failed." << std::endl;
    }
}

void ConsoleApp::handleLogout()
{
    loggedIn = false;
    currentUser = User(-1, "temporary");

    std::cout << "Logged out successfully." << std::endl;
}

void ConsoleApp::showDashboardHeader()
{
    clearScreen();

    if (!loggedIn)
    {
        std::cout << "No user is currently logged in." << std::endl;
        return;
    }

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::string leftHeader = "Welcome, " + currentUser.getUsername();

    std::string rightHeader = "No current semester";

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.isInProgress())
        {
            rightHeader = semester.getName() + " " + std::to_string(semester.getYear());

            break;
        }
    }

    int spacing = kScreenWidth - static_cast<int>(leftHeader.length()) -
                  static_cast<int>(rightHeader.length());

    if (spacing < 3)
    {
        spacing = 3;
    }

    std::cout << std::left << leftHeader << std::string(spacing, ' ') << rightHeader << std::endl;

    std::cout << std::string(kScreenWidth, '-') << std::endl;
}

void ConsoleApp::run()
{
    if (!initializeDatabase())
    {
        std::cout << "Failed to initialize database. Closing application." << std::endl;
        return;
    }
    while (running)
    {
        if (!loggedIn)
        {
            showStartMenu();
        }
        else
        {
            showDashboard();
        }
    }

    std::cout << "Application closed." << std::endl;
}

bool ConsoleApp::refreshCurrentUser()
{
    if (!loggedIn)
    {
        std::cout << "No user is currently logged in." << std::endl;
        return false;
    }

    if (currentUser.getID() <= 0 || currentUser.getUsername().empty())
    {
        std::cout << "Invalid current user." << std::endl;
        return false;
    }

    try
    {
        currentUser = database.loadFullUserByUsername(currentUser.getUsername());
        return true;
    }
    catch (const std::exception &error)
    {
        std::cout << "Failed to refresh user data: " << error.what() << std::endl;

        return false;
    }
}

std::string ConsoleApp::formatDouble(double value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return stream.str();
}

void ConsoleApp::waitForEnter()
{
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

std::string ConsoleApp::readNonEmptyLine(const std::string &prompt)
{
    std::string input;

    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, input);

        if (isBlank(input))
        {
            std::cout << "Input cannot be empty." << std::endl;
            continue;
        }

        return input;
    }
}

int ConsoleApp::readIntInRange(const std::string &prompt, int minValue, int maxValue)
{
    int value;

    while (true)
    {
        std::cout << prompt;

        if (!(std::cin >> value))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a valid number." << std::endl;
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (value < minValue || value > maxValue)
        {
            std::cout << "Value must be between " << minValue << " and " << maxValue << "."
                      << std::endl;
            continue;
        }

        return value;
    }
}

double ConsoleApp::readDoubleInRange(const std::string &prompt, double minValue, double maxValue)
{
    double value;

    while (true)
    {
        std::cout << prompt;

        if (!(std::cin >> value))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a valid number." << std::endl;
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (value < minValue || value > maxValue)
        {
            std::cout << "Value must be between " << minValue << " and " << maxValue << "."
                      << std::endl;
            continue;
        }

        return value;
    }
}

bool ConsoleApp::askYesNo(const std::string &prompt)
{
    char answer;

    while (true)
    {
        std::cout << prompt << " (y/n): ";
        std::cin >> answer;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        answer = static_cast<char>(std::tolower(static_cast<unsigned char>(answer)));

        if (answer == 'y')
        {
            return true;
        }

        if (answer == 'n')
        {
            return false;
        }

        std::cout << "Please enter y or n." << std::endl;
    }
}

void ConsoleApp::handleAddSemester()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Add Semester =====" << std::endl;

    std::string name = readNonEmptyLine("Semester name: ");
    int year = readIntInRange("Semester year: ", kMinimumYear, kMaximumYear);

    bool wantsInProgress = askYesNo("Is this the current semester");

    bool alreadyHasCurrentSemester = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.isInProgress())
        {
            alreadyHasCurrentSemester = true;
            break;
        }
    }

    if (wantsInProgress && alreadyHasCurrentSemester)
    {
        bool replaceCurrent =
            askYesNo("A current semester already exists. Replace it");

        if (replaceCurrent)
        {
            if (!database.clearCurrentSemesterForUser(currentUser.getID()))
            {
                std::cout << "Failed to clear the previous current semester."
                          << std::endl;
                return;
            }
        }
        else
        {
            wantsInProgress = false;
        }
    }

    if (database.addSemester(currentUser.getID(), name, year, wantsInProgress))
    {
        std::cout << "Semester added successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to add semester." << std::endl;
    }
}

void ConsoleApp::handleAddCourse()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    if (currentUser.getSemesters().empty())
    {
        std::cout << "You need to add a semester first." << std::endl;
        return;
    }

    std::cout << "===== Add Course =====" << std::endl;
    std::cout << "\nChoose a semester:" << std::endl;

    for (const auto &semester : currentUser.getSemesters())
    {
        std::cout << semester.getID() << ". " << semester.getName() << " " << semester.getYear();

        if (semester.isInProgress())
        {
            std::cout << " (Current)";
        }

        std::cout << std::endl;
    }

    int semesterID = readIntInRange("Semester ID: ", kMinimumId, kMaximumId);

    bool semesterExists = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.getID() == semesterID)
        {
            semesterExists = true;
            break;
        }
    }

    if (!semesterExists)
    {
        std::cout << "Invalid semester ID." << std::endl;
        return;
    }

    std::string courseCode = readNonEmptyLine("Course code: ");
    std::string courseName = readNonEmptyLine("Course name: ");
    int credits = readIntInRange("Credits: ", kMinimumCredits, kMaximumCredits);

    if (database.addCourse(semesterID, courseCode, courseName, credits))
    {
        std::cout << "Course added successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to add course." << std::endl;
    }
}

void ConsoleApp::handleAddAssignment()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    bool hasCourses = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        if (!semester.getCourses().empty())
        {
            hasCourses = true;
            break;
        }
    }

    if (!hasCourses)
    {
        std::cout << "You need to add a course first." << std::endl;
        return;
    }

    std::cout << "===== Add Assignment =====" << std::endl;
    std::cout << "\nChoose a course:" << std::endl;

    for (const auto &semester : currentUser.getSemesters())
    {
        std::cout << "\n" << semester.getName() << " " << semester.getYear();

        if (semester.isInProgress())
        {
            std::cout << " (Current)";
        }

        std::cout << std::endl;

        for (const auto &course : semester.getCourses())
        {
            std::cout << "  " << course.getID() << ". " << course.getCode() << " - "
                      << course.getName() << std::endl;
        }
    }

    int courseID = readIntInRange("Course ID: ", kMinimumId, kMaximumId);

    bool courseExists = false;
    int currentTotalWeight = 0;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            if (course.getID() == courseID)
            {
                courseExists = true;

                for (const auto &assignment : course.getAssignments())
                {
                    currentTotalWeight += toPercentage(assignment.getWeight());
                }

                break;
            }
        }
    }

    if (!courseExists)
    {
        std::cout << "Invalid course ID." << std::endl;
        return;
    }

    std::cout << "Current assignment weight total: " << currentTotalWeight << "%" << std::endl;

    int remainingWeight = 100 - currentTotalWeight;

    if (remainingWeight <= 0)
    {
        std::cout << "This course already has 100% assignment weight." << std::endl;
        return;
    }

    std::cout << "Remaining weight allowed: " << remainingWeight << "%" << std::endl;

    std::string assignmentName = readNonEmptyLine("Assignment name: ");
    double grade = readDoubleInRange("Grade: ", kMinimumGrade, kMaximumGrade);
    int weight = readIntInRange("Weight percentage: ", 1, remainingWeight);

    if (database.addAssignment(courseID, assignmentName, grade, weight))
    {
        std::cout << "Assignment added successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to add assignment." << std::endl;
    }
}

void ConsoleApp::showManageDataMenu()
{
    clearScreen();

    std::cout << "===== Manage Data =====" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "1. Edit semester"
              << "2. Delete semester" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "3. Edit course"
              << "4. Delete course" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "5. Edit assignment"
              << "6. Delete assignment" << std::endl;

    std::cout << std::left << std::setw(kColumnWidth) << "7. Delete my account"
              << "8. Back" << std::endl;

    std::cout << "\nChoose option: ";

    int choice = getMenuChoice();

    if (choice == 1)
    {
        handleEditSemester();
        waitForEnter();
    }
    else if (choice == 2)
    {
        handleDeleteSemester();
        waitForEnter();
    }
    else if (choice == 3)
    {
        handleEditCourse();
        waitForEnter();
    }
    else if (choice == 4)
    {
        handleDeleteCourse();
        waitForEnter();
    }
    else if (choice == 5)
    {
        handleEditAssignment();
        waitForEnter();
    }
    else if (choice == 6)
    {
        handleDeleteAssignment();
        waitForEnter();
    }
    else if (choice == 7)
    {
        handleDeleteAccount();
    }
    else if (choice == 8)
    {
        return;
    }
    else
    {
        std::cout << "Invalid choice." << std::endl;
        waitForEnter();
    }
}

void ConsoleApp::handleDeleteAssignment()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Delete Assignment =====" << std::endl;

    bool hasAssignments = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            for (const auto &assignment : course.getAssignments())
            {
                hasAssignments = true;

                std::cout << assignment.getID() << ". " << assignment.getName()
                          << " | Course: " << course.getCode()
                          << " | Grade: " << formatDouble(assignment.getGrade())
                          << " | Weight: " << formatDouble(assignment.getWeight() * 100) << "%"
                          << std::endl;
            }
        }
    }

    if (!hasAssignments)
    {
        std::cout << "No assignments found." << std::endl;
        return;
    }

    int assignmentID = readIntInRange("Assignment ID to delete: ", kMinimumId, kMaximumId);

    bool found = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            for (const auto &assignment : course.getAssignments())
            {
                if (assignment.getID() == assignmentID)
                {
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found)
    {
        std::cout << "Invalid assignment ID." << std::endl;
        return;
    }

    if (!askYesNo("Are you sure you want to delete this assignment"))
    {
        std::cout << "Delete cancelled." << std::endl;
        return;
    }

    if (database.deleteAssignment(assignmentID))
    {
        std::cout << "Assignment deleted successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to delete assignment." << std::endl;
    }
}

void ConsoleApp::handleDeleteCourse()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Delete Course =====" << std::endl;

    bool hasCourses = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            hasCourses = true;

            std::cout << course.getID() << ". " << course.getCode() << " - " << course.getName()
                      << " | Semester: " << semester.getName() << " " << semester.getYear()
                      << " | Assignments: " << course.getAssignments().size() << std::endl;
        }
    }

    if (!hasCourses)
    {
        std::cout << "No courses found." << std::endl;
        return;
    }

    int courseID = readIntInRange("Course ID to delete: ", kMinimumId, kMaximumId);

    bool found = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            if (course.getID() == courseID)
            {
                found = true;
                break;
            }
        }
    }

    if (!found)
    {
        std::cout << "Invalid course ID." << std::endl;
        return;
    }

    std::cout << "Deleting a course will also delete its assignments." << std::endl;

    if (!askYesNo("Are you sure you want to delete this course"))
    {
        std::cout << "Delete cancelled." << std::endl;
        return;
    }

    if (database.deleteCourse(courseID))
    {
        std::cout << "Course deleted successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to delete course." << std::endl;
    }
}

void ConsoleApp::handleDeleteSemester()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Delete Semester =====" << std::endl;

    if (currentUser.getSemesters().empty())
    {
        std::cout << "No semesters found." << std::endl;
        return;
    }

    for (const auto &semester : currentUser.getSemesters())
    {
        std::cout << semester.getID() << ". " << semester.getName() << " " << semester.getYear();

        if (semester.isInProgress())
        {
            std::cout << " (Current)";
        }

        std::cout << " | Courses: " << semester.getCourses().size() << std::endl;
    }

    int semesterID = readIntInRange("Semester ID to delete: ", kMinimumId, kMaximumId);

    bool found = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.getID() == semesterID)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cout << "Invalid semester ID." << std::endl;
        return;
    }

    std::cout << "Deleting a semester will also delete its courses and assignments." << std::endl;

    if (!askYesNo("Are you sure you want to delete this semester"))
    {
        std::cout << "Delete cancelled." << std::endl;
        return;
    }

    if (database.deleteSemester(semesterID))
    {
        std::cout << "Semester deleted successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to delete semester." << std::endl;
    }
}

void ConsoleApp::handleDeleteAccount()
{
    clearScreen();

    if (!loggedIn)
    {
        std::cout << "No user is currently logged in." << std::endl;
        return;
    }

    std::cout << "===== Delete Account =====" << std::endl;
    std::cout << "This will delete your account, semesters, courses, and assignments." << std::endl;

    if (!askYesNo("Are you absolutely sure"))
    {
        std::cout << "Delete cancelled." << std::endl;
        waitForEnter();
        return;
    }

    std::string usernameConfirm = readNonEmptyLine("Type your username to confirm: ");

    if (usernameConfirm != currentUser.getUsername())
    {
        std::cout << "Username did not match. Delete cancelled." << std::endl;
        waitForEnter();
        return;
    }

    int userID = currentUser.getID();

    if (database.deleteUser(userID))
    {
        std::cout << "Account deleted successfully." << std::endl;
        loggedIn = false;
        currentUser = User(-1, "temporary");
        waitForEnter();
    }
    else
    {
        std::cout << "Failed to delete account." << std::endl;
        waitForEnter();
    }
}

void ConsoleApp::handleEditSemester()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Edit Semester =====" << std::endl;

    if (currentUser.getSemesters().empty())
    {
        std::cout << "No semesters found." << std::endl;
        return;
    }

    for (const auto &semester : currentUser.getSemesters())
    {
        std::cout << semester.getID() << ". " << semester.getName() << " " << semester.getYear();

        if (semester.isInProgress())
        {
            std::cout << " (Current)";
        }

        std::cout << std::endl;
    }

    int semesterID = readIntInRange("Semester ID to edit: ", kMinimumId, kMaximumId);

    bool found = false;
    std::string oldName;
    int oldYear = 0;

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.getID() == semesterID)
        {
            found = true;
            oldName = semester.getName();
            oldYear = semester.getYear();
            break;
        }
    }

    if (!found)
    {
        std::cout << "Invalid semester ID." << std::endl;
        return;
    }

    std::cout << "\nCurrent name: " << oldName << std::endl;
    std::string newName = readNonEmptyLine("New semester name: ");

    std::cout << "Current year: " << oldYear << std::endl;
    int newYear = readIntInRange("New semester year: ", kMinimumYear, kMaximumYear);

    bool newInProgress = askYesNo("Mark this semester as current");

    if (newInProgress)
    {
        for (const auto &semester : currentUser.getSemesters())
        {
            if (semester.isInProgress() && semester.getID() != semesterID)
            {
                std::cout << "You already have another current semester." << std::endl;
                std::cout << "For now, edit cancelled to avoid multiple current semesters."
                          << std::endl;
                return;
            }
        }
    }

    if (!askYesNo("Save semester changes"))
    {
        std::cout << "Edit cancelled." << std::endl;
        return;
    }

    if (database.updateSemester(semesterID, newName, newYear, newInProgress))
    {
        std::cout << "Semester updated successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to update semester." << std::endl;
    }
}

void ConsoleApp::handleEditCourse()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Edit Course =====" << std::endl;

    bool hasCourses = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            hasCourses = true;

            std::cout << course.getID() << ". " << course.getCode() << " - " << course.getName()
                      << " | Semester: " << semester.getName() << " " << semester.getYear()
                      << " | Credits: " << course.getCredits() << std::endl;
        }
    }

    if (!hasCourses)
    {
        std::cout << "No courses found." << std::endl;
        return;
    }

    int courseID = readIntInRange("Course ID to edit: ", kMinimumId, kMaximumId);

    bool found = false;
    std::string oldCode;
    std::string oldName;
    int oldCredits = 0;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            if (course.getID() == courseID)
            {
                found = true;
                oldCode = course.getCode();
                oldName = course.getName();
                oldCredits = course.getCredits();
                break;
            }
        }
    }

    if (!found)
    {
        std::cout << "Invalid course ID." << std::endl;
        return;
    }

    std::cout << "\nCurrent code: " << oldCode << std::endl;
    std::string newCode = readNonEmptyLine("New course code: ");

    std::cout << "Current name: " << oldName << std::endl;
    std::string newName = readNonEmptyLine("New course name: ");

    std::cout << "Current credits: " << oldCredits << std::endl;
    int newCredits = readIntInRange("New credits: ", kMinimumCredits, kMaximumCredits);

    if (!askYesNo("Save course changes"))
    {
        std::cout << "Edit cancelled." << std::endl;
        return;
    }

    if (database.updateCourse(courseID, newCode, newName, newCredits))
    {
        std::cout << "Course updated successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to update course." << std::endl;
    }
}

void ConsoleApp::handleEditAssignment()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Edit Assignment =====" << std::endl;

    bool hasAssignments = false;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            for (const auto &assignment : course.getAssignments())
            {
                hasAssignments = true;

                std::cout << assignment.getID() << ". " << assignment.getName()
                          << " | Course: " << course.getCode()
                          << " | Grade: " << formatDouble(assignment.getGrade())
                          << " | Weight: " << formatDouble(assignment.getWeight() * 100) << "%"
                          << std::endl;
            }
        }
    }

    if (!hasAssignments)
    {
        std::cout << "No assignments found." << std::endl;
        return;
    }

    int assignmentID = readIntInRange("Assignment ID to edit: ", kMinimumId, kMaximumId);

    bool found = false;
    int courseID = -1;
    std::string oldName;
    double oldGrade = 0.0;
    int oldWeight = 0;
    int totalWeightWithoutThisAssignment = 0;

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            int courseTotalWeight = 0;

            for (const auto &assignment : course.getAssignments())
            {
                int assignmentWeight = toPercentage(assignment.getWeight());

                courseTotalWeight += assignmentWeight;

                if (assignment.getID() == assignmentID)
                {
                    found = true;
                    courseID = course.getID();
                    oldName = assignment.getName();
                    oldGrade = assignment.getGrade();
                    oldWeight = assignmentWeight;
                }
            }

            if (found && course.getID() == courseID)
            {
                totalWeightWithoutThisAssignment = courseTotalWeight - oldWeight;
                break;
            }
        }
    }

    if (!found)
    {
        std::cout << "Invalid assignment ID." << std::endl;
        return;
    }

    int maxAllowedWeight = 100 - totalWeightWithoutThisAssignment;

    std::cout << "\nCurrent name: " << oldName << std::endl;
    std::string newName = readNonEmptyLine("New assignment name: ");

    std::cout << "Current grade: " << formatDouble(oldGrade) << std::endl;
    double newGrade = readDoubleInRange("New grade: ", kMinimumGrade, kMaximumGrade);

    std::cout << "Current weight: " << oldWeight << "%" << std::endl;
    std::cout << "Maximum allowed weight for this assignment: " << maxAllowedWeight << "%"
              << std::endl;

    int newWeight = readIntInRange("New weight percentage: ", 1, maxAllowedWeight);

    if (!askYesNo("Save assignment changes"))
    {
        std::cout << "Edit cancelled." << std::endl;
        return;
    }

    if (database.updateAssignment(assignmentID, newName, newGrade, newWeight))
    {
        std::cout << "Assignment updated successfully." << std::endl;
        refreshCurrentUser();
    }
    else
    {
        std::cout << "Failed to update assignment." << std::endl;
    }
}

void ConsoleApp::showSemesterDashboard()
{
    bool inSemesterDashboard = true;

    while (inSemesterDashboard && loggedIn && running)
    {
        clearScreen();

        if (!refreshCurrentUser())
        {
            std::cout << "Could not load latest user data." << std::endl;
            waitForEnter();
            return;
        }

        std::cout << "===== Semester Dashboard =====" << std::endl;

        if (currentUser.getSemesters().empty())
        {
            std::cout << "No semesters found." << std::endl;
            std::cout << "\n1. Add semester" << std::endl;
            std::cout << "2. Back" << std::endl;
            std::cout << "Choose option: ";

            int emptyChoice = getMenuChoice();

            if (emptyChoice == 1)
            {
                handleAddSemester();
                waitForEnter();
            }
            else
            {
                return;
            }

            continue;
        }

        for (const auto &semester : currentUser.getSemesters())
        {
            std::cout << semester.getID() << ". " << semester.getName() << " "
                      << semester.getYear();

            if (semester.isInProgress())
            {
                std::cout << " (Current)";
            }

            std::cout << " | GPA: " << formatDouble(semester.calculateSemesterGPA())
                      << " | Courses: " << semester.getCourses().size() << std::endl;
        }

        std::cout << "\n===== Options =====" << std::endl;
        std::cout << std::left << std::setw(kColumnWidth) << "1. Open semester"
                  << "2. Add semester" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "3. Edit semester"
                  << "4. Delete semester" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "5. Back" << std::endl;

        std::cout << "\nChoose option: ";

        int choice = getMenuChoice();

        if (choice == 1)
        {
            int semesterID = readIntInRange("Semester ID to open: ", kMinimumId, kMaximumId);

            if (!semesterBelongsToCurrentUser(semesterID))
            {
                std::cout << "Invalid semester ID." << std::endl;
                waitForEnter();
                continue;
            }

            showCourseDashboard(semesterID);
        }
        else if (choice == 2)
        {
            handleAddSemester();
            waitForEnter();
        }
        else if (choice == 3)
        {
            handleEditSemester();
            waitForEnter();
        }
        else if (choice == 4)
        {
            handleDeleteSemester();
            waitForEnter();
        }
        else if (choice == 5)
        {
            inSemesterDashboard = false;
        }
        else
        {
            std::cout << "Invalid choice." << std::endl;
            waitForEnter();
        }
    }
}

void ConsoleApp::showCourseDashboard(int semesterID)
{
    bool inCourseDashboard = true;

    while (inCourseDashboard && loggedIn && running)
    {
        clearScreen();

        if (!refreshCurrentUser())
        {
            std::cout << "Could not load latest user data." << std::endl;
            waitForEnter();
            return;
        }

        const Semester *selectedSemester = nullptr;

        for (const auto &semester : currentUser.getSemesters())
        {
            if (semester.getID() == semesterID)
            {
                selectedSemester = &semester;
                break;
            }
        }

        if (selectedSemester == nullptr)
        {
            std::cout << "Semester no longer exists." << std::endl;
            waitForEnter();
            return;
        }

        std::cout << "===== " << selectedSemester->getName() << " " << selectedSemester->getYear()
                  << " =====";

        if (selectedSemester->isInProgress())
        {
            std::cout << " (Current)";
        }

        std::cout << std::endl;

        std::cout << "Semester GPA: " << formatDouble(selectedSemester->calculateSemesterGPA())
                  << std::endl;

        std::cout << "Courses: " << selectedSemester->getCourses().size() << std::endl;

        std::cout << kDivider << std::endl;

        if (selectedSemester->getCourses().empty())
        {
            std::cout << "No courses in this semester." << std::endl;
        }
        else
        {
            for (const auto &course : selectedSemester->getCourses())
            {
                std::cout << course.getID() << ". " << course.getCode() << " - " << course.getName()
                << " | Credits: " << course.getCredits()
                << " | Grade: " << formatDouble(course.calculateCourseGrade())
                << " | GPA: " << formatDouble(course.calculateCourseGPA())
                << " | Assignments: " << course.getAssignments().size() << std::endl;
            }
        }

        std::cout << "\n===== Course Options =====" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "1. Open course"
                  << "2. Add course" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "3. Edit course"
                  << "4. Delete course" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "5. Back" << std::endl;

        std::cout << "\nChoose option: ";

        int choice = getMenuChoice();

        if (choice == 1)
        {
            int courseID = readIntInRange("Course ID to open: ", kMinimumId, kMaximumId);

            if (!courseBelongsToCurrentUser(courseID))
            {
                std::cout << "Invalid course ID." << std::endl;
                waitForEnter();
                continue;
            }
            bool validCourse = false;

            for (const auto &course : selectedSemester->getCourses())
            {
                if (course.getID() == courseID)
                {
                    validCourse = true;
                    break;
                }
            }

            if (!validCourse)
            {
                std::cout << "Invalid course ID for this semester." << std::endl;
                waitForEnter();
                continue;
            }

            showCourseDetailDashboard(courseID);
        }
        else if (choice == 2)
        {
            std::string courseCode = readNonEmptyLine("Course code: ");
            std::string courseName = readNonEmptyLine("Course name: ");
            int credits = readIntInRange("Credits: ", kMinimumCredits, kMaximumCredits);

            if (database.addCourse(semesterID, courseCode, courseName, credits))
            {
                std::cout << "Course added successfully." << std::endl;
                refreshCurrentUser();
            }
            else
            {
                std::cout << "Failed to add course." << std::endl;
            }

            waitForEnter();
        }
        else if (choice == 3)
        {
            handleEditCourse();
            waitForEnter();
        }
        else if (choice == 4)
        {
            handleDeleteCourse();
            waitForEnter();
        }
        else if (choice == 5)
        {
            inCourseDashboard = false;
        }
        else
        {
            std::cout << "Invalid choice." << std::endl;
            waitForEnter();
        }
    }
}

bool ConsoleApp::semesterBelongsToCurrentUser(int semesterID)
{
    if (!refreshCurrentUser())
    {
        return false;
    }

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.getID() == semesterID)
        {
            return true;
        }
    }

    return false;
}

bool ConsoleApp::courseBelongsToCurrentUser(int courseID)
{
    if (!refreshCurrentUser())
    {
        return false;
    }

    for (const auto &semester : currentUser.getSemesters())
    {
        for (const auto &course : semester.getCourses())
        {
            if (course.getID() == courseID)
            {
                return true;
            }
        }
    }

    return false;
}

void ConsoleApp::showSettingsPage()
{
    bool inSettings = true;

    while (inSettings && loggedIn && running)
    {
        clearScreen();

        if (!refreshCurrentUser())
        {
            std::cout << "Could not load latest user data." << std::endl;
            waitForEnter();
            return;
        }

        std::cout << "===== Settings =====" << std::endl;
        std::cout << "Username: " << currentUser.getUsername() << std::endl;
        std::cout << "User ID: " << currentUser.getID() << std::endl;

        std::cout << "\n";

        std::cout << std::left << std::setw(kColumnWidth) << "1. Change username"
                  << "2. Change password" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "3. Set current semester"
                  << "4. Delete account" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "5. Back" << std::endl;

        std::cout << "\nChoose option: ";

        int choice = getMenuChoice();

        if (choice == 1)
        {
            handleChangeUsername();
            waitForEnter();
        }
        else if (choice == 2)
        {
            handleChangePassword();
            waitForEnter();
        }
        else if (choice == 3)
        {
            handleSetCurrentSemester();
            waitForEnter();
        }
        else if (choice == 4)
        {
            handleDeleteAccount();

            if (!loggedIn)
            {
                return;
            }
        }
        else if (choice == 5)
        {
            inSettings = false;
        }
        else
        {
            std::cout << "Invalid choice." << std::endl;
            waitForEnter();
        }
    }
}

void ConsoleApp::handleChangeUsername()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Change Username =====" << std::endl;
    std::cout << "Current username: " << currentUser.getUsername() << std::endl;

    std::string newUsername;

    while (true)
    {
        newUsername = readNonEmptyLine("New username: ");

        if (containsWhitespace(newUsername))
        {
            std::cout << "Username cannot contain spaces." << std::endl;
            continue;
        }

        if (newUsername == currentUser.getUsername())
        {
            std::cout << "New username must be different." << std::endl;
            continue;
        }

        if (database.userExists(newUsername))
        {
            std::cout << "That username is already taken." << std::endl;
            continue;
        }

        break;
    }

    if (!askYesNo("Save new username"))
    {
        std::cout << "Username change cancelled." << std::endl;
        return;
    }

    int userID = currentUser.getID();

    if (database.updateUsername(userID, newUsername))
    {
        currentUser.setUsername(newUsername);

        if (!refreshCurrentUser())
        {
            std::cout << "Username changed, but user data could not be refreshed." << std::endl;
            return;
        }

        std::cout << "Username changed successfully." << std::endl;
    }
    else
    {
        std::cout << "Failed to change username." << std::endl;
    }
}

void ConsoleApp::handleSetCurrentSemester()
{
    clearScreen();

    if (!refreshCurrentUser())
    {
        std::cout << "Could not load latest user data." << std::endl;
        return;
    }

    std::cout << "===== Set Current Semester =====" << std::endl;

    if (currentUser.getSemesters().empty())
    {
        std::cout << "No semesters found." << std::endl;
        return;
    }

    for (const auto &semester : currentUser.getSemesters())
    {
        std::cout << semester.getID() << ". " << semester.getName() << " " << semester.getYear();

        if (semester.isInProgress())
        {
            std::cout << " (Current)";
        }

        std::cout << std::endl;
    }

    int semesterID = readIntInRange("Semester ID to set as current: ", kMinimumId, kMaximumId);

    const Semester *selectedSemester = nullptr;

    for (const auto &semester : currentUser.getSemesters())
    {
        if (semester.getID() == semesterID)
        {
            selectedSemester = &semester;
            break;
        }
    }

    if (selectedSemester == nullptr)
    {
        std::cout << "Invalid semester ID." << std::endl;
        return;
    }

    if (selectedSemester->isInProgress())
    {
        std::cout << "That semester is already current." << std::endl;
        return;
    }

    if (!askYesNo("Set this semester as current"))
    {
        std::cout << "Operation cancelled." << std::endl;
        return;
    }

    if (!database.clearCurrentSemesterForUser(currentUser.getID()))
    {
        std::cout << "Failed to clear previous current semester." << std::endl;
        return;
    }

    if (database.updateSemester(selectedSemester->getID(), selectedSemester->getName(),
                                selectedSemester->getYear(), true))
    {
        refreshCurrentUser();
        std::cout << "Current semester updated successfully." << std::endl;
    }
    else
    {
        std::cout << "Failed to set current semester." << std::endl;
    }
}

void ConsoleApp::handleChangePassword()
{
    clearScreen();

    if (!loggedIn || !refreshCurrentUser())
    {
        std::cout << "Could not load the current account." << std::endl;
        return;
    }

    std::cout << "===== Change Password =====" << std::endl;

    std::string currentPassword = readNonEmptyLine("Current password: ");

    std::string newPassword;

    while (true)
    {
        newPassword = readNonEmptyLine("New password: ");

        if (newPassword.length() < 8)
        {
            std::cout << "Password must contain at least 8 characters." << std::endl;
            continue;
        }

        if (newPassword == currentPassword)
        {
            std::cout << "New password must be different from the current password." << std::endl;
            continue;
        }

        std::string confirmation = readNonEmptyLine("Confirm new password: ");

        if (newPassword != confirmation)
        {
            std::cout << "Passwords do not match." << std::endl;
            continue;
        }

        break;
    }

    if (!askYesNo("Save the new password"))
    {
        std::cout << "Password change cancelled." << std::endl;
        return;
    }

    if (authService.changePassword(currentUser.getID(), currentUser.getUsername(), currentPassword,
                                   newPassword))
    {
        std::cout << "Password changed successfully." << std::endl;
    }
    else
    {
        std::cout << "Password change failed. Check your current password." << std::endl;
    }
}

void ConsoleApp::showCourseDetailDashboard(int courseID)
{
    bool insideCoursePage = true;

    while (insideCoursePage && loggedIn && running)
    {
        clearScreen();

        if (!refreshCurrentUser())
        {
            std::cout << "Could not load latest user data." << std::endl;
            waitForEnter();
            return;
        }

        const Course *selectedCourse = nullptr;
        std::string semesterDescription;

        for (const auto &semester : currentUser.getSemesters())
        {
            for (const auto &course : semester.getCourses())
            {
                if (course.getID() == courseID)
                {
                    selectedCourse = &course;

                    semesterDescription =
                        semester.getName() + " " + std::to_string(semester.getYear());

                    break;
                }
            }

            if (selectedCourse != nullptr)
            {
                break;
            }
        }

        if (selectedCourse == nullptr)
        {
            std::cout << "Course no longer exists." << std::endl;
            waitForEnter();
            return;
        }

        int totalWeight = 0;

        for (const auto &assignment : selectedCourse->getAssignments())
        {
            totalWeight += toPercentage(assignment.getWeight());
        }

        int remainingWeight = 100 - totalWeight;

        std::cout << "===== " << selectedCourse->getCode() << " - " << selectedCourse->getName()
                  << " =====" << std::endl;

        std::cout << "Semester: " << semesterDescription << std::endl;

        std::cout << "Course ID: " << selectedCourse->getID() << std::endl;

        std::cout << "Credits: " << selectedCourse->getCredits() << std::endl;

        std::cout << "Course Grade: " << formatDouble(selectedCourse->calculateCourseGrade())
                  << std::endl;

        std::cout << "Course GPA: " << formatDouble(selectedCourse->calculateCourseGPA())
                  << std::endl;

        std::cout << "Assignment Weight Used: " << totalWeight << "%" << std::endl;

        std::cout << "Assignment Weight Remaining: " << remainingWeight << "%" << std::endl;

        std::cout << kDivider << std::endl;

        std::cout << "===== Assignments =====" << std::endl;

        if (selectedCourse->getAssignments().empty())
        {
            std::cout << "No assignments found." << std::endl;
        }
        else
        {
            for (const auto &assignment : selectedCourse->getAssignments())
            {
                std::string left = std::to_string(assignment.getID()) + ". " + assignment.getName();

                std::string right = "Grade: " + formatDouble(assignment.getGrade()) +
                                    " | Weight: " + formatDouble(assignment.getWeight() * 100) +
                                    "%";

                std::cout << std::left << std::setw(kColumnWidth) << left << right << std::endl;
            }
        }

        std::cout << kDivider << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "1. Add assignment"
                  << "2. Edit assignment" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "3. Delete assignment"
                  << "4. Edit course information" << std::endl;

        std::cout << std::left << std::setw(kColumnWidth) << "5. Delete course"
                  << "6. Back" << std::endl;

        std::cout << "\nChoose option: ";

        int choice = getMenuChoice();

        if (choice == 1)
        {
            handleAddAssignment();
            waitForEnter();
        }
        else if (choice == 2)
        {
            handleEditAssignment();
            waitForEnter();
        }
        else if (choice == 3)
        {
            handleDeleteAssignment();
            waitForEnter();
        }
        else if (choice == 4)
        {
            handleEditCourse();
            waitForEnter();
        }
        else if (choice == 5)
        {
            handleDeleteCourse();
            waitForEnter();

            refreshCurrentUser();

            bool courseStillExists = false;

            for (const auto &semester : currentUser.getSemesters())
            {
                for (const auto &course : semester.getCourses())
                {
                    if (course.getID() == courseID)
                    {
                        courseStillExists = true;
                        break;
                    }
                }
            }

            if (!courseStillExists)
            {
                insideCoursePage = false;
            }
        }
        else if (choice == 6)
        {
            insideCoursePage = false;
        }
        else
        {
            std::cout << "Invalid choice." << std::endl;
            waitForEnter();
        }
    }
}