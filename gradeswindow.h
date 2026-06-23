#ifndef GRADESWINDOW_H
#define GRADESWINDOW_H

#include "databasemanager.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class GradesWindow;
}
QT_END_NAMESPACE

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
    struct CourseGradeResult
    {
        bool hasGrades = false;
        double percentage = 0.0;
        double gpa = 0.0;
        int totalWeight = 0;
    };

    int selectedSemesterID() const;

    CourseGradeResult calculateCourseGrade(
        const Course &course
    ) const;

    void addCourseGradeRow(
        const Course &course,
        const CourseGradeResult &result
    );

    void updateSummary(
        const std::vector<Course> &courses
    );

    void updateEmptyState();

    static double percentageToGpa(double percentage);
    static QString percentageToLetterGrade(double percentage);
    static QString courseCountText(int count);

    DatabaseManager &database;
    int userID;
    Ui::GradesWindow *ui;
};

#endif // GRADESWINDOW_H
