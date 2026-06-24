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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
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
    QLabel *brandFeatureLabel;
    QSpacerItem *accentBottomSpacer;
    QSpacerItem *leftCardSpacer;
    QFrame *authCard;
    QVBoxLayout *authCardLayout;
    QStackedWidget *authStackedWidget;
    QWidget *welcomePage;
    QVBoxLayout *welcomeLayout;
    QSpacerItem *welcomeTopSpacer;
    QLabel *welcomeTitleLabel;
    QLabel *welcomeSubtitleLabel;
    QSpacerItem *welcomeButtonSpacer;
    QPushButton *getStartedButton;
    QSpacerItem *welcomeBottomSpacer;
    QWidget *accountChoicePage;
    QVBoxLayout *accountChoiceLayout;
    QSpacerItem *choiceTopSpacer;
    QLabel *choiceTitleLabel;
    QLabel *choiceSubtitleLabel;
    QSpacerItem *choiceButtonSpacer;
    QPushButton *chooseSignInButton;
    QPushButton *chooseCreateAccountButton;
    QPushButton *choiceBackButton;
    QSpacerItem *choiceBottomSpacer;
    QWidget *signInPage;
    QVBoxLayout *signInLayout;
    QLabel *signInTitleLabel;
    QLabel *signInSubtitleLabel;
    QSpacerItem *signInTopSpacer;
    QLabel *loginIdentifierLabel;
    QLineEdit *loginIdentifierLineEdit;
    QLabel *loginPasswordLabel;
    QLineEdit *loginPasswordLineEdit;
    QSpacerItem *signInButtonSpacer;
    QPushButton *loginButton;
    QPushButton *loginBackButton;
    QSpacerItem *signInBottomSpacer;
    QWidget *createAccountPage;
    QVBoxLayout *createAccountLayout;
    QLabel *createTitleLabel;
    QLabel *createSubtitleLabel;
    QLabel *registerUsernameLabel;
    QLineEdit *registerUsernameLineEdit;
    QLabel *registerEmailLabel;
    QLineEdit *registerEmailLineEdit;
    QLabel *registerPasswordLabel;
    QLineEdit *registerPasswordLineEdit;
    QLabel *registerConfirmPasswordLabel;
    QLineEdit *registerConfirmPasswordLineEdit;
    QLabel *registerMaxCreditsLabel;
    QSpinBox *registerMaxCreditsSpinBox;
    QPushButton *registerButton;
    QPushButton *registerBackButton;
    QLabel *authMessageLabel;
    QSpacerItem *rightCardSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 760);
        MainWindow->setMinimumSize(QSize(900, 640));
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow,\n"
"QWidget#centralwidget {\n"
"    background-color: #f5f7fb;\n"
"}\n"
"\n"
"QFrame#accentPanel {\n"
"    background-color: #2563eb;\n"
"    border-radius: 20px;\n"
"}\n"
"\n"
"QLabel#brandTitleLabel {\n"
"    color: #ffffff;\n"
"    font-size: 31px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QLabel#brandTextLabel {\n"
"    color: #dbeafe;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QLabel#brandFeatureLabel {\n"
"    color: #eff6ff;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QFrame#authCard {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 20px;\n"
"}\n"
"\n"
"QLabel[authTitle=\"true\"] {\n"
"    color: #111827;\n"
"    font-size: 27px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QLabel[authSubtitle=\"true\"] {\n"
"    color: #64748b;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLabel[authFieldLabel=\"true\"] {\n"
"    color: #334155;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QLabel#authMessageLabel {\n"
"    color: #b91c1c;\n"
"    background-"
                        "color: #fef2f2;\n"
"    border: 1px solid #fecaca;\n"
"    border-radius: 8px;\n"
"    padding: 9px 10px;\n"
"}\n"
"\n"
"QLineEdit,\n"
"QSpinBox {\n"
"    min-height: 42px;\n"
"    padding: 0 13px;\n"
"    color: #111827;\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 9px;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QLineEdit:focus,\n"
"QSpinBox:focus {\n"
"    border: 2px solid #2563eb;\n"
"}\n"
"\n"
"QPushButton[authPrimary=\"true\"] {\n"
"    min-height: 42px;\n"
"    color: #ffffff;\n"
"    background-color: #2563eb;\n"
"    border: 1px solid #2563eb;\n"
"    border-radius: 9px;\n"
"    padding: 0 18px;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QPushButton[authPrimary=\"true\"]:hover {\n"
"    background-color: #1d4ed8;\n"
"    border-color: #1d4ed8;\n"
"}\n"
"\n"
"QPushButton[authPrimary=\"true\"]:pressed {\n"
"    background-color: #1e40af;\n"
"    border-color: #1e40af;\n"
"}\n"
"\n"
"QPushButton[authSecondary=\"true\"] {\n"
"    min-heig"
                        "ht: 42px;\n"
"    color: #2563eb;\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #bfdbfe;\n"
"    border-radius: 9px;\n"
"    padding: 0 18px;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QPushButton[authSecondary=\"true\"]:hover {\n"
"    background-color: #eff6ff;\n"
"}\n"
"\n"
"QPushButton[authLink=\"true\"] {\n"
"    min-height: 30px;\n"
"    color: #2563eb;\n"
"    background-color: transparent;\n"
"    border: none;\n"
"    padding: 0 4px;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QPushButton[authLink=\"true\"]:hover {\n"
"    color: #1d4ed8;\n"
"    text-decoration: underline;\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"    color: #94a3b8;\n"
"    background-color: #e2e8f0;\n"
"    border-color: #e2e8f0;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QHBoxLayout(centralwidget);
        mainLayout->setSpacing(28);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(42, 42, 42, 42);
        accentPanel = new QFrame(centralwidget);
        accentPanel->setObjectName("accentPanel");
        accentPanel->setMinimumSize(QSize(340, 0));
        accentPanel->setMaximumSize(QSize(400, 16777215));
        accentLayout = new QVBoxLayout(accentPanel);
        accentLayout->setSpacing(18);
        accentLayout->setObjectName("accentLayout");
        accentLayout->setContentsMargins(34, 34, 34, 34);
        accentTopSpacer = new QSpacerItem(20, 90, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        accentLayout->addItem(accentTopSpacer);

        brandTitleLabel = new QLabel(accentPanel);
        brandTitleLabel->setObjectName("brandTitleLabel");

        accentLayout->addWidget(brandTitleLabel);

        brandTextLabel = new QLabel(accentPanel);
        brandTextLabel->setObjectName("brandTextLabel");
        brandTextLabel->setWordWrap(true);

        accentLayout->addWidget(brandTextLabel);

        brandFeatureLabel = new QLabel(accentPanel);
        brandFeatureLabel->setObjectName("brandFeatureLabel");
        brandFeatureLabel->setWordWrap(true);

        accentLayout->addWidget(brandFeatureLabel);

        accentBottomSpacer = new QSpacerItem(20, 200, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        accentLayout->addItem(accentBottomSpacer);


        mainLayout->addWidget(accentPanel);

        leftCardSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        mainLayout->addItem(leftCardSpacer);

        authCard = new QFrame(centralwidget);
        authCard->setObjectName("authCard");
        authCard->setMinimumSize(QSize(450, 620));
        authCard->setMaximumSize(QSize(520, 680));
        authCardLayout = new QVBoxLayout(authCard);
        authCardLayout->setSpacing(12);
        authCardLayout->setObjectName("authCardLayout");
        authCardLayout->setContentsMargins(38, 34, 38, 34);
        authStackedWidget = new QStackedWidget(authCard);
        authStackedWidget->setObjectName("authStackedWidget");
        welcomePage = new QWidget();
        welcomePage->setObjectName("welcomePage");
        welcomeLayout = new QVBoxLayout(welcomePage);
        welcomeLayout->setSpacing(16);
        welcomeLayout->setObjectName("welcomeLayout");
        welcomeTopSpacer = new QSpacerItem(20, 110, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        welcomeLayout->addItem(welcomeTopSpacer);

        welcomeTitleLabel = new QLabel(welcomePage);
        welcomeTitleLabel->setObjectName("welcomeTitleLabel");
        welcomeTitleLabel->setProperty("authTitle", QVariant(true));
        welcomeTitleLabel->setAlignment(Qt::AlignCenter);
        welcomeTitleLabel->setWordWrap(true);

        welcomeLayout->addWidget(welcomeTitleLabel);

        welcomeSubtitleLabel = new QLabel(welcomePage);
        welcomeSubtitleLabel->setObjectName("welcomeSubtitleLabel");
        welcomeSubtitleLabel->setProperty("authSubtitle", QVariant(true));
        welcomeSubtitleLabel->setAlignment(Qt::AlignCenter);
        welcomeSubtitleLabel->setWordWrap(true);

        welcomeLayout->addWidget(welcomeSubtitleLabel);

        welcomeButtonSpacer = new QSpacerItem(20, 42, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        welcomeLayout->addItem(welcomeButtonSpacer);

        getStartedButton = new QPushButton(welcomePage);
        getStartedButton->setObjectName("getStartedButton");
        getStartedButton->setProperty("authPrimary", QVariant(true));

        welcomeLayout->addWidget(getStartedButton);

        welcomeBottomSpacer = new QSpacerItem(20, 120, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        welcomeLayout->addItem(welcomeBottomSpacer);

        authStackedWidget->addWidget(welcomePage);
        accountChoicePage = new QWidget();
        accountChoicePage->setObjectName("accountChoicePage");
        accountChoiceLayout = new QVBoxLayout(accountChoicePage);
        accountChoiceLayout->setSpacing(14);
        accountChoiceLayout->setObjectName("accountChoiceLayout");
        choiceTopSpacer = new QSpacerItem(20, 95, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        accountChoiceLayout->addItem(choiceTopSpacer);

        choiceTitleLabel = new QLabel(accountChoicePage);
        choiceTitleLabel->setObjectName("choiceTitleLabel");
        choiceTitleLabel->setProperty("authTitle", QVariant(true));
        choiceTitleLabel->setAlignment(Qt::AlignCenter);
        choiceTitleLabel->setWordWrap(true);

        accountChoiceLayout->addWidget(choiceTitleLabel);

        choiceSubtitleLabel = new QLabel(accountChoicePage);
        choiceSubtitleLabel->setObjectName("choiceSubtitleLabel");
        choiceSubtitleLabel->setProperty("authSubtitle", QVariant(true));
        choiceSubtitleLabel->setAlignment(Qt::AlignCenter);
        choiceSubtitleLabel->setWordWrap(true);

        accountChoiceLayout->addWidget(choiceSubtitleLabel);

        choiceButtonSpacer = new QSpacerItem(20, 30, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        accountChoiceLayout->addItem(choiceButtonSpacer);

        chooseSignInButton = new QPushButton(accountChoicePage);
        chooseSignInButton->setObjectName("chooseSignInButton");
        chooseSignInButton->setProperty("authPrimary", QVariant(true));

        accountChoiceLayout->addWidget(chooseSignInButton);

        chooseCreateAccountButton = new QPushButton(accountChoicePage);
        chooseCreateAccountButton->setObjectName("chooseCreateAccountButton");
        chooseCreateAccountButton->setProperty("authSecondary", QVariant(true));

        accountChoiceLayout->addWidget(chooseCreateAccountButton);

        choiceBackButton = new QPushButton(accountChoicePage);
        choiceBackButton->setObjectName("choiceBackButton");
        choiceBackButton->setProperty("authLink", QVariant(true));

        accountChoiceLayout->addWidget(choiceBackButton);

        choiceBottomSpacer = new QSpacerItem(20, 100, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        accountChoiceLayout->addItem(choiceBottomSpacer);

        authStackedWidget->addWidget(accountChoicePage);
        signInPage = new QWidget();
        signInPage->setObjectName("signInPage");
        signInLayout = new QVBoxLayout(signInPage);
        signInLayout->setSpacing(10);
        signInLayout->setObjectName("signInLayout");
        signInTitleLabel = new QLabel(signInPage);
        signInTitleLabel->setObjectName("signInTitleLabel");
        signInTitleLabel->setProperty("authTitle", QVariant(true));

        signInLayout->addWidget(signInTitleLabel);

        signInSubtitleLabel = new QLabel(signInPage);
        signInSubtitleLabel->setObjectName("signInSubtitleLabel");
        signInSubtitleLabel->setProperty("authSubtitle", QVariant(true));

        signInLayout->addWidget(signInSubtitleLabel);

        signInTopSpacer = new QSpacerItem(20, 22, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        signInLayout->addItem(signInTopSpacer);

        loginIdentifierLabel = new QLabel(signInPage);
        loginIdentifierLabel->setObjectName("loginIdentifierLabel");
        loginIdentifierLabel->setProperty("authFieldLabel", QVariant(true));

        signInLayout->addWidget(loginIdentifierLabel);

        loginIdentifierLineEdit = new QLineEdit(signInPage);
        loginIdentifierLineEdit->setObjectName("loginIdentifierLineEdit");
        loginIdentifierLineEdit->setClearButtonEnabled(true);

        signInLayout->addWidget(loginIdentifierLineEdit);

        loginPasswordLabel = new QLabel(signInPage);
        loginPasswordLabel->setObjectName("loginPasswordLabel");
        loginPasswordLabel->setProperty("authFieldLabel", QVariant(true));

        signInLayout->addWidget(loginPasswordLabel);

        loginPasswordLineEdit = new QLineEdit(signInPage);
        loginPasswordLineEdit->setObjectName("loginPasswordLineEdit");
        loginPasswordLineEdit->setEchoMode(QLineEdit::Password);

        signInLayout->addWidget(loginPasswordLineEdit);

        signInButtonSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        signInLayout->addItem(signInButtonSpacer);

        loginButton = new QPushButton(signInPage);
        loginButton->setObjectName("loginButton");
        loginButton->setProperty("authPrimary", QVariant(true));

        signInLayout->addWidget(loginButton);

        loginBackButton = new QPushButton(signInPage);
        loginBackButton->setObjectName("loginBackButton");
        loginBackButton->setProperty("authLink", QVariant(true));

        signInLayout->addWidget(loginBackButton);

        signInBottomSpacer = new QSpacerItem(20, 70, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        signInLayout->addItem(signInBottomSpacer);

        authStackedWidget->addWidget(signInPage);
        createAccountPage = new QWidget();
        createAccountPage->setObjectName("createAccountPage");
        createAccountLayout = new QVBoxLayout(createAccountPage);
        createAccountLayout->setSpacing(8);
        createAccountLayout->setObjectName("createAccountLayout");
        createTitleLabel = new QLabel(createAccountPage);
        createTitleLabel->setObjectName("createTitleLabel");
        createTitleLabel->setProperty("authTitle", QVariant(true));

        createAccountLayout->addWidget(createTitleLabel);

        createSubtitleLabel = new QLabel(createAccountPage);
        createSubtitleLabel->setObjectName("createSubtitleLabel");
        createSubtitleLabel->setProperty("authSubtitle", QVariant(true));

        createAccountLayout->addWidget(createSubtitleLabel);

        registerUsernameLabel = new QLabel(createAccountPage);
        registerUsernameLabel->setObjectName("registerUsernameLabel");
        registerUsernameLabel->setProperty("authFieldLabel", QVariant(true));

        createAccountLayout->addWidget(registerUsernameLabel);

        registerUsernameLineEdit = new QLineEdit(createAccountPage);
        registerUsernameLineEdit->setObjectName("registerUsernameLineEdit");
        registerUsernameLineEdit->setClearButtonEnabled(true);

        createAccountLayout->addWidget(registerUsernameLineEdit);

        registerEmailLabel = new QLabel(createAccountPage);
        registerEmailLabel->setObjectName("registerEmailLabel");
        registerEmailLabel->setProperty("authFieldLabel", QVariant(true));

        createAccountLayout->addWidget(registerEmailLabel);

        registerEmailLineEdit = new QLineEdit(createAccountPage);
        registerEmailLineEdit->setObjectName("registerEmailLineEdit");
        registerEmailLineEdit->setClearButtonEnabled(true);

        createAccountLayout->addWidget(registerEmailLineEdit);

        registerPasswordLabel = new QLabel(createAccountPage);
        registerPasswordLabel->setObjectName("registerPasswordLabel");
        registerPasswordLabel->setProperty("authFieldLabel", QVariant(true));

        createAccountLayout->addWidget(registerPasswordLabel);

        registerPasswordLineEdit = new QLineEdit(createAccountPage);
        registerPasswordLineEdit->setObjectName("registerPasswordLineEdit");
        registerPasswordLineEdit->setEchoMode(QLineEdit::Password);

        createAccountLayout->addWidget(registerPasswordLineEdit);

        registerConfirmPasswordLabel = new QLabel(createAccountPage);
        registerConfirmPasswordLabel->setObjectName("registerConfirmPasswordLabel");
        registerConfirmPasswordLabel->setProperty("authFieldLabel", QVariant(true));

        createAccountLayout->addWidget(registerConfirmPasswordLabel);

        registerConfirmPasswordLineEdit = new QLineEdit(createAccountPage);
        registerConfirmPasswordLineEdit->setObjectName("registerConfirmPasswordLineEdit");
        registerConfirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

        createAccountLayout->addWidget(registerConfirmPasswordLineEdit);

        registerMaxCreditsLabel = new QLabel(createAccountPage);
        registerMaxCreditsLabel->setObjectName("registerMaxCreditsLabel");
        registerMaxCreditsLabel->setProperty("authFieldLabel", QVariant(true));

        createAccountLayout->addWidget(registerMaxCreditsLabel);

        registerMaxCreditsSpinBox = new QSpinBox(createAccountPage);
        registerMaxCreditsSpinBox->setObjectName("registerMaxCreditsSpinBox");
        registerMaxCreditsSpinBox->setMinimum(1);
        registerMaxCreditsSpinBox->setMaximum(1000);
        registerMaxCreditsSpinBox->setValue(120);

        createAccountLayout->addWidget(registerMaxCreditsSpinBox);

        registerButton = new QPushButton(createAccountPage);
        registerButton->setObjectName("registerButton");
        registerButton->setProperty("authPrimary", QVariant(true));

        createAccountLayout->addWidget(registerButton);

        registerBackButton = new QPushButton(createAccountPage);
        registerBackButton->setObjectName("registerBackButton");
        registerBackButton->setProperty("authLink", QVariant(true));

        createAccountLayout->addWidget(registerBackButton);

        authStackedWidget->addWidget(createAccountPage);

        authCardLayout->addWidget(authStackedWidget);

        authMessageLabel = new QLabel(authCard);
        authMessageLabel->setObjectName("authMessageLabel");
        authMessageLabel->setWordWrap(true);

        authCardLayout->addWidget(authMessageLabel);


        mainLayout->addWidget(authCard);

        rightCardSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        mainLayout->addItem(rightCardSpacer);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setVisible(false);
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        statusbar->setVisible(false);
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        authStackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Semester Tracker", nullptr));
        brandTitleLabel->setText(QCoreApplication::translate("MainWindow", "Semester\n"
"Tracker", nullptr));
        brandTextLabel->setText(QCoreApplication::translate("MainWindow", "Plan your degree, track your coursework, and stay ahead of every deadline.", nullptr));
        brandFeatureLabel->setText(QCoreApplication::translate("MainWindow", "\342\234\223 Semesters and course progress\n"
"\342\234\223 Assignments and due dates\n"
"\342\234\223 GPA, CGPA, and completed credits\n"
"\342\234\223 Secure local accounts", nullptr));
        welcomeTitleLabel->setText(QCoreApplication::translate("MainWindow", "Welcome to Semester Tracker", nullptr));
        welcomeSubtitleLabel->setText(QCoreApplication::translate("MainWindow", "Your academic workspace, organized in one place.", nullptr));
        getStartedButton->setText(QCoreApplication::translate("MainWindow", "Get Started", nullptr));
        choiceTitleLabel->setText(QCoreApplication::translate("MainWindow", "How would you like to continue?", nullptr));
        choiceSubtitleLabel->setText(QCoreApplication::translate("MainWindow", "Sign in to an existing account or create a new one.", nullptr));
        chooseSignInButton->setText(QCoreApplication::translate("MainWindow", "Sign In", nullptr));
        chooseCreateAccountButton->setText(QCoreApplication::translate("MainWindow", "Create Account", nullptr));
        choiceBackButton->setText(QCoreApplication::translate("MainWindow", "Back", nullptr));
        signInTitleLabel->setText(QCoreApplication::translate("MainWindow", "Sign in", nullptr));
        signInSubtitleLabel->setText(QCoreApplication::translate("MainWindow", "Use your username or email address.", nullptr));
        loginIdentifierLabel->setText(QCoreApplication::translate("MainWindow", "Username or email", nullptr));
        loginIdentifierLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter username or email", nullptr));
        loginPasswordLabel->setText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        loginPasswordLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter your password", nullptr));
        loginButton->setText(QCoreApplication::translate("MainWindow", "Log In", nullptr));
        loginBackButton->setText(QCoreApplication::translate("MainWindow", "Back", nullptr));
        createTitleLabel->setText(QCoreApplication::translate("MainWindow", "Create your account", nullptr));
        createSubtitleLabel->setText(QCoreApplication::translate("MainWindow", "You will be signed in automatically after registration.", nullptr));
        registerUsernameLabel->setText(QCoreApplication::translate("MainWindow", "Username", nullptr));
        registerUsernameLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "3\342\200\22340 letters, numbers, . _ -", nullptr));
        registerEmailLabel->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        registerEmailLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "you@example.com", nullptr));
        registerPasswordLabel->setText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        registerPasswordLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "At least 8 characters, with a letter and number", nullptr));
        registerConfirmPasswordLabel->setText(QCoreApplication::translate("MainWindow", "Confirm password", nullptr));
        registerConfirmPasswordLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Re-enter your password", nullptr));
        registerMaxCreditsLabel->setText(QCoreApplication::translate("MainWindow", "Degree credit target", nullptr));
        registerButton->setText(QCoreApplication::translate("MainWindow", "Create Account", nullptr));
        registerBackButton->setText(QCoreApplication::translate("MainWindow", "Back", nullptr));
        authMessageLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
