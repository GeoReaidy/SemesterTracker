#include "gradeswindow.h"
#include "ui_gradeswindow.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProgressBar>
#include <QString>
#include <QVBoxLayout>

#include <algorithm>

namespace
{
constexpr int UniformRowHeight = 88;
}

GradesWindow::GradesWindow(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      userID(userID),
      ui(new Ui::GradesWindow)
{
    ui->setupUi(this);

    /*
     * Apply popup styling directly to the view.
     * This prevents white-on-white dropdown text on Windows.
     */
    const QString comboPopupStyle = R"(
        QListView {
            background-color: #ffffff;
            color: #1f2937;
            border: 1px solid #cbd5e1;
            outline: none;
            padding: 4px;
        }

        QListView::item {
            min-height: 30px;
            padding: 4px 8px;
            color: #1f2937;
            background-color: #ffffff;
        }

        QListView::item:hover {
            color: #1f2937;
            background-color: #eff6ff;
        }

        QListView::item:selected {
            color: #1f2937;
            background-color: #dbeafe;
        }
    )";

    ui->semesterComboBox->view()->setStyleSheet(
        comboPopupStyle
    );

    ui->semesterComboBox->setMaxVisibleItems(10);

    /*
     * The QListWidget item itself stays transparent.
     * Each custom row draws exactly one outer card, preventing
     * the "box inside another box" issue.
     */
    ui->courseGradesListWidget->setStyleSheet(R"(
        QListWidget {
            background-color: transparent;
            border: none;
            outline: none;
        }

        QListWidget::item {
            margin: 4px 0;
            padding: 0;
            border: none;
            background-color: transparent;
        }

        QListWidget::item:hover,
        QListWidget::item:selected {
            color: #1f2937;
            background-color: transparent;
        }
    )");

    connect(
        ui->semesterComboBox,
        &QComboBox::currentIndexChanged,
        this,
        &GradesWindow::handleSemesterChanged
    );

    refreshSemesters();
}

GradesWindow::~GradesWindow()
{
    delete ui;
}

void GradesWindow::setUserID(int newUserID)
{
    userID = newUserID;
    refreshSemesters();
}

void GradesWindow::refreshSemesters()
{
    const int previousSemesterID =
        selectedSemesterID();

    ui->semesterComboBox->blockSignals(true);
    ui->semesterComboBox->clear();

    if (userID < 0)
    {
        ui->semesterComboBox->addItem(
            "No user selected",
            -1
        );

        ui->semesterComboBox->setEnabled(false);
        ui->semesterComboBox->blockSignals(false);

        refreshGrades();
        return;
    }

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    int previousIndex = -1;
    int currentIndex = -1;

    for (const Semester &semester : semesters)
    {
        const QString label =
            QString("%1 %2")
                .arg(QString::fromStdString(
                    semester.getName()
                ))
                .arg(semester.getYear());

        ui->semesterComboBox->addItem(
            label,
            semester.getID()
        );

        const int index =
            ui->semesterComboBox->count() - 1;

        if (semester.getID() == previousSemesterID)
        {
            previousIndex = index;
        }

        if (semester.isInProgress())
        {
            currentIndex = index;
        }
    }

    if (semesters.empty())
    {
        ui->semesterComboBox->addItem(
            "Create a semester first",
            -1
        );

        ui->semesterComboBox->setEnabled(false);
    }
    else
    {
        ui->semesterComboBox->setEnabled(true);

        if (previousIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(
                previousIndex
            );
        }
        else if (currentIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(
                currentIndex
            );
        }
        else
        {
            ui->semesterComboBox->setCurrentIndex(0);
        }
    }

    ui->semesterComboBox->blockSignals(false);
    refreshGrades();
}

void GradesWindow::refreshGrades()
{
    ui->courseGradesListWidget->clear();

    const int semesterID = selectedSemesterID();

    if (semesterID < 0)
    {
        updateSummary({});
        updateEmptyState();
        return;
    }

    const std::vector<Course> courses =
        database.loadCoursesForSemester(semesterID);

    for (const Course &course : courses)
    {
        const CourseGradeResult result =
            calculateCourseGrade(course);

        addCourseGradeRow(course, result);
    }

    updateSummary(courses);
    updateEmptyState();
}

void GradesWindow::handleSemesterChanged(int index)
{
    Q_UNUSED(index);
    refreshGrades();
}

int GradesWindow::selectedSemesterID() const
{
    if (!ui ||
        !ui->semesterComboBox ||
        ui->semesterComboBox->count() == 0)
    {
        return -1;
    }

    return ui->semesterComboBox
        ->currentData()
        .toInt();
}

GradesWindow::CourseGradeResult
GradesWindow::calculateCourseGrade(
    const Course &course) const
{
    CourseGradeResult result;

    const std::vector<Assignment> assignments =
        database.loadAssignmentsForCourse(
            course.getID()
        );

    double weightedGradeTotal = 0.0;
    int totalWeight = 0;

    for (const Assignment &assignment : assignments)
    {
        if (!assignment.hasGrade())
        {
            continue;
        }

        const int weight =
            assignment.getWeightPercentage();

        if (weight <= 0)
        {
            continue;
        }

        weightedGradeTotal +=
            assignment.getGrade() * weight;

        totalWeight += weight;
    }

    result.totalWeight = totalWeight;

    if (totalWeight <= 0)
    {
        return result;
    }

    /*
     * Normalize by the weight currently entered.
     * Example: one assignment worth 20% with a grade of 90%
     * displays a current course grade of 90%, not 18%.
     */
    result.hasGrades = true;
    result.percentage =
        weightedGradeTotal
        / static_cast<double>(totalWeight);

    result.percentage =
        std::clamp(result.percentage, 0.0, 100.0);

    result.gpa =
        percentageToGpa(result.percentage);

    return result;
}

void GradesWindow::addCourseGradeRow(
    const Course &course,
    const CourseGradeResult &result)
{
    auto *item =
        new QListWidgetItem(
            ui->courseGradesListWidget
        );

    auto *rowWidget =
        new QWidget(
            ui->courseGradesListWidget
        );

    rowWidget->setObjectName("gradeRow");
    rowWidget->setMinimumHeight(UniformRowHeight);

    rowWidget->setStyleSheet(R"(
        QWidget#gradeRow {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 9px;
        }

        QWidget#gradeRow QWidget,
        QWidget#gradeRow QLabel,
        QWidget#gradeRow QProgressBar {
            background-color: transparent;
            border: none;
        }
    )");

    auto *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(14, 0, 14, 0);
    rowLayout->setSpacing(14);
    rowLayout->setAlignment(Qt::AlignVCenter);

    auto *textContainer = new QWidget(rowWidget);
    textContainer->setStyleSheet(
        "background: transparent;"
        "border: none;"
    );

    auto *textLayout =
        new QVBoxLayout(textContainer);

    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(4);

    auto *courseLabel = new QLabel(
        QString("%1 — %2")
            .arg(QString::fromStdString(
                course.getCode()
            ))
            .arg(QString::fromStdString(
                course.getName()
            )),
        textContainer
    );

    courseLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    courseLabel->setStyleSheet(
        "color: #111827;"
        "font-size: 15px;"
        "font-weight: 600;"
        "border: none;"
        "background: transparent;"
    );

    QString detailsText =
        result.hasGrades
            ? QString("Assignment weight entered: %1%")
                  .arg(result.totalWeight)
            : tr("Add graded assignments to calculate this course.");

    if (course.isExcludedFromCGPA())
    {
        detailsText += tr(
            "  •  Historical attempt — excluded from CGPA"
        );
    }
    else if (course.isRetaken())
    {
        detailsText += tr(
            "  •  Retake attempt"
        );
    }

    auto *detailsLabel = new QLabel(
        detailsText,
        textContainer
    );

    detailsLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    detailsLabel->setStyleSheet(
        "color: #64748b;"
        "font-size: 13px;"
        "border: none;"
        "background: transparent;"
    );

    textLayout->addWidget(courseLabel);
    textLayout->addWidget(detailsLabel);

    auto *gradeContainer = new QWidget(rowWidget);
    gradeContainer->setStyleSheet(
        "background: transparent;"
        "border: none;"
    );

    auto *gradeLayout =
        new QVBoxLayout(gradeContainer);

    gradeLayout->setContentsMargins(0, 0, 0, 0);
    gradeLayout->setSpacing(4);
    gradeLayout->setAlignment(Qt::AlignVCenter);

    const QString gradeDisplayText =
        result.hasGrades
            ? QString("%1%  •  %2  •  %3 / 4.00")
                  .arg(result.percentage, 0, 'f', 2)
                  .arg(percentageToLetterGrade(
                      result.percentage
                  ))
                  .arg(result.gpa, 0, 'f', 2)
            : "No graded assignments";

    auto *gradeLabel = new QLabel(
        gradeDisplayText,
        gradeContainer
    );

    gradeLabel->setAlignment(
        Qt::AlignRight | Qt::AlignVCenter
    );

    gradeLabel->setStyleSheet(
        result.hasGrades
            ? "color: #2563eb;"
              "font-size: 14px;"
              "font-weight: 700;"
              "border: none;"
              "background: transparent;"
            : "color: #94a3b8;"
              "font-size: 13px;"
              "font-weight: 600;"
              "border: none;"
              "background: transparent;"
    );

    auto *progressBar =
        new QProgressBar(gradeContainer);

    progressBar->setRange(0, 100);
    progressBar->setValue(
        result.hasGrades
            ? static_cast<int>(result.percentage)
            : 0
    );

    progressBar->setTextVisible(false);
    progressBar->setFixedSize(190, 8);

    progressBar->setStyleSheet(R"(
        QProgressBar {
            background-color: #e5e7eb;
            border: none;
            border-radius: 4px;
        }

        QProgressBar::chunk {
            background-color: #2563eb;
            border-radius: 4px;
        }
    )");

    gradeLayout->addWidget(
        gradeLabel,
        0,
        Qt::AlignRight
    );

    gradeLayout->addWidget(
        progressBar,
        0,
        Qt::AlignRight
    );

    rowLayout->addWidget(
        textContainer,
        1,
        Qt::AlignVCenter
    );

    rowLayout->addWidget(
        gradeContainer,
        0,
        Qt::AlignVCenter
    );

    item->setSizeHint(
        QSize(0, UniformRowHeight)
    );

    ui->courseGradesListWidget->setItemWidget(
        item,
        rowWidget
    );
}

void GradesWindow::updateSummary(
    const std::vector<Course> &courses)
{
    double creditWeightedGpaTotal = 0.0;
    int gradedCredits = 0;

    double percentageTotal = 0.0;
    int gradedCourseCount = 0;
    int completedCourseCount = 0;

    for (const Course &course : courses)
    {
        if (course.isCompleted())
        {
            ++completedCourseCount;
        }

        if (course.isWithdrawn())
        {
            continue;
        }

        const CourseGradeResult result =
            calculateCourseGrade(course);

        if (!result.hasGrades)
        {
            continue;
        }

        ++gradedCourseCount;
        percentageTotal += result.percentage;

        const int credits =
            std::max(course.getCredits(), 0);

        creditWeightedGpaTotal +=
            result.gpa * credits;

        gradedCredits += credits;
    }

    ui->completedCoursesValueLabel->setText(
        QString::number(completedCourseCount)
    );

    if (gradedCourseCount == 0)
    {
        ui->semesterGpaValueLabel->setText("—");
        ui->averageGradeValueLabel->setText("—");
        return;
    }

    const double averagePercentage =
        percentageTotal
        / static_cast<double>(gradedCourseCount);

    const double semesterGpa =
        gradedCredits > 0
            ? creditWeightedGpaTotal
              / static_cast<double>(gradedCredits)
            : percentageToGpa(averagePercentage);

    ui->semesterGpaValueLabel->setText(
        QString("%1 / 4.00")
            .arg(semesterGpa, 0, 'f', 2)
    );

    ui->averageGradeValueLabel->setText(
        QString("%1%")
            .arg(averagePercentage, 0, 'f', 2)
    );
}

void GradesWindow::updateEmptyState()
{
    const int courseCount =
        ui->courseGradesListWidget->count();

    ui->courseCountLabel->setText(
        courseCountText(courseCount)
    );

    const bool hasCourses = courseCount > 0;
    const bool hasSemester =
        selectedSemesterID() >= 0;

    ui->courseGradesListWidget->setVisible(
        hasCourses
    );

    ui->emptyStateTitleLabel->setVisible(
        !hasCourses
    );

    ui->emptyStateLabel->setVisible(
        !hasCourses
    );

    if (!hasSemester)
    {
        ui->emptyStateTitleLabel->setText(
            "No semester selected"
        );

        ui->emptyStateLabel->setText(
            "Create or select a semester first."
        );
    }
    else
    {
        ui->emptyStateTitleLabel->setText(
            "No courses to grade"
        );

        ui->emptyStateLabel->setText(
            "Add courses and assignments to this semester first."
        );
    }
}

double GradesWindow::percentageToGpa(
    double percentage)
{
    if (percentage >= 90.0) return 4.0;
    if (percentage >= 87.0) return 3.7;
    if (percentage >= 83.0) return 3.3;
    if (percentage >= 80.0) return 3.0;
    if (percentage >= 77.0) return 2.7;
    if (percentage >= 73.0) return 2.3;
    if (percentage >= 70.0) return 2.0;
    if (percentage >= 67.0) return 1.7;
    if (percentage >= 63.0) return 1.3;
    if (percentage >= 60.0) return 1.0;
    return 0.0;
}

QString GradesWindow::percentageToLetterGrade(
    double percentage)
{
    if (percentage >= 90.0) return "A";
    if (percentage >= 87.0) return "A-";
    if (percentage >= 83.0) return "B+";
    if (percentage >= 80.0) return "B";
    if (percentage >= 77.0) return "B-";
    if (percentage >= 73.0) return "C+";
    if (percentage >= 70.0) return "C";
    if (percentage >= 67.0) return "C-";
    if (percentage >= 63.0) return "D+";
    if (percentage >= 60.0) return "D";
    return "F";
}

QString GradesWindow::courseCountText(int count)
{
    return QString("%1 course%2")
        .arg(count)
        .arg(count == 1 ? "" : "s");
}
