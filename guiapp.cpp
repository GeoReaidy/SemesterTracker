#include "guiapp.h"

#include "dashboard.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "updatemanager.h"

#include <QApplication>
#include <QDebug>
#include <QIcon>
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
        this->window.ui->getStartedButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showAuthPage(
                this->window.ui->accountChoicePage
            );
        }
    );

    connect(
        this->window.ui->choiceBackButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showAuthPage(
                this->window.ui->welcomePage
            );
        }
    );

    connect(
        this->window.ui->chooseSignInButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showAuthPage(
                this->window.ui->signInPage
            );

            this->window.ui
                ->loginIdentifierLineEdit
                ->setFocus();
        }
    );

    connect(
        this->window.ui->chooseCreateAccountButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showAuthPage(
                this->window.ui->createAccountPage
            );

            this->window.ui
                ->registerUsernameLineEdit
                ->setFocus();
        }
    );

    connect(
        this->window.ui->loginBackButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showAuthPage(
                this->window.ui->accountChoicePage
            );
        }
    );

    connect(
        this->window.ui->registerBackButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showAuthPage(
                this->window.ui->accountChoicePage
            );
        }
    );

    connect(
        this->window.ui->loginButton,
        &QPushButton::clicked,
        this,
        &GUIApp::handleLogin
    );

    connect(
        this->window.ui->loginPasswordLineEdit,
        &QLineEdit::returnPressed,
        this,
        &GUIApp::handleLogin
    );

    connect(
        this->window.ui->registerButton,
        &QPushButton::clicked,
        this,
        &GUIApp::handleCreateAccount
    );

    connect(
        this->window.ui->registerConfirmPasswordLineEdit,
        &QLineEdit::returnPressed,
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

    if (!restoreSavedLogin())
    {
        showAuthPage(
            window.ui->welcomePage
        );
    }

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


void GUIApp::showAuthPage(QWidget *page)
{
    clearAuthMessage();

    window.ui->authStackedWidget
        ->setCurrentWidget(page);
}

void GUIApp::showAuthMessage(
    const QString &message,
    bool success)
{
    if (message.trimmed().isEmpty())
    {
        clearAuthMessage();
        return;
    }

    window.ui->authMessageLabel
        ->setStyleSheet(
            success
                ? QStringLiteral(
                      "color: #166534;"
                      "background-color: #f0fdf4;"
                      "border: 1px solid #bbf7d0;"
                      "border-radius: 8px;"
                      "padding: 9px 10px;"
                  )
                : QStringLiteral(
                      "color: #b91c1c;"
                      "background-color: #fef2f2;"
                      "border: 1px solid #fecaca;"
                      "border-radius: 8px;"
                      "padding: 9px 10px;"
                  )
        );

    window.ui->authMessageLabel
        ->setText(message);

    window.ui->authMessageLabel
        ->show();
}

void GUIApp::clearAuthMessage()
{
    window.ui->authMessageLabel->clear();
    window.ui->authMessageLabel->hide();
}

bool GUIApp::validEmail(
    const QString &email) const
{
    static const QRegularExpression pattern(
        QStringLiteral(
            "^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$"
        )
    );

    return email.size() <= 254 &&
           pattern.match(
               email.trimmed()
           ).hasMatch();
}

bool GUIApp::validPassword(
    const QString &password) const
{
    return password.size() >= 8 &&
           password.size() <= 128 &&
           password.contains(
               QRegularExpression(
                   QStringLiteral("[A-Za-z]")
               )
           ) &&
           password.contains(
               QRegularExpression(
                   QStringLiteral("[0-9]")
               )
           );
}

void GUIApp::createSessionAndOpenDashboard(
    User user)
{
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

    openDashboardForUser(
        std::move(user)
    );
}

void GUIApp::resetAuthenticationFields()
{
    window.ui->loginIdentifierLineEdit->clear();
    window.ui->loginPasswordLineEdit->clear();

    window.ui->registerUsernameLineEdit->clear();
    window.ui->registerEmailLineEdit->clear();
    window.ui->registerPasswordLineEdit->clear();
    window.ui
        ->registerConfirmPasswordLineEdit
        ->clear();

    window.ui->registerMaxCreditsSpinBox
        ->setValue(120);

    clearAuthMessage();
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
    clearAuthMessage();

    const QString identifier =
        window.ui
            ->loginIdentifierLineEdit
            ->text()
            .trimmed();

    const QString password =
        window.ui
            ->loginPasswordLineEdit
            ->text();

    if (identifier.isEmpty() ||
        password.isEmpty())
    {
        showAuthMessage(
            tr(
                "Enter your username or email and password."
            )
        );
        return;
    }

    User user(-1, "temporary");

    const bool success =
        authService.loginUser(
            identifier.toStdString(),
            password.toStdString(),
            user
        );

    if (!success)
    {
        showAuthMessage(
            tr(
                "The username/email or password is incorrect."
            )
        );

        window.ui
            ->loginPasswordLineEdit
            ->clear();

        window.ui
            ->loginPasswordLineEdit
            ->setFocus();

        return;
    }

    window.ui
        ->loginPasswordLineEdit
        ->clear();

    createSessionAndOpenDashboard(
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
                if (assignment.isCompleted())
                {
                    continue;
                }

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
    clearAuthMessage();

    const QString username =
        window.ui
            ->registerUsernameLineEdit
            ->text()
            .trimmed();

    const QString email =
        window.ui
            ->registerEmailLineEdit
            ->text()
            .trimmed()
            .toLower();

    const QString password =
        window.ui
            ->registerPasswordLineEdit
            ->text();

    const QString confirmedPassword =
        window.ui
            ->registerConfirmPasswordLineEdit
            ->text();

    const int maxCredits =
        window.ui
            ->registerMaxCreditsSpinBox
            ->value();

    const QRegularExpression usernamePattern(
        QStringLiteral(
            "^[A-Za-z0-9._-]{3,40}$"
        )
    );

    if (!usernamePattern
             .match(username)
             .hasMatch())
    {
        showAuthMessage(
            tr(
                "Username must contain 3–40 letters, "
                "numbers, periods, underscores, or hyphens."
            )
        );
        return;
    }

    if (!validEmail(email))
    {
        showAuthMessage(
            tr("Enter a valid email address.")
        );
        return;
    }

    if (!validPassword(password))
    {
        showAuthMessage(
            tr(
                "Password must contain 8–128 characters, "
                "including at least one letter and one number."
            )
        );
        return;
    }

    if (password != confirmedPassword)
    {
        showAuthMessage(
            tr("The password confirmation does not match.")
        );
        return;
    }

    if (database.userExists(
            username.toStdString()))
    {
        showAuthMessage(
            tr("That username is already used.")
        );
        return;
    }

    if (database.emailExists(
            email.toStdString()))
    {
        showAuthMessage(
            tr("That email is already linked to an account.")
        );
        return;
    }

    window.ui->registerButton->setEnabled(
        false
    );

    const bool registered =
        authService.registerUser(
            username.toStdString(),
            email.toStdString(),
            password.toStdString(),
            maxCredits
        );

    window.ui->registerButton->setEnabled(
        true
    );

    if (!registered)
    {
        showAuthMessage(
            tr(
                "The account could not be created. "
                "Check the information and try again."
            )
        );
        return;
    }

    User user(-1, "temporary");

    if (!authService.loginUser(
            username.toStdString(),
            password.toStdString(),
            user))
    {
        showAuthPage(
            window.ui->signInPage
        );

        window.ui
            ->loginIdentifierLineEdit
            ->setText(username);

        showAuthMessage(
            tr(
                "The account was created. "
                "Sign in to continue."
            ),
            true
        );

        return;
    }

    resetAuthenticationFields();

    createSessionAndOpenDashboard(
        std::move(user)
    );
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

    resetAuthenticationFields();

    showAuthPage(
        window.ui->welcomePage
    );

    window.show();
    window.raise();
    window.activateWindow();

    window.ui->getStartedButton->setFocus();
}
