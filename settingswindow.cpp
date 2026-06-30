#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "updatemanager.h"

#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>

#include <exception>

SettingsWindow::SettingsWindow(
    DatabaseManager &database,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      authService(database),
      ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    setStyleSheet(styleSheet() + R"(
    QComboBox {
        background-color: white;
        color: #1f2937;
        border: 1px solid #cbd5e1;
        border-radius: 6px;
        padding: 6px 30px 6px 10px;
        min-height: 22px;
    }

    QComboBox:hover {
        border-color: #94a3b8;
    }

    QComboBox:focus {
        border-color: #2563eb;
    }

    QComboBox:disabled {
        background-color: #f1f5f9;
        color: #94a3b8;
    }

    QComboBox::drop-down {
        subcontrol-origin: padding;
        subcontrol-position: top right;
        width: 28px;
        border-left: 1px solid #cbd5e1;
    }

    QComboBox QAbstractItemView {
        background-color: white;
        color: #1f2937;
        border: 1px solid #cbd5e1;
        selection-background-color: #2563eb;
        selection-color: white;
        outline: none;
        padding: 4px;
    }

    QComboBox QAbstractItemView::item {
        min-height: 28px;
        padding: 4px 8px;
        color: #1f2937;
        background-color: white;
    }

    QComboBox QAbstractItemView::item:hover {
        background-color: #e2e8f0;
        color: #111827;
    }

    QComboBox QAbstractItemView::item:selected {
        background-color: #2563eb;
        color: white;
    }
)");

    updateManager =
        new UpdateManager(
            this,
            this
        );

    QLabel *versionLabel =
        new QLabel(
            QString("Current version: %1")
                .arg(QStringLiteral(APP_VERSION)),
            this
        );

    versionLabel->setStyleSheet(
        "color: #64748b; font-size: 12px;"
    );

    QPushButton *checkUpdatesButton =
        new QPushButton(
            "Check for Updates",
            this
        );

    checkUpdatesButton->setStyleSheet(R"(
        QPushButton {
            min-height: 38px;
            padding: 0 18px;
            color: #4f46e5;
            background-color: #eef2ff;
            border: 1px solid #c7d2fe;
            border-radius: 8px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #e0e7ff;
        }
    )");

    QHBoxLayout *updateLayout =
        new QHBoxLayout();

    updateLayout->addWidget(versionLabel);
    updateLayout->addStretch();
    updateLayout->addWidget(checkUpdatesButton);

    ui->preferencesCardLayout->addLayout(
        updateLayout
    );

    connect(
        checkUpdatesButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            updateManager->checkForUpdates(true);
        }
    );

    ui->maximumCreditsLineEdit->setValidator(
        new QIntValidator(
            1,
            1000,
            ui->maximumCreditsLineEdit
        )
    );

    ui->currentPasswordLineEdit->setEchoMode(
        QLineEdit::Password
    );

    ui->newPasswordLineEdit->setEchoMode(
        QLineEdit::Password
    );

    ui->confirmPasswordLineEdit->setEchoMode(
        QLineEdit::Password
    );

    ui->settingsScrollArea->setStyleSheet(R"(
        QScrollArea {
            background-color: #f5f7fb;
            border: none;
        }

        QScrollArea > QWidget > QWidget {
            background-color: #f5f7fb;
        }
    )");

    const QString cardStyle = R"(
        QFrame {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
        }

        QFrame QLabel,
        QFrame QLineEdit,
        QFrame QPushButton,
        QFrame QCheckBox,
        QFrame QComboBox {
            border: none;
        }

        QLineEdit,
        QComboBox {
            min-height: 38px;
            padding: 0 12px;
            color: #111827;
            background-color: #f8fafc;
            border: 1px solid #dbe2ea;
            border-radius: 8px;
        }

        QLineEdit:focus,
        QComboBox:focus {
            border: 1px solid #4f46e5;
            background-color: #ffffff;
        }

        QPushButton {
            min-height: 38px;
            padding: 0 18px;
            border-radius: 8px;
            font-weight: 600;
        }
    )";

    ui->profileCard->setStyleSheet(cardStyle);
    ui->securityCard->setStyleSheet(cardStyle);
    ui->preferencesCard->setStyleSheet(cardStyle);
    ui->dataManagementCard->setStyleSheet(cardStyle);
    ui->dangerCard->setStyleSheet(cardStyle);

    ui->saveProfileButton->setStyleSheet(R"(
        QPushButton {
            color: #ffffff;
            background-color: #4f46e5;
        }

        QPushButton:hover {
            background-color: #4338ca;
        }

        QPushButton:disabled {
            color: #cbd5e1;
            background-color: #e2e8f0;
        }
    )");

    ui->changePasswordButton->setStyleSheet(
        ui->saveProfileButton->styleSheet()
    );

    ui->backupDatabaseButton->setStyleSheet(
        ui->saveProfileButton->styleSheet()
    );

    ui->restoreDatabaseButton->setStyleSheet(R"(
        QPushButton {
            color: #334155;
            background-color: #f8fafc;
            border: 1px solid #cbd5e1;
        }

        QPushButton:hover {
            background-color: #f1f5f9;
            border-color: #94a3b8;
        }
    )");

    ui->exportUserDataButton->setStyleSheet(
        ui->saveProfileButton->styleSheet()
    );

    ui->importUserDataButton->setStyleSheet(
        ui->restoreDatabaseButton->styleSheet()
    );

    ui->deleteAccountButton->setStyleSheet(R"(
        QPushButton {
            color: #b91c1c;
            background-color: #fee2e2;
            border: 1px solid #fecaca;
        }

        QPushButton:hover {
            background-color: #fecaca;
        }

        QPushButton:disabled {
            color: #94a3b8;
            background-color: #f1f5f9;
            border: 1px solid #e2e8f0;
        }
    )");

    connect(
        ui->saveProfileButton,
        &QPushButton::clicked,
        this,
        &SettingsWindow::saveProfile
    );

    connect(
        ui->changePasswordButton,
        &QPushButton::clicked,
        this,
        &SettingsWindow::changePassword
    );

    connect(
        ui->deleteAccountButton,
        &QPushButton::clicked,
        this,
        &SettingsWindow::deleteAccount
    );

    connect(
        ui->backupDatabaseButton,
        &QPushButton::clicked,
        this,
        &SettingsWindow::createDatabaseBackup
    );

    connect(
        ui->restoreDatabaseButton,
        &QPushButton::clicked,
        this,
        &SettingsWindow::restoreDatabaseBackup
    );

    connect(
        ui->exportUserDataButton,
        &QPushButton::clicked,
        this,
        &SettingsWindow::exportUserData
    );

    connect(
        ui->importUserDataButton,
        &QPushButton::clicked,
        this,
        &SettingsWindow::importUserData
    );

    connect(
        ui->deadlineRemindersCheckBox,
        &QCheckBox::toggled,
        this,
        [this](bool enabled)
        {
            ui->reminderDaysComboBox->setEnabled(enabled);
            saveReminderPreferences();
        }
    );

    connect(
        ui->reminderDaysComboBox,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            saveReminderPreferences();
        }
    );

    loadReminderPreferences();

    ui->saveProfileButton->setEnabled(false);
    ui->changePasswordButton->setEnabled(false);
    ui->deleteAccountButton->setEnabled(false);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setUserData(const User &user)
{
    currentUserID = user.getID();
    currentUsername = user.getUsername();
    currentEmail =
        database.getEmailByUserID(
            currentUserID
        );

    ui->usernameLineEdit->setText(
        QString::fromStdString(
            currentUsername
        )
    );

    ui->emailLineEdit->setText(
        QString::fromStdString(
            currentEmail
        )
    );

    ui->maximumCreditsLineEdit->setText(
        QString::number(
            user.getMaxCredits()
        )
    );

    ui->accountIdValueLabel->setText(
        QString::number(
            currentUserID
        )
    );

    const bool validUser = currentUserID > 0;

    ui->saveProfileButton->setEnabled(validUser);
    ui->changePasswordButton->setEnabled(validUser);
    ui->deleteAccountButton->setEnabled(validUser);
}

void SettingsWindow::saveProfile()
{
    if (currentUserID <= 0)
    {
        return;
    }

    const QString username =
        ui->usernameLineEdit->text().trimmed();

    const QString email =
        ui->emailLineEdit
            ->text()
            .trimmed()
            .toLower();

    static const QRegularExpression emailPattern(
        QStringLiteral(
            "^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$"
        )
    );

    if (email.size() > 254 ||
        !emailPattern.match(email).hasMatch())
    {
        QMessageBox::warning(
            this,
            "Invalid Email",
            "Enter a valid email address."
        );
        return;
    }

    bool creditsAccepted = false;

    const int maximumCredits =
        ui->maximumCreditsLineEdit->text().toInt(
            &creditsAccepted
        );

    if (!creditsAccepted)
    {
        QMessageBox::warning(
            this,
            "Invalid Credits",
            "Maximum credits must be a whole number."
        );
        return;
    }

    try
    {
        User candidate =
            database.loadFullUserByUsername(
                currentUsername
            );

        candidate.setUsername(
            username.toStdString()
        );

        candidate.setMaxCredits(
            maximumCredits
        );
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            "Invalid Profile",
            error.what()
        );
        return;
    }

    if (!database.updateUserProfile(
            currentUserID,
            username.toStdString(),
            email.toStdString(),
            maximumCredits))
    {
        QMessageBox::warning(
            this,
            "Profile Not Updated",
            "The username or email may already be used, "
            "or the values may be invalid."
        );
        return;
    }

    currentUsername = username.toStdString();
    currentEmail = email.toStdString();

    QMessageBox::information(
        this,
        "Profile Updated",
        "Your profile was updated successfully."
    );

    emit profileUpdated();
}

void SettingsWindow::changePassword()
{
    if (currentUserID <= 0)
    {
        return;
    }

    const QString currentPassword =
        ui->currentPasswordLineEdit->text();

    const QString newPassword =
        ui->newPasswordLineEdit->text();

    const QString confirmedPassword =
        ui->confirmPasswordLineEdit->text();

    if (currentPassword.isEmpty() ||
        newPassword.isEmpty() ||
        confirmedPassword.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Missing Password",
            "Complete all three password fields."
        );
        return;
    }

    if (newPassword != confirmedPassword)
    {
        QMessageBox::warning(
            this,
            "Passwords Do Not Match",
            "The new password and confirmation do not match."
        );
        return;
    }

    if (!authService.changePassword(
            currentUserID,
            currentUsername,
            currentPassword.toStdString(),
            newPassword.toStdString()))
    {
        QMessageBox::warning(
            this,
            "Password Not Changed",
            "Check your current password. The new password must contain "
            "8–128 characters, including at least one letter and one number, "
            "and it must differ from the current password."
        );
        return;
    }

    ui->currentPasswordLineEdit->clear();
    ui->newPasswordLineEdit->clear();
    ui->confirmPasswordLineEdit->clear();

    QMessageBox::information(
        this,
        "Password Changed",
        "Your password was changed successfully."
    );
}

void SettingsWindow::deleteAccount()
{
    if (currentUserID <= 0)
    {
        return;
    }

    const QMessageBox::StandardButton confirmation =
        QMessageBox::warning(
            this,
            "Delete Account",
            "This permanently deletes your account and all associated "
            "semesters, courses, assignments, and grades.\n\n"
            "This action cannot be undone.",
            QMessageBox::Yes | QMessageBox::Cancel,
            QMessageBox::Cancel
        );

    if (confirmation != QMessageBox::Yes)
    {
        return;
    }

    bool passwordAccepted = false;

    const QString password =
        QInputDialog::getText(
            this,
            "Confirm Account Deletion",
            "Enter your current password:",
            QLineEdit::Password,
            QString(),
            &passwordAccepted
        );

    if (!passwordAccepted || password.isEmpty())
    {
        return;
    }

    User verifiedUser(-1, "temporary", 1);

    if (!authService.loginUser(
            currentUsername,
            password.toStdString(),
            verifiedUser))
    {
        QMessageBox::warning(
            this,
            "Incorrect Password",
            "The account was not deleted because the password was incorrect."
        );
        return;
    }

    const QMessageBox::StandardButton finalConfirmation =
        QMessageBox::critical(
            this,
            "Final Confirmation",
            QString(
                "Permanently delete the account “%1”?"
            ).arg(
                QString::fromStdString(
                    currentUsername
                )
            ),
            QMessageBox::Yes | QMessageBox::Cancel,
            QMessageBox::Cancel
        );

    if (finalConfirmation != QMessageBox::Yes)
    {
        return;
    }

    if (!database.deleteUser(currentUserID))
    {
        QMessageBox::critical(
            this,
            "Deletion Failed",
            "The account could not be deleted."
        );
        return;
    }

    currentUserID = -1;
    currentUsername.clear();
    currentEmail.clear();

    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );
    settings.remove("authentication/sessionToken");
    settings.sync();

    emit accountDeleted();
}

void SettingsWindow::loadReminderPreferences()
{
    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );

    const bool remindersEnabled =
        settings.value(
            "notifications/enabled",
            true
        ).toBool();

    const int reminderDays =
        settings.value(
            "notifications/days",
            3
        ).toInt();

    ui->deadlineRemindersCheckBox->setChecked(
        remindersEnabled
    );

    ui->reminderDaysComboBox->setCurrentIndex(
        reminderDays == 2 ? 0 : 1
    );

    ui->reminderDaysComboBox->setEnabled(
        remindersEnabled
    );
}

void SettingsWindow::saveReminderPreferences()
{
    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );

    settings.setValue(
        "notifications/enabled",
        ui->deadlineRemindersCheckBox->isChecked()
    );

    settings.setValue(
        "notifications/days",
        ui->reminderDaysComboBox->currentIndex() == 0
            ? 2
            : 3
    );
}


void SettingsWindow::createDatabaseBackup()
{
    QString defaultDirectory =
        QStandardPaths::writableLocation(
            QStandardPaths::DocumentsLocation
        );

    if (defaultDirectory.isEmpty())
    {
        defaultDirectory =
            QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation
            );
    }

    const QString defaultName =
        QString(
            "SemesterTracker_Backup_%1.db"
        ).arg(
            QDateTime::currentDateTime().toString(
                "yyyy-MM-dd_HH-mm-ss"
            )
        );

    const QString destinationPath =
        QFileDialog::getSaveFileName(
            this,
            "Create Database Backup",
            defaultDirectory + "/" + defaultName,
            "SemesterTracker Database (*.db *.sqlite *.bak);;All Files (*)"
        );

    if (destinationPath.isEmpty())
    {
        return;
    }

    std::string errorMessage;

    if (!database.backupDatabase(
            destinationPath.toStdString(),
            &errorMessage))
    {
        QMessageBox::critical(
            this,
            "Backup Failed",
            QString::fromStdString(errorMessage)
        );
        return;
    }

    QMessageBox::information(
        this,
        "Backup Created",
        QString(
            "A complete database backup was created successfully.\n\n%1"
        ).arg(
            QDir::toNativeSeparators(
                destinationPath
            )
        )
    );
}

void SettingsWindow::restoreDatabaseBackup()
{
    const QString sourcePath =
        QFileDialog::getOpenFileName(
            this,
            "Restore Database Backup",
            QStandardPaths::writableLocation(
                QStandardPaths::DocumentsLocation
            ),
            "SemesterTracker Database (*.db *.sqlite *.bak);;All Files (*)"
        );

    if (sourcePath.isEmpty())
    {
        return;
    }

    const QMessageBox::StandardButton confirmation =
        QMessageBox::warning(
            this,
            "Restore Database Backup",
            "Restoring replaces the current database with the selected backup.\n\n"
            "SemesterTracker will create an automatic safety backup first, "
            "then close after the restore so the restored data can be loaded cleanly.",
            QMessageBox::Yes | QMessageBox::Cancel,
            QMessageBox::Cancel
        );

    if (confirmation != QMessageBox::Yes)
    {
        return;
    }

    std::string safetyBackupPath;
    std::string errorMessage;

    if (!database.restoreDatabase(
            sourcePath.toStdString(),
            &safetyBackupPath,
            &errorMessage))
    {
        QMessageBox::critical(
            this,
            "Restore Failed",
            QString::fromStdString(errorMessage)
        );
        return;
    }

    QSettings settings(
        "SemesterTracker",
        "SemesterTrackerGUI"
    );

    settings.remove(
        "authentication/sessionToken"
    );
    settings.sync();

    QMessageBox::information(
        this,
        "Restore Complete",
        QString(
            "The database was restored successfully.\n\n"
            "A safety backup of the previous database was saved at:\n%1\n\n"
            "SemesterTracker will now close. Open it again to load the restored data."
        ).arg(
            QDir::toNativeSeparators(
                QString::fromStdString(
                    safetyBackupPath
                )
            )
        )
    );

    QApplication::quit();
}


void SettingsWindow::exportUserData()
{
    if (currentUserID < 0)
    {
        QMessageBox::warning(
            this,
            "Export Unavailable",
            "No signed-in user is available to export."
        );
        return;
    }

    QString directory =
        QStandardPaths::writableLocation(
            QStandardPaths::DocumentsLocation
        );

    if (directory.isEmpty())
    {
        directory =
            QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation
            );
    }

    const QString defaultName =
        QString(
            "SemesterTracker_UserData_%1_%2.json"
        ).arg(
            QString::fromStdString(
                currentUsername
            ),
            QDateTime::currentDateTime().toString(
                "yyyy-MM-dd_HH-mm-ss"
            )
        );

    const QString destinationPath =
        QFileDialog::getSaveFileName(
            this,
            "Export User Data",
            directory + "/" + defaultName,
            "SemesterTracker User Data (*.json)"
        );

    if (destinationPath.isEmpty())
    {
        return;
    }

    std::string errorMessage;

    if (!database.exportUserData(
            currentUserID,
            destinationPath.toStdString(),
            &errorMessage))
    {
        QMessageBox::critical(
            this,
            "Export Failed",
            QString::fromStdString(
                errorMessage
            )
        );
        return;
    }

    QMessageBox::information(
        this,
        "User Data Exported",
        QString(
            "Your semesters, courses, assignments, categories, "
            "and academic settings were exported successfully.\n\n%1\n\n"
            "Passwords and login sessions are never included."
        ).arg(
            QDir::toNativeSeparators(
                destinationPath
            )
        )
    );
}

void SettingsWindow::importUserData()
{
    if (currentUserID < 0)
    {
        QMessageBox::warning(
            this,
            "Import Unavailable",
            "No signed-in user is available for import."
        );
        return;
    }

    const QString sourcePath =
        QFileDialog::getOpenFileName(
            this,
            "Import User Data",
            QStandardPaths::writableLocation(
                QStandardPaths::DocumentsLocation
            ),
            "SemesterTracker User Data (*.json)"
        );

    if (sourcePath.isEmpty())
    {
        return;
    }

    UserDataImportSummary preview;
    std::string errorMessage;

    if (!database.inspectUserDataFile(
            sourcePath.toStdString(),
            &preview,
            &errorMessage))
    {
        QMessageBox::critical(
            this,
            "Invalid User Data",
            QString::fromStdString(
                errorMessage
            )
        );
        return;
    }

    QMessageBox modeDialog(this);
    modeDialog.setIcon(
        QMessageBox::Question
    );
    modeDialog.setWindowTitle(
        "Choose Import Mode"
    );
    modeDialog.setText(
        QString(
            "The file contains %1 semester(s), %2 course(s), "
            "%3 assignment(s), and %4 category item(s)."
        ).arg(
            preview.semesters
        ).arg(
            preview.courses
        ).arg(
            preview.assignments
        ).arg(
            preview.categories
        )
    );
    modeDialog.setInformativeText(
        "Merge adds the imported records to your current data.\n"
        "Replace removes your current academic records first.\n\n"
        "Your username, email, password, and login session are not replaced."
    );

    QPushButton *mergeButton =
        modeDialog.addButton(
            "Merge",
            QMessageBox::AcceptRole
        );

    QPushButton *replaceButton =
        modeDialog.addButton(
            "Replace",
            QMessageBox::DestructiveRole
        );

    modeDialog.addButton(
        QMessageBox::Cancel
    );
    modeDialog.exec();

    if (modeDialog.clickedButton() !=
            mergeButton &&
        modeDialog.clickedButton() !=
            replaceButton)
    {
        return;
    }

    const UserDataImportMode mode =
        modeDialog.clickedButton() ==
                replaceButton
            ? UserDataImportMode::Replace
            : UserDataImportMode::Merge;

    if (mode ==
        UserDataImportMode::Replace)
    {
        const QMessageBox::StandardButton confirmation =
            QMessageBox::warning(
                this,
                "Replace Current User Data",
                "This removes the current user's semesters, courses, "
                "assignments, and custom categories before importing.\n\n"
                "Create a database backup first if you may need to undo this.",
                QMessageBox::Yes |
                    QMessageBox::Cancel,
                QMessageBox::Cancel
            );

        if (confirmation !=
            QMessageBox::Yes)
        {
            return;
        }
    }

    UserDataImportSummary imported;

    if (!database.importUserData(
            currentUserID,
            sourcePath.toStdString(),
            mode,
            &imported,
            &errorMessage))
    {
        QMessageBox::critical(
            this,
            "Import Failed",
            QString::fromStdString(
                errorMessage
            )
        );
        return;
    }

    QMessageBox::information(
        this,
        "Import Complete",
        QString(
            "Imported %1 semester(s), %2 course(s), "
            "%3 assignment(s), and %4 new category item(s).\n\n"
            "SemesterTracker will now close so all windows reload the imported data cleanly."
        ).arg(
            imported.semesters
        ).arg(
            imported.courses
        ).arg(
            imported.assignments
        ).arg(
            imported.categories
        )
    );

    QApplication::quit();
}
