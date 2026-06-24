#include "courseeditordialog.h"
#include "ui_courseeditordialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <exception>

namespace
{
double percentageForLetterGrade(const QString &letterGrade)
{
    if (letterGrade == "A")  return 90.0;
    if (letterGrade == "A-") return 87.0;
    if (letterGrade == "B+") return 83.0;
    if (letterGrade == "B")  return 80.0;
    if (letterGrade == "B-") return 77.0;
    if (letterGrade == "C+") return 73.0;
    if (letterGrade == "C")  return 70.0;
    if (letterGrade == "C-") return 67.0;
    if (letterGrade == "D+") return 63.0;
    if (letterGrade == "D")  return 60.0;
    if (letterGrade == "F")  return 0.0;

    return -1.0;
}
}

CourseEditorDialog::CourseEditorDialog(
    DatabaseManager &database,
    int userID,
    int initialSemesterID,
    QWidget *parent)
    : QDialog(parent),
      database(database),
      userID(userID),
      courseID(-1),
      originalSemesterID(-1),
      editMode(false),
      ui(new Ui::CourseEditorDialog)
{
    ui->setupUi(this);
    populateSemesters(initialSemesterID);
    configureForAdd();

    connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &CourseEditorDialog::saveCourse
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );
}

CourseEditorDialog::CourseEditorDialog(
    DatabaseManager &database,
    int userID,
    int currentSemesterID,
    const Course &course,
    QWidget *parent)
    : QDialog(parent),
      database(database),
      userID(userID),
      courseID(course.getID()),
      originalSemesterID(currentSemesterID),
      editMode(true),
      ui(new Ui::CourseEditorDialog)
{
    ui->setupUi(this);
    populateSemesters(currentSemesterID);
    configureForEdit(course);

    connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &CourseEditorDialog::saveCourse
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );
}

CourseEditorDialog::~CourseEditorDialog()
{
    delete ui;
}

void CourseEditorDialog::populateSemesters(
    int selectedSemesterID)
{
    ui->semesterComboBox->clear();

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    int selectedIndex = -1;

    for (const Semester &semester : semesters)
    {
        if (semester.isSummaryOnly())
        {
            continue;
        }

        QString label =
            QString("%1 %2")
                .arg(QString::fromStdString(semester.getName()))
                .arg(semester.getYear());

        if (semester.getStatus() == SemesterStatus::Active)
        {
            label += tr(" — active");
        }
        else if (semester.getStatus() == SemesterStatus::Completed)
        {
            label += tr(" — completed");
        }

        ui->semesterComboBox->addItem(
            label,
            semester.getID()
        );

        ui->semesterComboBox->setItemData(
            ui->semesterComboBox->count() - 1,
            static_cast<int>(semester.getStatus()),
            Qt::UserRole + 1
        );

        if (semester.getID() == selectedSemesterID)
        {
            selectedIndex = ui->semesterComboBox->count() - 1;
        }
    }

    if (selectedIndex >= 0)
    {
        ui->semesterComboBox->setCurrentIndex(selectedIndex);
    }
    else if (ui->semesterComboBox->count() > 0)
    {
        ui->semesterComboBox->setCurrentIndex(0);
    }
}

void CourseEditorDialog::configureForAdd()
{
    setWindowTitle(tr("Add Course"));
    ui->dialogTitleLabel->setText(tr("Add Course"));
    ui->dialogSubtitleLabel->setText(
        tr("Enter the course details in one place.")
    );

    ui->courseCodeEdit->clear();
    ui->courseNameEdit->clear();
    ui->creditsSpinBox->setValue(3);
    ui->finalGradeComboBox->setCurrentIndex(0);
    ui->retakeCheckBox->setChecked(false);
    ui->retakeFrame->setVisible(true);
    ui->finalGradeLabel->setVisible(true);
    ui->finalGradeComboBox->setVisible(true);
    ui->editNoticeLabel->setVisible(false);
    ui->validationLabel->setVisible(false);

    ui->courseCodeEdit->setFocus();
}

void CourseEditorDialog::configureForEdit(
    const Course &course)
{
    setWindowTitle(tr("Edit Course"));
    ui->dialogTitleLabel->setText(tr("Edit Course"));
    ui->dialogSubtitleLabel->setText(
        tr("Update the course details and save your changes.")
    );

    ui->courseCodeEdit->setText(
        QString::fromStdString(course.getCode())
    );
    ui->courseNameEdit->setText(
        QString::fromStdString(course.getName())
    );
    ui->creditsSpinBox->setValue(course.getCredits());

    ui->retakeFrame->setVisible(false);
    ui->finalGradeLabel->setVisible(false);
    ui->finalGradeComboBox->setVisible(false);
    ui->editNoticeLabel->setText(
        tr("Moving or renaming this course will keep its existing "
           "assignments and grades unchanged.")
    );
    ui->editNoticeLabel->setVisible(true);
    ui->validationLabel->setVisible(false);

    ui->courseCodeEdit->selectAll();
    ui->courseCodeEdit->setFocus();
}

int CourseEditorDialog::selectedSemesterID() const
{
    if (!ui ||
        !ui->semesterComboBox ||
        ui->semesterComboBox->currentIndex() < 0)
    {
        return -1;
    }

    return ui->semesterComboBox->currentData().toInt();
}

SemesterStatus CourseEditorDialog::selectedSemesterStatus() const
{
    if (!ui ||
        !ui->semesterComboBox ||
        ui->semesterComboBox->currentIndex() < 0)
    {
        return SemesterStatus::Planned;
    }

    return static_cast<SemesterStatus>(
        ui->semesterComboBox->currentData(
            Qt::UserRole + 1
        ).toInt()
    );
}

double CourseEditorDialog::selectedFinalPercentage() const
{
    const QString grade =
        ui->finalGradeComboBox->currentText();

    return percentageForLetterGrade(grade);
}

void CourseEditorDialog::showValidationError(
    const QString &message)
{
    ui->validationLabel->setText(message);
    ui->validationLabel->setVisible(true);
}

void CourseEditorDialog::clearValidationError()
{
    ui->validationLabel->clear();
    ui->validationLabel->setVisible(false);
}

void CourseEditorDialog::saveCourse()
{
    clearValidationError();

    if (userID <= 0)
    {
        showValidationError(
            tr("No logged-in user is available.")
        );
        return;
    }

    const int semesterID = selectedSemesterID();
    const SemesterStatus semesterStatus =
        selectedSemesterStatus();

    if (semesterID <= 0)
    {
        showValidationError(
            tr("Choose a semester that accepts courses.")
        );
        return;
    }

    const QString courseCode =
        ui->courseCodeEdit->text().trimmed();
    const QString courseName =
        ui->courseNameEdit->text().trimmed();
    const int credits = ui->creditsSpinBox->value();

    try
    {
        const Course candidate(
            editMode ? courseID : -1,
            courseName.toStdString(),
            courseCode.toStdString(),
            credits
        );

        Q_UNUSED(candidate);
    }
    catch (const std::exception &error)
    {
        showValidationError(
            QString::fromUtf8(error.what())
        );
        return;
    }

    const int existingCourseID =
        database.findCourseByCodeForUser(
            userID,
            courseCode.toStdString()
        );

    if (editMode)
    {
        if (existingCourseID >= 0 &&
            existingCourseID != courseID)
        {
            showValidationError(
                tr("Another course with this code already exists in "
                   "your academic history.")
            );
            return;
        }

        if (semesterStatus == SemesterStatus::Completed &&
            semesterID != originalSemesterID)
        {
            showValidationError(
                tr("An existing course cannot be moved into a completed "
                   "semester. Add it there as a completed course instead.")
            );
            return;
        }

        if (!database.updateCourseDetails(
                courseID,
                semesterID,
                courseCode.toStdString(),
                courseName.toStdString(),
                credits))
        {
            showValidationError(
                tr("The course could not be updated. Your existing "
                   "data was not changed.")
            );
            return;
        }

        accept();
        return;
    }

    const bool retake = ui->retakeCheckBox->isChecked();
    const bool hasFinalGrade =
        ui->finalGradeComboBox->currentIndex() > 0;
    const QString letterGrade =
        hasFinalGrade
            ? ui->finalGradeComboBox->currentText()
            : QString();
    const double finalPercentage =
        hasFinalGrade
            ? selectedFinalPercentage()
            : -1.0;

    if (semesterStatus == SemesterStatus::Completed &&
        !hasFinalGrade)
    {
        showValidationError(
            tr("A course added to a completed semester must include "
               "its final letter grade.")
        );
        return;
    }

    if (existingCourseID >= 0 && !retake)
    {
        showValidationError(
            tr("A course with this code already exists. Check "
               "'This is a retake' to replace the previous attempt.")
        );
        return;
    }

    if (existingCourseID < 0 && retake)
    {
        showValidationError(
            tr("No previous course with this code was found, so it "
               "cannot be marked as a retake.")
        );
        return;
    }

    bool saved = false;

    if (retake)
    {
        saved = database.retakeCourse(
            existingCourseID,
            semesterID,
            courseCode.toStdString(),
            courseName.toStdString(),
            credits,
            hasFinalGrade,
            letterGrade.toStdString(),
            finalPercentage
        );
    }
    else if (hasFinalGrade)
    {
        saved = database.addCompletedCourse(
            semesterID,
            courseCode.toStdString(),
            courseName.toStdString(),
            credits,
            letterGrade.toStdString(),
            finalPercentage
        );
    }
    else
    {
        saved = database.addCourse(
            semesterID,
            courseCode.toStdString(),
            courseName.toStdString(),
            credits
        );
    }

    if (!saved)
    {
        showValidationError(
            tr("The course could not be saved. Your existing data "
               "was not changed.")
        );
        return;
    }

    accept();
}
