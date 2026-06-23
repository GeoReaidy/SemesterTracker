/********************************************************************************
** Form generated from reading UI file 'gradeswindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRADESWINDOW_H
#define UI_GRADESWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GradesWindow
{
public:
    QVBoxLayout *mainLayout;
    QFrame *headerCard;
    QVBoxLayout *headerLayout;
    QLabel *gradesTitleLabel;
    QLabel *gradesSubtitleLabel;
    QFrame *semesterCard;
    QHBoxLayout *semesterCardLayout;
    QVBoxLayout *semesterTextLayout;
    QLabel *semesterSectionLabel;
    QLabel *semesterHelpLabel;
    QSpacerItem *semesterSpacer;
    QComboBox *semesterComboBox;
    QFrame *summaryCard;
    QVBoxLayout *summaryCardLayout;
    QLabel *summaryCaptionLabel;
    QHBoxLayout *summaryValuesLayout;
    QFrame *semesterGpaFrame;
    QVBoxLayout *semesterGpaLayout;
    QLabel *semesterGpaTitleLabel;
    QLabel *semesterGpaValueLabel;
    QFrame *averageGradeFrame;
    QVBoxLayout *averageGradeLayout;
    QLabel *averageGradeTitleLabel;
    QLabel *averageGradeValueLabel;
    QFrame *completedCoursesFrame;
    QVBoxLayout *completedCoursesLayout;
    QLabel *completedCoursesTitleLabel;
    QLabel *completedCoursesValueLabel;
    QFrame *coursesCard;
    QVBoxLayout *coursesCardLayout;
    QHBoxLayout *courseGradesHeaderLayout;
    QLabel *courseGradesLabel;
    QSpacerItem *courseGradesSpacer;
    QLabel *courseCountLabel;
    QListWidget *courseGradesListWidget;
    QVBoxLayout *emptyStateLayout;
    QLabel *emptyStateTitleLabel;
    QLabel *emptyStateLabel;

    void setupUi(QWidget *GradesWindow)
    {
        if (GradesWindow->objectName().isEmpty())
            GradesWindow->setObjectName("GradesWindow");
        GradesWindow->resize(980, 680);
        GradesWindow->setMinimumSize(QSize(820, 560));
        GradesWindow->setStyleSheet(QString::fromUtf8("QWidget#GradesWindow {\n"
"    background-color: #f5f7fb;\n"
"}\n"
"\n"
"QFrame#headerCard,\n"
"QFrame#semesterCard,\n"
"QFrame#summaryCard,\n"
"QFrame#coursesCard {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"QLabel#gradesTitleLabel {\n"
"    color: #111827;\n"
"    font-size: 25px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QLabel#gradesSubtitleLabel,\n"
"QLabel#semesterHelpLabel,\n"
"QLabel#summaryCaptionLabel,\n"
"QLabel#emptyStateLabel {\n"
"    color: #64748b;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLabel#semesterSectionLabel,\n"
"QLabel#courseGradesLabel {\n"
"    color: #111827;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QLabel#semesterGpaTitleLabel,\n"
"QLabel#averageGradeTitleLabel,\n"
"QLabel#completedCoursesTitleLabel {\n"
"    color: #64748b;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QLabel#semesterGpaValueLabel,\n"
"QLabel#averageGradeValueLabel,\n"
"QLabel#completedCoursesValueLabe"
                        "l {\n"
"    color: #111827;\n"
"    font-size: 26px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox {\n"
"    min-height: 38px;\n"
"    min-width: 240px;\n"
"    padding: 0 12px;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    background-color: #ffffff;\n"
"    color: #334155;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox:hover {\n"
"    border-color: #93c5fd;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox:focus {\n"
"    border: 1px solid #2563eb;\n"
"}\n"
"\n"
"QComboBox#semesterComboBox QAbstractItemView {\n"
"    background-color: #ffffff;\n"
"    color: #1f2937;\n"
"    border: 1px solid #cbd5e1;\n"
"    selection-background-color: #dbeafe;\n"
"    selection-color: #1f2937;\n"
"}\n"
"\n"
"QListWidget#courseGradesListWidget {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    outline: none;\n"
"    color: #1f2937;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QListWidget#courseGradesListWidget::item {\n"
"    margin: 4px 0;\n"
"    bor"
                        "der: none;\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"QListWidget#courseGradesListWidget::item:hover,\n"
"QListWidget#courseGradesListWidget::item:selected {\n"
"    background-color: transparent;\n"
"    color: #1f2937;\n"
"}\n"
"\n"
"QLabel#emptyStateTitleLabel {\n"
"    color: #334155;\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"}"));
        mainLayout = new QVBoxLayout(GradesWindow);
        mainLayout->setSpacing(16);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(24, 24, 24, 24);
        headerCard = new QFrame(GradesWindow);
        headerCard->setObjectName("headerCard");
        headerCard->setMinimumSize(QSize(0, 96));
        headerCard->setMaximumSize(QSize(16777215, 96));
        headerLayout = new QVBoxLayout(headerCard);
        headerLayout->setSpacing(3);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(22, 16, 22, 16);
        gradesTitleLabel = new QLabel(headerCard);
        gradesTitleLabel->setObjectName("gradesTitleLabel");

        headerLayout->addWidget(gradesTitleLabel);

        gradesSubtitleLabel = new QLabel(headerCard);
        gradesSubtitleLabel->setObjectName("gradesSubtitleLabel");

        headerLayout->addWidget(gradesSubtitleLabel);


        mainLayout->addWidget(headerCard);

        semesterCard = new QFrame(GradesWindow);
        semesterCard->setObjectName("semesterCard");
        semesterCard->setMinimumSize(QSize(0, 108));
        semesterCard->setMaximumSize(QSize(16777215, 108));
        semesterCardLayout = new QHBoxLayout(semesterCard);
        semesterCardLayout->setObjectName("semesterCardLayout");
        semesterCardLayout->setContentsMargins(20, 14, 20, 14);
        semesterTextLayout = new QVBoxLayout();
        semesterTextLayout->setSpacing(4);
        semesterTextLayout->setObjectName("semesterTextLayout");
        semesterSectionLabel = new QLabel(semesterCard);
        semesterSectionLabel->setObjectName("semesterSectionLabel");

        semesterTextLayout->addWidget(semesterSectionLabel);

        semesterHelpLabel = new QLabel(semesterCard);
        semesterHelpLabel->setObjectName("semesterHelpLabel");

        semesterTextLayout->addWidget(semesterHelpLabel);


        semesterCardLayout->addLayout(semesterTextLayout);

        semesterSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        semesterCardLayout->addItem(semesterSpacer);

        semesterComboBox = new QComboBox(semesterCard);
        semesterComboBox->addItem(QString());
        semesterComboBox->setObjectName("semesterComboBox");

        semesterCardLayout->addWidget(semesterComboBox);


        mainLayout->addWidget(semesterCard);

        summaryCard = new QFrame(GradesWindow);
        summaryCard->setObjectName("summaryCard");
        summaryCard->setMinimumSize(QSize(0, 150));
        summaryCard->setMaximumSize(QSize(16777215, 150));
        summaryCardLayout = new QVBoxLayout(summaryCard);
        summaryCardLayout->setSpacing(10);
        summaryCardLayout->setObjectName("summaryCardLayout");
        summaryCardLayout->setContentsMargins(20, 14, 20, 14);
        summaryCaptionLabel = new QLabel(summaryCard);
        summaryCaptionLabel->setObjectName("summaryCaptionLabel");

        summaryCardLayout->addWidget(summaryCaptionLabel);

        summaryValuesLayout = new QHBoxLayout();
        summaryValuesLayout->setSpacing(18);
        summaryValuesLayout->setObjectName("summaryValuesLayout");
        semesterGpaFrame = new QFrame(summaryCard);
        semesterGpaFrame->setObjectName("semesterGpaFrame");
        semesterGpaFrame->setStyleSheet(QString::fromUtf8("QFrame#semesterGpaFrame {\n"
"    background-color: #eff6ff;\n"
"    border: 1px solid #bfdbfe;\n"
"    border-radius: 10px;\n"
"}"));
        semesterGpaLayout = new QVBoxLayout(semesterGpaFrame);
        semesterGpaLayout->setObjectName("semesterGpaLayout");
        semesterGpaLayout->setContentsMargins(16, 12, 16, 12);
        semesterGpaTitleLabel = new QLabel(semesterGpaFrame);
        semesterGpaTitleLabel->setObjectName("semesterGpaTitleLabel");

        semesterGpaLayout->addWidget(semesterGpaTitleLabel);

        semesterGpaValueLabel = new QLabel(semesterGpaFrame);
        semesterGpaValueLabel->setObjectName("semesterGpaValueLabel");

        semesterGpaLayout->addWidget(semesterGpaValueLabel);


        summaryValuesLayout->addWidget(semesterGpaFrame);

        averageGradeFrame = new QFrame(summaryCard);
        averageGradeFrame->setObjectName("averageGradeFrame");
        averageGradeFrame->setStyleSheet(QString::fromUtf8("QFrame#averageGradeFrame {\n"
"    background-color: #f0fdf4;\n"
"    border: 1px solid #bbf7d0;\n"
"    border-radius: 10px;\n"
"}"));
        averageGradeLayout = new QVBoxLayout(averageGradeFrame);
        averageGradeLayout->setObjectName("averageGradeLayout");
        averageGradeLayout->setContentsMargins(16, 12, 16, 12);
        averageGradeTitleLabel = new QLabel(averageGradeFrame);
        averageGradeTitleLabel->setObjectName("averageGradeTitleLabel");

        averageGradeLayout->addWidget(averageGradeTitleLabel);

        averageGradeValueLabel = new QLabel(averageGradeFrame);
        averageGradeValueLabel->setObjectName("averageGradeValueLabel");

        averageGradeLayout->addWidget(averageGradeValueLabel);


        summaryValuesLayout->addWidget(averageGradeFrame);

        completedCoursesFrame = new QFrame(summaryCard);
        completedCoursesFrame->setObjectName("completedCoursesFrame");
        completedCoursesFrame->setStyleSheet(QString::fromUtf8("QFrame#completedCoursesFrame {\n"
"    background-color: #fff7ed;\n"
"    border: 1px solid #fed7aa;\n"
"    border-radius: 10px;\n"
"}"));
        completedCoursesLayout = new QVBoxLayout(completedCoursesFrame);
        completedCoursesLayout->setObjectName("completedCoursesLayout");
        completedCoursesLayout->setContentsMargins(16, 12, 16, 12);
        completedCoursesTitleLabel = new QLabel(completedCoursesFrame);
        completedCoursesTitleLabel->setObjectName("completedCoursesTitleLabel");

        completedCoursesLayout->addWidget(completedCoursesTitleLabel);

        completedCoursesValueLabel = new QLabel(completedCoursesFrame);
        completedCoursesValueLabel->setObjectName("completedCoursesValueLabel");

        completedCoursesLayout->addWidget(completedCoursesValueLabel);


        summaryValuesLayout->addWidget(completedCoursesFrame);


        summaryCardLayout->addLayout(summaryValuesLayout);


        mainLayout->addWidget(summaryCard);

        coursesCard = new QFrame(GradesWindow);
        coursesCard->setObjectName("coursesCard");
        coursesCardLayout = new QVBoxLayout(coursesCard);
        coursesCardLayout->setSpacing(10);
        coursesCardLayout->setObjectName("coursesCardLayout");
        coursesCardLayout->setContentsMargins(18, 16, 18, 18);
        courseGradesHeaderLayout = new QHBoxLayout();
        courseGradesHeaderLayout->setObjectName("courseGradesHeaderLayout");
        courseGradesLabel = new QLabel(coursesCard);
        courseGradesLabel->setObjectName("courseGradesLabel");

        courseGradesHeaderLayout->addWidget(courseGradesLabel);

        courseGradesSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        courseGradesHeaderLayout->addItem(courseGradesSpacer);

        courseCountLabel = new QLabel(coursesCard);
        courseCountLabel->setObjectName("courseCountLabel");
        courseCountLabel->setStyleSheet(QString::fromUtf8("color: #64748b; font-size: 13px; font-weight: 600;"));

        courseGradesHeaderLayout->addWidget(courseCountLabel);


        coursesCardLayout->addLayout(courseGradesHeaderLayout);

        courseGradesListWidget = new QListWidget(coursesCard);
        courseGradesListWidget->setObjectName("courseGradesListWidget");

        coursesCardLayout->addWidget(courseGradesListWidget);

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


        retranslateUi(GradesWindow);

        QMetaObject::connectSlotsByName(GradesWindow);
    } // setupUi

    void retranslateUi(QWidget *GradesWindow)
    {
        GradesWindow->setWindowTitle(QCoreApplication::translate("GradesWindow", "Grades", nullptr));
        gradesTitleLabel->setText(QCoreApplication::translate("GradesWindow", "Grades", nullptr));
        gradesSubtitleLabel->setText(QCoreApplication::translate("GradesWindow", "Review course grades and semester performance.", nullptr));
        semesterSectionLabel->setText(QCoreApplication::translate("GradesWindow", "Selected Semester", nullptr));
        semesterHelpLabel->setText(QCoreApplication::translate("GradesWindow", "Only grades from the selected semester will be shown.", nullptr));
        semesterComboBox->setItemText(0, QCoreApplication::translate("GradesWindow", "Select a semester", nullptr));

        summaryCaptionLabel->setText(QCoreApplication::translate("GradesWindow", "Semester Summary", nullptr));
        semesterGpaTitleLabel->setText(QCoreApplication::translate("GradesWindow", "Semester GPA", nullptr));
        semesterGpaValueLabel->setText(QCoreApplication::translate("GradesWindow", "\342\200\224", nullptr));
        averageGradeTitleLabel->setText(QCoreApplication::translate("GradesWindow", "Average Grade", nullptr));
        averageGradeValueLabel->setText(QCoreApplication::translate("GradesWindow", "\342\200\224", nullptr));
        completedCoursesTitleLabel->setText(QCoreApplication::translate("GradesWindow", "Courses", nullptr));
        completedCoursesValueLabel->setText(QCoreApplication::translate("GradesWindow", "0", nullptr));
        courseGradesLabel->setText(QCoreApplication::translate("GradesWindow", "Course Grades", nullptr));
        courseCountLabel->setText(QCoreApplication::translate("GradesWindow", "0 courses", nullptr));
        emptyStateTitleLabel->setText(QCoreApplication::translate("GradesWindow", "No grades to show", nullptr));
        emptyStateLabel->setText(QCoreApplication::translate("GradesWindow", "Select a semester with courses and graded assignments.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GradesWindow: public Ui_GradesWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRADESWINDOW_H
