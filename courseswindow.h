#ifndef COURSESWINDOW_H
#define COURSESWINDOW_H

#include "databasemanager.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CoursesWindow;
}
QT_END_NAMESPACE

class QListWidgetItem;

class CoursesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CoursesWindow(
        DatabaseManager &database,
        int userID,
        QWidget *parent = nullptr
    );

    ~CoursesWindow() override;

    void setUserID(int userID);
    void refreshSemesters();
    void refreshCourses();

signals:
    void coursesChanged();

private slots:
    void handleSemesterChanged(int index);
    void handleAddCourse();

private:
    int selectedSemesterID() const;
    bool selectedSemesterIsSummaryOnly() const;
    void addCourseRow(const Course &course);
    void editCourseRow(QListWidgetItem *item);
    void deleteCourseRow(QListWidgetItem *item);
    void updateEmptyState();

    DatabaseManager &database;
    int userID;
    Ui::CoursesWindow *ui;
};

#endif // COURSESWINDOW_H
