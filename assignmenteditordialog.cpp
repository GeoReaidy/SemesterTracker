#include "assignmenteditordialog.h"
#include "ui_assignmenteditordialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>

#include <exception>

AssignmentEditorDialog::AssignmentEditorDialog(
    DatabaseManager &database,
    int courseID,
    const QString &courseDisplayName,
    QWidget *parent)
    : QDialog(parent),
      database(database),
      courseID(courseID),
      assignmentID(-1),
      editMode(false),
      courseDisplayName(courseDisplayName),
      ui(new Ui::AssignmentEditorDialog)
{
    ui->setupUi(this);
    loadCategories();
    configureForAdd();

    connect(
        ui->hasGradeCheckBox,
        &QCheckBox::toggled,
        this,
        &AssignmentEditorDialog::updateGradeState
    );

    connect(
        ui->hasDueDateCheckBox,
        &QCheckBox::toggled,
        this,
        &AssignmentEditorDialog::updateDueDateState
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &AssignmentEditorDialog::saveAssignment
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );
}

AssignmentEditorDialog::AssignmentEditorDialog(
    DatabaseManager &database,
    int courseID,
    const QString &courseDisplayName,
    const Assignment &assignment,
    QWidget *parent)
    : QDialog(parent),
      database(database),
      courseID(courseID),
      assignmentID(assignment.getID()),
      editMode(true),
      courseDisplayName(courseDisplayName),
      ui(new Ui::AssignmentEditorDialog)
{
    ui->setupUi(this);
    loadCategories();
    configureForEdit(assignment);

    connect(
        ui->hasGradeCheckBox,
        &QCheckBox::toggled,
        this,
        &AssignmentEditorDialog::updateGradeState
    );

    connect(
        ui->hasDueDateCheckBox,
        &QCheckBox::toggled,
        this,
        &AssignmentEditorDialog::updateDueDateState
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &AssignmentEditorDialog::saveAssignment
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );
}

AssignmentEditorDialog::~AssignmentEditorDialog()
{
    delete ui;
}


void AssignmentEditorDialog::loadCategories()
{
    ui->categoryComboBox->clear();
    ui->categoryComboBox->setEditable(true);
    ui->categoryComboBox->setInsertPolicy(QComboBox::NoInsert);
    ui->categoryComboBox->lineEdit()->setPlaceholderText(
        tr("Choose a category or type a custom one")
    );

    const auto categories =
        database.loadAssignmentCategoriesForCourse(courseID);

    for (const AssignmentCategory &category : categories)
    {
        ui->categoryComboBox->addItem(
            QString::fromStdString(category.name),
            category.id
        );
    }
}

void AssignmentEditorDialog::configureForAdd()
{
    setWindowTitle(tr("Add Assignment"));
    ui->dialogTitleLabel->setText(tr("Add Assignment"));
    ui->dialogSubtitleLabel->setText(
        tr("Enter the assignment details in one place.")
    );
    ui->courseValueLabel->setText(courseDisplayName);

    ui->assignmentNameEdit->clear();
    const int otherCategoryIndex =
        ui->categoryComboBox->findText(
            tr("Other"),
            Qt::MatchFixedString
        );

    if (otherCategoryIndex >= 0)
    {
        ui->categoryComboBox->setCurrentIndex(otherCategoryIndex);
    }
    else
    {
        ui->categoryComboBox->setCurrentIndex(-1);
    }
    ui->weightSpinBox->setValue(10);

    ui->hasGradeCheckBox->setChecked(false);
    ui->gradeSpinBox->setValue(0.0);
    updateGradeState(false);

    ui->hasDueDateCheckBox->setChecked(true);
    ui->dueDateEdit->setDate(QDate::currentDate());
    updateDueDateState(true);

    ui->validationLabel->setVisible(false);
    ui->assignmentNameEdit->setFocus();
}

void AssignmentEditorDialog::configureForEdit(
    const Assignment &assignment)
{
    setWindowTitle(tr("Edit Assignment"));
    ui->dialogTitleLabel->setText(tr("Edit Assignment"));
    ui->dialogSubtitleLabel->setText(
        tr("Update the assignment details and save your changes.")
    );
    ui->courseValueLabel->setText(courseDisplayName);

    ui->assignmentNameEdit->setText(
        QString::fromStdString(assignment.getName())
    );
    ui->weightSpinBox->setValue(
        static_cast<int>(assignment.getWeightPercentage())
    );

    const QString categoryName =
        QString::fromStdString(assignment.getCategoryName());
    const int categoryIndex =
        ui->categoryComboBox->findData(assignment.getCategoryID());

    if (categoryIndex >= 0)
    {
        ui->categoryComboBox->setCurrentIndex(categoryIndex);
    }
    else
    {
        ui->categoryComboBox->setCurrentText(categoryName);
    }

    ui->hasGradeCheckBox->setChecked(assignment.hasGrade());
    ui->gradeSpinBox->setValue(
        assignment.hasGrade() ? assignment.getGrade() : 0.0
    );
    updateGradeState(assignment.hasGrade());

    const QDate dueDate = QDate::fromString(
        QString::fromStdString(assignment.getDueDate()),
        Qt::ISODate
    );

    ui->hasDueDateCheckBox->setChecked(
        assignment.hasDueDate() && dueDate.isValid()
    );
    ui->dueDateEdit->setDate(
        dueDate.isValid() ? dueDate : QDate::currentDate()
    );
    updateDueDateState(
        assignment.hasDueDate() && dueDate.isValid()
    );

    ui->validationLabel->setVisible(false);
    ui->assignmentNameEdit->selectAll();
    ui->assignmentNameEdit->setFocus();
}

void AssignmentEditorDialog::updateGradeState(bool enabled)
{
    ui->gradeLabel->setEnabled(enabled);
    ui->gradeSpinBox->setEnabled(enabled);
}

void AssignmentEditorDialog::updateDueDateState(bool enabled)
{
    ui->dueDateLabel->setEnabled(enabled);
    ui->dueDateEdit->setEnabled(enabled);
}

bool AssignmentEditorDialog::assignmentNameAlreadyExists(
    const QString &name,
    int ignoredAssignmentID) const
{
    const QString normalizedName =
        name.trimmed().toCaseFolded();

    const std::vector<Assignment> assignments =
        database.loadAssignmentsForCourse(courseID);

    for (const Assignment &assignment : assignments)
    {
        if (assignment.getID() == ignoredAssignmentID)
        {
            continue;
        }

        const QString existingName =
            QString::fromStdString(
                assignment.getName()
            ).trimmed().toCaseFolded();

        if (existingName == normalizedName)
        {
            return true;
        }
    }

    return false;
}

int AssignmentEditorDialog::totalAssignmentWeight(
    int ignoredAssignmentID) const
{
    int total = 0;

    const std::vector<Assignment> assignments =
        database.loadAssignmentsForCourse(courseID);

    for (const Assignment &assignment : assignments)
    {
        if (assignment.getID() == ignoredAssignmentID)
        {
            continue;
        }

        total += static_cast<int>(
            assignment.getWeightPercentage()
        );
    }

    return total;
}

void AssignmentEditorDialog::showValidationError(
    const QString &message)
{
    ui->validationLabel->setText(message);
    ui->validationLabel->setVisible(true);
}

void AssignmentEditorDialog::clearValidationError()
{
    ui->validationLabel->clear();
    ui->validationLabel->setVisible(false);
}

void AssignmentEditorDialog::saveAssignment()
{
    clearValidationError();

    if (courseID <= 0)
    {
        showValidationError(
            tr("No valid course is selected.")
        );
        return;
    }

    const QString name =
        ui->assignmentNameEdit->text().trimmed();
    const int weight = ui->weightSpinBox->value();
    const double grade =
        ui->hasGradeCheckBox->isChecked()
            ? ui->gradeSpinBox->value()
            : -1.0;

    const std::string dueDate =
        ui->hasDueDateCheckBox->isChecked()
            ? ui->dueDateEdit->date()
                  .toString(Qt::ISODate)
                  .toStdString()
            : std::string();

    QString categoryName =
        ui->categoryComboBox->currentText().trimmed();

    if (categoryName.isEmpty())
    {
        categoryName = tr("Uncategorized");
    }

    const int categoryID =
        database.ensureAssignmentCategoryForCourse(
            courseID,
            categoryName.toStdString()
        );

    if (categoryID <= 0)
    {
        showValidationError(
            tr("The assignment category could not be saved.")
        );
        return;
    }

    try
    {
        const Assignment candidate(
            editMode ? assignmentID : -1,
            name.toStdString(),
            weight,
            grade,
            dueDate,
            false,
            -1.0,
            categoryID,
            categoryName.toStdString()
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

    if (assignmentNameAlreadyExists(
            name,
            editMode ? assignmentID : -1))
    {
        showValidationError(
            tr("Another assignment with this name already exists "
               "in the selected course.")
        );
        return;
    }

    const int otherWeight =
        totalAssignmentWeight(
            editMode ? assignmentID : -1
        );

    if (otherWeight + weight > 100)
    {
        showValidationError(
            tr("The total assignment weight would become %1%. "
               "A course cannot exceed 100%.")
                .arg(otherWeight + weight)
        );
        return;
    }

    bool saved = false;

    if (editMode)
    {
        saved = database.updateAssignment(
            assignmentID,
            name.toStdString(),
            grade,
            weight,
            dueDate,
            categoryID
        );
    }
    else
    {
        saved = database.addAssignment(
            courseID,
            name.toStdString(),
            grade,
            weight,
            dueDate,
            categoryID
        );
    }

    if (!saved)
    {
        showValidationError(
            editMode
                ? tr("The assignment could not be updated. "
                     "Your existing data was not changed.")
                : tr("The assignment could not be saved.")
        );
        return;
    }

    accept();
}
