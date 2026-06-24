/********************************************************************************
** Form generated from reading UI file 'assignmenteditordialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ASSIGNMENTEDITORDIALOG_H
#define UI_ASSIGNMENTEDITORDIALOG_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AssignmentEditorDialog
{
public:
    QVBoxLayout *mainLayout;
    QLabel *dialogTitleLabel;
    QLabel *dialogSubtitleLabel;
    QFrame *contentCard;
    QVBoxLayout *contentLayout;
    QLabel *courseLabel;
    QLabel *courseValueLabel;
    QLabel *assignmentNameLabel;
    QLineEdit *assignmentNameEdit;
    QLabel *weightLabel;
    QSpinBox *weightSpinBox;
    QFrame *gradeFrame;
    QVBoxLayout *gradeLayout;
    QCheckBox *hasGradeCheckBox;
    QLabel *gradeLabel;
    QDoubleSpinBox *gradeSpinBox;
    QFrame *dueDateFrame;
    QVBoxLayout *dueDateLayout;
    QCheckBox *hasDueDateCheckBox;
    QLabel *dueDateLabel;
    QDateEdit *dueDateEdit;
    QLabel *validationLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AssignmentEditorDialog)
    {
        if (AssignmentEditorDialog->objectName().isEmpty())
            AssignmentEditorDialog->setObjectName("AssignmentEditorDialog");
        AssignmentEditorDialog->resize(560, 650);
        AssignmentEditorDialog->setMinimumSize(QSize(520, 0));
        AssignmentEditorDialog->setModal(true);
        AssignmentEditorDialog->setStyleSheet(QString::fromUtf8("QDialog#AssignmentEditorDialog {\n"
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
"QLabel#courseLabel,\n"
"QLabel#assignmentNameLabel,\n"
"QLabel#weightLabel,\n"
"QLabel#gradeLabel,\n"
"QLabel#dueDateLabel {\n"
"    color: #334155;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"QLabel#courseValueLabel {\n"
"    min-height: 38px;\n"
"    padding: 0 10px;\n"
"    color: #1e3a8a;\n"
"    background-color: #eff6ff;\n"
"    border: 1px solid #bfdbfe;\n"
"    border-radius: 8px;\n"
"}\n"
"QLineEdit,\n"
"QSpinBox,\n"
"QDoubleSpinBox,\n"
"QDateEdit {\n"
"    min-height: 38px;\n"
"    padding: 0 10px;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    background-color"
                        ": #ffffff;\n"
"    color: #1f2937;\n"
"}\n"
"QLineEdit:focus,\n"
"QSpinBox:focus,\n"
"QDoubleSpinBox:focus,\n"
"QDateEdit:focus {\n"
"    border-color: #2563eb;\n"
"}\n"
"QLineEdit:disabled,\n"
"QSpinBox:disabled,\n"
"QDoubleSpinBox:disabled,\n"
"QDateEdit:disabled {\n"
"    background-color: #f1f5f9;\n"
"    color: #94a3b8;\n"
"}\n"
"QCheckBox {\n"
"    color: #334155;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    spacing: 8px;\n"
"}\n"
"QFrame#gradeFrame,\n"
"QFrame#dueDateFrame {\n"
"    background-color: #f8fafc;\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 9px;\n"
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
"QDialogButtonBox QPushButton[text=\"Save\"] {\n"
"    background-c"
                        "olor: #2563eb;\n"
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
        mainLayout = new QVBoxLayout(AssignmentEditorDialog);
        mainLayout->setSpacing(14);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(22, 22, 22, 22);
        dialogTitleLabel = new QLabel(AssignmentEditorDialog);
        dialogTitleLabel->setObjectName("dialogTitleLabel");

        mainLayout->addWidget(dialogTitleLabel);

        dialogSubtitleLabel = new QLabel(AssignmentEditorDialog);
        dialogSubtitleLabel->setObjectName("dialogSubtitleLabel");
        dialogSubtitleLabel->setWordWrap(true);

        mainLayout->addWidget(dialogSubtitleLabel);

        contentCard = new QFrame(AssignmentEditorDialog);
        contentCard->setObjectName("contentCard");
        contentLayout = new QVBoxLayout(contentCard);
        contentLayout->setSpacing(10);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(18, 18, 18, 18);
        courseLabel = new QLabel(contentCard);
        courseLabel->setObjectName("courseLabel");

        contentLayout->addWidget(courseLabel);

        courseValueLabel = new QLabel(contentCard);
        courseValueLabel->setObjectName("courseValueLabel");
        courseValueLabel->setAlignment(Qt::AlignmentFlag::AlignVCenter|Qt::AlignmentFlag::AlignLeft);

        contentLayout->addWidget(courseValueLabel);

        assignmentNameLabel = new QLabel(contentCard);
        assignmentNameLabel->setObjectName("assignmentNameLabel");

        contentLayout->addWidget(assignmentNameLabel);

        assignmentNameEdit = new QLineEdit(contentCard);
        assignmentNameEdit->setObjectName("assignmentNameEdit");
        assignmentNameEdit->setMaxLength(120);

        contentLayout->addWidget(assignmentNameEdit);

        weightLabel = new QLabel(contentCard);
        weightLabel->setObjectName("weightLabel");

        contentLayout->addWidget(weightLabel);

        weightSpinBox = new QSpinBox(contentCard);
        weightSpinBox->setObjectName("weightSpinBox");
        weightSpinBox->setMinimum(1);
        weightSpinBox->setMaximum(100);
        weightSpinBox->setValue(10);

        contentLayout->addWidget(weightSpinBox);

        gradeFrame = new QFrame(contentCard);
        gradeFrame->setObjectName("gradeFrame");
        gradeLayout = new QVBoxLayout(gradeFrame);
        gradeLayout->setSpacing(7);
        gradeLayout->setObjectName("gradeLayout");
        gradeLayout->setContentsMargins(12, 10, 12, 10);
        hasGradeCheckBox = new QCheckBox(gradeFrame);
        hasGradeCheckBox->setObjectName("hasGradeCheckBox");

        gradeLayout->addWidget(hasGradeCheckBox);

        gradeLabel = new QLabel(gradeFrame);
        gradeLabel->setObjectName("gradeLabel");

        gradeLayout->addWidget(gradeLabel);

        gradeSpinBox = new QDoubleSpinBox(gradeFrame);
        gradeSpinBox->setObjectName("gradeSpinBox");
        gradeSpinBox->setDecimals(2);
        gradeSpinBox->setMinimum(0.000000000000000);
        gradeSpinBox->setMaximum(100.000000000000000);

        gradeLayout->addWidget(gradeSpinBox);


        contentLayout->addWidget(gradeFrame);

        dueDateFrame = new QFrame(contentCard);
        dueDateFrame->setObjectName("dueDateFrame");
        dueDateLayout = new QVBoxLayout(dueDateFrame);
        dueDateLayout->setSpacing(7);
        dueDateLayout->setObjectName("dueDateLayout");
        dueDateLayout->setContentsMargins(12, 10, 12, 10);
        hasDueDateCheckBox = new QCheckBox(dueDateFrame);
        hasDueDateCheckBox->setObjectName("hasDueDateCheckBox");

        dueDateLayout->addWidget(hasDueDateCheckBox);

        dueDateLabel = new QLabel(dueDateFrame);
        dueDateLabel->setObjectName("dueDateLabel");

        dueDateLayout->addWidget(dueDateLabel);

        dueDateEdit = new QDateEdit(dueDateFrame);
        dueDateEdit->setObjectName("dueDateEdit");
        dueDateEdit->setCalendarPopup(true);
        dueDateEdit->setMinimumDate(QDate(1900, 1, 1));
        dueDateEdit->setMaximumDate(QDate(2100, 12, 31));

        dueDateLayout->addWidget(dueDateEdit);


        contentLayout->addWidget(dueDateFrame);

        validationLabel = new QLabel(contentCard);
        validationLabel->setObjectName("validationLabel");
        validationLabel->setWordWrap(true);

        contentLayout->addWidget(validationLabel);


        mainLayout->addWidget(contentCard);

        verticalSpacer = new QSpacerItem(20, 12, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        mainLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(AssignmentEditorDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Save);

        mainLayout->addWidget(buttonBox);


        retranslateUi(AssignmentEditorDialog);

        QMetaObject::connectSlotsByName(AssignmentEditorDialog);
    } // setupUi

    void retranslateUi(QDialog *AssignmentEditorDialog)
    {
        AssignmentEditorDialog->setWindowTitle(QCoreApplication::translate("AssignmentEditorDialog", "Assignment", nullptr));
        dialogTitleLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Assignment", nullptr));
        dialogSubtitleLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Enter assignment details.", nullptr));
        courseLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Course", nullptr));
        courseValueLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Selected course", nullptr));
        assignmentNameLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Assignment name", nullptr));
        assignmentNameEdit->setPlaceholderText(QCoreApplication::translate("AssignmentEditorDialog", "Example: Midterm Exam", nullptr));
        weightLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Weight", nullptr));
        weightSpinBox->setSuffix(QCoreApplication::translate("AssignmentEditorDialog", "%", nullptr));
        hasGradeCheckBox->setText(QCoreApplication::translate("AssignmentEditorDialog", "The grade has been released", nullptr));
        gradeLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Grade percentage", nullptr));
        gradeSpinBox->setSuffix(QCoreApplication::translate("AssignmentEditorDialog", "%", nullptr));
        hasDueDateCheckBox->setText(QCoreApplication::translate("AssignmentEditorDialog", "This assignment has a due date", nullptr));
        dueDateLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Due date", nullptr));
        dueDateEdit->setDisplayFormat(QCoreApplication::translate("AssignmentEditorDialog", "dd MMM yyyy", nullptr));
        validationLabel->setText(QCoreApplication::translate("AssignmentEditorDialog", "Validation message", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AssignmentEditorDialog: public Ui_AssignmentEditorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ASSIGNMENTEDITORDIALOG_H
