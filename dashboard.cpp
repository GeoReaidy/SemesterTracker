#include "dashboard.h"
#include "semesterswindow.h"
#include "courseswindow.h"
#include "assignmentswindow.h"
#include "gradeswindow.h"
#include "settingswindow.h"
#include "calendarwindow.h"
#include "ui_dashboard.h"

#include <QDate>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QColor>
#include <QBrush>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QVBoxLayout>

#include <algorithm>
#include <exception>

Dashboard::Dashboard(
    DatabaseManager &database,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    const QString dashboardListStyle = R"(
        QListWidget {
            background-color: transparent;
            color: #1f2937;
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

        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
            background: transparent;
        }
    )";

    ui->currentCoursesListWidget->setStyleSheet(
        dashboardListStyle
    );

    ui->upcomingAssignmentsListWidget->setStyleSheet(
        dashboardListStyle
    );

    ui->currentCoursesListWidget->setVerticalScrollMode(
        QAbstractItemView::ScrollPerPixel
    );

    ui->upcomingAssignmentsListWidget->setVerticalScrollMode(
        QAbstractItemView::ScrollPerPixel
    );

    ui->currentCoursesListWidget->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    ui->upcomingAssignmentsListWidget->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );

    ui->currentCoursesListWidget->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    ui->upcomingAssignmentsListWidget->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    connect(
        ui->semestersButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createSemestersPage();

            semestersWindow->setUserID(currentUserID);

            ui->mainStackedWidget->setCurrentWidget(
                semestersWindow
            );
        }
    );


    connect(
        ui->coursesButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createCoursesPage();

            coursesWindow->setUserID(currentUserID);

            ui->mainStackedWidget->setCurrentWidget(
                coursesWindow
            );
        }
    );


    connect(
        ui->assignmentsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createAssignmentsPage();

            assignmentsWindow->setUserID(currentUserID);

            ui->mainStackedWidget->setCurrentWidget(
                assignmentsWindow
            );
        }
    );


    connect(
        ui->gradesButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createGradesPage();

            gradesWindow->setUserID(currentUserID);

            ui->mainStackedWidget->setCurrentWidget(
                gradesWindow
            );
        }
    );


    connect(
        ui->settingsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createSettingsPage();

            ui->mainStackedWidget->setCurrentWidget(
                settingsWindow
            );
        }
    );

    connect(
        ui->calendarButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createCalendarPage();

            calendarWindow->setUserID(
                currentUserID
            );

            ui->mainStackedWidget->setCurrentWidget(
                calendarWindow
            );
        }
    );

    connect(
        ui->viewCalendarButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createCalendarPage();

            calendarWindow->setUserID(
                currentUserID
            );

            ui->mainStackedWidget->setCurrentWidget(
                calendarWindow
            );
        }
    );

    connect(
        ui->dashboardButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            ui->mainStackedWidget->setCurrentWidget(
                ui->dashboardPage
            );
        }
    );

    connect(
        ui->viewAllCoursesButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createCoursesPage();

            coursesWindow->setUserID(currentUserID);

            ui->mainStackedWidget->setCurrentWidget(
                coursesWindow
            );
        }
    );

    connect(
        ui->viewAllAssignmentsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            createAssignmentsPage();

            assignmentsWindow->setUserID(currentUserID);

            ui->mainStackedWidget->setCurrentWidget(
                assignmentsWindow
            );
        }
    );

    connect(
        ui->calendarTableWidget,
        &QTableWidget::cellClicked,
        this,
        [this](int row, int column)
        {
            QTableWidgetItem *item =
                ui->calendarTableWidget->item(
                    row,
                    column
                );

            if (!item)
            {
                return;
            }

            const QDate date =
                QDate::fromString(
                    item->data(Qt::UserRole).toString(),
                    Qt::ISODate
                );

            openCalendarDate(date);
        }
    );

    connect(
        ui->logoutButton,
        &QPushButton::clicked,
        this,
        &Dashboard::logoutRequested
    );

    ui->dashboardScrollArea->setStyleSheet(R"(
        QScrollArea {
            background-color: #f5f7fb;
            border: none;
        }

        QScrollArea > QWidget > QWidget {
            background-color: #f5f7fb;
        }
    )");

    ui->dashboardScrollAreaWidgetContents->setStyleSheet(
        "background-color: #f5f7fb;"
        );

    const QString cardStyle = R"(
    QFrame#%1 {
        background-color: #ffffff;
        border: 1px solid #e5e7eb;
        border-radius: 12px;
    }

    QFrame#%1 QLabel,
    QFrame#%1 QPushButton,
    QFrame#%1 QListWidget,
    QFrame#%1 QTableWidget {
        border: none;
        background-color: transparent;
    }
)";

    ui->headerFrame->setStyleSheet(
        cardStyle.arg("headerFrame")
        );

    ui->cgpaCard->setStyleSheet(
        cardStyle.arg("cgpaCard")
        );

    ui->creditsCard->setStyleSheet(
        cardStyle.arg("creditsCard")
        );

    ui->overallGradeCard->setStyleSheet(
        cardStyle.arg("overallGradeCard")
        );

    ui->currentCoursesCard->setStyleSheet(
        cardStyle.arg("currentCoursesCard")
        );

    ui->upcomingAssignmentsCard->setStyleSheet(
        cardStyle.arg("upcomingAssignmentsCard")
        );

    ui->calendarOverviewCard->setStyleSheet(
        cardStyle.arg("calendarOverviewCard")
        );

    // Keep the rest of your sample-data code below this.
}

Dashboard::~Dashboard()
{
    delete ui;
}

void Dashboard::createSemestersPage()
{
    if (semestersWindow)
    {
        return;
    }

    semestersWindow = new SemestersWindow(
        database,
        currentUserID,
        ui->mainStackedWidget
    );

    ui->mainStackedWidget->addWidget(
        semestersWindow
    );

    connect(
        semestersWindow,
        &SemestersWindow::semestersChanged,
        this,
        &Dashboard::refreshUserFromDatabase
    );
}

void Dashboard::createCoursesPage()
{
    if (coursesWindow)
    {
        return;
    }

    coursesWindow = new CoursesWindow(
        database,
        currentUserID,
        ui->mainStackedWidget
    );

    ui->mainStackedWidget->addWidget(
        coursesWindow
    );

    connect(
        coursesWindow,
        &CoursesWindow::coursesChanged,
        this,
        &Dashboard::refreshUserFromDatabase
    );
}

void Dashboard::createAssignmentsPage()
{
    if (assignmentsWindow)
    {
        return;
    }

    assignmentsWindow = new AssignmentsWindow(
        database,
        currentUserID,
        ui->mainStackedWidget
    );

    ui->mainStackedWidget->addWidget(
        assignmentsWindow
    );

    connect(
        assignmentsWindow,
        &AssignmentsWindow::assignmentsChanged,
        this,
        &Dashboard::refreshUserFromDatabase
    );
}

void Dashboard::createGradesPage()
{
    if (gradesWindow)
    {
        return;
    }

    gradesWindow = new GradesWindow(
        database,
        currentUserID,
        ui->mainStackedWidget
    );

    ui->mainStackedWidget->addWidget(
        gradesWindow
    );
}


void Dashboard::createSettingsPage()
{
    if (settingsWindow)
    {
        return;
    }

    settingsWindow = new SettingsWindow(
        database,
        ui->mainStackedWidget
    );

    ui->mainStackedWidget->addWidget(
        settingsWindow
    );

    connect(
        settingsWindow,
        &SettingsWindow::profileUpdated,
        this,
        &Dashboard::refreshUserFromDatabase
    );

    connect(
        settingsWindow,
        &SettingsWindow::accountDeleted,
        this,
        &Dashboard::logoutRequested
    );

    if (currentUserID > 0 && !currentUsername.empty())
    {
        try
        {
            const User user =
                database.loadFullUserByUsername(
                    currentUsername
                );

            settingsWindow->setUserData(user);
        }
        catch (const std::exception &error)
        {
            qWarning()
                << "Could not load settings data:"
                << error.what();
        }
    }
}

void Dashboard::createCalendarPage()
{
    if (calendarWindow)
    {
        return;
    }

    calendarWindow = new CalendarWindow(
        database,
        currentUserID,
        ui->mainStackedWidget
    );

    ui->mainStackedWidget->addWidget(
        calendarWindow
    );
}

void Dashboard::refreshUserFromDatabase()
{
    if (currentUsername.empty())
    {
        return;
    }

    try
    {
        const User refreshedUser =
            database.loadFullUserByUsername(
                currentUsername
            );

        setUserData(refreshedUser);
    }
    catch (const std::exception &error)
    {
        qWarning()
            << "Could not refresh dashboard:"
            << error.what();
    }
}

void Dashboard::setUserData(const User &user)
{
    currentUserID = user.getID();
    currentUsername = user.getUsername();

    qDebug() << "Dashboard user ID:" << currentUserID;

    if (semestersWindow)
    {
        semestersWindow->setUserID(currentUserID);
    }

    if (coursesWindow)
    {
        coursesWindow->setUserID(currentUserID);
    }


    if (assignmentsWindow)
    {
        assignmentsWindow->setUserID(currentUserID);
    }


    if (gradesWindow)
    {
        gradesWindow->setUserID(currentUserID);
    }


    if (settingsWindow)
    {
        settingsWindow->setUserData(user);
    }


    if (calendarWindow)
    {
        calendarWindow->setUserID(
            currentUserID
        );
    }

    const double cgpa = user.calculateCGPA();
    const int completedCredits =
        user.calculateCompletedCredits();
    const int maxCredits = user.getMaxCredits();

    const QString username =
        QString::fromStdString(user.getUsername());

    ui->welcomeLabel->setText(
        QString("Welcome, %1").arg(username)
    );

    ui->profileUsernameLabel->setText(username);

    const Semester *currentSemester = nullptr;

    for (const Semester &semester : user.getSemesters())
    {
        if (semester.isInProgress())
        {
            currentSemester = &semester;
            break;
        }
    }

    if (currentSemester)
    {
        ui->semesterLabel->setText(
            QString("%1 %2")
                .arg(QString::fromStdString(
                    currentSemester->getName()
                ))
                .arg(currentSemester->getYear())
        );
    }
    else
    {
        ui->semesterLabel->setText(
            "No Current Semester"
        );
    }


    refreshDashboardLists(user);
    refreshDashboardCalendar();

    if (!username.isEmpty())
    {
        ui->profileAvatarLabel->setText(
            username.left(1).toUpper()
        );
    }

    ui->cgpaValueLabel->setText(
        QString("%1 / 4.00")
            .arg(cgpa, 0, 'f', 2)
    );

    ui->cgpaProgressBar->setRange(0, 400);
    ui->cgpaProgressBar->setValue(
        static_cast<int>(cgpa * 100)
    );

    ui->cgpaProgressBar->setFormat(
        QString("%1%")
            .arg((cgpa / 4.0) * 100.0, 0, 'f', 2)
    );

    ui->creditsValueLabel->setText(
        QString("%1 / %2")
            .arg(completedCredits)
            .arg(maxCredits)
    );

    ui->creditsProgressBar->setRange(
        0,
        maxCredits
    );

    ui->creditsProgressBar->setValue(
        completedCredits
    );

    const double creditsPercentage =
        maxCredits > 0
            ? (static_cast<double>(completedCredits)
               / maxCredits) * 100.0
            : 0.0;

    ui->creditsProgressBar->setFormat(
        QString("%1%")
            .arg(creditsPercentage, 0, 'f', 2)
    );

    ui->creditsMaxLabel->setText(
        QString::number(maxCredits)
    );

    if (user.getSemesters().empty())
    {
        ui->overallGradeValueLabel->setText("—");
        ui->overallGradeGpaLabel->setText(
            "No grades yet"
        );
        return;
    }

    QString letterGrade;

    if (cgpa >= 4.0)
        letterGrade = "A";
    else if (cgpa >= 3.7)
        letterGrade = "A-";
    else if (cgpa >= 3.3)
        letterGrade = "B+";
    else if (cgpa >= 3.0)
        letterGrade = "B";
    else if (cgpa >= 2.7)
        letterGrade = "B-";
    else if (cgpa >= 2.3)
        letterGrade = "C+";
    else if (cgpa >= 2.0)
        letterGrade = "C";
    else if (cgpa >= 1.7)
        letterGrade = "C-";
    else if (cgpa >= 1.3)
        letterGrade = "D+";
    else if (cgpa >= 1.0)
        letterGrade = "D";
    else
        letterGrade = "F";

    ui->overallGradeValueLabel->setText(
        letterGrade
    );

    ui->overallGradeGpaLabel->setText(
        QString("%1 / 4.00")
            .arg(cgpa, 0, 'f', 2)
    );
}


void Dashboard::refreshDashboardLists(const User &user)
{
    int currentSemesterID = -1;

    for (const Semester &semester : user.getSemesters())
    {
        if (semester.isInProgress())
        {
            currentSemesterID = semester.getID();
            break;
        }
    }

    populateCurrentCourses(currentSemesterID);
    populateUpcomingAssignments(currentSemesterID);
}

void Dashboard::populateCurrentCourses(int semesterID)
{
    ui->currentCoursesListWidget->clear();

    if (semesterID < 0)
    {
        auto *item =
            new QListWidgetItem(
                "No current semester",
                ui->currentCoursesListWidget
            );

        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        return;
    }

    const std::vector<Course> courses =
        database.loadCoursesForSemester(semesterID);

    if (courses.empty())
    {
        auto *item =
            new QListWidgetItem(
                "No courses in the current semester",
                ui->currentCoursesListWidget
            );

        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        return;
    }

    for (const Course &course : courses)
    {
        addCurrentCourseRow(course);
    }
}

void Dashboard::populateUpcomingAssignments(int semesterID)
{
    ui->upcomingAssignmentsListWidget->clear();

    if (semesterID < 0)
    {
        auto *item =
            new QListWidgetItem(
                "No current semester",
                ui->upcomingAssignmentsListWidget
            );

        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        return;
    }

    struct UpcomingEntry
    {
        Course course;
        Assignment assignment;
        QDate dueDate;
    };

    std::vector<UpcomingEntry> upcoming;
    const QDate today = QDate::currentDate();

    const std::vector<Course> courses =
        database.loadCoursesForSemester(semesterID);

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

            if (!dueDate.isValid() || dueDate < today)
            {
                continue;
            }

            upcoming.push_back(
                {course, assignment, dueDate}
            );
        }
    }

    std::sort(
        upcoming.begin(),
        upcoming.end(),
        [](const UpcomingEntry &left,
           const UpcomingEntry &right)
        {
            if (left.dueDate != right.dueDate)
            {
                return left.dueDate < right.dueDate;
            }

            return left.assignment.getID()
                   < right.assignment.getID();
        }
    );

    if (upcoming.empty())
    {
        auto *item =
            new QListWidgetItem(
                "No upcoming assignments",
                ui->upcomingAssignmentsListWidget
            );

        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        return;
    }

    for (const UpcomingEntry &entry : upcoming)
    {
        addUpcomingAssignmentRow(
            entry.course,
            entry.assignment
        );
    }
}

void Dashboard::addCurrentCourseRow(
    const Course &course)
{
    constexpr int DashboardRowHeight = 72;

    auto *item =
        new QListWidgetItem(
            ui->currentCoursesListWidget
        );

    auto *row =
        new QWidget(
            ui->currentCoursesListWidget
        );

    row->setObjectName("dashboardCourseRow");
    row->setMinimumHeight(DashboardRowHeight);

    row->setStyleSheet(R"(
        QWidget#dashboardCourseRow {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
        }

        QWidget#dashboardCourseRow QWidget,
        QWidget#dashboardCourseRow QLabel {
            background-color: transparent;
            border: none;
        }
    )");

    auto *layout = new QVBoxLayout(row);
    layout->setContentsMargins(12, 0, 12, 0);
    layout->setSpacing(3);
    layout->setAlignment(Qt::AlignVCenter);

    auto *title = new QLabel(
        QString("%1 — %2")
            .arg(QString::fromStdString(
                course.getCode()
            ))
            .arg(QString::fromStdString(
                course.getName()
            )),
        row
    );

    title->setStyleSheet(
        "color: #111827;"
        "font-size: 13px;"
        "font-weight: 600;"
    );

    auto *credits = new QLabel(
        QString("%1 credit%2")
            .arg(course.getCredits())
            .arg(course.getCredits() == 1 ? "" : "s"),
        row
    );

    credits->setStyleSheet(
        "color: #64748b;"
        "font-size: 12px;"
    );

    layout->addWidget(title);
    layout->addWidget(credits);

    item->setSizeHint(
        QSize(0, DashboardRowHeight)
    );

    ui->currentCoursesListWidget->setItemWidget(
        item,
        row
    );
}

void Dashboard::addUpcomingAssignmentRow(
    const Course &course,
    const Assignment &assignment)
{
    constexpr int DashboardRowHeight = 72;

    auto *item =
        new QListWidgetItem(
            ui->upcomingAssignmentsListWidget
        );

    auto *row =
        new QWidget(
            ui->upcomingAssignmentsListWidget
        );

    row->setObjectName("dashboardAssignmentRow");
    row->setMinimumHeight(DashboardRowHeight);

    row->setStyleSheet(R"(
        QWidget#dashboardAssignmentRow {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
        }

        QWidget#dashboardAssignmentRow QWidget,
        QWidget#dashboardAssignmentRow QLabel {
            background-color: transparent;
            border: none;
        }
    )");

    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(12, 0, 12, 0);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignVCenter);

    auto *textContainer = new QWidget(row);
    auto *textLayout =
        new QVBoxLayout(textContainer);

    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);

    auto *title = new QLabel(
        QString::fromStdString(
            assignment.getName()
        ),
        textContainer
    );

    title->setStyleSheet(
        "color: #111827;"
        "font-size: 13px;"
        "font-weight: 600;"
    );

    auto *courseLabel = new QLabel(
        QString::fromStdString(
            course.getCode()
        ),
        textContainer
    );

    courseLabel->setStyleSheet(
        "color: #64748b;"
        "font-size: 12px;"
    );

    const QDate dueDate =
        QDate::fromString(
            QString::fromStdString(
                assignment.getDueDate()
            ),
            Qt::ISODate
        );

    auto *dueLabel = new QLabel(
        dueDate.isValid()
            ? dueDate.toString("dd MMM")
            : "No date",
        row
    );

    dueLabel->setAlignment(Qt::AlignCenter);
    dueLabel->setStyleSheet(
        dueDate == QDate::currentDate()
            ? "color: #dc2626;"
              "background-color: #fee2e2;"
              "border-radius: 7px;"
              "padding: 4px 8px;"
              "font-size: 12px;"
              "font-weight: 600;"
            : "color: #2563eb;"
              "background-color: #eff6ff;"
              "border-radius: 7px;"
              "padding: 4px 8px;"
              "font-size: 12px;"
              "font-weight: 600;"
    );

    textLayout->addWidget(title);
    textLayout->addWidget(courseLabel);

    layout->addWidget(
        textContainer,
        1,
        Qt::AlignVCenter
    );

    layout->addWidget(
        dueLabel,
        0,
        Qt::AlignVCenter
    );

    item->setSizeHint(
        QSize(0, DashboardRowHeight)
    );

    ui->upcomingAssignmentsListWidget->setItemWidget(
        item,
        row
    );
}


void Dashboard::refreshDashboardCalendar()
{
    ui->calendarTableWidget->clear();
    ui->calendarTableWidget->setRowCount(6);
    ui->calendarTableWidget->setColumnCount(7);

    ui->calendarTableWidget->setHorizontalHeaderLabels(
        {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"}
    );

    ui->calendarTableWidget->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );

    ui->calendarTableWidget->verticalHeader()
        ->setVisible(false);

    ui->calendarTableWidget->setShowGrid(false);
    ui->calendarTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->calendarTableWidget->setSelectionMode(
        QAbstractItemView::NoSelection
    );

    const int availableHeight =
        qMax(
            120,
            ui->calendarTableWidget->viewport()
                ->height()
        );

    const int rowHeight = availableHeight / 6;

    for (int row = 0; row < 6; ++row)
    {
        ui->calendarTableWidget->setRowHeight(
            row,
            rowHeight
        );
    }

    const QDate today = QDate::currentDate();
    const QDate firstOfMonth(
        today.year(),
        today.month(),
        1
    );

    const int daysBack =
        firstOfMonth.dayOfWeek() - 1;

    const QDate firstVisibleDate =
        firstOfMonth.addDays(-daysBack);

    std::vector<QDate> deadlineDates;

    if (currentUserID > 0)
    {
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

                    if (dueDate.isValid())
                    {
                        deadlineDates.push_back(
                            dueDate
                        );
                    }
                }
            }
        }
    }

    for (int index = 0; index < 42; ++index)
    {
        const QDate date =
            firstVisibleDate.addDays(index);

        const int row = index / 7;
        const int column = index % 7;

        const int deadlineCount =
            static_cast<int>(
                std::count(
                    deadlineDates.begin(),
                    deadlineDates.end(),
                    date
                )
            );

        QString text =
            QString::number(date.day());

        if (deadlineCount > 0)
        {
            text += deadlineCount == 1
                        ? "\n•"
                        : QString("\n• %1")
                              .arg(deadlineCount);
        }

        auto *item = new QTableWidgetItem(text);

        item->setTextAlignment(Qt::AlignCenter);
        item->setData(
            Qt::UserRole,
            date.toString(Qt::ISODate)
        );

        QFont font = item->font();
        font.setPointSize(10);

        if (deadlineCount > 0)
        {
            font.setBold(true);
            item->setForeground(
                QBrush(QColor("#1d4ed8"))
            );
            item->setBackground(
                QBrush(QColor("#eff6ff"))
            );
        }
        else if (date.month() != today.month())
        {
            item->setForeground(
                QBrush(QColor("#94a3b8"))
            );
        }
        else
        {
            item->setForeground(
                QBrush(QColor("#334155"))
            );
        }

        if (date == today)
        {
            font.setBold(true);
            item->setForeground(
                QBrush(QColor("#ffffff"))
            );
            item->setBackground(
                QBrush(QColor("#4f46e5"))
            );
        }

        item->setFont(font);

        ui->calendarTableWidget->setItem(
            row,
            column,
            item
        );
    }

    ui->calendarOverviewLabel->setText(
        QString("Calendar Overview — %1")
            .arg(today.toString("MMMM yyyy"))
    );
}

void Dashboard::openCalendarDate(
    const QDate &date)
{
    if (!date.isValid())
    {
        return;
    }

    createCalendarPage();

    calendarWindow->setUserID(
        currentUserID
    );

    calendarWindow->selectDate(date);

    ui->mainStackedWidget->setCurrentWidget(
        calendarWindow
    );
}
