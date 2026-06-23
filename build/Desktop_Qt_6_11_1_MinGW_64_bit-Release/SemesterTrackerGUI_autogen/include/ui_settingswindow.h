/********************************************************************************
** Form generated from reading UI file 'settingswindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSWINDOW_H
#define UI_SETTINGSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsWindow
{
public:
    QVBoxLayout *rootLayout;
    QScrollArea *settingsScrollArea;
    QWidget *settingsScrollAreaContents;
    QVBoxLayout *contentLayout;
    QLabel *pageTitleLabel;
    QLabel *pageSubtitleLabel;
    QFrame *profileCard;
    QVBoxLayout *profileCardLayout;
    QLabel *profileTitleLabel;
    QLabel *profileDescriptionLabel;
    QFormLayout *profileFormLayout;
    QLabel *usernameLabel;
    QLineEdit *usernameLineEdit;
    QLabel *maximumCreditsLabel;
    QLineEdit *maximumCreditsLineEdit;
    QLabel *accountIdLabel;
    QLabel *accountIdValueLabel;
    QHBoxLayout *profileActionLayout;
    QSpacerItem *profileActionSpacer;
    QPushButton *saveProfileButton;
    QFrame *securityCard;
    QVBoxLayout *securityCardLayout;
    QLabel *securityTitleLabel;
    QLabel *securityDescriptionLabel;
    QFormLayout *securityFormLayout;
    QLabel *currentPasswordLabel;
    QLineEdit *currentPasswordLineEdit;
    QLabel *newPasswordLabel;
    QLineEdit *newPasswordLineEdit;
    QLabel *confirmPasswordLabel;
    QLineEdit *confirmPasswordLineEdit;
    QHBoxLayout *securityActionLayout;
    QSpacerItem *securityActionSpacer;
    QPushButton *changePasswordButton;
    QFrame *preferencesCard;
    QVBoxLayout *preferencesCardLayout;
    QLabel *preferencesTitleLabel;
    QHBoxLayout *reminderLayout;
    QVBoxLayout *reminderTextLayout;
    QLabel *reminderTitleLabel;
    QLabel *reminderDescriptionLabel;
    QCheckBox *deadlineRemindersCheckBox;
    QHBoxLayout *reminderDaysLayout;
    QLabel *reminderDaysLabel;
    QSpacerItem *reminderDaysSpacer;
    QComboBox *reminderDaysComboBox;
    QFrame *dangerCard;
    QVBoxLayout *dangerCardLayout;
    QLabel *dangerTitleLabel;
    QLabel *dangerDescriptionLabel;
    QHBoxLayout *dangerActionLayout;
    QSpacerItem *dangerActionSpacer;
    QPushButton *deleteAccountButton;
    QSpacerItem *bottomSpacer;

    void setupUi(QWidget *SettingsWindow)
    {
        if (SettingsWindow->objectName().isEmpty())
            SettingsWindow->setObjectName("SettingsWindow");
        SettingsWindow->resize(1040, 760);
        SettingsWindow->setMinimumSize(QSize(720, 520));
        SettingsWindow->setStyleSheet(QString::fromUtf8("QWidget#SettingsWindow {\n"
"    background-color: #f5f7fb;\n"
"    color: #111827;\n"
"}"));
        rootLayout = new QVBoxLayout(SettingsWindow);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        settingsScrollArea = new QScrollArea(SettingsWindow);
        settingsScrollArea->setObjectName("settingsScrollArea");
        settingsScrollArea->setFrameShape(QFrame::NoFrame);
        settingsScrollArea->setWidgetResizable(true);
        settingsScrollAreaContents = new QWidget();
        settingsScrollAreaContents->setObjectName("settingsScrollAreaContents");
        settingsScrollAreaContents->setGeometry(QRect(0, 0, 1038, 920));
        contentLayout = new QVBoxLayout(settingsScrollAreaContents);
        contentLayout->setSpacing(22);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(34, 30, 34, 34);
        pageTitleLabel = new QLabel(settingsScrollAreaContents);
        pageTitleLabel->setObjectName("pageTitleLabel");
        pageTitleLabel->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 26px; font-weight: 700;"));

        contentLayout->addWidget(pageTitleLabel);

        pageSubtitleLabel = new QLabel(settingsScrollAreaContents);
        pageSubtitleLabel->setObjectName("pageSubtitleLabel");
        pageSubtitleLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 13px;"));

        contentLayout->addWidget(pageSubtitleLabel);

        profileCard = new QFrame(settingsScrollAreaContents);
        profileCard->setObjectName("profileCard");
        profileCardLayout = new QVBoxLayout(profileCard);
        profileCardLayout->setSpacing(14);
        profileCardLayout->setObjectName("profileCardLayout");
        profileCardLayout->setContentsMargins(24, 22, 24, 22);
        profileTitleLabel = new QLabel(profileCard);
        profileTitleLabel->setObjectName("profileTitleLabel");
        profileTitleLabel->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 18px; font-weight: 700;"));

        profileCardLayout->addWidget(profileTitleLabel);

        profileDescriptionLabel = new QLabel(profileCard);
        profileDescriptionLabel->setObjectName("profileDescriptionLabel");
        profileDescriptionLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 12px;"));

        profileCardLayout->addWidget(profileDescriptionLabel);

        profileFormLayout = new QFormLayout();
        profileFormLayout->setObjectName("profileFormLayout");
        profileFormLayout->setHorizontalSpacing(20);
        profileFormLayout->setVerticalSpacing(12);
        usernameLabel = new QLabel(profileCard);
        usernameLabel->setObjectName("usernameLabel");

        profileFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, usernameLabel);

        usernameLineEdit = new QLineEdit(profileCard);
        usernameLineEdit->setObjectName("usernameLineEdit");

        profileFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, usernameLineEdit);

        maximumCreditsLabel = new QLabel(profileCard);
        maximumCreditsLabel->setObjectName("maximumCreditsLabel");

        profileFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, maximumCreditsLabel);

        maximumCreditsLineEdit = new QLineEdit(profileCard);
        maximumCreditsLineEdit->setObjectName("maximumCreditsLineEdit");

        profileFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, maximumCreditsLineEdit);

        accountIdLabel = new QLabel(profileCard);
        accountIdLabel->setObjectName("accountIdLabel");

        profileFormLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, accountIdLabel);

        accountIdValueLabel = new QLabel(profileCard);
        accountIdValueLabel->setObjectName("accountIdValueLabel");
        accountIdValueLabel->setStyleSheet(QString::fromUtf8("color: #64748b;"));

        profileFormLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, accountIdValueLabel);


        profileCardLayout->addLayout(profileFormLayout);

        profileActionLayout = new QHBoxLayout();
        profileActionLayout->setObjectName("profileActionLayout");
        profileActionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        profileActionLayout->addItem(profileActionSpacer);

        saveProfileButton = new QPushButton(profileCard);
        saveProfileButton->setObjectName("saveProfileButton");

        profileActionLayout->addWidget(saveProfileButton);


        profileCardLayout->addLayout(profileActionLayout);


        contentLayout->addWidget(profileCard);

        securityCard = new QFrame(settingsScrollAreaContents);
        securityCard->setObjectName("securityCard");
        securityCardLayout = new QVBoxLayout(securityCard);
        securityCardLayout->setSpacing(14);
        securityCardLayout->setObjectName("securityCardLayout");
        securityCardLayout->setContentsMargins(24, 22, 24, 22);
        securityTitleLabel = new QLabel(securityCard);
        securityTitleLabel->setObjectName("securityTitleLabel");
        securityTitleLabel->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 18px; font-weight: 700;"));

        securityCardLayout->addWidget(securityTitleLabel);

        securityDescriptionLabel = new QLabel(securityCard);
        securityDescriptionLabel->setObjectName("securityDescriptionLabel");
        securityDescriptionLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 12px;"));

        securityCardLayout->addWidget(securityDescriptionLabel);

        securityFormLayout = new QFormLayout();
        securityFormLayout->setObjectName("securityFormLayout");
        securityFormLayout->setHorizontalSpacing(20);
        securityFormLayout->setVerticalSpacing(12);
        currentPasswordLabel = new QLabel(securityCard);
        currentPasswordLabel->setObjectName("currentPasswordLabel");

        securityFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, currentPasswordLabel);

        currentPasswordLineEdit = new QLineEdit(securityCard);
        currentPasswordLineEdit->setObjectName("currentPasswordLineEdit");

        securityFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, currentPasswordLineEdit);

        newPasswordLabel = new QLabel(securityCard);
        newPasswordLabel->setObjectName("newPasswordLabel");

        securityFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, newPasswordLabel);

        newPasswordLineEdit = new QLineEdit(securityCard);
        newPasswordLineEdit->setObjectName("newPasswordLineEdit");

        securityFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, newPasswordLineEdit);

        confirmPasswordLabel = new QLabel(securityCard);
        confirmPasswordLabel->setObjectName("confirmPasswordLabel");

        securityFormLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, confirmPasswordLabel);

        confirmPasswordLineEdit = new QLineEdit(securityCard);
        confirmPasswordLineEdit->setObjectName("confirmPasswordLineEdit");

        securityFormLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, confirmPasswordLineEdit);


        securityCardLayout->addLayout(securityFormLayout);

        securityActionLayout = new QHBoxLayout();
        securityActionLayout->setObjectName("securityActionLayout");
        securityActionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        securityActionLayout->addItem(securityActionSpacer);

        changePasswordButton = new QPushButton(securityCard);
        changePasswordButton->setObjectName("changePasswordButton");

        securityActionLayout->addWidget(changePasswordButton);


        securityCardLayout->addLayout(securityActionLayout);


        contentLayout->addWidget(securityCard);

        preferencesCard = new QFrame(settingsScrollAreaContents);
        preferencesCard->setObjectName("preferencesCard");
        preferencesCardLayout = new QVBoxLayout(preferencesCard);
        preferencesCardLayout->setSpacing(14);
        preferencesCardLayout->setObjectName("preferencesCardLayout");
        preferencesCardLayout->setContentsMargins(24, 22, 24, 22);
        preferencesTitleLabel = new QLabel(preferencesCard);
        preferencesTitleLabel->setObjectName("preferencesTitleLabel");
        preferencesTitleLabel->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 18px; font-weight: 700;"));

        preferencesCardLayout->addWidget(preferencesTitleLabel);

        reminderLayout = new QHBoxLayout();
        reminderLayout->setObjectName("reminderLayout");
        reminderTextLayout = new QVBoxLayout();
        reminderTextLayout->setObjectName("reminderTextLayout");
        reminderTitleLabel = new QLabel(preferencesCard);
        reminderTitleLabel->setObjectName("reminderTitleLabel");
        reminderTitleLabel->setStyleSheet(QString::fromUtf8("font-weight: 600;"));

        reminderTextLayout->addWidget(reminderTitleLabel);

        reminderDescriptionLabel = new QLabel(preferencesCard);
        reminderDescriptionLabel->setObjectName("reminderDescriptionLabel");
        reminderDescriptionLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 12px;"));

        reminderTextLayout->addWidget(reminderDescriptionLabel);


        reminderLayout->addLayout(reminderTextLayout);

        deadlineRemindersCheckBox = new QCheckBox(preferencesCard);
        deadlineRemindersCheckBox->setObjectName("deadlineRemindersCheckBox");
        deadlineRemindersCheckBox->setChecked(true);

        reminderLayout->addWidget(deadlineRemindersCheckBox);


        preferencesCardLayout->addLayout(reminderLayout);

        reminderDaysLayout = new QHBoxLayout();
        reminderDaysLayout->setObjectName("reminderDaysLayout");
        reminderDaysLabel = new QLabel(preferencesCard);
        reminderDaysLabel->setObjectName("reminderDaysLabel");

        reminderDaysLayout->addWidget(reminderDaysLabel);

        reminderDaysSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        reminderDaysLayout->addItem(reminderDaysSpacer);

        reminderDaysComboBox = new QComboBox(preferencesCard);
        reminderDaysComboBox->addItem(QString());
        reminderDaysComboBox->addItem(QString());
        reminderDaysComboBox->setObjectName("reminderDaysComboBox");

        reminderDaysLayout->addWidget(reminderDaysComboBox);


        preferencesCardLayout->addLayout(reminderDaysLayout);


        contentLayout->addWidget(preferencesCard);

        dangerCard = new QFrame(settingsScrollAreaContents);
        dangerCard->setObjectName("dangerCard");
        dangerCardLayout = new QVBoxLayout(dangerCard);
        dangerCardLayout->setSpacing(12);
        dangerCardLayout->setObjectName("dangerCardLayout");
        dangerCardLayout->setContentsMargins(24, 22, 24, 22);
        dangerTitleLabel = new QLabel(dangerCard);
        dangerTitleLabel->setObjectName("dangerTitleLabel");
        dangerTitleLabel->setStyleSheet(QString::fromUtf8("color: #b91c1c; font-size: 18px; font-weight: 700;"));

        dangerCardLayout->addWidget(dangerTitleLabel);

        dangerDescriptionLabel = new QLabel(dangerCard);
        dangerDescriptionLabel->setObjectName("dangerDescriptionLabel");
        dangerDescriptionLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 12px;"));
        dangerDescriptionLabel->setWordWrap(true);

        dangerCardLayout->addWidget(dangerDescriptionLabel);

        dangerActionLayout = new QHBoxLayout();
        dangerActionLayout->setObjectName("dangerActionLayout");
        dangerActionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        dangerActionLayout->addItem(dangerActionSpacer);

        deleteAccountButton = new QPushButton(dangerCard);
        deleteAccountButton->setObjectName("deleteAccountButton");

        dangerActionLayout->addWidget(deleteAccountButton);


        dangerCardLayout->addLayout(dangerActionLayout);


        contentLayout->addWidget(dangerCard);

        bottomSpacer = new QSpacerItem(20, 30, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        contentLayout->addItem(bottomSpacer);

        settingsScrollArea->setWidget(settingsScrollAreaContents);

        rootLayout->addWidget(settingsScrollArea);


        retranslateUi(SettingsWindow);

        QMetaObject::connectSlotsByName(SettingsWindow);
    } // setupUi

    void retranslateUi(QWidget *SettingsWindow)
    {
        SettingsWindow->setWindowTitle(QCoreApplication::translate("SettingsWindow", "Settings", nullptr));
        pageTitleLabel->setText(QCoreApplication::translate("SettingsWindow", "Settings", nullptr));
        pageSubtitleLabel->setText(QCoreApplication::translate("SettingsWindow", "Manage your profile, security, and application preferences.", nullptr));
        profileTitleLabel->setText(QCoreApplication::translate("SettingsWindow", "Profile", nullptr));
        profileDescriptionLabel->setText(QCoreApplication::translate("SettingsWindow", "Update your username and degree credit target.", nullptr));
        usernameLabel->setText(QCoreApplication::translate("SettingsWindow", "Username", nullptr));
        usernameLineEdit->setPlaceholderText(QCoreApplication::translate("SettingsWindow", "Enter username", nullptr));
        maximumCreditsLabel->setText(QCoreApplication::translate("SettingsWindow", "Maximum credits", nullptr));
        maximumCreditsLineEdit->setPlaceholderText(QCoreApplication::translate("SettingsWindow", "Example: 120", nullptr));
        accountIdLabel->setText(QCoreApplication::translate("SettingsWindow", "Account ID", nullptr));
        accountIdValueLabel->setText(QCoreApplication::translate("SettingsWindow", "\342\200\224", nullptr));
        saveProfileButton->setText(QCoreApplication::translate("SettingsWindow", "Save Profile", nullptr));
        securityTitleLabel->setText(QCoreApplication::translate("SettingsWindow", "Security", nullptr));
        securityDescriptionLabel->setText(QCoreApplication::translate("SettingsWindow", "Change your account password.", nullptr));
        currentPasswordLabel->setText(QCoreApplication::translate("SettingsWindow", "Current password", nullptr));
        currentPasswordLineEdit->setPlaceholderText(QCoreApplication::translate("SettingsWindow", "Enter current password", nullptr));
        newPasswordLabel->setText(QCoreApplication::translate("SettingsWindow", "New password", nullptr));
        newPasswordLineEdit->setPlaceholderText(QCoreApplication::translate("SettingsWindow", "At least 8 characters", nullptr));
        confirmPasswordLabel->setText(QCoreApplication::translate("SettingsWindow", "Confirm password", nullptr));
        confirmPasswordLineEdit->setPlaceholderText(QCoreApplication::translate("SettingsWindow", "Re-enter new password", nullptr));
        changePasswordButton->setText(QCoreApplication::translate("SettingsWindow", "Change Password", nullptr));
        preferencesTitleLabel->setText(QCoreApplication::translate("SettingsWindow", "Preferences", nullptr));
        reminderTitleLabel->setText(QCoreApplication::translate("SettingsWindow", "Deadline reminders", nullptr));
        reminderDescriptionLabel->setText(QCoreApplication::translate("SettingsWindow", "Show startup and system notifications for nearby deadlines.", nullptr));
        deadlineRemindersCheckBox->setText(QCoreApplication::translate("SettingsWindow", "Enabled", nullptr));
        reminderDaysLabel->setText(QCoreApplication::translate("SettingsWindow", "Reminder window", nullptr));
        reminderDaysComboBox->setItemText(0, QCoreApplication::translate("SettingsWindow", "2 days", nullptr));
        reminderDaysComboBox->setItemText(1, QCoreApplication::translate("SettingsWindow", "3 days", nullptr));

        dangerTitleLabel->setText(QCoreApplication::translate("SettingsWindow", "Danger Zone", nullptr));
        dangerDescriptionLabel->setText(QCoreApplication::translate("SettingsWindow", "Deleting your account permanently removes all semesters, courses, assignments, and grades.", nullptr));
        deleteAccountButton->setText(QCoreApplication::translate("SettingsWindow", "Delete Account", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsWindow: public Ui_SettingsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSWINDOW_H
