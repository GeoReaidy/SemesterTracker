/********************************************************************************
** Form generated from reading UI file 'calendarwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALENDARWINDOW_H
#define UI_CALENDARWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CalendarWindow
{
public:
    QVBoxLayout *rootLayout;
    QLabel *pageTitleLabel;
    QLabel *pageSubtitleLabel;
    QHBoxLayout *mainContentLayout;
    QVBoxLayout *leftColumnLayout;
    QFrame *calendarCard;
    QVBoxLayout *calendarCardLayout;
    QHBoxLayout *monthNavigationLayout;
    QPushButton *previousMonthButton;
    QSpacerItem *monthLeftSpacer;
    QLabel *monthTitleLabel;
    QSpacerItem *monthRightSpacer;
    QPushButton *todayButton;
    QPushButton *nextMonthButton;
    QCalendarWidget *calendarWidget;
    QFrame *selectedDateCard;
    QVBoxLayout *selectedDateCardLayout;
    QLabel *selectedDateLabel;
    QListWidget *selectedDateAssignmentsListWidget;
    QFrame *upcomingCard;
    QVBoxLayout *upcomingCardLayout;
    QHBoxLayout *upcomingHeaderLayout;
    QLabel *upcomingTitleLabel;
    QSpacerItem *upcomingHeaderSpacer;
    QLabel *upcomingCountLabel;
    QListWidget *upcomingDeadlinesListWidget;

    void setupUi(QWidget *CalendarWindow)
    {
        if (CalendarWindow->objectName().isEmpty())
            CalendarWindow->setObjectName("CalendarWindow");
        CalendarWindow->resize(1100, 760);
        CalendarWindow->setMinimumSize(QSize(760, 540));
        CalendarWindow->setStyleSheet(QString::fromUtf8("QWidget#CalendarWindow {\n"
"    background: #f5f7fb;\n"
"    color: #111827;\n"
"}\n"
"\n"
"QFrame#calendarCard,\n"
"QFrame#selectedDateCard,\n"
"QFrame#upcomingCard {\n"
"    background: #ffffff;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"QFrame#calendarCard QLabel,\n"
"QFrame#calendarCard QPushButton,\n"
"QFrame#calendarCard QCalendarWidget,\n"
"QFrame#selectedDateCard QLabel,\n"
"QFrame#selectedDateCard QListWidget,\n"
"QFrame#upcomingCard QLabel,\n"
"QFrame#upcomingCard QListWidget {\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton {\n"
"    min-height: 34px;\n"
"    padding: 0 14px;\n"
"    color: #334155;\n"
"    background: #f8fafc;\n"
"    border: 1px solid #dbe2ea;\n"
"    border-radius: 8px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: #eef2ff;\n"
"    border-color: #a5b4fc;\n"
"}\n"
"\n"
"QCalendarWidget QWidget {\n"
"    alternate-background-color: #f8fafc;\n"
"}\n"
"\n"
"QCalendarWidget QTableView {\n"
"    color: #111827;\n"
""
                        "    background: #ffffff;\n"
"    selection-color: #ffffff;\n"
"    selection-background-color: #4f46e5;\n"
"    border: none;\n"
"    outline: none;\n"
"}"));
        rootLayout = new QVBoxLayout(CalendarWindow);
        rootLayout->setSpacing(18);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(28, 26, 28, 26);
        pageTitleLabel = new QLabel(CalendarWindow);
        pageTitleLabel->setObjectName("pageTitleLabel");
        pageTitleLabel->setStyleSheet(QString::fromUtf8("font-size:26px; font-weight:700; color:#111827;"));

        rootLayout->addWidget(pageTitleLabel);

        pageSubtitleLabel = new QLabel(CalendarWindow);
        pageSubtitleLabel->setObjectName("pageSubtitleLabel");
        pageSubtitleLabel->setStyleSheet(QString::fromUtf8("font-size:13px; color:#64748b;"));

        rootLayout->addWidget(pageSubtitleLabel);

        mainContentLayout = new QHBoxLayout();
        mainContentLayout->setSpacing(18);
        mainContentLayout->setObjectName("mainContentLayout");
        leftColumnLayout = new QVBoxLayout();
        leftColumnLayout->setSpacing(18);
        leftColumnLayout->setObjectName("leftColumnLayout");
        calendarCard = new QFrame(CalendarWindow);
        calendarCard->setObjectName("calendarCard");
        calendarCardLayout = new QVBoxLayout(calendarCard);
        calendarCardLayout->setSpacing(12);
        calendarCardLayout->setObjectName("calendarCardLayout");
        calendarCardLayout->setContentsMargins(18, 16, 18, 18);
        monthNavigationLayout = new QHBoxLayout();
        monthNavigationLayout->setObjectName("monthNavigationLayout");
        previousMonthButton = new QPushButton(calendarCard);
        previousMonthButton->setObjectName("previousMonthButton");

        monthNavigationLayout->addWidget(previousMonthButton);

        monthLeftSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        monthNavigationLayout->addItem(monthLeftSpacer);

        monthTitleLabel = new QLabel(calendarCard);
        monthTitleLabel->setObjectName("monthTitleLabel");
        monthTitleLabel->setStyleSheet(QString::fromUtf8("font-size:18px; font-weight:700; color:#111827;"));
        monthTitleLabel->setAlignment(Qt::AlignCenter);

        monthNavigationLayout->addWidget(monthTitleLabel);

        monthRightSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        monthNavigationLayout->addItem(monthRightSpacer);

        todayButton = new QPushButton(calendarCard);
        todayButton->setObjectName("todayButton");

        monthNavigationLayout->addWidget(todayButton);

        nextMonthButton = new QPushButton(calendarCard);
        nextMonthButton->setObjectName("nextMonthButton");

        monthNavigationLayout->addWidget(nextMonthButton);


        calendarCardLayout->addLayout(monthNavigationLayout);

        calendarWidget = new QCalendarWidget(calendarCard);
        calendarWidget->setObjectName("calendarWidget");
        calendarWidget->setMinimumSize(QSize(500, 390));
        calendarWidget->setGridVisible(true);
        calendarWidget->setSelectionMode(QCalendarWidget::SingleSelection);

        calendarCardLayout->addWidget(calendarWidget);


        leftColumnLayout->addWidget(calendarCard);

        selectedDateCard = new QFrame(CalendarWindow);
        selectedDateCard->setObjectName("selectedDateCard");
        selectedDateCard->setMinimumSize(QSize(0, 190));
        selectedDateCardLayout = new QVBoxLayout(selectedDateCard);
        selectedDateCardLayout->setSpacing(10);
        selectedDateCardLayout->setObjectName("selectedDateCardLayout");
        selectedDateCardLayout->setContentsMargins(18, 16, 18, 16);
        selectedDateLabel = new QLabel(selectedDateCard);
        selectedDateLabel->setObjectName("selectedDateLabel");
        selectedDateLabel->setStyleSheet(QString::fromUtf8("font-size:16px; font-weight:700; color:#111827;"));

        selectedDateCardLayout->addWidget(selectedDateLabel);

        selectedDateAssignmentsListWidget = new QListWidget(selectedDateCard);
        selectedDateAssignmentsListWidget->setObjectName("selectedDateAssignmentsListWidget");

        selectedDateCardLayout->addWidget(selectedDateAssignmentsListWidget);


        leftColumnLayout->addWidget(selectedDateCard);


        mainContentLayout->addLayout(leftColumnLayout);

        upcomingCard = new QFrame(CalendarWindow);
        upcomingCard->setObjectName("upcomingCard");
        upcomingCard->setMinimumSize(QSize(330, 0));
        upcomingCard->setMaximumSize(QSize(430, 16777215));
        upcomingCardLayout = new QVBoxLayout(upcomingCard);
        upcomingCardLayout->setSpacing(10);
        upcomingCardLayout->setObjectName("upcomingCardLayout");
        upcomingCardLayout->setContentsMargins(18, 18, 18, 18);
        upcomingHeaderLayout = new QHBoxLayout();
        upcomingHeaderLayout->setObjectName("upcomingHeaderLayout");
        upcomingTitleLabel = new QLabel(upcomingCard);
        upcomingTitleLabel->setObjectName("upcomingTitleLabel");
        upcomingTitleLabel->setStyleSheet(QString::fromUtf8("font-size:18px; font-weight:700; color:#111827;"));

        upcomingHeaderLayout->addWidget(upcomingTitleLabel);

        upcomingHeaderSpacer = new QSpacerItem(30, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        upcomingHeaderLayout->addItem(upcomingHeaderSpacer);

        upcomingCountLabel = new QLabel(upcomingCard);
        upcomingCountLabel->setObjectName("upcomingCountLabel");
        upcomingCountLabel->setStyleSheet(QString::fromUtf8("color:#64748b; font-size:12px;"));

        upcomingHeaderLayout->addWidget(upcomingCountLabel);


        upcomingCardLayout->addLayout(upcomingHeaderLayout);

        upcomingDeadlinesListWidget = new QListWidget(upcomingCard);
        upcomingDeadlinesListWidget->setObjectName("upcomingDeadlinesListWidget");

        upcomingCardLayout->addWidget(upcomingDeadlinesListWidget);


        mainContentLayout->addWidget(upcomingCard);


        rootLayout->addLayout(mainContentLayout);


        retranslateUi(CalendarWindow);

        QMetaObject::connectSlotsByName(CalendarWindow);
    } // setupUi

    void retranslateUi(QWidget *CalendarWindow)
    {
        CalendarWindow->setWindowTitle(QCoreApplication::translate("CalendarWindow", "Calendar", nullptr));
        pageTitleLabel->setText(QCoreApplication::translate("CalendarWindow", "Calendar", nullptr));
        pageSubtitleLabel->setText(QCoreApplication::translate("CalendarWindow", "Track assignment deadlines across all semesters and courses.", nullptr));
        previousMonthButton->setText(QCoreApplication::translate("CalendarWindow", "\342\200\271", nullptr));
        monthTitleLabel->setText(QCoreApplication::translate("CalendarWindow", "Month Year", nullptr));
        todayButton->setText(QCoreApplication::translate("CalendarWindow", "Today", nullptr));
        nextMonthButton->setText(QCoreApplication::translate("CalendarWindow", "\342\200\272", nullptr));
        selectedDateLabel->setText(QCoreApplication::translate("CalendarWindow", "Selected date", nullptr));
        upcomingTitleLabel->setText(QCoreApplication::translate("CalendarWindow", "Upcoming Deadlines", nullptr));
        upcomingCountLabel->setText(QCoreApplication::translate("CalendarWindow", "0 upcoming", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CalendarWindow: public Ui_CalendarWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALENDARWINDOW_H
