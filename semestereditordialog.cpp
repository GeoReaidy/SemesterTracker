#include "semestereditordialog.h"
#include "ui_semestereditordialog.h"

#include <QComboBox>
#include <QDate>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>

#include <exception>

namespace
{
void applySemesterEditorStyleRoles(
    Ui::SemesterEditorDialog *ui)
{
    ui->contentCard->setProperty("card", true);
    ui->dialogTitleLabel->setProperty("role", "dialogTitle");
    ui->dialogSubtitleLabel->setProperty("role", "dialogSubtitle");

    ui->termLabel->setProperty("role", "fieldLabel");
    ui->yearLabel->setProperty("role", "fieldLabel");
    ui->completedCreditsLabel->setProperty("role", "fieldLabel");
    ui->semesterGpaLabel->setProperty("role", "fieldLabel");

    ui->completedFieldsFrame->setProperty("role", "infoPanel");
    ui->conversionNoticeLabel->setProperty("role", "notice");
    ui->validationLabel->setProperty("role", "validation");

    if (QPushButton *saveButton =
            ui->buttonBox->button(QDialogButtonBox::Save))
    {
        saveButton->setProperty("role", "primary");
    }

    if (QPushButton *cancelButton =
            ui->buttonBox->button(QDialogButtonBox::Cancel))
    {
        cancelButton->setProperty("role", "secondary");
    }
}
}

SemesterEditorDialog::SemesterEditorDialog(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QDialog(parent),
      database(database),
      userID(userID),
      semesterID(-1),
      editMode(false),
      existingSemesterHasCourses(false),
      ui(new Ui::SemesterEditorDialog)
{
    ui->setupUi(this);
    applySemesterEditorStyleRoles(ui);
    configureForAdd();

    connect(
        ui->completedCheckBox,
        &QCheckBox::toggled,
        this,
        &SemesterEditorDialog::updateCompletedState
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &SemesterEditorDialog::saveSemester
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );
}

SemesterEditorDialog::SemesterEditorDialog(
    DatabaseManager &database,
    int userID,
    const Semester &semester,
    QWidget *parent)
    : QDialog(parent),
      database(database),
      userID(userID),
      semesterID(semester.getID()),
      editMode(true),
      existingSemesterHasCourses(false),
      ui(new Ui::SemesterEditorDialog)
{
    ui->setupUi(this);
    applySemesterEditorStyleRoles(ui);
    configureForEdit(semester);

    connect(
        ui->completedCheckBox,
        &QCheckBox::toggled,
        this,
        &SemesterEditorDialog::updateCompletedState
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &SemesterEditorDialog::saveSemester
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );
}

SemesterEditorDialog::~SemesterEditorDialog()
{
    delete ui;
}

void SemesterEditorDialog::configureForAdd()
{
    setWindowTitle(tr("Add Semester"));
    ui->dialogTitleLabel->setText(tr("Add Semester"));
    ui->dialogSubtitleLabel->setText(
        tr("Enter all semester details in one place.")
    );

    ui->termComboBox->setCurrentIndex(0);
    ui->yearSpinBox->setValue(QDate::currentDate().year());
    ui->completedCheckBox->setChecked(false);
    ui->currentCheckBox->setChecked(false);
    ui->completedCreditsSpinBox->setValue(15);
    ui->semesterGpaSpinBox->setValue(3.00);
    ui->conversionNoticeLabel->setVisible(false);
    ui->validationLabel->setVisible(false);

    updateCompletedState(false);
}

void SemesterEditorDialog::configureForEdit(
    const Semester &semester)
{
    setWindowTitle(tr("Edit Semester"));
    ui->dialogTitleLabel->setText(tr("Edit Semester"));
    ui->dialogSubtitleLabel->setText(
        tr("Update the semester details and save your changes.")
    );

    const QString term =
        QString::fromStdString(semester.getName());

    const int termIndex = ui->termComboBox->findText(term);
    ui->termComboBox->setCurrentIndex(
        termIndex >= 0 ? termIndex : 0
    );

    ui->yearSpinBox->setValue(semester.getYear());
    ui->completedCheckBox->setChecked(semester.isSummaryOnly());
    ui->currentCheckBox->setChecked(semester.isInProgress());
    ui->completedCreditsSpinBox->setValue(
        semester.isSummaryOnly()
            ? semester.getSummaryCredits()
            : 15
    );
    ui->semesterGpaSpinBox->setValue(
        semester.isSummaryOnly()
            ? semester.getSummaryGPA()
            : 3.00
    );

    existingSemesterHasCourses =
        !database.loadCoursesForSemester(semester.getID()).empty();

    if (existingSemesterHasCourses && !semester.isSummaryOnly())
    {
        ui->completedCheckBox->setEnabled(false);
        ui->completedCheckBox->setToolTip(
            tr("A semester containing courses cannot be converted "
               "to a summary-only completed semester.")
        );
        ui->conversionNoticeLabel->setText(
            tr("This semester already contains courses, so its "
               "summary-only status cannot be changed.")
        );
        ui->conversionNoticeLabel->setVisible(true);
    }
    else
    {
        ui->conversionNoticeLabel->setVisible(false);
    }

    ui->validationLabel->setVisible(false);
    updateCompletedState(semester.isSummaryOnly());
}

void SemesterEditorDialog::updateCompletedState(bool completed)
{
    ui->completedFieldsFrame->setVisible(completed);
    ui->currentCheckBox->setEnabled(!completed);

    if (completed)
    {
        ui->currentCheckBox->setChecked(false);
    }
}

bool SemesterEditorDialog::semesterAlreadyExists(
    const QString &term,
    int year) const
{
    const QString normalizedTerm =
        term.trimmed().toCaseFolded();

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    for (const Semester &semester : semesters)
    {
        if (editMode && semester.getID() == semesterID)
        {
            continue;
        }

        const QString existingTerm =
            QString::fromStdString(semester.getName())
                .trimmed()
                .toCaseFolded();

        if (existingTerm == normalizedTerm &&
            semester.getYear() == year)
        {
            return true;
        }
    }

    return false;
}

void SemesterEditorDialog::showValidationError(
    const QString &message)
{
    ui->validationLabel->setText(message);
    ui->validationLabel->setVisible(true);
}

void SemesterEditorDialog::clearValidationError()
{
    ui->validationLabel->clear();
    ui->validationLabel->setVisible(false);
}

void SemesterEditorDialog::saveSemester()
{
    clearValidationError();

    if (userID <= 0)
    {
        showValidationError(
            tr("No logged-in user is available.")
        );
        return;
    }

    const QString term = ui->termComboBox->currentText().trimmed();
    const int year = ui->yearSpinBox->value();
    const bool completed = ui->completedCheckBox->isChecked();
    const bool current =
        !completed && ui->currentCheckBox->isChecked();
    const int completedCredits = completed
        ? ui->completedCreditsSpinBox->value()
        : 0;
    const double semesterGpa = completed
        ? ui->semesterGpaSpinBox->value()
        : 0.0;

    if (term.isEmpty())
    {
        showValidationError(
            tr("Choose a semester term.")
        );
        return;
    }

    if (editMode && existingSemesterHasCourses && completed)
    {
        showValidationError(
            tr("A semester containing courses cannot be converted "
               "to a summary-only semester.")
        );
        return;
    }

    try
    {
        const Semester candidate(
            editMode ? semesterID : -1,
            term.toStdString(),
            year,
            current,
            completed,
            completedCredits,
            semesterGpa
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

    if (semesterAlreadyExists(term, year))
    {
        showValidationError(
            tr("Another semester with this term and year already exists.")
        );
        return;
    }

    bool saved = false;

    if (editMode)
    {
        saved = database.updateSemesterDetails(
            semesterID,
            term.toStdString(),
            year,
            current,
            completed,
            completedCredits,
            semesterGpa
        );
    }
    else if (completed)
    {
        saved = database.addCompletedSemester(
            userID,
            term.toStdString(),
            year,
            completedCredits,
            semesterGpa
        );
    }
    else
    {
        saved = database.addSemester(
            userID,
            term.toStdString(),
            year,
            current
        );
    }

    if (!saved)
    {
        QMessageBox::warning(
            this,
            tr("Database Error"),
            tr("The semester could not be saved. Your existing data "
               "was not changed.")
        );
        return;
    }

    accept();
}
