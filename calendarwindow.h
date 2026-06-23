#ifndef CALENDARWINDOW_H
#define CALENDARWINDOW_H

#include "databasemanager.h"

#include <QDate>
#include <QString>
#include <QWidget>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CalendarWindow;
}
QT_END_NAMESPACE

class CalendarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarWindow(
        DatabaseManager &database,
        int userID,
        QWidget *parent = nullptr
    );

    ~CalendarWindow() override;

    void setUserID(int userID);
    void refreshCalendar();
    void selectDate(const QDate &date);

private:
    struct DeadlineEntry
    {
        int assignmentID = -1;
        QString assignmentName;
        QString courseCode;
        QString courseName;
        QDate dueDate;
        int weight = 0;
        bool graded = false;
        double grade = -1.0;
    };

    void loadDeadlines();
    void applyDeadlineHighlights();
    void showSelectedDate(const QDate &date);
    void populateUpcomingDeadlines();
    void updateMonthTitle(int year, int month);
    void addDeadlineRow(
        const DeadlineEntry &entry,
        class QListWidget *target,
        bool showFullDate
    );

    DatabaseManager &database;
    int currentUserID = -1;

    std::vector<DeadlineEntry> deadlines;
    std::vector<QDate> highlightedDates;

    Ui::CalendarWindow *ui;
};

#endif // CALENDARWINDOW_H
