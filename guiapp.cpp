#include "guiapp.h"

#include "dashboard.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "updatemanager.h"

#include <QAbstractButton>
#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>

#include <algorithm>

GUIApp::GUIApp(
    MainWindow &window,
    QObject *parent)
    : QObject(parent),
      window(window),
      authService(database)
{
    updateManager =
        new UpdateManager(
            &window,
            this
        );

    connect(
        window.ui->loginButton,
        &QPushButton::clicked,
        this,
        &GUIApp::handleLogin
    );

    connect(
        window.ui->passwordLineEdit,
        &QLineEdit::returnPressed,
        this,
        &GUIApp::handleLogin
    );

    connect(
        window.ui->createAccountButton,
        &QPushButton::clicked,
        this,
        &GUIApp::handleCreateAccount
    );
}

GUIApp::~GUIApp() = default;

bool GUIApp::initialize()
{
    if (!database.openDatabase("semester_tracker.db"))
    {
        QMessageBox::critical(
            &window,
            "Database Error",
            "Could not open the database."
        );

        return false;
    }

    if (!database.createTables())
    {
        QMessageBox::critical(
            &window,
            "Database Error",
            "Could not create the database tables."
        );

        return false;
    }

    setupNotificationTray();
    restoreSavedLogin();

    QTimer::singleShot(
        1800,
        this,
        [this]()
        {
            updateManager->checkForUpdates(false);
        }
    );

    return true;
}

bool GUIApp::hasActiveSession() const
{
    return loggedInUser != nullptr;
}

void GUIApp::saveLoginToken(
    const std::string &token)
{
    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );

    settings.setValue(
        "authentication/sessionToken",
        QString::fromStdString(token)
    );

    settings.sync();
    activeSessionToken = token;
}

void GUIApp::clearSavedLogin()
{
    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );

    settings.remove(
        "authentication/sessionToken"
    );

    settings.sync();
    activeSessionToken.clear();
}

bool GUIApp::restoreSavedLogin()
{
    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );

    const QString savedToken =
        settings.value(
            "authentication/sessionToken"
        ).toString();

    if (savedToken.isEmpty())
    {
        return false;
    }

    User restoredUser(-1, "temporary");

    if (!authService.loginWithSession(
            savedToken.toStdString(),
            restoredUser))
    {
        clearSavedLogin();
        return false;
    }

    activeSessionToken =
        savedToken.toStdString();

    openDashboardForUser(
        std::move(restoredUser)
    );

    return true;
}

void GUIApp::openDashboardForUser(
    User user)
{
    loggedInUser =
        std::make_unique<User>(
            std::move(user)
        );

    if (!dashboard)
    {
        dashboard =
            std::make_unique<Dashboard>(
                database
            );

        connect(
            dashboard.get(),
            &Dashboard::logoutRequested,
            this,
            &GUIApp::handleLogout
        );
    }

    dashboard->setUserData(
        *loggedInUser
    );

    dashboard->setWindowFlag(
        Qt::Window,
        true
    );

    dashboard->showMaximized();
    dashboard->raise();
    dashboard->activateWindow();

    window.hide();

    QTimer::singleShot(
        600,
        this,
        &GUIApp::showDeadlineReminders
    );
}

void GUIApp::setupNotificationTray()
{
    if (trayIcon ||
        !QSystemTrayIcon::isSystemTrayAvailable())
    {
        return;
    }

    QIcon icon = QApplication::windowIcon();

    if (icon.isNull())
    {
        icon = QApplication::style()->standardIcon(
            QStyle::SP_ComputerIcon
        );
    }

    trayIcon = new QSystemTrayIcon(icon, this);
    trayIcon->setToolTip("Semester Tracker");

    connect(
        trayIcon,
        &QSystemTrayIcon::messageClicked,
        this,
        &GUIApp::openPendingDeadline
    );

    trayIcon->show();
}

void GUIApp::handleLogin()
{
    const QString username =
        window.ui->usernameLineEdit->text().trimmed();

    const QString password =
        window.ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(
            &window,
            "Missing Information",
            "Enter both your username and password."
        );

        return;
    }

    User user(-1, username.toStdString());

    const bool success = authService.loginUser(
        username.toStdString(),
        password.toStdString(),
        user
    );

    if (!success)
    {
        QMessageBox::warning(
            &window,
            "Login Failed",
            "Incorrect username or password."
        );

        window.ui->passwordLineEdit->clear();
        return;
    }

    std::string sessionToken;

    if (authService.createPersistentSession(
            user.getID(),
            sessionToken))
    {
        saveLoginToken(sessionToken);
    }
    else
    {
        clearSavedLogin();
    }

    window.ui->passwordLineEdit->clear();

    openDashboardForUser(
        std::move(user)
    );
}

void GUIApp::showDeadlineReminders()
{
    if (!loggedInUser || !dashboard)
    {
        return;
    }

    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );

    const bool remindersEnabled =
        settings.value(
            "notifications/enabled",
            true
        ).toBool();

    if (!remindersEnabled)
    {
        return;
    }

    int reminderDays =
        settings.value(
            "notifications/days",
            3
        ).toInt();

    if (reminderDays != 2 && reminderDays != 3)
    {
        reminderDays = 3;
    }

    const std::vector<DeadlineReminder> deadlines =
        collectNearbyDeadlines(reminderDays);

    if (deadlines.empty())
    {
        return;
    }

    std::vector<DeadlineReminder> newDeadlines;

    for (const DeadlineReminder &deadline : deadlines)
    {
        const QString key =
            QString("%1|%2")
                .arg(deadline.assignmentID)
                .arg(
                    deadline.dueDate.toString(
                        Qt::ISODate
                    )
                );

        if (!notifiedDeadlineKeys.contains(key))
        {
            notifiedDeadlineKeys.insert(key);
            newDeadlines.push_back(deadline);
        }
    }

    if (newDeadlines.empty())
    {
        return;
    }

    pendingNotificationDate =
        newDeadlines.front().dueDate;

    showStartupReminderDialog(newDeadlines);

    setupNotificationTray();

    if (!trayIcon)
    {
        return;
    }

    const DeadlineReminder &nearest =
        newDeadlines.front();

    const int daysAway =
        QDate::currentDate().daysTo(
            nearest.dueDate
        );

    QString timingText;

    if (daysAway == 0)
    {
        timingText = "due today";
    }
    else if (daysAway == 1)
    {
        timingText = "due tomorrow";
    }
    else
    {
        timingText =
            QString("due in %1 days")
                .arg(daysAway);
    }

    QString body =
        QString("%1 — %2 is %3.")
            .arg(nearest.courseCode)
            .arg(nearest.assignmentName)
            .arg(timingText);

    if (newDeadlines.size() > 1)
    {
        body += QString(
            "\n%1 more deadline%2 nearby."
        )
            .arg(
                static_cast<int>(
                    newDeadlines.size()
                ) - 1
            )
            .arg(
                newDeadlines.size() == 2
                    ? ""
                    : "s"
            );
    }

    body += "\nClick to open Calendar.";

    trayIcon->showMessage(
        "Upcoming Deadline",
        body,
        QSystemTrayIcon::Information,
        10000
    );
}

std::vector<GUIApp::DeadlineReminder>
GUIApp::collectNearbyDeadlines(
    int reminderDays)
{
    std::vector<DeadlineReminder> deadlines;

    if (!loggedInUser)
    {
        return deadlines;
    }

    const QDate today = QDate::currentDate();
    const QDate finalDate =
        today.addDays(reminderDays);

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(
            loggedInUser->getID()
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

                if (!dueDate.isValid() ||
                    dueDate < today ||
                    dueDate > finalDate)
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
                        dueDate
                    }
                );
            }
        }
    }

    std::sort(
        deadlines.begin(),
        deadlines.end(),
        [](const DeadlineReminder &left,
           const DeadlineReminder &right)
        {
            if (left.dueDate != right.dueDate)
            {
                return left.dueDate <
                       right.dueDate;
            }

            if (left.courseCode != right.courseCode)
            {
                return left.courseCode <
                       right.courseCode;
            }

            return left.assignmentName <
                   right.assignmentName;
        }
    );

    return deadlines;
}

void GUIApp::showStartupReminderDialog(
    const std::vector<DeadlineReminder> &deadlines)
{
    if (!dashboard || deadlines.empty())
    {
        return;
    }

    QString details;

    const int visibleCount =
        std::min(
            static_cast<int>(deadlines.size()),
            6
        );

    for (int index = 0;
         index < visibleCount;
         ++index)
    {
        const DeadlineReminder &deadline =
            deadlines[index];

        const int daysAway =
            QDate::currentDate().daysTo(
                deadline.dueDate
            );

        QString timing;

        if (daysAway == 0)
        {
            timing = "Today";
        }
        else if (daysAway == 1)
        {
            timing = "Tomorrow";
        }
        else
        {
            timing =
                QString("In %1 days")
                    .arg(daysAway);
        }

        details +=
            QString("• %1 — %2 (%3)\n")
                .arg(deadline.courseCode)
                .arg(deadline.assignmentName)
                .arg(timing);
    }

    if (deadlines.size() >
        static_cast<std::size_t>(visibleCount))
    {
        details += QString(
            "• And %1 more…"
        ).arg(
            static_cast<int>(deadlines.size()) -
            visibleCount
        );
    }

    QMessageBox reminderBox(dashboard.get());

    reminderBox.setWindowTitle(
        "Upcoming Deadlines"
    );

    reminderBox.setIcon(
        QMessageBox::Information
    );

    reminderBox.setText(
        QString(
            "You have %1 assignment deadline%2 "
            "within your reminder window."
        )
            .arg(
                static_cast<int>(
                    deadlines.size()
                )
            )
            .arg(
                deadlines.size() == 1
                    ? ""
                    : "s"
            )
    );

    reminderBox.setInformativeText(details);

    QAbstractButton *openCalendarButton =
        reminderBox.addButton(
            "Open Calendar",
            QMessageBox::ActionRole
        );

    reminderBox.addButton(
        QMessageBox::Ok
    );

    reminderBox.exec();

    if (reminderBox.clickedButton() ==
        openCalendarButton)
    {
        openPendingDeadline();
    }
}

void GUIApp::openPendingDeadline()
{
    if (!dashboard ||
        !pendingNotificationDate.isValid())
    {
        return;
    }

    dashboard->showMaximized();
    dashboard->raise();
    dashboard->activateWindow();

    dashboard->openCalendarDate(
        pendingNotificationDate
    );
}

void GUIApp::handleCreateAccount()
{
    const QString username =
        window.ui->usernameLineEdit->text().trimmed();

    const QString password =
        window.ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(
            &window,
            "Missing Information",
            "Enter both a username and password."
        );

        return;
    }

    const QRegularExpression usernamePattern(
        "^[A-Za-z0-9._-]{3,40}$"
    );

    if (!usernamePattern.match(username).hasMatch())
    {
        QMessageBox::warning(
            &window,
            "Invalid Username",
            "Username must contain 3 to 40 characters and may only use "
            "letters, numbers, periods, underscores, and hyphens."
        );

        return;
    }

    if (database.userExists(username.toStdString()))
    {
        QMessageBox::warning(
            &window,
            "Username Already Used",
            "An account with this username already exists."
        );

        return;
    }

    if (password.length() < 8 ||
        password.length() > 128)
    {
        QMessageBox::warning(
            &window,
            "Invalid Password",
            "Password must contain between 8 and 128 characters."
        );

        return;
    }

    const bool hasLetter =
        password.contains(
            QRegularExpression("[A-Za-z]")
        );

    const bool hasNumber =
        password.contains(
            QRegularExpression("[0-9]")
        );

    if (!hasLetter || !hasNumber)
    {
        QMessageBox::warning(
            &window,
            "Invalid Password",
            "Password must include at least one letter and one number."
        );

        return;
    }

    bool accepted = false;

    const int maxCredits = QInputDialog::getInt(
        &window,
        "Maximum Credits",
        "Enter the total credits required for your degree:",
        120,
        1,
        1000,
        1,
        &accepted
    );

    if (!accepted)
    {
        return;
    }

    const bool success = authService.registerUser(
        username.toStdString(),
        password.toStdString(),
        maxCredits
    );

    if (!success)
    {
        QMessageBox::warning(
            &window,
            "Registration Failed",
            "The account could not be created because of a database error."
        );

        return;
    }

    QMessageBox::information(
        &window,
        "Account Created",
        "Your account was created successfully."
    );

    window.ui->passwordLineEdit->clear();
}

void GUIApp::handleLogout()
{
    if (!activeSessionToken.empty())
    {
        authService.logoutSession(
            activeSessionToken
        );
    }

    clearSavedLogin();
    loggedInUser.reset();
    notifiedDeadlineKeys.clear();
    pendingNotificationDate = QDate();

    if (dashboard)
    {
        dashboard->hide();
    }

    window.ui->usernameLineEdit->clear();
    window.ui->passwordLineEdit->clear();

    window.show();
    window.raise();
    window.activateWindow();

    window.ui->usernameLineEdit->setFocus();
}
