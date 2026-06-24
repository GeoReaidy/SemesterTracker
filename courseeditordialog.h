#ifndef COURSEEDITORDIALOG_H
#define COURSEEDITORDIALOG_H

#include "course.h"
#include "databasemanager.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CourseEditorDialog;
}
QT_END_NAMESPACE

class CourseEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CourseEditorDialog(
        DatabaseManager &database,
        int userID,
        int initialSemesterID,
        QWidget *parent = nullptr
    );

    CourseEditorDialog(
        DatabaseManager &database,
        int userID,
        int currentSemesterID,
        const Course &course,
        QWidget *parent = nullptr
    );

    ~CourseEditorDialog() override;

private slots:
    void saveCourse();

private:
    void populateSemesters(int selectedSemesterID);
    void configureForAdd();
    void configureForEdit(const Course &course);
    int selectedSemesterID() const;
    SemesterStatus selectedSemesterStatus() const;
    double selectedFinalPercentage() const;
    void showValidationError(const QString &message);
    void clearValidationError();

    DatabaseManager &database;
    int userID;
    int courseID;
    int originalSemesterID;
    bool editMode;
    Ui::CourseEditorDialog *ui;
};

#endif // COURSEEDITORDIALOG_H
