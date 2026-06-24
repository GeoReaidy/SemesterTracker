/********************************************************************************
** Form generated from reading UI file 'dashboard.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dashboard
{
public:
    QHBoxLayout *mainHorizontalLayout;
    QFrame *sidebarFrame;
    QVBoxLayout *sidebarLayout;
    QLabel *appTitleLabel;
    QSpacerItem *sidebarTitleSpacer;
    QPushButton *dashboardButton;
    QPushButton *semestersButton;
    QPushButton *coursesButton;
    QPushButton *assignmentsButton;
    QPushButton *calendarButton;
    QPushButton *gradesButton;
    QPushButton *settingsButton;
    QSpacerItem *sidebarBottomSpacer;
    QFrame *profileFrame;
    QHBoxLayout *profileLayout;
    QLabel *profileAvatarLabel;
    QVBoxLayout *profileTextLayout;
    QLabel *profileUsernameLabel;
    QLabel *profileRoleLabel;
    QPushButton *logoutButton;
    QStackedWidget *mainStackedWidget;
    QWidget *dashboardPage;
    QVBoxLayout *dashboardPageLayout;
    QScrollArea *dashboardScrollArea;
    QWidget *dashboardScrollAreaWidgetContents;
    QVBoxLayout *contentLayout;
    QFrame *headerFrame;
    QHBoxLayout *headerLayout;
    QLabel *welcomeLabel;
    QSpacerItem *headerSpacer;
    QLabel *semesterLabel;
    QHBoxLayout *statisticsLayout;
    QFrame *cgpaCard;
    QVBoxLayout *cgpaCardLayout;
    QLabel *cgpaTitleLabel;
    QLabel *cgpaValueLabel;
    QSpacerItem *cgpaSpacer;
    QProgressBar *cgpaProgressBar;
    QHBoxLayout *cgpaScaleLayout;
    QLabel *cgpaMinLabel;
    QSpacerItem *cgpaScaleSpacer;
    QLabel *cgpaMaxLabel;
    QFrame *creditsCard;
    QVBoxLayout *creditsCardLayout;
    QLabel *creditsTitleLabel;
    QLabel *creditsValueLabel;
    QSpacerItem *creditsSpacer;
    QProgressBar *creditsProgressBar;
    QHBoxLayout *creditsScaleLayout;
    QLabel *creditsMinLabel;
    QSpacerItem *creditsScaleSpacer;
    QLabel *creditsMaxLabel;
    QFrame *overallGradeCard;
    QVBoxLayout *overallGradeCardLayout;
    QLabel *overallGradeTitleLabel;
    QLabel *overallGradeValueLabel;
    QLabel *overallGradeGpaLabel;
    QSpacerItem *overallGradeSpacer;
    QHBoxLayout *middlePanelsLayout;
    QFrame *currentCoursesCard;
    QVBoxLayout *currentCoursesCardLayout;
    QHBoxLayout *currentCoursesHeaderLayout;
    QLabel *currentCoursesLabel;
    QSpacerItem *currentCoursesHeaderSpacer;
    QPushButton *viewAllCoursesButton;
    QListWidget *currentCoursesListWidget;
    QFrame *upcomingAssignmentsCard;
    QVBoxLayout *upcomingAssignmentsCardLayout;
    QHBoxLayout *upcomingAssignmentsHeaderLayout;
    QLabel *upcomingAssignmentsLabel;
    QSpacerItem *upcomingAssignmentsHeaderSpacer;
    QPushButton *viewAllAssignmentsButton;
    QListWidget *upcomingAssignmentsListWidget;
    QFrame *calendarOverviewCard;
    QVBoxLayout *calendarOverviewCardLayout;
    QHBoxLayout *calendarHeaderLayout;
    QLabel *calendarOverviewLabel;
    QSpacerItem *calendarHeaderSpacer;
    QPushButton *viewCalendarButton;
    QTableWidget *calendarTableWidget;
    QSpacerItem *contentBottomSpacer;

    void setupUi(QWidget *Dashboard)
    {
        if (Dashboard->objectName().isEmpty())
            Dashboard->setObjectName("Dashboard");
        Dashboard->resize(1280, 820);
        Dashboard->setMinimumSize(QSize(1050, 680));
        Dashboard->setStyleSheet(QString::fromUtf8("QWidget#Dashboard {\n"
"    background-color: #f5f7fb;\n"
"}\n"
"QFrame#sidebarFrame {\n"
"    background-color: #ffffff;\n"
"    border-right: 1px solid #e2e8f0;\n"
"}\n"
"QLabel#appTitleLabel {\n"
"    color: #111827;\n"
"    font-size: 22px;\n"
"    font-weight: 700;\n"
"}\n"
"QPushButton[class=\"navButton\"] {\n"
"    min-height: 50px;\n"
"    padding: 0 16px;\n"
"    text-align: left;\n"
"    border: none;\n"
"    border-radius: 9px;\n"
"    background-color: transparent;\n"
"    color: #334155;\n"
"    font-size: 15px;\n"
"    font-weight: 500;\n"
"}\n"
"QPushButton[class=\"navButton\"]:hover {\n"
"    background-color: #eff6ff;\n"
"    color: #2563eb;\n"
"}\n"
"QPushButton[class=\"navButton\"]:checked {\n"
"    background-color: #dbeafe;\n"
"    color: #2563eb;\n"
"    font-weight: 600;\n"
"}\n"
"QPushButton#logoutButton {\n"
"    min-height: 50px;\n"
"    padding: 0 16px;\n"
"    text-align: left;\n"
"    border: none;\n"
"    border-radius: 9px;\n"
"    background-color: transparent;\n"
"    color: #d"
                        "c2626;\n"
"    font-size: 15px;\n"
"    font-weight: 500;\n"
"}\n"
"QPushButton#logoutButton:hover {\n"
"    background-color: #fee2e2;\n"
"}\n"
"QFrame[class=\"card\"] {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 12px;\n"
"}\n"
"QLabel[class=\"sectionTitle\"] {\n"
"    color: #111827;\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"}\n"
"QPushButton[class=\"linkButton\"] {\n"
"    border: none;\n"
"    background: transparent;\n"
"    color: #2563eb;\n"
"    font-size: 13px;\n"
"}\n"
"QPushButton[class=\"linkButton\"]:hover {\n"
"    text-decoration: underline;\n"
"}\n"
"QListWidget {\n"
"    border: none;\n"
"    background: transparent;\n"
"    outline: none;\n"
"    color: #1f2937;\n"
"    font-size: 13px;\n"
"}\n"
"QListWidget::item {\n"
"    min-height: 42px;\n"
"    padding: 5px 8px;\n"
"    border-bottom: 1px solid #eef2f7;\n"
"}\n"
"QListWidget::item:selected {\n"
"    background-color: #eff6ff;\n"
"    color: #1f2937;\n"
"}\n"
"QTableWidget {\n"
""
                        "    border: none;\n"
"    background: transparent;\n"
"    gridline-color: #e5e7eb;\n"
"    color: #334155;\n"
"    font-size: 12px;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #f8fafc;\n"
"    color: #475569;\n"
"    border: none;\n"
"    border-bottom: 1px solid #e5e7eb;\n"
"    padding: 7px;\n"
"    font-weight: 600;\n"
"}\n"
"QProgressBar {\n"
"    min-height: 18px;\n"
"    max-height: 18px;\n"
"    border: none;\n"
"    border-radius: 9px;\n"
"    background-color: #e5e7eb;\n"
"    text-align: center;\n"
"    color: #ffffff;\n"
"    font-size: 11px;\n"
"    font-weight: 600;\n"
"}\n"
"QProgressBar::chunk {\n"
"    background-color: #2563eb;\n"
"    border-radius: 9px;\n"
"}"));
        mainHorizontalLayout = new QHBoxLayout(Dashboard);
        mainHorizontalLayout->setSpacing(0);
        mainHorizontalLayout->setObjectName("mainHorizontalLayout");
        mainHorizontalLayout->setContentsMargins(0, 0, 0, 0);
        sidebarFrame = new QFrame(Dashboard);
        sidebarFrame->setObjectName("sidebarFrame");
        sidebarFrame->setMinimumSize(QSize(220, 0));
        sidebarFrame->setMaximumSize(QSize(220, 16777215));
        sidebarLayout = new QVBoxLayout(sidebarFrame);
        sidebarLayout->setSpacing(7);
        sidebarLayout->setObjectName("sidebarLayout");
        sidebarLayout->setContentsMargins(18, 24, 18, 18);
        appTitleLabel = new QLabel(sidebarFrame);
        appTitleLabel->setObjectName("appTitleLabel");

        sidebarLayout->addWidget(appTitleLabel);

        sidebarTitleSpacer = new QSpacerItem(20, 24, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        sidebarLayout->addItem(sidebarTitleSpacer);

        dashboardButton = new QPushButton(sidebarFrame);
        dashboardButton->setObjectName("dashboardButton");
        dashboardButton->setCheckable(true);
        dashboardButton->setChecked(true);
        dashboardButton->setAutoExclusive(true);

        sidebarLayout->addWidget(dashboardButton);

        semestersButton = new QPushButton(sidebarFrame);
        semestersButton->setObjectName("semestersButton");
        semestersButton->setCheckable(true);
        semestersButton->setAutoExclusive(true);

        sidebarLayout->addWidget(semestersButton);

        coursesButton = new QPushButton(sidebarFrame);
        coursesButton->setObjectName("coursesButton");
        coursesButton->setCheckable(true);
        coursesButton->setAutoExclusive(true);

        sidebarLayout->addWidget(coursesButton);

        assignmentsButton = new QPushButton(sidebarFrame);
        assignmentsButton->setObjectName("assignmentsButton");
        assignmentsButton->setCheckable(true);
        assignmentsButton->setAutoExclusive(true);

        sidebarLayout->addWidget(assignmentsButton);

        calendarButton = new QPushButton(sidebarFrame);
        calendarButton->setObjectName("calendarButton");
        calendarButton->setCheckable(true);
        calendarButton->setAutoExclusive(true);

        sidebarLayout->addWidget(calendarButton);

        gradesButton = new QPushButton(sidebarFrame);
        gradesButton->setObjectName("gradesButton");
        gradesButton->setCheckable(true);
        gradesButton->setAutoExclusive(true);

        sidebarLayout->addWidget(gradesButton);

        settingsButton = new QPushButton(sidebarFrame);
        settingsButton->setObjectName("settingsButton");
        settingsButton->setCheckable(true);
        settingsButton->setAutoExclusive(true);

        sidebarLayout->addWidget(settingsButton);

        sidebarBottomSpacer = new QSpacerItem(20, 160, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        sidebarLayout->addItem(sidebarBottomSpacer);

        profileFrame = new QFrame(sidebarFrame);
        profileFrame->setObjectName("profileFrame");
        profileFrame->setStyleSheet(QString::fromUtf8("QFrame#profileFrame {\n"
"    background-color: #f8fafc;\n"
"    border-radius: 10px;\n"
"}"));
        profileLayout = new QHBoxLayout(profileFrame);
        profileLayout->setObjectName("profileLayout");
        profileLayout->setContentsMargins(12, 10, 12, 10);
        profileAvatarLabel = new QLabel(profileFrame);
        profileAvatarLabel->setObjectName("profileAvatarLabel");
        profileAvatarLabel->setMinimumSize(QSize(38, 38));
        profileAvatarLabel->setMaximumSize(QSize(38, 38));
        profileAvatarLabel->setStyleSheet(QString::fromUtf8("background-color: #dbeafe; color: #2563eb; border-radius: 19px; font-size: 18px; font-weight: 700;"));
        profileAvatarLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        profileLayout->addWidget(profileAvatarLabel);

        profileTextLayout = new QVBoxLayout();
        profileTextLayout->setSpacing(1);
        profileTextLayout->setObjectName("profileTextLayout");
        profileUsernameLabel = new QLabel(profileFrame);
        profileUsernameLabel->setObjectName("profileUsernameLabel");
        profileUsernameLabel->setStyleSheet(QString::fromUtf8("color: #111827; font-weight: 600;"));

        profileTextLayout->addWidget(profileUsernameLabel);

        profileRoleLabel = new QLabel(profileFrame);
        profileRoleLabel->setObjectName("profileRoleLabel");
        profileRoleLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 12px;"));

        profileTextLayout->addWidget(profileRoleLabel);


        profileLayout->addLayout(profileTextLayout);


        sidebarLayout->addWidget(profileFrame);

        logoutButton = new QPushButton(sidebarFrame);
        logoutButton->setObjectName("logoutButton");

        sidebarLayout->addWidget(logoutButton);


        mainHorizontalLayout->addWidget(sidebarFrame);

        mainStackedWidget = new QStackedWidget(Dashboard);
        mainStackedWidget->setObjectName("mainStackedWidget");
        dashboardPage = new QWidget();
        dashboardPage->setObjectName("dashboardPage");
        dashboardPageLayout = new QVBoxLayout(dashboardPage);
        dashboardPageLayout->setSpacing(0);
        dashboardPageLayout->setObjectName("dashboardPageLayout");
        dashboardPageLayout->setContentsMargins(0, 0, 0, 0);
        dashboardScrollArea = new QScrollArea(dashboardPage);
        dashboardScrollArea->setObjectName("dashboardScrollArea");
        dashboardScrollArea->setFrameShape(QFrame::Shape::NoFrame);
        dashboardScrollArea->setWidgetResizable(true);
        dashboardScrollAreaWidgetContents = new QWidget();
        dashboardScrollAreaWidgetContents->setObjectName("dashboardScrollAreaWidgetContents");
        dashboardScrollAreaWidgetContents->setGeometry(QRect(0, 0, 1058, 818));
        contentLayout = new QVBoxLayout(dashboardScrollAreaWidgetContents);
        contentLayout->setSpacing(14);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(24, 22, 24, 22);
        headerFrame = new QFrame(dashboardScrollAreaWidgetContents);
        headerFrame->setObjectName("headerFrame");
        headerFrame->setMinimumSize(QSize(0, 78));
        headerFrame->setMaximumSize(QSize(16777215, 78));
        headerLayout = new QHBoxLayout(headerFrame);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(22, -1, 22, -1);
        welcomeLabel = new QLabel(headerFrame);
        welcomeLabel->setObjectName("welcomeLabel");
        welcomeLabel->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 23px; font-weight: 700;"));

        headerLayout->addWidget(welcomeLabel);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        semesterLabel = new QLabel(headerFrame);
        semesterLabel->setObjectName("semesterLabel");
        semesterLabel->setStyleSheet(QString::fromUtf8("color: #2563eb; font-size: 15px; font-weight: 600;"));

        headerLayout->addWidget(semesterLabel);


        contentLayout->addWidget(headerFrame);

        statisticsLayout = new QHBoxLayout();
        statisticsLayout->setSpacing(14);
        statisticsLayout->setObjectName("statisticsLayout");
        cgpaCard = new QFrame(dashboardScrollAreaWidgetContents);
        cgpaCard->setObjectName("cgpaCard");
        cgpaCard->setMinimumSize(QSize(220, 160));
        cgpaCard->setMaximumSize(QSize(16777215, 170));
        cgpaCardLayout = new QVBoxLayout(cgpaCard);
        cgpaCardLayout->setSpacing(7);
        cgpaCardLayout->setObjectName("cgpaCardLayout");
        cgpaCardLayout->setContentsMargins(18, 16, 18, 16);
        cgpaTitleLabel = new QLabel(cgpaCard);
        cgpaTitleLabel->setObjectName("cgpaTitleLabel");
        cgpaTitleLabel->setStyleSheet(QString::fromUtf8("color: #374151; font-size: 14px; font-weight: 600;"));

        cgpaCardLayout->addWidget(cgpaTitleLabel);

        cgpaValueLabel = new QLabel(cgpaCard);
        cgpaValueLabel->setObjectName("cgpaValueLabel");
        cgpaValueLabel->setStyleSheet(QString::fromUtf8("color: #2563eb; font-size: 29px; font-weight: 700;"));

        cgpaCardLayout->addWidget(cgpaValueLabel);

        cgpaSpacer = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cgpaCardLayout->addItem(cgpaSpacer);

        cgpaProgressBar = new QProgressBar(cgpaCard);
        cgpaProgressBar->setObjectName("cgpaProgressBar");
        cgpaProgressBar->setMaximum(400);
        cgpaProgressBar->setValue(367);

        cgpaCardLayout->addWidget(cgpaProgressBar);

        cgpaScaleLayout = new QHBoxLayout();
        cgpaScaleLayout->setObjectName("cgpaScaleLayout");
        cgpaMinLabel = new QLabel(cgpaCard);
        cgpaMinLabel->setObjectName("cgpaMinLabel");
        cgpaMinLabel->setStyleSheet(QString::fromUtf8("color: #94a3b8; font-size: 11px;"));

        cgpaScaleLayout->addWidget(cgpaMinLabel);

        cgpaScaleSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        cgpaScaleLayout->addItem(cgpaScaleSpacer);

        cgpaMaxLabel = new QLabel(cgpaCard);
        cgpaMaxLabel->setObjectName("cgpaMaxLabel");
        cgpaMaxLabel->setStyleSheet(QString::fromUtf8("color: #94a3b8; font-size: 11px;"));

        cgpaScaleLayout->addWidget(cgpaMaxLabel);


        cgpaCardLayout->addLayout(cgpaScaleLayout);


        statisticsLayout->addWidget(cgpaCard);

        creditsCard = new QFrame(dashboardScrollAreaWidgetContents);
        creditsCard->setObjectName("creditsCard");
        creditsCard->setMinimumSize(QSize(220, 160));
        creditsCard->setMaximumSize(QSize(16777215, 170));
        creditsCardLayout = new QVBoxLayout(creditsCard);
        creditsCardLayout->setSpacing(7);
        creditsCardLayout->setObjectName("creditsCardLayout");
        creditsCardLayout->setContentsMargins(18, 16, 18, 16);
        creditsTitleLabel = new QLabel(creditsCard);
        creditsTitleLabel->setObjectName("creditsTitleLabel");
        creditsTitleLabel->setStyleSheet(QString::fromUtf8("color: #374151; font-size: 14px; font-weight: 600;"));

        creditsCardLayout->addWidget(creditsTitleLabel);

        creditsValueLabel = new QLabel(creditsCard);
        creditsValueLabel->setObjectName("creditsValueLabel");
        creditsValueLabel->setStyleSheet(QString::fromUtf8("color: #16a34a; font-size: 29px; font-weight: 700;"));

        creditsCardLayout->addWidget(creditsValueLabel);

        creditsSpacer = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        creditsCardLayout->addItem(creditsSpacer);

        creditsProgressBar = new QProgressBar(creditsCard);
        creditsProgressBar->setObjectName("creditsProgressBar");
        creditsProgressBar->setStyleSheet(QString::fromUtf8("QProgressBar::chunk { background-color: #16a34a; border-radius: 9px; }"));
        creditsProgressBar->setMaximum(120);
        creditsProgressBar->setValue(42);

        creditsCardLayout->addWidget(creditsProgressBar);

        creditsScaleLayout = new QHBoxLayout();
        creditsScaleLayout->setObjectName("creditsScaleLayout");
        creditsMinLabel = new QLabel(creditsCard);
        creditsMinLabel->setObjectName("creditsMinLabel");
        creditsMinLabel->setStyleSheet(QString::fromUtf8("color: #94a3b8; font-size: 11px;"));

        creditsScaleLayout->addWidget(creditsMinLabel);

        creditsScaleSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        creditsScaleLayout->addItem(creditsScaleSpacer);

        creditsMaxLabel = new QLabel(creditsCard);
        creditsMaxLabel->setObjectName("creditsMaxLabel");
        creditsMaxLabel->setStyleSheet(QString::fromUtf8("color: #94a3b8; font-size: 11px;"));

        creditsScaleLayout->addWidget(creditsMaxLabel);


        creditsCardLayout->addLayout(creditsScaleLayout);


        statisticsLayout->addWidget(creditsCard);

        overallGradeCard = new QFrame(dashboardScrollAreaWidgetContents);
        overallGradeCard->setObjectName("overallGradeCard");
        overallGradeCard->setMinimumSize(QSize(220, 160));
        overallGradeCard->setMaximumSize(QSize(16777215, 170));
        overallGradeCardLayout = new QVBoxLayout(overallGradeCard);
        overallGradeCardLayout->setSpacing(7);
        overallGradeCardLayout->setObjectName("overallGradeCardLayout");
        overallGradeCardLayout->setContentsMargins(18, 16, 18, 16);
        overallGradeTitleLabel = new QLabel(overallGradeCard);
        overallGradeTitleLabel->setObjectName("overallGradeTitleLabel");
        overallGradeTitleLabel->setStyleSheet(QString::fromUtf8("color: #374151; font-size: 14px; font-weight: 600;"));

        overallGradeCardLayout->addWidget(overallGradeTitleLabel);

        overallGradeValueLabel = new QLabel(overallGradeCard);
        overallGradeValueLabel->setObjectName("overallGradeValueLabel");
        overallGradeValueLabel->setStyleSheet(QString::fromUtf8("color: #f59e0b; font-size: 31px; font-weight: 700;"));

        overallGradeCardLayout->addWidget(overallGradeValueLabel);

        overallGradeGpaLabel = new QLabel(overallGradeCard);
        overallGradeGpaLabel->setObjectName("overallGradeGpaLabel");
        overallGradeGpaLabel->setStyleSheet(QString::fromUtf8("color: #475569; font-size: 14px;"));

        overallGradeCardLayout->addWidget(overallGradeGpaLabel);

        overallGradeSpacer = new QSpacerItem(20, 18, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        overallGradeCardLayout->addItem(overallGradeSpacer);


        statisticsLayout->addWidget(overallGradeCard);


        contentLayout->addLayout(statisticsLayout);

        middlePanelsLayout = new QHBoxLayout();
        middlePanelsLayout->setSpacing(14);
        middlePanelsLayout->setObjectName("middlePanelsLayout");
        currentCoursesCard = new QFrame(dashboardScrollAreaWidgetContents);
        currentCoursesCard->setObjectName("currentCoursesCard");
        currentCoursesCard->setMinimumSize(QSize(0, 245));
        currentCoursesCardLayout = new QVBoxLayout(currentCoursesCard);
        currentCoursesCardLayout->setSpacing(6);
        currentCoursesCardLayout->setObjectName("currentCoursesCardLayout");
        currentCoursesCardLayout->setContentsMargins(16, 14, 16, 14);
        currentCoursesHeaderLayout = new QHBoxLayout();
        currentCoursesHeaderLayout->setObjectName("currentCoursesHeaderLayout");
        currentCoursesLabel = new QLabel(currentCoursesCard);
        currentCoursesLabel->setObjectName("currentCoursesLabel");

        currentCoursesHeaderLayout->addWidget(currentCoursesLabel);

        currentCoursesHeaderSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        currentCoursesHeaderLayout->addItem(currentCoursesHeaderSpacer);

        viewAllCoursesButton = new QPushButton(currentCoursesCard);
        viewAllCoursesButton->setObjectName("viewAllCoursesButton");

        currentCoursesHeaderLayout->addWidget(viewAllCoursesButton);


        currentCoursesCardLayout->addLayout(currentCoursesHeaderLayout);

        currentCoursesListWidget = new QListWidget(currentCoursesCard);
        currentCoursesListWidget->setObjectName("currentCoursesListWidget");

        currentCoursesCardLayout->addWidget(currentCoursesListWidget);


        middlePanelsLayout->addWidget(currentCoursesCard);

        upcomingAssignmentsCard = new QFrame(dashboardScrollAreaWidgetContents);
        upcomingAssignmentsCard->setObjectName("upcomingAssignmentsCard");
        upcomingAssignmentsCard->setMinimumSize(QSize(0, 245));
        upcomingAssignmentsCardLayout = new QVBoxLayout(upcomingAssignmentsCard);
        upcomingAssignmentsCardLayout->setSpacing(6);
        upcomingAssignmentsCardLayout->setObjectName("upcomingAssignmentsCardLayout");
        upcomingAssignmentsCardLayout->setContentsMargins(16, 14, 16, 14);
        upcomingAssignmentsHeaderLayout = new QHBoxLayout();
        upcomingAssignmentsHeaderLayout->setObjectName("upcomingAssignmentsHeaderLayout");
        upcomingAssignmentsLabel = new QLabel(upcomingAssignmentsCard);
        upcomingAssignmentsLabel->setObjectName("upcomingAssignmentsLabel");

        upcomingAssignmentsHeaderLayout->addWidget(upcomingAssignmentsLabel);

        upcomingAssignmentsHeaderSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        upcomingAssignmentsHeaderLayout->addItem(upcomingAssignmentsHeaderSpacer);

        viewAllAssignmentsButton = new QPushButton(upcomingAssignmentsCard);
        viewAllAssignmentsButton->setObjectName("viewAllAssignmentsButton");

        upcomingAssignmentsHeaderLayout->addWidget(viewAllAssignmentsButton);


        upcomingAssignmentsCardLayout->addLayout(upcomingAssignmentsHeaderLayout);

        upcomingAssignmentsListWidget = new QListWidget(upcomingAssignmentsCard);
        upcomingAssignmentsListWidget->setObjectName("upcomingAssignmentsListWidget");

        upcomingAssignmentsCardLayout->addWidget(upcomingAssignmentsListWidget);


        middlePanelsLayout->addWidget(upcomingAssignmentsCard);


        contentLayout->addLayout(middlePanelsLayout);

        calendarOverviewCard = new QFrame(dashboardScrollAreaWidgetContents);
        calendarOverviewCard->setObjectName("calendarOverviewCard");
        calendarOverviewCard->setMinimumSize(QSize(0, 175));
        calendarOverviewCardLayout = new QVBoxLayout(calendarOverviewCard);
        calendarOverviewCardLayout->setSpacing(6);
        calendarOverviewCardLayout->setObjectName("calendarOverviewCardLayout");
        calendarOverviewCardLayout->setContentsMargins(16, 14, 16, 14);
        calendarHeaderLayout = new QHBoxLayout();
        calendarHeaderLayout->setObjectName("calendarHeaderLayout");
        calendarOverviewLabel = new QLabel(calendarOverviewCard);
        calendarOverviewLabel->setObjectName("calendarOverviewLabel");

        calendarHeaderLayout->addWidget(calendarOverviewLabel);

        calendarHeaderSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        calendarHeaderLayout->addItem(calendarHeaderSpacer);

        viewCalendarButton = new QPushButton(calendarOverviewCard);
        viewCalendarButton->setObjectName("viewCalendarButton");

        calendarHeaderLayout->addWidget(viewCalendarButton);


        calendarOverviewCardLayout->addLayout(calendarHeaderLayout);

        calendarTableWidget = new QTableWidget(calendarOverviewCard);
        calendarTableWidget->setObjectName("calendarTableWidget");
        calendarTableWidget->setRowCount(0);
        calendarTableWidget->setColumnCount(0);
        calendarTableWidget->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        calendarTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        calendarTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        calendarTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        calendarTableWidget->horizontalHeader()->setDefaultSectionSize(115);
        calendarTableWidget->horizontalHeader()->setStretchLastSection(true);
        calendarTableWidget->verticalHeader()->setVisible(false);

        calendarOverviewCardLayout->addWidget(calendarTableWidget);


        contentLayout->addWidget(calendarOverviewCard);

        contentBottomSpacer = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        contentLayout->addItem(contentBottomSpacer);

        dashboardScrollArea->setWidget(dashboardScrollAreaWidgetContents);

        dashboardPageLayout->addWidget(dashboardScrollArea);

        mainStackedWidget->addWidget(dashboardPage);

        mainHorizontalLayout->addWidget(mainStackedWidget);


        retranslateUi(Dashboard);

        mainStackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Dashboard);
    } // setupUi

    void retranslateUi(QWidget *Dashboard)
    {
        Dashboard->setWindowTitle(QCoreApplication::translate("Dashboard", "Semester Tracker", nullptr));
        appTitleLabel->setText(QCoreApplication::translate("Dashboard", "Semester\n"
"Tracker", nullptr));
        dashboardButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "navButton", nullptr)));
        dashboardButton->setText(QCoreApplication::translate("Dashboard", "Dashboard", nullptr));
        semestersButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "navButton", nullptr)));
        semestersButton->setText(QCoreApplication::translate("Dashboard", "Semesters", nullptr));
        coursesButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "navButton", nullptr)));
        coursesButton->setText(QCoreApplication::translate("Dashboard", "Courses", nullptr));
        assignmentsButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "navButton", nullptr)));
        assignmentsButton->setText(QCoreApplication::translate("Dashboard", "Assignments", nullptr));
        calendarButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "navButton", nullptr)));
        calendarButton->setText(QCoreApplication::translate("Dashboard", "Calendar", nullptr));
        gradesButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "navButton", nullptr)));
        gradesButton->setText(QCoreApplication::translate("Dashboard", "Grades", nullptr));
        settingsButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "navButton", nullptr)));
        settingsButton->setText(QCoreApplication::translate("Dashboard", "Settings", nullptr));
        profileAvatarLabel->setText(QCoreApplication::translate("Dashboard", "U", nullptr));
        profileUsernameLabel->setText(QCoreApplication::translate("Dashboard", "username", nullptr));
        profileRoleLabel->setText(QCoreApplication::translate("Dashboard", "Student", nullptr));
        logoutButton->setText(QCoreApplication::translate("Dashboard", "Logout", nullptr));
        headerFrame->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "card", nullptr)));
        welcomeLabel->setText(QCoreApplication::translate("Dashboard", "Welcome, username", nullptr));
        semesterLabel->setText(QCoreApplication::translate("Dashboard", "Current Semester", nullptr));
        cgpaCard->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "card", nullptr)));
        cgpaTitleLabel->setText(QCoreApplication::translate("Dashboard", "CGPA", nullptr));
        cgpaValueLabel->setText(QCoreApplication::translate("Dashboard", "3.67 / 4.00", nullptr));
        cgpaProgressBar->setFormat(QCoreApplication::translate("Dashboard", "91.75%", nullptr));
        cgpaMinLabel->setText(QCoreApplication::translate("Dashboard", "0.00", nullptr));
        cgpaMaxLabel->setText(QCoreApplication::translate("Dashboard", "4.00", nullptr));
        creditsCard->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "card", nullptr)));
        creditsTitleLabel->setText(QCoreApplication::translate("Dashboard", "Credits Completed", nullptr));
        creditsValueLabel->setText(QCoreApplication::translate("Dashboard", "42 / 120", nullptr));
        creditsProgressBar->setFormat(QCoreApplication::translate("Dashboard", "35.00%", nullptr));
        creditsMinLabel->setText(QCoreApplication::translate("Dashboard", "0", nullptr));
        creditsMaxLabel->setText(QCoreApplication::translate("Dashboard", "120", nullptr));
        overallGradeCard->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "card", nullptr)));
        overallGradeTitleLabel->setText(QCoreApplication::translate("Dashboard", "Overall Grade", nullptr));
        overallGradeValueLabel->setText(QCoreApplication::translate("Dashboard", "A-", nullptr));
        overallGradeGpaLabel->setText(QCoreApplication::translate("Dashboard", "3.67 / 4.00", nullptr));
        currentCoursesCard->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "card", nullptr)));
        currentCoursesLabel->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "sectionTitle", nullptr)));
        currentCoursesLabel->setText(QCoreApplication::translate("Dashboard", "Current Courses", nullptr));
        viewAllCoursesButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "linkButton", nullptr)));
        viewAllCoursesButton->setText(QCoreApplication::translate("Dashboard", "View All", nullptr));
        upcomingAssignmentsCard->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "card", nullptr)));
        upcomingAssignmentsLabel->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "sectionTitle", nullptr)));
        upcomingAssignmentsLabel->setText(QCoreApplication::translate("Dashboard", "Upcoming Assignments", nullptr));
        viewAllAssignmentsButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "linkButton", nullptr)));
        viewAllAssignmentsButton->setText(QCoreApplication::translate("Dashboard", "View All", nullptr));
        calendarOverviewCard->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "card", nullptr)));
        calendarOverviewLabel->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "sectionTitle", nullptr)));
        calendarOverviewLabel->setText(QCoreApplication::translate("Dashboard", "Calendar Overview", nullptr));
        viewCalendarButton->setProperty("class", QVariant(QCoreApplication::translate("Dashboard", "linkButton", nullptr)));
        viewCalendarButton->setText(QCoreApplication::translate("Dashboard", "View Calendar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dashboard: public Ui_Dashboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DASHBOARD_H
