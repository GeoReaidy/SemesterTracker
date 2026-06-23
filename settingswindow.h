#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "authservice.h"
#include "databasemanager.h"
#include "user.h"

#include <QWidget>

#include <string>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SettingsWindow;
}
QT_END_NAMESPACE

class UpdateManager;

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(
        DatabaseManager &database,
        QWidget *parent = nullptr
    );

    ~SettingsWindow() override;

    void setUserData(const User &user);

signals:
    void profileUpdated();
    void accountDeleted();

private:
    void saveProfile();
    void changePassword();
    void deleteAccount();
    void loadReminderPreferences();
    void saveReminderPreferences();

    DatabaseManager &database;
    AuthService authService;
    UpdateManager *updateManager = nullptr;

    int currentUserID = -1;
    std::string currentUsername;

    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
