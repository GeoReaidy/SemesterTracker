/********************************************************************************
** Form generated from reading UI file 'courseeditordialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COURSEEDITORDIALOG_H
#define UI_COURSEEDITORDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CourseEditorDialog
{
public:
    QVBoxLayout *mainLayout;
    QLabel *dialogTitleLabel;
    QLabel *dialogSubtitleLabel;
    QFrame *contentCard;
    QVBoxLayout *contentLayout;
    QLabel *semesterLabel;
    QComboBox *semesterComboBox;
    QLabel *courseCodeLabel;
    QLineEdit *courseCodeEdit;
    QLabel *courseNameLabel;
    QLineEdit *courseNameEdit;
    QLabel *creditsLabel;
    QSpinBox *creditsSpinBox;
    QLabel *finalGradeLabel;
    QComboBox *finalGradeComboBox;
    QFrame *retakeFrame;
    QVBoxLayout *retakeLayout;
    QCheckBox *retakeCheckBox;
    QLabel *retakeHelpLabel;
    QLabel *editNoticeLabel;
    QLabel *validationLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *CourseEditorDialog)
    {
        if (CourseEditorDialog->objectName().isEmpty())
            CourseEditorDialog->setObjectName("CourseEditorDialog");
        CourseEditorDialog->resize(560, 640);
        CourseEditorDialog->setMinimumSize(QSize(520, 0));
        CourseEditorDialog->setModal(true);
        CourseEditorDialog->setStyleSheet(QString::fromUtf8("QDialog#CourseEditorDialog {\n"
"    background-color: #f8fafc;\n"
"}\n"
"QFrame#contentCard {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 12px;\n"
"}\n"
"QLabel#dialogTitleLabel {\n"
"    color: #0f172a;\n"
"    font-size: 22px;\n"
"    font-weight: 700;\n"
"}\n"
"QLabel#dialogSubtitleLabel {\n"
"    color: #64748b;\n"
"    font-size: 13px;\n"
"}\n"
"QLabel#semesterLabel,\n"
"QLabel#courseCodeLabel,\n"
"QLabel#courseNameLabel,\n"
"QLabel#creditsLabel,\n"
"QLabel#finalGradeLabel {\n"
"    color: #334155;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"QLineEdit,\n"
"QComboBox,\n"
"QSpinBox {\n"
"    min-height: 38px;\n"
"    padding: 0 10px;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    background-color: #ffffff;\n"
"    color: #1f2937;\n"
"}\n"
"QLineEdit:focus,\n"
"QComboBox:focus,\n"
"QSpinBox:focus {\n"
"    border-color: #2563eb;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #ffffff;\n"
"    color: #"
                        "1f2937;\n"
"    selection-background-color: #dbeafe;\n"
"    selection-color: #1f2937;\n"
"    border: 1px solid #cbd5e1;\n"
"    outline: none;\n"
"}\n"
"QFrame#retakeFrame {\n"
"    background-color: #eff6ff;\n"
"    border: 1px solid #bfdbfe;\n"
"    border-radius: 9px;\n"
"}\n"
"QCheckBox#retakeCheckBox {\n"
"    color: #1e3a8a;\n"
"    font-weight: 600;\n"
"    spacing: 8px;\n"
"}\n"
"QLabel#retakeHelpLabel {\n"
"    color: #475569;\n"
"    font-size: 12px;\n"
"}\n"
"QLabel#editNoticeLabel {\n"
"    color: #475569;\n"
"    background-color: #f8fafc;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 7px;\n"
"    padding: 8px;\n"
"}\n"
"QLabel#validationLabel {\n"
"    color: #b91c1c;\n"
"    background-color: #fef2f2;\n"
"    border: 1px solid #fecaca;\n"
"    border-radius: 7px;\n"
"    padding: 8px;\n"
"}\n"
"QDialogButtonBox QPushButton {\n"
"    min-width: 96px;\n"
"    min-height: 38px;\n"
"    padding: 0 16px;\n"
"    border-radius: 8px;\n"
"    font-weight: 600;\n"
"}\n"
"QDialogButtonBox QP"
                        "ushButton[text=\"Save\"] {\n"
"    background-color: #2563eb;\n"
"    color: #ffffff;\n"
"    border: none;\n"
"}\n"
"QDialogButtonBox QPushButton[text=\"Save\"]:hover {\n"
"    background-color: #1d4ed8;\n"
"}\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"] {\n"
"    background-color: #ffffff;\n"
"    color: #334155;\n"
"    border: 1px solid #cbd5e1;\n"
"}"));
        mainLayout = new QVBoxLayout(CourseEditorDialog);
        mainLayout->setSpacing(14);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(22, 22, 22, 22);
        dialogTitleLabel = new QLabel(CourseEditorDialog);
        dialogTitleLabel->setObjectName("dialogTitleLabel");

        mainLayout->addWidget(dialogTitleLabel);

        dialogSubtitleLabel = new QLabel(CourseEditorDialog);
        dialogSubtitleLabel->setObjectName("dialogSubtitleLabel");
        dialogSubtitleLabel->setWordWrap(true);

        mainLayout->addWidget(dialogSubtitleLabel);

        contentCard = new QFrame(CourseEditorDialog);
        contentCard->setObjectName("contentCard");
        contentLayout = new QVBoxLayout(contentCard);
        contentLayout->setSpacing(10);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(18, 18, 18, 18);
        semesterLabel = new QLabel(contentCard);
        semesterLabel->setObjectName("semesterLabel");

        contentLayout->addWidget(semesterLabel);

        semesterComboBox = new QComboBox(contentCard);
        semesterComboBox->setObjectName("semesterComboBox");

        contentLayout->addWidget(semesterComboBox);

        courseCodeLabel = new QLabel(contentCard);
        courseCodeLabel->setObjectName("courseCodeLabel");

        contentLayout->addWidget(courseCodeLabel);

        courseCodeEdit = new QLineEdit(contentCard);
        courseCodeEdit->setObjectName("courseCodeEdit");
        courseCodeEdit->setMaxLength(30);

        contentLayout->addWidget(courseCodeEdit);

        courseNameLabel = new QLabel(contentCard);
        courseNameLabel->setObjectName("courseNameLabel");

        contentLayout->addWidget(courseNameLabel);

        courseNameEdit = new QLineEdit(contentCard);
        courseNameEdit->setObjectName("courseNameEdit");
        courseNameEdit->setMaxLength(120);

        contentLayout->addWidget(courseNameEdit);

        creditsLabel = new QLabel(contentCard);
        creditsLabel->setObjectName("creditsLabel");

        contentLayout->addWidget(creditsLabel);

        creditsSpinBox = new QSpinBox(contentCard);
        creditsSpinBox->setObjectName("creditsSpinBox");
        creditsSpinBox->setMinimum(1);
        creditsSpinBox->setMaximum(30);
        creditsSpinBox->setValue(3);

        contentLayout->addWidget(creditsSpinBox);

        finalGradeLabel = new QLabel(contentCard);
        finalGradeLabel->setObjectName("finalGradeLabel");

        contentLayout->addWidget(finalGradeLabel);

        finalGradeComboBox = new QComboBox(contentCard);
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->addItem(QString());
        finalGradeComboBox->setObjectName("finalGradeComboBox");

        contentLayout->addWidget(finalGradeComboBox);

        retakeFrame = new QFrame(contentCard);
        retakeFrame->setObjectName("retakeFrame");
        retakeLayout = new QVBoxLayout(retakeFrame);
        retakeLayout->setSpacing(5);
        retakeLayout->setObjectName("retakeLayout");
        retakeLayout->setContentsMargins(12, 10, 12, 10);
        retakeCheckBox = new QCheckBox(retakeFrame);
        retakeCheckBox->setObjectName("retakeCheckBox");

        retakeLayout->addWidget(retakeCheckBox);

        retakeHelpLabel = new QLabel(retakeFrame);
        retakeHelpLabel->setObjectName("retakeHelpLabel");
        retakeHelpLabel->setWordWrap(true);

        retakeLayout->addWidget(retakeHelpLabel);


        contentLayout->addWidget(retakeFrame);

        editNoticeLabel = new QLabel(contentCard);
        editNoticeLabel->setObjectName("editNoticeLabel");
        editNoticeLabel->setWordWrap(true);

        contentLayout->addWidget(editNoticeLabel);

        validationLabel = new QLabel(contentCard);
        validationLabel->setObjectName("validationLabel");
        validationLabel->setWordWrap(true);

        contentLayout->addWidget(validationLabel);


        mainLayout->addWidget(contentCard);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        mainLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(CourseEditorDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Save);

        mainLayout->addWidget(buttonBox);


        retranslateUi(CourseEditorDialog);

        QMetaObject::connectSlotsByName(CourseEditorDialog);
    } // setupUi

    void retranslateUi(QDialog *CourseEditorDialog)
    {
        CourseEditorDialog->setWindowTitle(QCoreApplication::translate("CourseEditorDialog", "Course", nullptr));
        dialogTitleLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Course", nullptr));
        dialogSubtitleLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Enter course details.", nullptr));
        semesterLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Semester", nullptr));
        courseCodeLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Course code", nullptr));
        courseCodeEdit->setPlaceholderText(QCoreApplication::translate("CourseEditorDialog", "Example: MATH201", nullptr));
        courseNameLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Course name", nullptr));
        courseNameEdit->setPlaceholderText(QCoreApplication::translate("CourseEditorDialog", "Example: Calculus II", nullptr));
        creditsLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Credit hours", nullptr));
        finalGradeLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Final letter grade (optional)", nullptr));
        finalGradeComboBox->setItemText(0, QCoreApplication::translate("CourseEditorDialog", "No final grade yet", nullptr));
        finalGradeComboBox->setItemText(1, QCoreApplication::translate("CourseEditorDialog", "A", nullptr));
        finalGradeComboBox->setItemText(2, QCoreApplication::translate("CourseEditorDialog", "A-", nullptr));
        finalGradeComboBox->setItemText(3, QCoreApplication::translate("CourseEditorDialog", "B+", nullptr));
        finalGradeComboBox->setItemText(4, QCoreApplication::translate("CourseEditorDialog", "B", nullptr));
        finalGradeComboBox->setItemText(5, QCoreApplication::translate("CourseEditorDialog", "B-", nullptr));
        finalGradeComboBox->setItemText(6, QCoreApplication::translate("CourseEditorDialog", "C+", nullptr));
        finalGradeComboBox->setItemText(7, QCoreApplication::translate("CourseEditorDialog", "C", nullptr));
        finalGradeComboBox->setItemText(8, QCoreApplication::translate("CourseEditorDialog", "C-", nullptr));
        finalGradeComboBox->setItemText(9, QCoreApplication::translate("CourseEditorDialog", "D+", nullptr));
        finalGradeComboBox->setItemText(10, QCoreApplication::translate("CourseEditorDialog", "D", nullptr));
        finalGradeComboBox->setItemText(11, QCoreApplication::translate("CourseEditorDialog", "F", nullptr));

        retakeCheckBox->setText(QCoreApplication::translate("CourseEditorDialog", "This is a retake of an existing course", nullptr));
        retakeHelpLabel->setText(QCoreApplication::translate("CourseEditorDialog", "The previous attempt, assignments, and grade will be replaced by this new attempt.", nullptr));
        editNoticeLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Existing assignments and grades will be kept.", nullptr));
        validationLabel->setText(QCoreApplication::translate("CourseEditorDialog", "Validation message", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CourseEditorDialog: public Ui_CourseEditorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COURSEEDITORDIALOG_H
