#ifndef SEMESTEREDITORDIALOG_H
#define SEMESTEREDITORDIALOG_H

#include "databasemanager.h"
#include "semester.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SemesterEditorDialog;
}
QT_END_NAMESPACE

class SemesterEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SemesterEditorDialog(
        DatabaseManager &database,
        int userID,
        QWidget *parent = nullptr
    );

    SemesterEditorDialog(
        DatabaseManager &database,
        int userID,
        const Semester &semester,
        QWidget *parent = nullptr
    );

    ~SemesterEditorDialog() override;

private slots:
    void updateCompletedState(bool completed);
    void saveSemester();

private:
    void configureForAdd();
    void configureForEdit(const Semester &semester);
    bool semesterAlreadyExists(
        const QString &term,
        int year
    ) const;
    void showValidationError(const QString &message);
    void clearValidationError();

    DatabaseManager &database;
    int userID;
    int semesterID;
    bool editMode;
    SemesterStatus existingStatus;
    bool existingSemesterHasCourses;
    Ui::SemesterEditorDialog *ui;
};

#endif // SEMESTEREDITORDIALOG_H
