/********************************************************************************
** Form generated from reading UI file 'semestereditordialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEMESTEREDITORDIALOG_H
#define UI_SEMESTEREDITORDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SemesterEditorDialog
{
public:
    QVBoxLayout *mainLayout;
    QLabel *dialogTitleLabel;
    QLabel *dialogSubtitleLabel;
    QFrame *contentCard;
    QVBoxLayout *contentLayout;
    QLabel *termLabel;
    QComboBox *termComboBox;
    QLabel *yearLabel;
    QSpinBox *yearSpinBox;
    QCheckBox *completedCheckBox;
    QFrame *completedFieldsFrame;
    QFormLayout *completedFieldsLayout;
    QLabel *completedCreditsLabel;
    QSpinBox *completedCreditsSpinBox;
    QLabel *semesterGpaLabel;
    QDoubleSpinBox *semesterGpaSpinBox;
    QCheckBox *currentCheckBox;
    QLabel *conversionNoticeLabel;
    QLabel *validationLabel;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SemesterEditorDialog)
    {
        if (SemesterEditorDialog->objectName().isEmpty())
            SemesterEditorDialog->setObjectName("SemesterEditorDialog");
        SemesterEditorDialog->resize(520, 520);
        SemesterEditorDialog->setMinimumSize(QSize(500, 0));
        SemesterEditorDialog->setModal(true);
        mainLayout = new QVBoxLayout(SemesterEditorDialog);
        mainLayout->setSpacing(14);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(22, 22, 22, 22);
        dialogTitleLabel = new QLabel(SemesterEditorDialog);
        dialogTitleLabel->setObjectName("dialogTitleLabel");

        mainLayout->addWidget(dialogTitleLabel);

        dialogSubtitleLabel = new QLabel(SemesterEditorDialog);
        dialogSubtitleLabel->setObjectName("dialogSubtitleLabel");
        dialogSubtitleLabel->setWordWrap(true);

        mainLayout->addWidget(dialogSubtitleLabel);

        contentCard = new QFrame(SemesterEditorDialog);
        contentCard->setObjectName("contentCard");
        contentLayout = new QVBoxLayout(contentCard);
        contentLayout->setSpacing(12);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(18, 18, 18, 18);
        termLabel = new QLabel(contentCard);
        termLabel->setObjectName("termLabel");

        contentLayout->addWidget(termLabel);

        termComboBox = new QComboBox(contentCard);
        termComboBox->addItem(QString());
        termComboBox->addItem(QString());
        termComboBox->addItem(QString());
        termComboBox->setObjectName("termComboBox");

        contentLayout->addWidget(termComboBox);

        yearLabel = new QLabel(contentCard);
        yearLabel->setObjectName("yearLabel");

        contentLayout->addWidget(yearLabel);

        yearSpinBox = new QSpinBox(contentCard);
        yearSpinBox->setObjectName("yearSpinBox");
        yearSpinBox->setMinimum(1900);
        yearSpinBox->setMaximum(2100);

        contentLayout->addWidget(yearSpinBox);

        completedCheckBox = new QCheckBox(contentCard);
        completedCheckBox->setObjectName("completedCheckBox");

        contentLayout->addWidget(completedCheckBox);

        completedFieldsFrame = new QFrame(contentCard);
        completedFieldsFrame->setObjectName("completedFieldsFrame");
        completedFieldsLayout = new QFormLayout(completedFieldsFrame);
        completedFieldsLayout->setObjectName("completedFieldsLayout");
        completedFieldsLayout->setHorizontalSpacing(12);
        completedFieldsLayout->setVerticalSpacing(10);
        completedFieldsLayout->setContentsMargins(12, 12, 12, 12);
        completedCreditsLabel = new QLabel(completedFieldsFrame);
        completedCreditsLabel->setObjectName("completedCreditsLabel");

        completedFieldsLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, completedCreditsLabel);

        completedCreditsSpinBox = new QSpinBox(completedFieldsFrame);
        completedCreditsSpinBox->setObjectName("completedCreditsSpinBox");
        completedCreditsSpinBox->setMinimum(1);
        completedCreditsSpinBox->setMaximum(300);

        completedFieldsLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, completedCreditsSpinBox);

        semesterGpaLabel = new QLabel(completedFieldsFrame);
        semesterGpaLabel->setObjectName("semesterGpaLabel");

        completedFieldsLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, semesterGpaLabel);

        semesterGpaSpinBox = new QDoubleSpinBox(completedFieldsFrame);
        semesterGpaSpinBox->setObjectName("semesterGpaSpinBox");
        semesterGpaSpinBox->setDecimals(2);
        semesterGpaSpinBox->setMinimum(0.000000000000000);
        semesterGpaSpinBox->setMaximum(4.000000000000000);
        semesterGpaSpinBox->setSingleStep(0.050000000000000);

        completedFieldsLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, semesterGpaSpinBox);


        contentLayout->addWidget(completedFieldsFrame);

        currentCheckBox = new QCheckBox(contentCard);
        currentCheckBox->setObjectName("currentCheckBox");

        contentLayout->addWidget(currentCheckBox);

        conversionNoticeLabel = new QLabel(contentCard);
        conversionNoticeLabel->setObjectName("conversionNoticeLabel");
        conversionNoticeLabel->setWordWrap(true);

        contentLayout->addWidget(conversionNoticeLabel);

        validationLabel = new QLabel(contentCard);
        validationLabel->setObjectName("validationLabel");
        validationLabel->setWordWrap(true);

        contentLayout->addWidget(validationLabel);


        mainLayout->addWidget(contentCard);

        buttonBox = new QDialogButtonBox(SemesterEditorDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Save);

        mainLayout->addWidget(buttonBox);


        retranslateUi(SemesterEditorDialog);

        QMetaObject::connectSlotsByName(SemesterEditorDialog);
    } // setupUi

    void retranslateUi(QDialog *SemesterEditorDialog)
    {
        SemesterEditorDialog->setWindowTitle(QCoreApplication::translate("SemesterEditorDialog", "Semester", nullptr));
        dialogTitleLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Semester", nullptr));
        dialogSubtitleLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Enter semester details.", nullptr));
        termLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Semester term", nullptr));
        termComboBox->setItemText(0, QCoreApplication::translate("SemesterEditorDialog", "Spring", nullptr));
        termComboBox->setItemText(1, QCoreApplication::translate("SemesterEditorDialog", "Summer", nullptr));
        termComboBox->setItemText(2, QCoreApplication::translate("SemesterEditorDialog", "Fall", nullptr));

        yearLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Year", nullptr));
        completedCheckBox->setText(QCoreApplication::translate("SemesterEditorDialog", "This semester is already completed (summary only)", nullptr));
        completedCreditsLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Completed credits", nullptr));
        semesterGpaLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Semester GPA", nullptr));
        currentCheckBox->setText(QCoreApplication::translate("SemesterEditorDialog", "Set as the current semester", nullptr));
        conversionNoticeLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Notice", nullptr));
        validationLabel->setText(QCoreApplication::translate("SemesterEditorDialog", "Validation error", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SemesterEditorDialog: public Ui_SemesterEditorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEMESTEREDITORDIALOG_H
