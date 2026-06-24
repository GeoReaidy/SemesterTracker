#include "courseswindow.h"
#include "courseeditordialog.h"
#include "ui_courseswindow.h"

#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSize>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>

#include <exception>

namespace
{
QIcon makePencilIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor("#2563eb"));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    painter.drawLine(6, 18, 17, 7);
    painter.drawLine(8, 20, 19, 9);
    painter.drawLine(6, 18, 8, 20);
    painter.drawLine(17, 7, 19, 9);
    painter.drawLine(5, 21, 9, 20);

    return QIcon(pixmap);
}

QIcon makeBinIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor("#dc2626"));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    painter.drawLine(6, 7, 18, 7);
    painter.drawLine(9, 4, 15, 4);
    painter.drawLine(10, 4, 10, 7);
    painter.drawLine(14, 4, 14, 7);
    painter.drawRoundedRect(7, 8, 10, 12, 2, 2);
    painter.drawLine(10, 11, 10, 17);
    painter.drawLine(14, 11, 14, 17);

    return QIcon(pixmap);
}

QString courseStatusText(CourseStatus status)
{
    switch (status)
    {
    case CourseStatus::Planned:
        return QObject::tr("Planned");
    case CourseStatus::InProgress:
        return QObject::tr("In progress");
    case CourseStatus::Completed:
        return QObject::tr("Completed");
    case CourseStatus::Withdrawn:
        return QObject::tr("Withdrawn");
    }

    return QObject::tr("In progress");
}

QString courseStatusButtonStyle(CourseStatus status)
{
    switch (status)
    {
    case CourseStatus::Planned:
        return R"(
            QToolButton {
                color: #475569;
                background-color: #f1f5f9;
                border: 1px solid #cbd5e1;
                border-radius: 8px;
                padding: 5px 24px 5px 10px;
                font-size: 12px;
                font-weight: 600;
            }

            QToolButton:hover {
                background-color: #e2e8f0;
            }

            QToolButton::menu-indicator {
                subcontrol-origin: padding;
                subcontrol-position: center right;
                right: 7px;
            }
        )";
    case CourseStatus::InProgress:
        return R"(
            QToolButton {
                color: #1d4ed8;
                background-color: #eff6ff;
                border: 1px solid #bfdbfe;
                border-radius: 8px;
                padding: 5px 24px 5px 10px;
                font-size: 12px;
                font-weight: 600;
            }

            QToolButton:hover {
                background-color: #dbeafe;
            }

            QToolButton::menu-indicator {
                subcontrol-origin: padding;
                subcontrol-position: center right;
                right: 7px;
            }
        )";
    case CourseStatus::Completed:
        return R"(
            QToolButton {
                color: #166534;
                background-color: #dcfce7;
                border: 1px solid #bbf7d0;
                border-radius: 8px;
                padding: 5px 24px 5px 10px;
                font-size: 12px;
                font-weight: 600;
            }

            QToolButton:hover {
                background-color: #bbf7d0;
            }

            QToolButton::menu-indicator {
                subcontrol-origin: padding;
                subcontrol-position: center right;
                right: 7px;
            }
        )";
    case CourseStatus::Withdrawn:
        return R"(
            QToolButton {
                color: #991b1b;
                background-color: #fef2f2;
                border: 1px solid #fecaca;
                border-radius: 8px;
                padding: 5px 24px 5px 10px;
                font-size: 12px;
                font-weight: 600;
            }

            QToolButton:hover {
                background-color: #fee2e2;
            }

            QToolButton::menu-indicator {
                subcontrol-origin: padding;
                subcontrol-position: center right;
                right: 7px;
            }
        )";
    }

    return {};
}

QString statusMenuStyle()
{
    return R"(
        QMenu {
            background-color: white;
            color: #1f2937;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            padding: 5px;
            font-size: 13px;
            font-weight: 600;
        }

        QMenu::item {
            color: #1f2937;
            background-color: transparent;
            border-radius: 6px;
            padding: 9px 14px;
            margin: 2px;
        }

        QMenu::item:selected {
            color: white;
            background-color: #2563eb;
        }

        QMenu::indicator {
            width: 14px;
            height: 14px;
        }
    )";
}
}

CoursesWindow::CoursesWindow(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      userID(userID),
      ui(new Ui::CoursesWindow)
{
    ui->setupUi(this);

    ui->coursesListWidget->setStyleSheet(R"(
        QListWidget {
            background-color: transparent;
            color: #1f2937;
            border: none;
            outline: none;
        }

        QListWidget::item {
            background-color: transparent;
            color: #1f2937;
            border: none;
            padding: 0;
            margin: 4px 0;
        }

        QListWidget::item:hover,
        QListWidget::item:selected {
            background-color: transparent;
            color: #1f2937;
            border: none;
        }
    )");

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
            background-color: #ffffff;
            color: #1f2937;
        }

        QListView::item:hover {
            background-color: #eff6ff;
            color: #1f2937;
        }

        QListView::item:selected {
            background-color: #dbeafe;
            color: #1f2937;
        }
    )";

    ui->semesterComboBox->view()->setStyleSheet(
        comboPopupStyle
    );

    ui->semesterComboBox->setMaxVisibleItems(10);

    connect(
        ui->semesterComboBox,
        &QComboBox::currentIndexChanged,
        this,
        &CoursesWindow::handleSemesterChanged
    );

    connect(
        ui->addCourseButton,
        &QPushButton::clicked,
        this,
        &CoursesWindow::handleAddCourse
    );

    refreshSemesters();
}

CoursesWindow::~CoursesWindow()
{
    delete ui;
}

void CoursesWindow::setUserID(int newUserID)
{
    userID = newUserID;
    refreshSemesters();
}

void CoursesWindow::refreshSemesters()
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
        ui->addCourseButton->setEnabled(false);
        ui->semesterComboBox->blockSignals(false);

        refreshCourses();
        return;
    }

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    int currentSemesterIndex = -1;
    int previousSemesterIndex = -1;

    for (const Semester &semester : semesters)
    {
        QString label =
            QString("%1 %2")
                .arg(QString::fromStdString(
                    semester.getName()
                ))
                .arg(semester.getYear());

        if (semester.isSummaryOnly())
        {
            label += " — completed summary";
        }
        else if (semester.getStatus() == SemesterStatus::Active)
        {
            label += " — active";
        }
        else if (semester.getStatus() == SemesterStatus::Completed)
        {
            label += " — completed";
        }
        else
        {
            label += " — planned";
        }

        ui->semesterComboBox->addItem(label, semester.getID());
        ui->semesterComboBox->setItemData(
            ui->semesterComboBox->count() - 1,
            semester.isSummaryOnly(),
            Qt::UserRole + 1
        );

        const int index =
            ui->semesterComboBox->count() - 1;

        if (semester.isInProgress())
        {
            currentSemesterIndex = index;
        }

        if (semester.getID() == previousSemesterID)
        {
            previousSemesterIndex = index;
        }
    }

    if (semesters.empty())
    {
        ui->semesterComboBox->addItem(
            "Create a semester first",
            -1
        );

        ui->semesterComboBox->setEnabled(false);
        ui->addCourseButton->setEnabled(false);
    }
    else
    {
        ui->semesterComboBox->setEnabled(true);
        ui->addCourseButton->setEnabled(true);

        if (previousSemesterIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(
                previousSemesterIndex
            );
        }
        else if (currentSemesterIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(
                currentSemesterIndex
            );
        }
        else
        {
            ui->semesterComboBox->setCurrentIndex(0);
        }
    }

    ui->semesterComboBox->blockSignals(false);
    ui->addCourseButton->setEnabled(
        ui->semesterComboBox->isEnabled() &&
        !selectedSemesterIsSummaryOnly()
    );

    refreshCourses();
}

void CoursesWindow::refreshCourses()
{
    ui->coursesListWidget->clear();

    const int semesterID = selectedSemesterID();

    if (semesterID < 0)
    {
        updateEmptyState();
        return;
    }

    const std::vector<Course> courses =
        database.loadCoursesForSemester(semesterID);

    for (const Course &course : courses)
    {
        addCourseRow(course);
    }

    updateEmptyState();
}

void CoursesWindow::handleSemesterChanged(int index)
{
    Q_UNUSED(index);
    ui->addCourseButton->setEnabled(
        selectedSemesterID() >= 0 &&
        !selectedSemesterIsSummaryOnly()
    );
    refreshCourses();
}

void CoursesWindow::handleAddCourse()
{
    const int semesterID = selectedSemesterID();

    if (semesterID < 0)
    {
        QMessageBox::warning(
            this,
            tr("No Semester Selected"),
            tr("Select or create a semester before adding a course.")
        );
        return;
    }

    if (selectedSemesterIsSummaryOnly())
    {
        QMessageBox::information(
            this,
            tr("Completed Semester"),
            tr("This semester stores a credits-and-GPA summary and "
               "is read-only for courses.")
        );
        return;
    }

    CourseEditorDialog dialog(
        database,
        userID,
        semesterID,
        this
    );

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    refreshSemesters();
    refreshCourses();
    emit coursesChanged();
}

int CoursesWindow::selectedSemesterID() const
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

bool CoursesWindow::selectedSemesterIsSummaryOnly() const
{
    if (!ui || !ui->semesterComboBox || ui->semesterComboBox->currentIndex() < 0)
    {
        return false;
    }

    return ui->semesterComboBox->currentData(Qt::UserRole + 1).toBool();
}

void CoursesWindow::addCourseRow(
    const Course &course)
{
    auto *item =
        new QListWidgetItem(ui->coursesListWidget);

    item->setData(Qt::UserRole, course.getID());
    item->setData(
        Qt::UserRole + 1,
        QString::fromStdString(course.getCode())
    );
    item->setData(
        Qt::UserRole + 2,
        QString::fromStdString(course.getName())
    );
    item->setData(
        Qt::UserRole + 3,
        course.getCredits()
    );
    item->setData(
        Qt::UserRole + 4,
        static_cast<int>(course.getStatus())
    );
    item->setData(
        Qt::UserRole + 5,
        course.isRetaken()
    );

    auto *rowWidget =
        new QWidget(ui->coursesListWidget);

    rowWidget->setMinimumHeight(88);
    rowWidget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
    );
    rowWidget->setAttribute(Qt::WA_StyledBackground, true);
    rowWidget->setObjectName("courseRow");
    rowWidget->setStyleSheet(R"(
        QWidget#courseRow {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 9px;
        }

        QWidget#courseRow QLabel {
            background-color: transparent;
            border: none;
        }

        QWidget#courseRow QWidget {
            background-color: transparent;
            border: none;
        }
    )");

    auto *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(16, 14, 12, 14);
    rowLayout->setSpacing(12);
    rowLayout->setAlignment(Qt::AlignVCenter);

    auto *textContainer = new QWidget(rowWidget);
    textContainer->setStyleSheet(
        "background: transparent;"
        "border: none;"
    );

    auto *textLayout = new QVBoxLayout(textContainer);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);

    auto *courseTitleLabel = new QLabel(
        QString("%1 — %2")
            .arg(QString::fromStdString(
                course.getCode()
            ))
            .arg(QString::fromStdString(
                course.getName()
            )),
        textContainer
    );

    courseTitleLabel->setStyleSheet(
        "color: #111827;"
        "font-size: 15px;"
        "font-weight: 600;"
    );

    auto *creditsLabel = new QLabel(
        QString("%1 credit%2")
            .arg(course.getCredits())
            .arg(course.getCredits() == 1 ? "" : "s"),
        textContainer
    );

    creditsLabel->setStyleSheet(
        "color: #64748b;"
        "font-size: 13px;"
    );

    textLayout->addWidget(courseTitleLabel);
    textLayout->addWidget(creditsLabel);

    auto *statusButton = new QToolButton(rowWidget);
    statusButton->setText(
        courseStatusText(course.getStatus())
    );
    statusButton->setCursor(Qt::PointingHandCursor);
    statusButton->setPopupMode(QToolButton::InstantPopup);
    statusButton->setToolButtonStyle(
        Qt::ToolButtonTextOnly
    );
    statusButton->setMinimumWidth(118);
    statusButton->setFixedHeight(32);
    statusButton->setStyleSheet(
        courseStatusButtonStyle(
            course.getStatus()
        )
    );
    statusButton->setToolTip(
        tr("Change course status")
    );
    statusButton->setAccessibleName(
        tr("Course status: %1")
            .arg(courseStatusText(
                course.getStatus()
            ))
    );

    auto *statusMenu = new QMenu(statusButton);
    statusMenu->setMinimumWidth(175);
    statusMenu->setStyleSheet(statusMenuStyle());

    auto *statusGroup =
        new QActionGroup(statusMenu);
    statusGroup->setExclusive(true);

    const struct StatusOption
    {
        CourseStatus status;
        const char *label;
    } statusOptions[] = {
        {CourseStatus::Planned, "Planned"},
        {CourseStatus::InProgress, "In progress"},
        {CourseStatus::Completed, "Completed"},
        {CourseStatus::Withdrawn, "Withdrawn"}
    };

    for (const StatusOption &option : statusOptions)
    {
        QAction *action =
            statusMenu->addAction(
                tr(option.label)
            );

        action->setCheckable(true);
        action->setChecked(
            option.status == course.getStatus()
        );
        statusGroup->addAction(action);

        connect(
            action,
            &QAction::triggered,
            this,
            [this, item, option]()
            {
                setCourseStatus(
                    item,
                    option.status
                );
            }
        );
    }

    statusButton->setMenu(statusMenu);

    QLabel *retakeBadge = nullptr;

    if (course.isRetaken())
    {
        retakeBadge = new QLabel(
            tr("Retake"),
            rowWidget
        );
        retakeBadge->setAlignment(Qt::AlignCenter);
        retakeBadge->setFixedHeight(28);
        retakeBadge->setMinimumWidth(62);
        retakeBadge->setToolTip(
            tr("This course replaced a previous attempt")
        );
        retakeBadge->setStyleSheet(R"(
            QLabel {
                color: #7c3aed;
                background-color: #f5f3ff;
                border: 1px solid #ddd6fe;
                border-radius: 8px;
                padding: 3px 9px;
                font-size: 12px;
                font-weight: 600;
            }
        )");
    }

    auto *editButton = new QToolButton(rowWidget);
    editButton->setIcon(makePencilIcon());
    editButton->setIconSize(QSize(20, 20));
    editButton->setToolTip("Edit course");
    editButton->setAccessibleName("Edit course");
    editButton->setCursor(Qt::PointingHandCursor);
    editButton->setFixedSize(36, 36);
    editButton->setStyleSheet(R"(
        QToolButton {
            border: none;
            border-radius: 8px;
            background-color: #eff6ff;
        }

        QToolButton:hover {
            background-color: #dbeafe;
        }
    )");

    auto *deleteButton = new QToolButton(rowWidget);
    deleteButton->setIcon(makeBinIcon());
    deleteButton->setIconSize(QSize(20, 20));
    deleteButton->setToolTip("Delete course");
    deleteButton->setAccessibleName("Delete course");
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setFixedSize(36, 36);
    deleteButton->setStyleSheet(R"(
        QToolButton {
            border: none;
            border-radius: 8px;
            background-color: #fef2f2;
        }

        QToolButton:hover {
            background-color: #fee2e2;
        }
    )");

    rowLayout->addWidget(
        textContainer,
        1,
        Qt::AlignVCenter
    );

    rowLayout->addStretch();

    rowLayout->addWidget(
        statusButton,
        0,
        Qt::AlignVCenter
    );

    if (retakeBadge)
    {
        rowLayout->addWidget(
            retakeBadge,
            0,
            Qt::AlignVCenter
        );
    }

    rowLayout->addWidget(
        editButton,
        0,
        Qt::AlignVCenter
    );

    rowLayout->addWidget(
        deleteButton,
        0,
        Qt::AlignVCenter
    );

    item->setSizeHint(QSize(0, 96));

    ui->coursesListWidget->setItemWidget(
        item,
        rowWidget
    );

    connect(
        editButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            editCourseRow(item);
        }
    );

    connect(
        deleteButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            deleteCourseRow(item);
        }
    );
}

void CoursesWindow::setCourseStatus(
    QListWidgetItem *item,
    CourseStatus status)
{
    if (!item)
    {
        return;
    }

    const CourseStatus currentStatus =
        static_cast<CourseStatus>(
            item->data(
                Qt::UserRole + 4
            ).toInt()
        );

    if (status == currentStatus)
    {
        return;
    }

    const int courseID =
        item->data(Qt::UserRole).toInt();

    if (status == CourseStatus::Completed)
    {
        const std::vector<Assignment> assignments =
            database.loadAssignmentsForCourse(
                courseID
            );

        int gradedWeight = 0;

        for (const Assignment &assignment : assignments)
        {
            if (assignment.hasGrade())
            {
                gradedWeight += static_cast<int>(
                    assignment.getWeightPercentage()
                );
            }
        }

        if (gradedWeight != 100)
        {
            QMessageBox::information(
                this,
                tr("Complete Grade Required"),
                tr("Graded assignments must cover exactly 100% of "
                   "the course weight before the course can be "
                   "marked as completed.")
            );
            return;
        }
    }

    if (!database.setCourseStatus(
            courseID,
            status))
    {
        QMessageBox::warning(
            this,
            tr("Database Error"),
            tr("The course status could not be updated.")
        );
        return;
    }

    refreshCourses();
    emit coursesChanged();
}

void CoursesWindow::editCourseRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    try
    {
        const Course course(
            item->data(Qt::UserRole).toInt(),
            item->data(Qt::UserRole + 2)
                .toString()
                .toStdString(),
            item->data(Qt::UserRole + 1)
                .toString()
                .toStdString(),
            item->data(Qt::UserRole + 3).toInt(),
            static_cast<CourseStatus>(
                item->data(
                    Qt::UserRole + 4
                ).toInt()
            ),
            item->data(
                Qt::UserRole + 5
            ).toBool()
        );

        CourseEditorDialog dialog(
            database,
            userID,
            selectedSemesterID(),
            course,
            this
        );

        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            tr("Invalid Course"),
            QString::fromUtf8(error.what())
        );
        return;
    }

    refreshSemesters();
    refreshCourses();
    emit coursesChanged();
}

void CoursesWindow::deleteCourseRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    const int courseID =
        item->data(Qt::UserRole).toInt();

    const QString courseCode =
        item->data(Qt::UserRole + 1).toString();

    const QString courseName =
        item->data(Qt::UserRole + 2).toString();

    const QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Delete Course",
            QString("Delete %1 — %2?")
                .arg(courseCode)
                .arg(courseName),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (answer != QMessageBox::Yes)
    {
        return;
    }

    if (!database.deleteCourse(courseID))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not delete the course."
        );
        return;
    }

    refreshCourses();
    emit coursesChanged();
}

void CoursesWindow::updateEmptyState()
{
    const int count =
        ui->coursesListWidget->count();

    ui->courseCountLabel->setText(
        QString::number(count)
    );

    const bool hasCourses = count > 0;
    const bool hasSemester =
        selectedSemesterID() >= 0;

    ui->coursesListWidget->setVisible(hasCourses);

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
            "Create a semester first, then add courses to it."
        );
    }
    else
    {
        ui->emptyStateTitleLabel->setText(
            "No courses yet"
        );

        ui->emptyStateLabel->setText(
            "Add a course to the selected semester."
        );
    }
}
