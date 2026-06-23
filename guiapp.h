#ifndef GUIAPP_H
#define GUIAPP_H

#include "authservice.h"
#include "databasemanager.h"
#include "user.h"

#include <QDate>
#include <QObject>
#include <QSet>
#include <QString>

#include <memory>
#include <vector>

class MainWindow;
class Dashboard;
class QSystemTrayIcon;
class UpdateManager;

class GUIApp : public QObject
{
    Q_OBJECT

public:
    explicit GUIApp(
        MainWindow &window,
        QObject *parent = nullptr
    );

    ~GUIApp() override;

    bool initialize();
    bool hasActiveSession() const;

private slots:
    void handleLogin();
    void handleCreateAccount();
    void handleLogout();
    void openPendingDeadline();

private:
    struct DeadlineReminder
    {
        int assignmentID = -1;
        QString assignmentName;
        QString courseCode;
        QDate dueDate;
    };

    void setupNotificationTray();
    void openDashboardForUser(User user);
    bool restoreSavedLogin();
    void saveLoginToken(const std::string &token);
    void clearSavedLogin();
    void showDeadlineReminders();
    std::vector<DeadlineReminder>
    collectNearbyDeadlines(int reminderDays);
    void showStartupReminderDialog(
        const std::vector<DeadlineReminder> &deadlines
    );

    MainWindow &window;
    DatabaseManager database;
    AuthService authService;

    std::unique_ptr<User> loggedInUser;
    std::unique_ptr<Dashboard> dashboard;
    std::string activeSessionToken;

    QSystemTrayIcon *trayIcon = nullptr;
    UpdateManager *updateManager = nullptr;
    QDate pendingNotificationDate;
    QSet<QString> notifiedDeadlineKeys;
};

#endif // GUIAPP_H
