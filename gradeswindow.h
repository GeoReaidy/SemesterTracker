#ifndef GRADESWINDOW_H
#define GRADESWINDOW_H

#include "databasemanager.h"
#include "gradeprojection.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class GradesWindow;
}
QT_END_NAMESPACE

class GpaTrendChart;

class GradesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GradesWindow(
        DatabaseManager &database,
        int userID,
        QWidget *parent = nullptr
    );

    ~GradesWindow() override;

    void setUserID(int userID);
    void refreshSemesters();
    void refreshGrades();

private slots:
    void handleSemesterChanged(int index);

private:
    int selectedSemesterID() const;

    void addCourseGradeRow(
        const Course &course,
        const CourseProjectionResult &result
    );

    void openCourseProjection(
        const Course &course
    );

    void updateSummary(
        const User &user,
        const Semester *selectedSemester
    );

    void updateTrend(const User &user);
    void updateEmptyState();

    static QString courseCountText(int count);

    DatabaseManager &database;
    int userID;
    Ui::GradesWindow *ui;
    GpaTrendChart *trendChart;
};

#endif // GRADESWINDOW_H
