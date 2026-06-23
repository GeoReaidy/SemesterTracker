#ifndef ASSIGNMENTSWINDOW_H
#define ASSIGNMENTSWINDOW_H

#include "databasemanager.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AssignmentsWindow;
}
QT_END_NAMESPACE

class QListWidgetItem;

class AssignmentsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AssignmentsWindow(
        DatabaseManager &database,
        int userID,
        QWidget *parent = nullptr
    );

    ~AssignmentsWindow() override;

    void setUserID(int userID);
    void refreshSemesters();
    void refreshCourses();
    void refreshAssignments();

signals:
    void assignmentsChanged();

private slots:
    void handleSemesterChanged(int index);
    void handleCourseChanged(int index);
    void handleAddAssignment();

private:
    int selectedSemesterID() const;
    int selectedCourseID() const;

    void addAssignmentRow(const Assignment &assignment);
    void editAssignmentRow(QListWidgetItem *item);
    void deleteAssignmentRow(QListWidgetItem *item);
    void updateEmptyState();

    DatabaseManager &database;
    int userID;
    Ui::AssignmentsWindow *ui;
};

#endif // ASSIGNMENTSWINDOW_H
