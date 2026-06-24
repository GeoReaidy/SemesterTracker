#include "calendarwindow.h"
#include "ui_calendarwindow.h"

#include <QAbstractItemView>
#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QScrollBar>
#include <QTextCharFormat>
#include <QVBoxLayout>

#include <algorithm>

CalendarWindow::CalendarWindow(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      currentUserID(userID),
      ui(new Ui::CalendarWindow)
{
    ui->setupUi(this);

    ui->calendarWidget->setNavigationBarVisible(false);
    ui->calendarWidget->setGridVisible(true);
    ui->calendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->calendarWidget->setVerticalHeaderFormat(
        QCalendarWidget::NoVerticalHeader
    );

    const QString listStyle = R"(
        QListWidget {
            color: #111827;
            background: transparent;
            border: none;
            outline: none;
        }

        QListWidget::item {
            margin: 4px 0;
            padding: 0;
            border: none;
            background: transparent;
        }

        QListWidget::item:selected,
        QListWidget::item:hover {
            color: #111827;
            background: transparent;
            border: none;
        }

        QScrollBar:vertical {
            width: 10px;
            margin: 2px;
            background: transparent;
        }

        QScrollBar::handle:vertical {
            min-height: 28px;
            border-radius: 5px;
            background: #cbd5e1;
        }

        QScrollBar::handle:vertical:hover {
            background: #94a3b8;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0;
        }
    )";

    ui->selectedDateAssignmentsListWidget->setStyleSheet(
        listStyle
    );

    ui->upcomingDeadlinesListWidget->setStyleSheet(
        listStyle
    );

    ui->selectedDateAssignmentsListWidget->setVerticalScrollMode(
        QAbstractItemView::ScrollPerPixel
    );

    ui->upcomingDeadlinesListWidget->setVerticalScrollMode(
        QAbstractItemView::ScrollPerPixel
    );

    connect(
        ui->previousMonthButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            ui->calendarWidget->showPreviousMonth();
        }
    );

    connect(
        ui->nextMonthButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            ui->calendarWidget->showNextMonth();
        }
    );

    connect(
        ui->todayButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            selectDate(QDate::currentDate());
        }
    );

    connect(
        ui->calendarWidget,
        &QCalendarWidget::selectionChanged,
        this,
        [this]()
        {
            showSelectedDate(
                ui->calendarWidget->selectedDate()
            );
        }
    );

    connect(
        ui->calendarWidget,
        &QCalendarWidget::currentPageChanged,
        this,
        &CalendarWindow::updateMonthTitle
    );

    refreshCalendar();
}

CalendarWindow::~CalendarWindow()
{
    delete ui;
}

void CalendarWindow::setUserID(int userID)
{
    currentUserID = userID;
    refreshCalendar();
}

void CalendarWindow::refreshCalendar()
{
    for (const QDate &date : highlightedDates)
    {
        ui->calendarWidget->setDateTextFormat(
            date,
            QTextCharFormat()
        );
    }

    highlightedDates.clear();
    deadlines.clear();

    loadDeadlines();
    applyDeadlineHighlights();
    populateUpcomingDeadlines();

    updateMonthTitle(
        ui->calendarWidget->yearShown(),
        ui->calendarWidget->monthShown()
    );

    showSelectedDate(
        ui->calendarWidget->selectedDate()
    );
}

void CalendarWindow::selectDate(const QDate &date)
{
    if (!date.isValid())
    {
        return;
    }

    ui->calendarWidget->setSelectedDate(date);
    ui->calendarWidget->setCurrentPage(
        date.year(),
        date.month()
    );

    showSelectedDate(date);
}

void CalendarWindow::loadDeadlines()
{
    if (currentUserID <= 0)
    {
        return;
    }

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(
            currentUserID
        );

    for (const Semester &semester : semesters)
    {
        const std::vector<Course> courses =
            database.loadCoursesForSemester(
                semester.getID()
            );

        for (const Course &course : courses)
        {
            const std::vector<Assignment> assignments =
                database.loadAssignmentsForCourse(
                    course.getID()
                );

            for (const Assignment &assignment : assignments)
            {
                if (!assignment.hasDueDate())
                {
                    continue;
                }

                const QDate dueDate =
                    QDate::fromString(
                        QString::fromStdString(
                            assignment.getDueDate()
                        ),
                        Qt::ISODate
                    );

                if (!dueDate.isValid())
                {
                    continue;
                }

                deadlines.push_back(
                    {
                        assignment.getID(),
                        QString::fromStdString(
                            assignment.getName()
                        ),
                        QString::fromStdString(
                            course.getCode()
                        ),
                        QString::fromStdString(
                            course.getName()
                        ),
                        dueDate,
                        static_cast<int>(
                            assignment.getWeightPercentage()
                        ),
                        assignment.hasGrade(),
                        assignment.getGrade(),
                        assignment.isCompleted()
                    }
                );
            }
        }
    }

    std::sort(
        deadlines.begin(),
        deadlines.end(),
        [](const DeadlineEntry &left,
           const DeadlineEntry &right)
        {
            if (left.dueDate != right.dueDate)
            {
                return left.dueDate < right.dueDate;
            }

            if (left.courseCode != right.courseCode)
            {
                return left.courseCode < right.courseCode;
            }

            return left.assignmentName <
                   right.assignmentName;
        }
    );
}

void CalendarWindow::applyDeadlineHighlights()
{
    const QDate today = QDate::currentDate();
    std::vector<QDate> processedDates;

    for (const DeadlineEntry &entry : deadlines)
    {
        if (std::find(
                processedDates.begin(),
                processedDates.end(),
                entry.dueDate) != processedDates.end())
        {
            continue;
        }

        bool hasPendingAssignment = false;

        for (const DeadlineEntry &sameDateEntry : deadlines)
        {
            if (sameDateEntry.dueDate == entry.dueDate &&
                !sameDateEntry.completed)
            {
                hasPendingAssignment = true;
                break;
            }
        }

        QTextCharFormat format;
        format.setFontWeight(QFont::Bold);

        if (!hasPendingAssignment)
        {
            format.setForeground(QColor("#166534"));
            format.setBackground(QColor("#dcfce7"));
        }
        else if (entry.dueDate <= today)
        {
            format.setForeground(QColor("#b91c1c"));
            format.setBackground(QColor("#fee2e2"));
        }
        else
        {
            format.setForeground(QColor("#1d4ed8"));
            format.setBackground(QColor("#dbeafe"));
        }

        ui->calendarWidget->setDateTextFormat(
            entry.dueDate,
            format
        );

        processedDates.push_back(entry.dueDate);
        highlightedDates.push_back(entry.dueDate);
    }
}

void CalendarWindow::showSelectedDate(
    const QDate &date)
{
    ui->selectedDateLabel->setText(
        date.toString("dddd, d MMMM yyyy")
    );

    ui->selectedDateAssignmentsListWidget->clear();

    int matchCount = 0;

    for (const DeadlineEntry &entry : deadlines)
    {
        if (entry.dueDate != date)
        {
            continue;
        }

        addDeadlineRow(
            entry,
            ui->selectedDateAssignmentsListWidget,
            false
        );

        ++matchCount;
    }

    if (matchCount == 0)
    {
        auto *item = new QListWidgetItem(
            "No assignments due on this date.",
            ui->selectedDateAssignmentsListWidget
        );

        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(0, 58));
    }
}

void CalendarWindow::populateUpcomingDeadlines()
{
    ui->upcomingDeadlinesListWidget->clear();

    const QDate today = QDate::currentDate();
    int upcomingCount = 0;

    for (const DeadlineEntry &entry : deadlines)
    {
        if (entry.completed ||
            entry.dueDate < today)
        {
            continue;
        }

        addDeadlineRow(
            entry,
            ui->upcomingDeadlinesListWidget,
            true
        );

        ++upcomingCount;
    }

    ui->upcomingCountLabel->setText(
        QString("%1 upcoming")
            .arg(upcomingCount)
    );

    if (upcomingCount == 0)
    {
        auto *item = new QListWidgetItem(
            "No upcoming deadlines.",
            ui->upcomingDeadlinesListWidget
        );

        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(0, 58));
    }
}

void CalendarWindow::updateMonthTitle(
    int year,
    int month)
{
    ui->monthTitleLabel->setText(
        QDate(year, month, 1)
            .toString("MMMM yyyy")
    );
}

void CalendarWindow::addDeadlineRow(
    const DeadlineEntry &entry,
    QListWidget *target,
    bool showFullDate)
{
    constexpr int RowHeight = 76;

    auto *item = new QListWidgetItem(target);
    auto *row = new QWidget(target);

    row->setObjectName("deadlineRow");
    row->setMinimumHeight(RowHeight);

    row->setStyleSheet(R"(
        QWidget#deadlineRow {
            background: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 9px;
        }

        QWidget#deadlineRow QWidget,
        QWidget#deadlineRow QLabel {
            background: transparent;
            border: none;
        }
    )");

    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(12, 0, 12, 0);
    layout->setSpacing(12);

    auto *textContainer = new QWidget(row);
    auto *textLayout = new QVBoxLayout(textContainer);

    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);
    textLayout->setAlignment(Qt::AlignVCenter);

    auto *assignmentLabel = new QLabel(
        entry.assignmentName,
        textContainer
    );

    assignmentLabel->setStyleSheet(
        "color:#111827;"
        "font-size:13px;"
        "font-weight:600;"
    );

    auto *courseLabel = new QLabel(
        QString("%1 — %2")
            .arg(entry.courseCode)
            .arg(entry.courseName),
        textContainer
    );

    courseLabel->setStyleSheet(
        "color:#64748b;"
        "font-size:12px;"
    );

    textLayout->addWidget(assignmentLabel);
    textLayout->addWidget(courseLabel);

    auto *dateLabel = new QLabel(
        entry.completed
            ? "Completed"
            : showFullDate
                ? entry.dueDate.toString("ddd, d MMM")
                : QString("%1%")
                      .arg(entry.weight),
        row
    );

    dateLabel->setAlignment(Qt::AlignCenter);

    if (entry.completed)
    {
        dateLabel->setStyleSheet(
            "color:#166534;"
            "background:#dcfce7;"
            "border-radius:7px;"
            "padding:5px 9px;"
            "font-size:12px;"
            "font-weight:600;"
        );
    }
    else if (entry.dueDate <= QDate::currentDate())
    {
        dateLabel->setStyleSheet(
            "color:#b91c1c;"
            "background:#fee2e2;"
            "border-radius:7px;"
            "padding:5px 9px;"
            "font-size:12px;"
            "font-weight:600;"
        );
    }
    else
    {
        dateLabel->setStyleSheet(
            "color:#1d4ed8;"
            "background:#eff6ff;"
            "border-radius:7px;"
            "padding:5px 9px;"
            "font-size:12px;"
            "font-weight:600;"
        );
    }

    layout->addWidget(
        textContainer,
        1,
        Qt::AlignVCenter
    );

    layout->addWidget(
        dateLabel,
        0,
        Qt::AlignVCenter
    );

    item->setSizeHint(QSize(0, RowHeight));
    target->setItemWidget(item, row);
}
