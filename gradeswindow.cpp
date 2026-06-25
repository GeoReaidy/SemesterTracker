#include "gradeswindow.h"
#include "courseprojectiondialog.h"
#include "gpatrendchart.h"
#include "ui_gradeswindow.h"

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProgressBar>
#include <QPushButton>
#include <QSize>
#include <QVBoxLayout>

#include <algorithm>
#include <exception>

namespace
{
constexpr int UniformRowHeight = 82;

QString gradeText(
    double percentage,
    double gpa)
{
    return QString("%1%  •  %2  •  %3 / 4.00")
        .arg(percentage, 0, 'f', 2)
        .arg(
            QString::fromStdString(
                GradeProjection::
                    percentageToLetterGrade(
                        percentage
                    )
            )
        )
        .arg(gpa, 0, 'f', 2);
}
}

GradesWindow::GradesWindow(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      userID(userID),
      ui(new Ui::GradesWindow),
      trendChart(nullptr)
{
    ui->setupUi(this);

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

    auto *chartLayout =
        new QVBoxLayout(
            ui->trendChartContainer
        );

    chartLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    trendChart =
        new GpaTrendChart(
            ui->trendChartContainer
        );

    chartLayout->addWidget(trendChart);

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
            tr("No user selected"),
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
        QString label =
            QString("%1 %2")
                .arg(
                    QString::fromStdString(
                        semester.getName()
                    )
                )
                .arg(semester.getYear());

        if (semester.isInProgress())
        {
            label += tr(" — active");
        }
        else if (semester.isCompleted())
        {
            label += tr(" — completed");
        }
        else
        {
            label += tr(" — planned");
        }

        ui->semesterComboBox->addItem(
            label,
            semester.getID()
        );

        const int index =
            ui->semesterComboBox->count() - 1;

        if (semester.getID() ==
            previousSemesterID)
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
            tr("Create a semester first"),
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

    if (userID < 0)
    {
        ui->semesterGpaValueLabel->setText(
            tr("—")
        );
        ui->averageGradeValueLabel->setText(
            tr("—")
        );
        ui->completedCoursesValueLabel->setText(
            tr("—")
        );
        ui->projectedCgpaValueLabel->setText(
            tr("—")
        );
        trendChart->setPoints({});
        updateEmptyState();
        return;
    }

    try
    {
        const User user =
            database.loadFullUserByID(userID);

        const Semester *selectedSemester =
            nullptr;

        const int semesterID =
            selectedSemesterID();

        for (const Semester &semester :
             user.getSemesters())
        {
            if (semester.getID() ==
                semesterID)
            {
                selectedSemester =
                    &semester;
                break;
            }
        }

        if (selectedSemester != nullptr &&
            !selectedSemester->isSummaryOnly())
        {
            for (const Course &course :
                 selectedSemester->getCourses())
            {
                const CourseProjectionResult result =
                    GradeProjection::calculateCourse(
                        course
                    );

                addCourseGradeRow(
                    course,
                    result
                );
            }
        }

        updateSummary(
            user,
            selectedSemester
        );

        updateTrend(user);
    }
    catch (const std::exception &)
    {
        ui->semesterGpaValueLabel->setText(
            tr("—")
        );
        ui->averageGradeValueLabel->setText(
            tr("—")
        );
        ui->completedCoursesValueLabel->setText(
            tr("—")
        );
        ui->projectedCgpaValueLabel->setText(
            tr("—")
        );
        trendChart->setPoints({});
    }

    updateEmptyState();
}

void GradesWindow::handleSemesterChanged(
    int index)
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

void GradesWindow::addCourseGradeRow(
    const Course &course,
    const CourseProjectionResult &result)
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
    rowWidget->setMinimumHeight(
        UniformRowHeight
    );
    rowWidget->setAttribute(
        Qt::WA_StyledBackground,
        true
    );

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

    auto *rowLayout =
        new QHBoxLayout(rowWidget);

    rowLayout->setContentsMargins(
        14,
        9,
        12,
        9
    );
    rowLayout->setSpacing(12);
    rowLayout->setAlignment(
        Qt::AlignVCenter
    );

    auto *textContainer =
        new QWidget(rowWidget);

    auto *textLayout =
        new QVBoxLayout(textContainer);

    textLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );
    textLayout->setSpacing(4);

    auto *courseLabel = new QLabel(
        QString("%1 — %2")
            .arg(
                QString::fromStdString(
                    course.getCode()
                )
            )
            .arg(
                QString::fromStdString(
                    course.getName()
                )
            ),
        textContainer
    );

    courseLabel->setStyleSheet(
        QStringLiteral(
            "color: #111827;"
            "font-size: 14px;"
            "font-weight: 600;"
        )
    );

    QString detail =
        tr("%1 credit%2 • %3% assignment weight configured")
            .arg(course.getCredits())
            .arg(
                course.getCredits() == 1
                    ? QString()
                    : QStringLiteral("s")
            )
            .arg(result.configuredWeight);

    if (course.isExcludedFromCGPA())
    {
        detail += tr(
            " • Historical — excluded from CGPA"
        );
    }
    else if (course.isRetaken())
    {
        detail += tr(" • Retake");
    }

    auto *detailsLabel =
        new QLabel(
            detail,
            textContainer
        );

    detailsLabel->setStyleSheet(
        QStringLiteral(
            "color: #64748b;"
            "font-size: 12px;"
        )
    );

    textLayout->addWidget(courseLabel);
    textLayout->addWidget(detailsLabel);

    auto *gradeContainer =
        new QWidget(rowWidget);

    // Keep enough horizontal room for the complete current/projected
    // grade strings. A fixed 205 px width clipped the "Projected:" text.
    gradeContainer->setMinimumWidth(300);
    gradeContainer->setSizePolicy(
        QSizePolicy::Minimum,
        QSizePolicy::Preferred
    );

    auto *gradeLayout =
        new QVBoxLayout(
            gradeContainer
        );

    gradeLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );
    gradeLayout->setSpacing(3);

    const QString currentText =
        result.hasActualGrades
            ? tr("Current: %1")
                  .arg(
                      gradeText(
                          result.currentGrade,
                          GradeProjection::
                              percentageToGpa(
                                  result.currentGrade
                              )
                      )
                  )
            : tr("Current: —");

    const QString projectedText =
        result.hasProjectedResult
            ? tr("Projected: %1")
                  .arg(
                      gradeText(
                          result.projectedGrade,
                          result.projectedGpa
                      )
                  )
            : tr("Projected: —");

    auto *currentLabel =
        new QLabel(
            currentText,
            gradeContainer
        );

    auto *projectedLabel =
        new QLabel(
            projectedText,
            gradeContainer
        );

    currentLabel->setAlignment(
        Qt::AlignRight |
        Qt::AlignVCenter
    );

    projectedLabel->setAlignment(
        Qt::AlignRight |
        Qt::AlignVCenter
    );

    currentLabel->setWordWrap(false);
    projectedLabel->setWordWrap(false);
    currentLabel->setMinimumWidth(300);
    projectedLabel->setMinimumWidth(300);

    currentLabel->setStyleSheet(
        QStringLiteral(
            "color: #475569;"
            "font-size: 12px;"
            "font-weight: 600;"
        )
    );

    projectedLabel->setStyleSheet(
        result.hasProjectedResult
            ? QStringLiteral(
                  "color: #2563eb;"
                  "font-size: 13px;"
                  "font-weight: 700;"
              )
            : QStringLiteral(
                  "color: #94a3b8;"
                  "font-size: 12px;"
                  "font-weight: 600;"
              )
    );

    auto *progressBar =
        new QProgressBar(
            gradeContainer
        );

    progressBar->setRange(0, 100);
    progressBar->setValue(
        result.hasProjectedResult
            ? static_cast<int>(
                  result.projectedGrade
              )
            : result.hasActualGrades
                  ? static_cast<int>(
                        result.currentGrade
                    )
                  : 0
    );

    progressBar->setTextVisible(false);
    progressBar->setFixedSize(180, 6);
    progressBar->setVisible(
        result.hasProjectedResult ||
        result.hasActualGrades
    );

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
        currentLabel,
        0,
        Qt::AlignRight
    );

    gradeLayout->addWidget(
        projectedLabel,
        0,
        Qt::AlignRight
    );

    gradeLayout->addWidget(
        progressBar,
        0,
        Qt::AlignRight
    );

    auto *projectionButton =
        new QPushButton(
            tr("Projection"),
            rowWidget
        );

    projectionButton->setFixedHeight(32);
    projectionButton->setMinimumWidth(86);
    projectionButton->setCursor(
        Qt::PointingHandCursor
    );
    projectionButton->setStyleSheet(R"(
        QPushButton {
            color: #6d28d9;
            background-color: #f5f3ff;
            border: 1px solid #ddd6fe;
            border-radius: 8px;
            padding: 0 12px;
            font-size: 12px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #ede9fe;
            border-color: #c4b5fd;
        }
    )");

    if (course.isExcludedFromCGPA() ||
        course.isWithdrawn())
    {
        projectionButton->setEnabled(false);
    }

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

    rowLayout->addWidget(
        projectionButton,
        0,
        Qt::AlignVCenter
    );

    item->setSizeHint(
        QSize(0, UniformRowHeight)
    );

    ui->courseGradesListWidget
        ->setItemWidget(
            item,
            rowWidget
        );

    connect(
        projectionButton,
        &QPushButton::clicked,
        this,
        [this, course]()
        {
            openCourseProjection(course);
        }
    );
}

void GradesWindow::openCourseProjection(
    const Course &course)
{
    CourseProjectionDialog dialog(
        database,
        course,
        this
    );

    if (dialog.exec() !=
        QDialog::Accepted)
    {
        return;
    }

    refreshGrades();
}

void GradesWindow::updateSummary(
    const User &user,
    const Semester *selectedSemester)
{
    bool hasCurrentSemesterGpa = false;
    bool hasProjectedSemesterGpa = false;

    double currentSemesterGpa = 0.0;
    double projectedSemesterGpa = 0.0;

    if (selectedSemester != nullptr)
    {
        currentSemesterGpa =
            GradeProjection::
                calculateSemesterGpa(
                    *selectedSemester,
                    false,
                    &hasCurrentSemesterGpa
                );

        projectedSemesterGpa =
            GradeProjection::
                calculateSemesterGpa(
                    *selectedSemester,
                    true,
                    &hasProjectedSemesterGpa
                );
    }

    bool hasCurrentCgpa = false;
    bool hasProjectedCgpa = false;

    const double currentCgpa =
        GradeProjection::calculateCgpa(
            user,
            false,
            &hasCurrentCgpa
        );

    const double projectedCgpa =
        GradeProjection::calculateCgpa(
            user,
            true,
            &hasProjectedCgpa
        );

    ui->semesterGpaValueLabel->setText(
        hasCurrentSemesterGpa
            ? tr("%1 / 4.00")
                  .arg(
                      currentSemesterGpa,
                      0,
                      'f',
                      2
                  )
            : tr("—")
    );

    ui->averageGradeValueLabel->setText(
        hasProjectedSemesterGpa
            ? tr("%1 / 4.00")
                  .arg(
                      projectedSemesterGpa,
                      0,
                      'f',
                      2
                  )
            : tr("—")
    );

    ui->completedCoursesValueLabel->setText(
        hasCurrentCgpa
            ? tr("%1 / 4.00")
                  .arg(
                      currentCgpa,
                      0,
                      'f',
                      2
                  )
            : tr("—")
    );

    ui->projectedCgpaValueLabel->setText(
        hasProjectedCgpa
            ? tr("%1 / 4.00")
                  .arg(
                      projectedCgpa,
                      0,
                      'f',
                      2
                  )
            : tr("—")
    );

    if (hasCurrentCgpa &&
        hasProjectedCgpa)
    {
        const double change =
            projectedCgpa -
            currentCgpa;

        ui->projectedCgpaValueLabel
            ->setToolTip(
                tr("Projected change: %1%2")
                    .arg(
                        change >= 0.0
                            ? QStringLiteral("+")
                            : QString()
                    )
                    .arg(
                        change,
                        0,
                        'f',
                        2
                    )
            );
    }
    else
    {
        ui->projectedCgpaValueLabel
            ->setToolTip(QString());
    }
}

void GradesWindow::updateTrend(
    const User &user)
{
    trendChart->setPoints(
        GradeProjection::buildTrend(user)
    );
}

void GradesWindow::updateEmptyState()
{
    const int courseCount =
        ui->courseGradesListWidget->count();

    ui->courseCountLabel->setText(
        courseCountText(courseCount)
    );

    const bool hasCourses =
        courseCount > 0;

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
            tr("No semester selected")
        );

        ui->emptyStateLabel->setText(
            tr("Create or select a semester first.")
        );
    }
    else
    {
        ui->emptyStateTitleLabel->setText(
            tr("No courses to grade")
        );

        ui->emptyStateLabel->setText(
            tr("Add courses and assignments to this semester first.")
        );
    }
}

QString GradesWindow::courseCountText(
    int count)
{
    return tr("%1 course%2")
        .arg(count)
        .arg(
            count == 1
                ? QString()
                : QStringLiteral("s")
        );
}
