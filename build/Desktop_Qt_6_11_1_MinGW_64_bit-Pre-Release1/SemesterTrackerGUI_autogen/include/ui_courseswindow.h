/********************************************************************************
** Form generated from reading UI file 'courseswindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COURSESWINDOW_H
#define UI_COURSESWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CoursesWindow
{
public:
    QVBoxLayout *mainLayout;
    QFrame *headerCard;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleLayout;
    QLabel *coursesTitleLabel;
    QLabel *coursesSubtitleLabel;
    QSpacerItem *headerSpacer;
    QPushButton *addCourseButton;
    QFrame *semesterSelectorCard;
    QHBoxLayout *semesterSelectorLayout;
    QVBoxLayout *semesterTextLayout;
    QLabel *semesterSelectorLabel;
    QLabel *semesterHelpLabel;
    QSpacerItem *semesterSelectorSpacer;
    QComboBox *semesterComboBox;
    QFrame *coursesCard;
    QVBoxLayout *coursesCardLayout;
    QHBoxLayout *coursesSectionLayout;
    QLabel *coursesSectionLabel;
    QSpacerItem *coursesSectionSpacer;
    QLabel *courseCountLabel;
    QListWidget *coursesListWidget;
    QVBoxLayout *emptyStateLayout;
    QLabel *emptyStateTitleLabel;
    QLabel *emptyStateLabel;

    void setupUi(QWidget *CoursesWindow)
    {
        if (CoursesWindow->objectName().isEmpty())
            CoursesWindow->setObjectName("CoursesWindow");
        CoursesWindow->resize(900, 620);
        CoursesWindow->setMinimumSize(QSize(760, 520));
        CoursesWindow->setStyleSheet(QString::fromUtf8("QWidget#CoursesWindow {\n"
"    background-color: #f5f7fb;\n"
"}\n"
"QFrame#headerCard,\n"
"QFrame#semesterSelectorCard,\n"
"QFrame#coursesCard {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 12px;\n"
"}\n"
"QLabel#coursesTitleLabel {\n"
"    color: #111827;\n"
"    font-size: 25px;\n"
"    font-weight: 700;\n"
"}\n"
"QLabel#coursesSubtitleLabel,\n"
"QLabel#semesterHelpLabel {\n"
"    color: #64748b;\n"
"    font-size: 13px;\n"
"}\n"
"QLabel#semesterSelectorLabel,\n"
"QLabel#coursesSectionLabel {\n"
"    color: #111827;\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"}\n"
"QPushButton#addCourseButton {\n"
"    min-height: 40px;\n"
"    padding: 0 18px;\n"
"    border: none;\n"
"    border-radius: 9px;\n"
"    background-color: #2563eb;\n"
"    color: #ffffff;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"}\n"
"QPushButton#addCourseButton:hover {\n"
"    background-color: #1d4ed8;\n"
"}\n"
"QPushButton#addCourseButton:pressed {\n"
"    background-co"
                        "lor: #1e40af;\n"
"}\n"
"QPushButton#addCourseButton:disabled {\n"
"    background-color: #cbd5e1;\n"
"    color: #f8fafc;\n"
"}\n"
"QComboBox#semesterComboBox {\n"
"    min-height: 38px;\n"
"    min-width: 230px;\n"
"    padding: 0 12px;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    background-color: #ffffff;\n"
"    color: #334155;\n"
"    font-size: 13px;\n"
"}\n"
"QComboBox#semesterComboBox:hover {\n"
"    border-color: #93c5fd;\n"
"}\n"
"QComboBox#semesterComboBox:focus {\n"
"    border: 1px solid #2563eb;\n"
"}\n"
"QComboBox#semesterComboBox QAbstractItemView {\n"
"    background-color: #ffffff;\n"
"    color: #1f2937;\n"
"    border: 1px solid #cbd5e1;\n"
"    selection-background-color: #dbeafe;\n"
"    selection-color: #1f2937;\n"
"}\n"
"QListWidget#coursesListWidget {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    outline: none;\n"
"    color: #1f2937;\n"
"    font-size: 14px;\n"
"}\n"
"QListWidget#coursesListWidget::item {\n"
"    padding: 0;\n"
"    mar"
                        "gin: 4px 0;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"}\n"
"QListWidget#coursesListWidget::item:hover {\n"
"    background-color: transparent;\n"
"    border: none;\n"
"}\n"
"QListWidget#coursesListWidget::item:selected {\n"
"    background-color: transparent;\n"
"    color: #1f2937;\n"
"    border: none;\n"
"}\n"
"QLabel#emptyStateTitleLabel {\n"
"    color: #334155;\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"}\n"
"QLabel#emptyStateLabel {\n"
"    color: #94a3b8;\n"
"    font-size: 14px;\n"
"}"));
        mainLayout = new QVBoxLayout(CoursesWindow);
        mainLayout->setSpacing(16);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(24, 24, 24, 24);
        headerCard = new QFrame(CoursesWindow);
        headerCard->setObjectName("headerCard");
        headerCard->setMinimumSize(QSize(0, 96));
        headerCard->setMaximumSize(QSize(16777215, 96));
        headerLayout = new QHBoxLayout(headerCard);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(22, 16, 22, 16);
        titleLayout = new QVBoxLayout();
        titleLayout->setSpacing(3);
        titleLayout->setObjectName("titleLayout");
        coursesTitleLabel = new QLabel(headerCard);
        coursesTitleLabel->setObjectName("coursesTitleLabel");

        titleLayout->addWidget(coursesTitleLabel);

        coursesSubtitleLabel = new QLabel(headerCard);
        coursesSubtitleLabel->setObjectName("coursesSubtitleLabel");

        titleLayout->addWidget(coursesSubtitleLabel);


        headerLayout->addLayout(titleLayout);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        addCourseButton = new QPushButton(headerCard);
        addCourseButton->setObjectName("addCourseButton");
        addCourseButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        headerLayout->addWidget(addCourseButton);


        mainLayout->addWidget(headerCard);

        semesterSelectorCard = new QFrame(CoursesWindow);
        semesterSelectorCard->setObjectName("semesterSelectorCard");
        semesterSelectorCard->setMinimumSize(QSize(0, 92));
        semesterSelectorCard->setMaximumSize(QSize(16777215, 92));
        semesterSelectorLayout = new QHBoxLayout(semesterSelectorCard);
        semesterSelectorLayout->setSpacing(18);
        semesterSelectorLayout->setObjectName("semesterSelectorLayout");
        semesterSelectorLayout->setContentsMargins(20, 14, 20, 14);
        semesterTextLayout = new QVBoxLayout();
        semesterTextLayout->setSpacing(3);
        semesterTextLayout->setObjectName("semesterTextLayout");
        semesterSelectorLabel = new QLabel(semesterSelectorCard);
        semesterSelectorLabel->setObjectName("semesterSelectorLabel");

        semesterTextLayout->addWidget(semesterSelectorLabel);

        semesterHelpLabel = new QLabel(semesterSelectorCard);
        semesterHelpLabel->setObjectName("semesterHelpLabel");

        semesterTextLayout->addWidget(semesterHelpLabel);


        semesterSelectorLayout->addLayout(semesterTextLayout);

        semesterSelectorSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        semesterSelectorLayout->addItem(semesterSelectorSpacer);

        semesterComboBox = new QComboBox(semesterSelectorCard);
        semesterComboBox->addItem(QString());
        semesterComboBox->setObjectName("semesterComboBox");

        semesterSelectorLayout->addWidget(semesterComboBox);


        mainLayout->addWidget(semesterSelectorCard);

        coursesCard = new QFrame(CoursesWindow);
        coursesCard->setObjectName("coursesCard");
        coursesCardLayout = new QVBoxLayout(coursesCard);
        coursesCardLayout->setSpacing(10);
        coursesCardLayout->setObjectName("coursesCardLayout");
        coursesCardLayout->setContentsMargins(18, 16, 18, 18);
        coursesSectionLayout = new QHBoxLayout();
        coursesSectionLayout->setObjectName("coursesSectionLayout");
        coursesSectionLabel = new QLabel(coursesCard);
        coursesSectionLabel->setObjectName("coursesSectionLabel");

        coursesSectionLayout->addWidget(coursesSectionLabel);

        coursesSectionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        coursesSectionLayout->addItem(coursesSectionSpacer);

        courseCountLabel = new QLabel(coursesCard);
        courseCountLabel->setObjectName("courseCountLabel");
        courseCountLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 13px; font-weight: 600;"));

        coursesSectionLayout->addWidget(courseCountLabel);


        coursesCardLayout->addLayout(coursesSectionLayout);

        coursesListWidget = new QListWidget(coursesCard);
        coursesListWidget->setObjectName("coursesListWidget");

        coursesCardLayout->addWidget(coursesListWidget);

        emptyStateLayout = new QVBoxLayout();
        emptyStateLayout->setSpacing(5);
        emptyStateLayout->setObjectName("emptyStateLayout");
        emptyStateTitleLabel = new QLabel(coursesCard);
        emptyStateTitleLabel->setObjectName("emptyStateTitleLabel");
        emptyStateTitleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        emptyStateLayout->addWidget(emptyStateTitleLabel);

        emptyStateLabel = new QLabel(coursesCard);
        emptyStateLabel->setObjectName("emptyStateLabel");
        emptyStateLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        emptyStateLabel->setWordWrap(true);

        emptyStateLayout->addWidget(emptyStateLabel);


        coursesCardLayout->addLayout(emptyStateLayout);


        mainLayout->addWidget(coursesCard);


        retranslateUi(CoursesWindow);

        QMetaObject::connectSlotsByName(CoursesWindow);
    } // setupUi

    void retranslateUi(QWidget *CoursesWindow)
    {
        CoursesWindow->setWindowTitle(QCoreApplication::translate("CoursesWindow", "Courses", nullptr));
        coursesTitleLabel->setText(QCoreApplication::translate("CoursesWindow", "Courses", nullptr));
        coursesSubtitleLabel->setText(QCoreApplication::translate("CoursesWindow", "View and manage courses for one semester at a time.", nullptr));
        addCourseButton->setText(QCoreApplication::translate("CoursesWindow", "+ Add Course", nullptr));
        semesterSelectorLabel->setText(QCoreApplication::translate("CoursesWindow", "Selected Semester", nullptr));
        semesterHelpLabel->setText(QCoreApplication::translate("CoursesWindow", "Only courses belonging to this semester will be shown.", nullptr));
        semesterComboBox->setItemText(0, QCoreApplication::translate("CoursesWindow", "Select a semester", nullptr));

        coursesSectionLabel->setText(QCoreApplication::translate("CoursesWindow", "Semester Courses", nullptr));
        courseCountLabel->setText(QCoreApplication::translate("CoursesWindow", "0 courses", nullptr));
        emptyStateTitleLabel->setText(QCoreApplication::translate("CoursesWindow", "No courses to show", nullptr));
        emptyStateLabel->setText(QCoreApplication::translate("CoursesWindow", "Select a semester, then click \342\200\234Add Course\342\200\235 to create its first course.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CoursesWindow: public Ui_CoursesWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COURSESWINDOW_H
