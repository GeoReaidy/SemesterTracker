/********************************************************************************
** Form generated from reading UI file 'assignmentswindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ASSIGNMENTSWINDOW_H
#define UI_ASSIGNMENTSWINDOW_H

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

class Ui_AssignmentsWindow
{
public:
    QVBoxLayout *mainLayout;
    QFrame *headerCard;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleLayout;
    QLabel *assignmentsTitleLabel;
    QLabel *assignmentsSubtitleLabel;
    QSpacerItem *headerSpacer;
    QPushButton *addAssignmentButton;
    QFrame *selectionCard;
    QVBoxLayout *selectionCardLayout;
    QLabel *selectionHelpLabel;
    QHBoxLayout *selectorsLayout;
    QVBoxLayout *semesterSelectorLayout;
    QLabel *semesterLabel;
    QComboBox *semesterComboBox;
    QVBoxLayout *courseSelectorLayout;
    QLabel *courseLabel;
    QComboBox *courseComboBox;
    QSpacerItem *selectorsSpacer;
    QFrame *assignmentsCard;
    QVBoxLayout *assignmentsCardLayout;
    QHBoxLayout *assignmentsSectionLayout;
    QLabel *assignmentsSectionLabel;
    QSpacerItem *assignmentsSectionSpacer;
    QLabel *assignmentCountLabel;
    QListWidget *assignmentsListWidget;
    QVBoxLayout *emptyStateLayout;
    QLabel *emptyStateTitleLabel;
    QLabel *emptyStateLabel;

    void setupUi(QWidget *AssignmentsWindow)
    {
        if (AssignmentsWindow->objectName().isEmpty())
            AssignmentsWindow->setObjectName("AssignmentsWindow");
        AssignmentsWindow->resize(920, 650);
        AssignmentsWindow->setMinimumSize(QSize(780, 540));
        AssignmentsWindow->setStyleSheet(QString::fromUtf8("QWidget#AssignmentsWindow {\n"
"    background-color: #f5f7fb;\n"
"}\n"
"\n"
"QFrame#headerCard,\n"
"QFrame#selectionCard,\n"
"QFrame#assignmentsCard {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"QLabel#assignmentsTitleLabel {\n"
"    color: #111827;\n"
"    font-size: 25px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QLabel#assignmentsSubtitleLabel,\n"
"QLabel#selectionHelpLabel {\n"
"    color: #64748b;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLabel#semesterLabel,\n"
"QLabel#courseLabel,\n"
"QLabel#assignmentsSectionLabel {\n"
"    color: #111827;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QPushButton#addAssignmentButton {\n"
"    min-height: 40px;\n"
"    padding: 0 18px;\n"
"    border: none;\n"
"    border-radius: 9px;\n"
"    background-color: #2563eb;\n"
"    color: #ffffff;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QPushButton#addAssignmentButton:hover {\n"
"    background-color: #1d4ed8;\n"
""
                        "}\n"
"\n"
"QPushButton#addAssignmentButton:pressed {\n"
"    background-color: #1e40af;\n"
"}\n"
"\n"
"QPushButton#addAssignmentButton:disabled {\n"
"    background-color: #cbd5e1;\n"
"    color: #f8fafc;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox,\n"
"QComboBox#courseComboBox {\n"
"    min-height: 38px;\n"
"    min-width: 220px;\n"
"    padding: 0 12px;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    background-color: #ffffff;\n"
"    color: #334155;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox:hover,\n"
"QComboBox#courseComboBox:hover {\n"
"    border-color: #93c5fd;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox:focus,\n"
"QComboBox#courseComboBox:focus {\n"
"    border: 1px solid #2563eb;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox QAbstractItemView,\n"
"QComboBox#courseComboBox QAbstractItemView {\n"
"    background-color: #ffffff;\n"
"    color: #1f2937;\n"
"    border: 1px solid #cbd5e1;\n"
"    selection-background-color: #dbeafe;\n"
"    selection-color: #1f2937;\n"
""
                        "}\n"
"\n"
"QListWidget#assignmentsListWidget {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    outline: none;\n"
"    color: #1f2937;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QListWidget#assignmentsListWidget::item {\n"
"    margin: 4px 0;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"QListWidget#assignmentsListWidget::item:hover {\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"QListWidget#assignmentsListWidget::item:selected {\n"
"    background-color: transparent;\n"
"    color: #1f2937;\n"
"}\n"
"\n"
"QLabel#emptyStateTitleLabel {\n"
"    color: #334155;\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QLabel#emptyStateLabel {\n"
"    color: #94a3b8;\n"
"    font-size: 14px;\n"
"}"));
        mainLayout = new QVBoxLayout(AssignmentsWindow);
        mainLayout->setSpacing(16);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(24, 24, 24, 24);
        headerCard = new QFrame(AssignmentsWindow);
        headerCard->setObjectName("headerCard");
        headerCard->setMinimumSize(QSize(0, 96));
        headerCard->setMaximumSize(QSize(16777215, 96));
        headerLayout = new QHBoxLayout(headerCard);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(22, 16, 22, 16);
        titleLayout = new QVBoxLayout();
        titleLayout->setSpacing(3);
        titleLayout->setObjectName("titleLayout");
        assignmentsTitleLabel = new QLabel(headerCard);
        assignmentsTitleLabel->setObjectName("assignmentsTitleLabel");

        titleLayout->addWidget(assignmentsTitleLabel);

        assignmentsSubtitleLabel = new QLabel(headerCard);
        assignmentsSubtitleLabel->setObjectName("assignmentsSubtitleLabel");

        titleLayout->addWidget(assignmentsSubtitleLabel);


        headerLayout->addLayout(titleLayout);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        addAssignmentButton = new QPushButton(headerCard);
        addAssignmentButton->setObjectName("addAssignmentButton");
        addAssignmentButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        headerLayout->addWidget(addAssignmentButton);


        mainLayout->addWidget(headerCard);

        selectionCard = new QFrame(AssignmentsWindow);
        selectionCard->setObjectName("selectionCard");
        selectionCard->setMinimumSize(QSize(0, 130));
        selectionCard->setMaximumSize(QSize(16777215, 130));
        selectionCardLayout = new QVBoxLayout(selectionCard);
        selectionCardLayout->setSpacing(10);
        selectionCardLayout->setObjectName("selectionCardLayout");
        selectionCardLayout->setContentsMargins(20, 14, 20, 14);
        selectionHelpLabel = new QLabel(selectionCard);
        selectionHelpLabel->setObjectName("selectionHelpLabel");

        selectionCardLayout->addWidget(selectionHelpLabel);

        selectorsLayout = new QHBoxLayout();
        selectorsLayout->setSpacing(18);
        selectorsLayout->setObjectName("selectorsLayout");
        semesterSelectorLayout = new QVBoxLayout();
        semesterSelectorLayout->setSpacing(5);
        semesterSelectorLayout->setObjectName("semesterSelectorLayout");
        semesterLabel = new QLabel(selectionCard);
        semesterLabel->setObjectName("semesterLabel");

        semesterSelectorLayout->addWidget(semesterLabel);

        semesterComboBox = new QComboBox(selectionCard);
        semesterComboBox->addItem(QString());
        semesterComboBox->setObjectName("semesterComboBox");

        semesterSelectorLayout->addWidget(semesterComboBox);


        selectorsLayout->addLayout(semesterSelectorLayout);

        courseSelectorLayout = new QVBoxLayout();
        courseSelectorLayout->setSpacing(5);
        courseSelectorLayout->setObjectName("courseSelectorLayout");
        courseLabel = new QLabel(selectionCard);
        courseLabel->setObjectName("courseLabel");

        courseSelectorLayout->addWidget(courseLabel);

        courseComboBox = new QComboBox(selectionCard);
        courseComboBox->addItem(QString());
        courseComboBox->setObjectName("courseComboBox");

        courseSelectorLayout->addWidget(courseComboBox);


        selectorsLayout->addLayout(courseSelectorLayout);

        selectorsSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        selectorsLayout->addItem(selectorsSpacer);


        selectionCardLayout->addLayout(selectorsLayout);


        mainLayout->addWidget(selectionCard);

        assignmentsCard = new QFrame(AssignmentsWindow);
        assignmentsCard->setObjectName("assignmentsCard");
        assignmentsCardLayout = new QVBoxLayout(assignmentsCard);
        assignmentsCardLayout->setSpacing(10);
        assignmentsCardLayout->setObjectName("assignmentsCardLayout");
        assignmentsCardLayout->setContentsMargins(18, 16, 18, 18);
        assignmentsSectionLayout = new QHBoxLayout();
        assignmentsSectionLayout->setObjectName("assignmentsSectionLayout");
        assignmentsSectionLabel = new QLabel(assignmentsCard);
        assignmentsSectionLabel->setObjectName("assignmentsSectionLabel");

        assignmentsSectionLayout->addWidget(assignmentsSectionLabel);

        assignmentsSectionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        assignmentsSectionLayout->addItem(assignmentsSectionSpacer);

        assignmentCountLabel = new QLabel(assignmentsCard);
        assignmentCountLabel->setObjectName("assignmentCountLabel");
        assignmentCountLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 13px; font-weight: 600;"));

        assignmentsSectionLayout->addWidget(assignmentCountLabel);


        assignmentsCardLayout->addLayout(assignmentsSectionLayout);

        assignmentsListWidget = new QListWidget(assignmentsCard);
        assignmentsListWidget->setObjectName("assignmentsListWidget");

        assignmentsCardLayout->addWidget(assignmentsListWidget);

        emptyStateLayout = new QVBoxLayout();
        emptyStateLayout->setSpacing(5);
        emptyStateLayout->setObjectName("emptyStateLayout");
        emptyStateTitleLabel = new QLabel(assignmentsCard);
        emptyStateTitleLabel->setObjectName("emptyStateTitleLabel");
        emptyStateTitleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        emptyStateLayout->addWidget(emptyStateTitleLabel);

        emptyStateLabel = new QLabel(assignmentsCard);
        emptyStateLabel->setObjectName("emptyStateLabel");
        emptyStateLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        emptyStateLabel->setWordWrap(true);

        emptyStateLayout->addWidget(emptyStateLabel);


        assignmentsCardLayout->addLayout(emptyStateLayout);


        mainLayout->addWidget(assignmentsCard);


        retranslateUi(AssignmentsWindow);

        QMetaObject::connectSlotsByName(AssignmentsWindow);
    } // setupUi

    void retranslateUi(QWidget *AssignmentsWindow)
    {
        AssignmentsWindow->setWindowTitle(QCoreApplication::translate("AssignmentsWindow", "Assignments", nullptr));
        assignmentsTitleLabel->setText(QCoreApplication::translate("AssignmentsWindow", "Assignments", nullptr));
        assignmentsSubtitleLabel->setText(QCoreApplication::translate("AssignmentsWindow", "View and manage assignments for one course at a time.", nullptr));
        addAssignmentButton->setText(QCoreApplication::translate("AssignmentsWindow", "+ Add Assignment", nullptr));
        selectionHelpLabel->setText(QCoreApplication::translate("AssignmentsWindow", "Select a semester first, then choose one of its courses.", nullptr));
        semesterLabel->setText(QCoreApplication::translate("AssignmentsWindow", "Semester", nullptr));
        semesterComboBox->setItemText(0, QCoreApplication::translate("AssignmentsWindow", "Select a semester", nullptr));

        courseLabel->setText(QCoreApplication::translate("AssignmentsWindow", "Course", nullptr));
        courseComboBox->setItemText(0, QCoreApplication::translate("AssignmentsWindow", "Select a course", nullptr));

        assignmentsSectionLabel->setText(QCoreApplication::translate("AssignmentsWindow", "Course Assignments", nullptr));
        assignmentCountLabel->setText(QCoreApplication::translate("AssignmentsWindow", "0 assignments", nullptr));
        emptyStateTitleLabel->setText(QCoreApplication::translate("AssignmentsWindow", "No assignments to show", nullptr));
        emptyStateLabel->setText(QCoreApplication::translate("AssignmentsWindow", "Select a course, then click \342\200\234Add Assignment\342\200\235 to create its first assignment.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AssignmentsWindow: public Ui_AssignmentsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ASSIGNMENTSWINDOW_H
