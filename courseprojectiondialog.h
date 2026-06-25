#ifndef COURSEPROJECTIONDIALOG_H
#define COURSEPROJECTIONDIALOG_H

#include "course.h"
#include "gradeprojection.h"
#include "databasemanager.h"

#include <QDialog>

#include <vector>

class QCheckBox;
class QDoubleSpinBox;

QT_BEGIN_NAMESPACE
namespace Ui
{
class CourseProjectionDialog;
}
QT_END_NAMESPACE

class CourseProjectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CourseProjectionDialog(
        DatabaseManager &database,
        Course course,
        QWidget *parent = nullptr
    );

    ~CourseProjectionDialog() override;

private slots:
    void refreshProjection();
    void saveProjection();

private:
    struct ProjectionEditor
    {
        int assignmentID = -1;
        double storedProjection = -1.0;
        QCheckBox *enabledCheckBox = nullptr;
        QDoubleSpinBox *gradeSpinBox = nullptr;
    };

    Course courseFromEditors() const;
    QString targetMessage(
        const CourseProjectionResult &result
    ) const;

    DatabaseManager &database;
    Course course;
    Ui::CourseProjectionDialog *ui;
    std::vector<ProjectionEditor> editors;
};

#endif // COURSEPROJECTIONDIALOG_H
