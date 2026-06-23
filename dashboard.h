#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "databasemanager.h"
#include "user.h"

#include <QDate>
#include <QWidget>

#include <string>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Dashboard;
}
QT_END_NAMESPACE

class GUIApp;
class SemestersWindow;
class CoursesWindow;
class AssignmentsWindow;
class GradesWindow;
class SettingsWindow;
class CalendarWindow;

class Dashboard : public QWidget
{
    Q_OBJECT

    friend class GUIApp;

public:
    explicit Dashboard(
        DatabaseManager &database,
        QWidget *parent = nullptr
        );

    ~Dashboard() override;

    void setUserData(const User &user);

signals:
    void logoutRequested();

private:
    void createSemestersPage();
    void createCoursesPage();
    void createAssignmentsPage();
    void createGradesPage();
    void createSettingsPage();
    void createCalendarPage();
    void refreshUserFromDatabase();

    void refreshDashboardLists(const User &user);
    void refreshDashboardCalendar();
    void openCalendarDate(const QDate &date);
    void populateCurrentCourses(int semesterID);
    void populateUpcomingAssignments(int semesterID);
    void addCurrentCourseRow(const Course &course);
    void addUpcomingAssignmentRow(
        const Course &course,
        const Assignment &assignment
        );

    DatabaseManager &database;
    int currentUserID = -1;
    std::string currentUsername;

    Ui::Dashboard *ui;
    SemestersWindow *semestersWindow = nullptr;
    CoursesWindow *coursesWindow = nullptr;
    AssignmentsWindow *assignmentsWindow = nullptr;
    GradesWindow *gradesWindow = nullptr;
    SettingsWindow *settingsWindow = nullptr;
    CalendarWindow *calendarWindow = nullptr;
};

#endif // DASHBOARD_H
