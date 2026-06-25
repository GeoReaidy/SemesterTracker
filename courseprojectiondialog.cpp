#include "courseprojectiondialog.h"
#include "ui_courseprojectiondialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

#include <algorithm>
#include <utility>

namespace
{
QString percentText(double value)
{
    return QStringLiteral("%1%")
        .arg(value, 0, 'f', 2);
}
}

CourseProjectionDialog::CourseProjectionDialog(
    DatabaseManager &database,
    Course course,
    QWidget *parent)
    : QDialog(parent),
      database(database),
      course(std::move(course)),
      ui(new Ui::CourseProjectionDialog)
{
    ui->setupUi(this);

    setWindowTitle(
        tr("Project %1")
            .arg(QString::fromStdString(
                this->course.getCode()
            ))
    );

    ui->dialogTitleLabel->setText(
        tr("%1 — %2")
            .arg(QString::fromStdString(
                this->course.getCode()
            ))
            .arg(QString::fromStdString(
                this->course.getName()
            ))
    );

    ui->assignmentsTableWidget->setColumnCount(4);
    ui->assignmentsTableWidget->setHorizontalHeaderLabels(
        QStringList{
            tr("Assignment"),
            tr("Weight"),
            tr("Actual grade"),
            tr("Projected grade")
        }
    );

    ui->assignmentsTableWidget
        ->horizontalHeader()
        ->setSectionResizeMode(
            0,
            QHeaderView::Stretch
        );

    ui->assignmentsTableWidget
        ->horizontalHeader()
        ->setSectionResizeMode(
            1,
            QHeaderView::ResizeToContents
        );

    ui->assignmentsTableWidget
        ->horizontalHeader()
        ->setSectionResizeMode(
            2,
            QHeaderView::ResizeToContents
        );

    ui->assignmentsTableWidget
        ->horizontalHeader()
        ->setSectionResizeMode(
            3,
            QHeaderView::ResizeToContents
        );

    ui->assignmentsTableWidget->setRowCount(
        static_cast<int>(
            this->course
                .getAssignments()
                .size()
        )
    );

    // Give each projection editor enough vertical room for the styled
    // checkbox and spin-box controls.
    ui->assignmentsTableWidget->verticalHeader()->setDefaultSectionSize(46);
    ui->assignmentsTableWidget->horizontalHeader()->setMinimumHeight(40);

    int row = 0;

    for (const Assignment &assignment :
         this->course.getAssignments())
    {
        auto *nameItem = new QTableWidgetItem(
            QString::fromStdString(
                assignment.getName()
            )
        );

        nameItem->setFlags(
            nameItem->flags() &
            ~Qt::ItemIsEditable
        );

        auto *weightItem = new QTableWidgetItem(
            tr("%1%")
                .arg(
                    assignment
                        .getWeightPercentage(),
                    0,
                    'f',
                    0
                )
        );

        weightItem->setTextAlignment(
            Qt::AlignCenter
        );

        weightItem->setFlags(
            weightItem->flags() &
            ~Qt::ItemIsEditable
        );

        auto *actualItem = new QTableWidgetItem(
            assignment.hasGrade()
                ? percentText(
                      assignment.getGrade()
                  )
                : tr("Not graded")
        );

        actualItem->setTextAlignment(
            Qt::AlignCenter
        );

        actualItem->setFlags(
            actualItem->flags() &
            ~Qt::ItemIsEditable
        );

        ui->assignmentsTableWidget->setItem(
            row,
            0,
            nameItem
        );

        ui->assignmentsTableWidget->setItem(
            row,
            1,
            weightItem
        );

        ui->assignmentsTableWidget->setItem(
            row,
            2,
            actualItem
        );

        ProjectionEditor editor;
        editor.assignmentID =
            assignment.getID();
        editor.storedProjection =
            assignment.getProjectedGrade();

        if (assignment.hasGrade())
        {
            auto *actualUsedLabel =
                new QLabel(
                    tr("Actual grade used"),
                    ui->assignmentsTableWidget
                );

            actualUsedLabel->setAlignment(
                Qt::AlignCenter
            );

            actualUsedLabel->setStyleSheet(
                QStringLiteral(
                    "color: #64748b;"
                    "font-weight: 600;"
                )
            );

            ui->assignmentsTableWidget
                ->setCellWidget(
                    row,
                    3,
                    actualUsedLabel
                );
        }
        else
        {
            auto *editorWidget =
                new QWidget(
                    ui->assignmentsTableWidget
                );

            auto *editorLayout =
                new QHBoxLayout(editorWidget);

            editorLayout->setContentsMargins(
                8,
                5,
                8,
                5
            );

            editorLayout->setSpacing(10);

            editor.enabledCheckBox =
                new QCheckBox(
                    tr("Use"),
                    editorWidget
                );

            editor.gradeSpinBox =
                new QDoubleSpinBox(
                    editorWidget
                );

            editor.gradeSpinBox->setRange(
                0.0,
                100.0
            );

            editor.gradeSpinBox->setDecimals(2);
            editor.gradeSpinBox->setSuffix(
                QStringLiteral("%")
            );

            editor.gradeSpinBox->setSingleStep(
                1.0
            );

            editor.gradeSpinBox->setMinimumWidth(180);
            editor.gradeSpinBox->setMinimumHeight(36);

            const bool hasProjection =
                assignment.hasProjectedGrade();

            editor.enabledCheckBox->setChecked(
                hasProjection
            );

            editor.gradeSpinBox->setEnabled(
                hasProjection
            );

            editor.gradeSpinBox->setValue(
                hasProjection
                    ? assignment
                          .getProjectedGrade()
                    : 75.0
            );

            editorLayout->addWidget(
                editor.enabledCheckBox
            );

            editorLayout->addWidget(
                editor.gradeSpinBox
            );

            ui->assignmentsTableWidget
                ->setCellWidget(
                    row,
                    3,
                    editorWidget
                );

            connect(
                editor.enabledCheckBox,
                &QCheckBox::toggled,
                editor.gradeSpinBox,
                &QDoubleSpinBox::setEnabled
            );

            connect(
                editor.enabledCheckBox,
                &QCheckBox::toggled,
                this,
                &CourseProjectionDialog::
                    refreshProjection
            );

            connect(
                editor.gradeSpinBox,
                qOverload<double>(
                    &QDoubleSpinBox::valueChanged
                ),
                this,
                &CourseProjectionDialog::
                    refreshProjection
            );
        }

        editors.push_back(editor);
        ++row;
    }

    ui->targetGradeCheckBox->setChecked(
        this->course.hasTargetGrade()
    );

    ui->targetGradeSpinBox->setEnabled(
        this->course.hasTargetGrade()
    );

    ui->targetGradeSpinBox->setValue(
        this->course.hasTargetGrade()
            ? this->course.getTargetGrade()
            : 85.0
    );

    connect(
        ui->targetGradeCheckBox,
        &QCheckBox::toggled,
        ui->targetGradeSpinBox,
        &QDoubleSpinBox::setEnabled
    );

    connect(
        ui->targetGradeCheckBox,
        &QCheckBox::toggled,
        this,
        &CourseProjectionDialog::
            refreshProjection
    );

    connect(
        ui->targetGradeSpinBox,
        qOverload<double>(
            &QDoubleSpinBox::valueChanged
        ),
        this,
        &CourseProjectionDialog::
            refreshProjection
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &CourseProjectionDialog::
            saveProjection
    );

    connect(
        ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );

    auto applyButtonRole = [](QPushButton *button,
                              const char *role)
    {
        if (!button)
        {
            return;
        }

        button->setProperty("role", role);

        // The buttons are created and polished by QDialogButtonBox before
        // these dynamic properties are assigned. Re-polish them so the
        // global AppStyle selectors (for example role="primary") take effect.
        button->style()->unpolish(button);
        button->style()->polish(button);
        button->update();
    };

    QPushButton *saveButton =
        ui->buttonBox->button(
            QDialogButtonBox::Save
        );

    QPushButton *cancelButton =
        ui->buttonBox->button(
            QDialogButtonBox::Cancel
        );

    applyButtonRole(saveButton, "primary");
    applyButtonRole(cancelButton, "secondary");

    if (saveButton)
    {
        saveButton->setDefault(true);
        saveButton->setAutoDefault(true);
    }

    if (this->course.isExcludedFromCGPA())
    {
        ui->validationLabel->setText(
            tr("Historical attempts are read-only.")
        );

        ui->validationLabel->setVisible(true);
        ui->targetGradeCheckBox->setEnabled(false);
        ui->targetGradeSpinBox->setEnabled(false);

        for (ProjectionEditor &editor : editors)
        {
            if (editor.enabledCheckBox)
            {
                editor.enabledCheckBox
                    ->setEnabled(false);
            }

            if (editor.gradeSpinBox)
            {
                editor.gradeSpinBox
                    ->setEnabled(false);
            }
        }

        if (QPushButton *saveButton =
                ui->buttonBox->button(
                    QDialogButtonBox::Save
                ))
        {
            saveButton->setEnabled(false);
        }
    }

    refreshProjection();
}

CourseProjectionDialog::~CourseProjectionDialog()
{
    delete ui;
}

Course CourseProjectionDialog::courseFromEditors() const
{
    Course projectedCourse = course;

    projectedCourse.setTargetGrade(
        ui->targetGradeCheckBox->isChecked()
            ? ui->targetGradeSpinBox->value()
            : -1.0
    );

    for (const ProjectionEditor &editor :
         editors)
    {
        Assignment *assignment =
            projectedCourse.findAssignment(
                editor.assignmentID
            );

        if (!assignment ||
            assignment->hasGrade())
        {
            continue;
        }

        assignment->setProjectedGrade(
            editor.enabledCheckBox &&
                    editor.enabledCheckBox
                        ->isChecked()
                ? editor.gradeSpinBox->value()
                : -1.0
        );
    }

    return projectedCourse;
}

QString CourseProjectionDialog::targetMessage(
    const CourseProjectionResult &result) const
{
    switch (result.targetState)
    {
    case ProjectionTargetState::NoTarget:
        return tr(
            "Set a target to calculate the average needed "
            "across the remaining course weight."
        );

    case ProjectionTargetState::AlreadyReached:
        return tr(
            "The target is already secured by the grades entered."
        );

    case ProjectionTargetState::Achievable:
        return tr(
            "You need an average of %1% across the remaining %2%."
        )
            .arg(
                result.requiredRemainingAverage,
                0,
                'f',
                2
            )
            .arg(
                std::max(
                    0,
                    100 - result.actualWeight
                )
            );

    case ProjectionTargetState::Impossible:
        return tr(
            "The target would require %1% across the remaining work."
        )
            .arg(
                result.requiredRemainingAverage,
                0,
                'f',
                2
            );

    case ProjectionTargetState::Complete:
        return tr(
            "All course weight is graded. The final calculated grade is %1%."
        )
            .arg(
                result.currentGrade,
                0,
                'f',
                2
            );
    }

    return {};
}

void CourseProjectionDialog::refreshProjection()
{
    const Course projectedCourse =
        courseFromEditors();

    const CourseProjectionResult result =
        GradeProjection::calculateCourse(
            projectedCourse
        );

    ui->currentGradeValueLabel->setText(
        result.hasActualGrades
            ? percentText(
                  result.currentGrade
              )
            : tr("—")
    );

    ui->projectedGradeValueLabel->setText(
        result.hasProjectedResult
            ? tr("%1% • %2 • %3 / 4.00")
                  .arg(
                      result.projectedGrade,
                      0,
                      'f',
                      2
                  )
                  .arg(
                      QString::fromStdString(
                          GradeProjection::
                              percentageToLetterGrade(
                                  result.projectedGrade
                              )
                      )
                  )
                  .arg(
                      result.projectedGpa,
                      0,
                      'f',
                      2
                  )
            : tr("—")
    );

    ui->coverageValueLabel->setText(
        tr("%1% covered • %2% unresolved")
            .arg(result.coveredWeight)
            .arg(result.unresolvedWeight)
    );

    ui->targetResultLabel->setText(
        targetMessage(result)
    );

    const bool impossible =
        result.targetState ==
        ProjectionTargetState::Impossible;

    ui->targetResultLabel->setStyleSheet(
        impossible
            ? QStringLiteral(
                  "color: #b91c1c;"
                  "font-weight: 600;"
              )
            : QStringLiteral(
                  "color: #475569;"
                  "font-weight: 600;"
              )
    );
}

void CourseProjectionDialog::saveProjection()
{
    if (course.isExcludedFromCGPA())
    {
        return;
    }

    std::vector<std::pair<int, double>>
        projections;

    projections.reserve(editors.size());

    for (const ProjectionEditor &editor :
         editors)
    {
        double projectedGrade =
            editor.storedProjection;

        if (editor.enabledCheckBox &&
            editor.gradeSpinBox)
        {
            projectedGrade =
                editor.enabledCheckBox
                        ->isChecked()
                    ? editor.gradeSpinBox
                          ->value()
                    : -1.0;
        }

        projections.emplace_back(
            editor.assignmentID,
            projectedGrade
        );
    }

    const double targetGrade =
        ui->targetGradeCheckBox->isChecked()
            ? ui->targetGradeSpinBox->value()
            : -1.0;

    if (!database.saveCourseProjection(
            course.getID(),
            targetGrade,
            projections))
    {
        QMessageBox::warning(
            this,
            tr("Projection Not Saved"),
            tr("The course projection could not be saved.")
        );
        return;
    }

    accept();
}
