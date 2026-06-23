/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *mainLayout;
    QFrame *accentPanel;
    QVBoxLayout *accentLayout;
    QSpacerItem *accentTopSpacer;
    QLabel *brandTitleLabel;
    QLabel *brandTextLabel;
    QSpacerItem *accentBottomSpacer;
    QSpacerItem *leftCardSpacer;
    QFrame *loginCard;
    QVBoxLayout *loginCardLayout;
    QLabel *loginLabel;
    QLabel *loginLabel_2;
    QLabel *subtitleLabel;
    QSpacerItem *formTopSpacer;
    QLabel *usernameFieldLabel;
    QLineEdit *usernameLineEdit;
    QLabel *passwordFieldLabel;
    QLineEdit *passwordLineEdit;
    QSpacerItem *buttonTopSpacer;
    QPushButton *loginButton;
    QPushButton *createAccountButton;
    QSpacerItem *formBottomSpacer;
    QSpacerItem *rightCardSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 680);
        MainWindow->setMinimumSize(QSize(820, 560));
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow { background-color: #f5f7fb; }\n"
"QWidget#centralwidget { background-color: #f5f7fb; }\n"
"QFrame#loginCard { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 18px; }\n"
"QLabel#loginLabel { color: #111827; font-size: 30px; font-weight: 700; }\n"
"QLabel#loginLabel_2 { color: #374151; font-size: 18px; font-weight: 600; }\n"
"QLabel#subtitleLabel { color: #64748b; font-size: 13px; }\n"
"QLabel#usernameFieldLabel, QLabel#passwordFieldLabel { color: #374151; font-size: 13px; font-weight: 600; }\n"
"QLineEdit { min-height: 42px; padding: 0 14px; border: 1px solid #cbd5e1; border-radius: 9px; background-color: #ffffff; color: #111827; font-size: 14px; }\n"
"QLineEdit:focus { border: 2px solid #2563eb; }\n"
"QPushButton#loginButton { min-height: 42px; border: none; border-radius: 9px; background-color: #2563eb; color: #ffffff; font-size: 14px; font-weight: 600; }\n"
"QPushButton#loginButton:hover { background-color: #1d4ed8; }\n"
"QPushButton#loginButton:pressed { background-color: #"
                        "1e40af; }\n"
"QPushButton#createAccountButton { min-height: 42px; border: 1px solid #cbd5e1; border-radius: 9px; background-color: #ffffff; color: #2563eb; font-size: 14px; font-weight: 600; }\n"
"QPushButton#createAccountButton:hover { background-color: #eff6ff; border-color: #93c5fd; }\n"
"QFrame#accentPanel { background-color: #2563eb; border-radius: 18px; }\n"
"QLabel#brandTitleLabel { color: #ffffff; font-size: 28px; font-weight: 700; }\n"
"QLabel#brandTextLabel { color: #dbeafe; font-size: 14px; }"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QHBoxLayout(centralwidget);
        mainLayout->setSpacing(24);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(40, 40, 40, 40);
        accentPanel = new QFrame(centralwidget);
        accentPanel->setObjectName("accentPanel");
        accentPanel->setMinimumSize(QSize(330, 0));
        accentPanel->setMaximumSize(QSize(380, 16777215));
        accentLayout = new QVBoxLayout(accentPanel);
        accentLayout->setObjectName("accentLayout");
        accentLayout->setContentsMargins(32, 32, 32, 32);
        accentTopSpacer = new QSpacerItem(20, 90, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        accentLayout->addItem(accentTopSpacer);

        brandTitleLabel = new QLabel(accentPanel);
        brandTitleLabel->setObjectName("brandTitleLabel");

        accentLayout->addWidget(brandTitleLabel);

        brandTextLabel = new QLabel(accentPanel);
        brandTextLabel->setObjectName("brandTextLabel");
        brandTextLabel->setWordWrap(true);

        accentLayout->addWidget(brandTextLabel);

        accentBottomSpacer = new QSpacerItem(20, 220, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        accentLayout->addItem(accentBottomSpacer);


        mainLayout->addWidget(accentPanel);

        leftCardSpacer = new QSpacerItem(30, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        mainLayout->addItem(leftCardSpacer);

        loginCard = new QFrame(centralwidget);
        loginCard->setObjectName("loginCard");
        loginCard->setMinimumSize(QSize(400, 500));
        loginCard->setMaximumSize(QSize(460, 560));
        loginCardLayout = new QVBoxLayout(loginCard);
        loginCardLayout->setSpacing(12);
        loginCardLayout->setObjectName("loginCardLayout");
        loginCardLayout->setContentsMargins(38, 38, 38, 38);
        loginLabel = new QLabel(loginCard);
        loginLabel->setObjectName("loginLabel");
        loginLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        loginCardLayout->addWidget(loginLabel);

        loginLabel_2 = new QLabel(loginCard);
        loginLabel_2->setObjectName("loginLabel_2");
        loginLabel_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        loginCardLayout->addWidget(loginLabel_2);

        subtitleLabel = new QLabel(loginCard);
        subtitleLabel->setObjectName("subtitleLabel");
        subtitleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        loginCardLayout->addWidget(subtitleLabel);

        formTopSpacer = new QSpacerItem(20, 22, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        loginCardLayout->addItem(formTopSpacer);

        usernameFieldLabel = new QLabel(loginCard);
        usernameFieldLabel->setObjectName("usernameFieldLabel");

        loginCardLayout->addWidget(usernameFieldLabel);

        usernameLineEdit = new QLineEdit(loginCard);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setClearButtonEnabled(true);

        loginCardLayout->addWidget(usernameLineEdit);

        passwordFieldLabel = new QLabel(loginCard);
        passwordFieldLabel->setObjectName("passwordFieldLabel");

        loginCardLayout->addWidget(passwordFieldLabel);

        passwordLineEdit = new QLineEdit(loginCard);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);

        loginCardLayout->addWidget(passwordLineEdit);

        buttonTopSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        loginCardLayout->addItem(buttonTopSpacer);

        loginButton = new QPushButton(loginCard);
        loginButton->setObjectName("loginButton");

        loginCardLayout->addWidget(loginButton);

        createAccountButton = new QPushButton(loginCard);
        createAccountButton->setObjectName("createAccountButton");

        loginCardLayout->addWidget(createAccountButton);

        formBottomSpacer = new QSpacerItem(20, 18, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        loginCardLayout->addItem(formBottomSpacer);


        mainLayout->addWidget(loginCard);

        rightCardSpacer = new QSpacerItem(30, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        mainLayout->addItem(rightCardSpacer);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 21));
        menubar->setVisible(false);
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        statusbar->setVisible(false);
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        loginButton->setDefault(true);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Semester Tracker", nullptr));
        brandTitleLabel->setText(QCoreApplication::translate("MainWindow", "Semester\n"
"Tracker", nullptr));
        brandTextLabel->setText(QCoreApplication::translate("MainWindow", "Track semesters, courses, assignments,\n"
"grades, GPA, and CGPA in one place.", nullptr));
        loginLabel->setText(QCoreApplication::translate("MainWindow", "Semester Tracker", nullptr));
        loginLabel_2->setText(QCoreApplication::translate("MainWindow", "Welcome back", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("MainWindow", "Sign in to continue to your dashboard", nullptr));
        usernameFieldLabel->setText(QCoreApplication::translate("MainWindow", "Username", nullptr));
        usernameLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter your username", nullptr));
        passwordFieldLabel->setText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        passwordLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter your password", nullptr));
        loginButton->setText(QCoreApplication::translate("MainWindow", "Log In", nullptr));
        createAccountButton->setText(QCoreApplication::translate("MainWindow", "Create Account", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
