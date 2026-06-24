#ifndef SEMESTERSWINDOW_H
#define SEMESTERSWINDOW_H

#include "databasemanager.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SemestersWindow;
}
QT_END_NAMESPACE

class QListWidgetItem;

class SemestersWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SemestersWindow(
        DatabaseManager &database,
        int userID,
        QWidget *parent = nullptr
    );

    ~SemestersWindow() override;

    void setUserID(int userID);
    void refreshSemesters();

signals:
    void semestersChanged();

private slots:
    void handleAddSemester();
    void handleFilterChanged(int index);

private:
    void addSemesterRow(const Semester &semester);
    void setSemesterStatus(
        QListWidgetItem *item,
        SemesterStatus status
    );
    void editSemesterRow(QListWidgetItem *item);
    void deleteSemesterRow(QListWidgetItem *item);
    void updateEmptyState();

    DatabaseManager &database;
    int userID;
    Ui::SemestersWindow *ui;
};

#endif // SEMESTERSWINDOW_H
