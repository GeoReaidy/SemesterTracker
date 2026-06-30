#ifndef ASSIGNMENTEDITORDIALOG_H
#define ASSIGNMENTEDITORDIALOG_H

#include "assignment.h"
#include "databasemanager.h"

#include <QDialog>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AssignmentEditorDialog;
}
QT_END_NAMESPACE

class AssignmentEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AssignmentEditorDialog(
        DatabaseManager &database,
        int courseID,
        const QString &courseDisplayName,
        QWidget *parent = nullptr
    );

    AssignmentEditorDialog(
        DatabaseManager &database,
        int courseID,
        const QString &courseDisplayName,
        const Assignment &assignment,
        QWidget *parent = nullptr
    );

    ~AssignmentEditorDialog() override;

private slots:
    void updateGradeState(bool enabled);
    void updateDueDateState(bool enabled);
    void saveAssignment();

private:
    void loadCategories();
    void configureForAdd();
    void configureForEdit(const Assignment &assignment);
    bool assignmentNameAlreadyExists(
        const QString &name,
        int ignoredAssignmentID = -1
    ) const;
    int totalAssignmentWeight(
        int ignoredAssignmentID = -1
    ) const;
    void showValidationError(const QString &message);
    void clearValidationError();

    DatabaseManager &database;
    int courseID;
    int assignmentID;
    bool editMode;
    QString courseDisplayName;
    Ui::AssignmentEditorDialog *ui;
};

#endif // ASSIGNMENTEDITORDIALOG_H
