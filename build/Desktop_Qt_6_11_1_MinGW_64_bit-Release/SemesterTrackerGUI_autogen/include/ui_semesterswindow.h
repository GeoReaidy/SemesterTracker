/********************************************************************************
** Form generated from reading UI file 'semesterswindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEMESTERSWINDOW_H
#define UI_SEMESTERSWINDOW_H

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

class Ui_SemestersWindow
{
public:
    QVBoxLayout *mainLayout;
    QFrame *headerCard;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleLayout;
    QLabel *semestersTitleLabel;
    QLabel *semestersSubtitleLabel;
    QSpacerItem *headerSpacer;
    QPushButton *addSemesterButton;
    QFrame *semestersCard;
    QVBoxLayout *semestersCardLayout;
    QHBoxLayout *filterLayout;
    QLabel *allSemestersLabel;
    QSpacerItem *filterSpacer;
    QComboBox *semesterFilterComboBox;
    QListWidget *semestersListWidget;
    QLabel *emptyStateLabel;

    void setupUi(QWidget *SemestersWindow)
    {
        if (SemestersWindow->objectName().isEmpty())
            SemestersWindow->setObjectName("SemestersWindow");
        SemestersWindow->resize(900, 620);
        SemestersWindow->setMinimumSize(QSize(760, 520));
        SemestersWindow->setStyleSheet(QString::fromUtf8("QWidget#SemestersWindow {\n"
"    background-color: #f5f7fb;\n"
"}\n"
"QFrame#headerCard,\n"
"QFrame#semestersCard {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 12px;\n"
"}\n"
"QLabel#semestersTitleLabel {\n"
"    color: #111827;\n"
"    font-size: 25px;\n"
"    font-weight: 700;\n"
"}\n"
"QLabel#semestersSubtitleLabel {\n"
"    color: #64748b;\n"
"    font-size: 13px;\n"
"}\n"
"QPushButton#addSemesterButton {\n"
"    min-height: 40px;\n"
"    padding: 0 18px;\n"
"    border: none;\n"
"    border-radius: 9px;\n"
"    background-color: #2563eb;\n"
"    color: #ffffff;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"}\n"
"QPushButton#addSemesterButton:hover { background-color: #1d4ed8; }\n"
"QPushButton#addSemesterButton:pressed { background-color: #1e40af; }\n"
"QComboBox#semesterFilterComboBox {\n"
"    min-height: 36px;\n"
"    padding: 0 12px;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    background-color: #ffffff;\n"
"    color"
                        ": #334155;\n"
"    font-size: 13px;\n"
"}\n"
"QListWidget#semestersListWidget {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    outline: none;\n"
"    color: #1f2937;\n"
"    font-size: 14px;\n"
"}\n"
"QListWidget#semestersListWidget::item {\n"
"    margin: 4px 0;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"}\n"
"QListWidget#semestersListWidget::item:hover {\n"
"    background-color: transparent;\n"
"}\n"
"QListWidget#semestersListWidget::item:selected {\n"
"    background-color: transparent;\n"
"    color: #1f2937;\n"
"}\n"
"QLabel#emptyStateLabel {\n"
"    color: #94a3b8;\n"
"    font-size: 14px;\n"
"}"));
        mainLayout = new QVBoxLayout(SemestersWindow);
        mainLayout->setSpacing(16);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(24, 24, 24, 24);
        headerCard = new QFrame(SemestersWindow);
        headerCard->setObjectName("headerCard");
        headerCard->setMinimumSize(QSize(0, 96));
        headerCard->setMaximumSize(QSize(16777215, 96));
        headerLayout = new QHBoxLayout(headerCard);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(22, 16, 22, 16);
        titleLayout = new QVBoxLayout();
        titleLayout->setSpacing(3);
        titleLayout->setObjectName("titleLayout");
        semestersTitleLabel = new QLabel(headerCard);
        semestersTitleLabel->setObjectName("semestersTitleLabel");

        titleLayout->addWidget(semestersTitleLabel);

        semestersSubtitleLabel = new QLabel(headerCard);
        semestersSubtitleLabel->setObjectName("semestersSubtitleLabel");

        titleLayout->addWidget(semestersSubtitleLabel);


        headerLayout->addLayout(titleLayout);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        addSemesterButton = new QPushButton(headerCard);
        addSemesterButton->setObjectName("addSemesterButton");
        addSemesterButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        headerLayout->addWidget(addSemesterButton);


        mainLayout->addWidget(headerCard);

        semestersCard = new QFrame(SemestersWindow);
        semestersCard->setObjectName("semestersCard");
        semestersCardLayout = new QVBoxLayout(semestersCard);
        semestersCardLayout->setSpacing(10);
        semestersCardLayout->setObjectName("semestersCardLayout");
        semestersCardLayout->setContentsMargins(18, 16, 18, 18);
        filterLayout = new QHBoxLayout();
        filterLayout->setObjectName("filterLayout");
        allSemestersLabel = new QLabel(semestersCard);
        allSemestersLabel->setObjectName("allSemestersLabel");
        allSemestersLabel->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 16px; font-weight: 700;"));

        filterLayout->addWidget(allSemestersLabel);

        filterSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        filterLayout->addItem(filterSpacer);

        semesterFilterComboBox = new QComboBox(semestersCard);
        semesterFilterComboBox->addItem(QString());
        semesterFilterComboBox->addItem(QString());
        semesterFilterComboBox->addItem(QString());
        semesterFilterComboBox->addItem(QString());
        semesterFilterComboBox->setObjectName("semesterFilterComboBox");

        filterLayout->addWidget(semesterFilterComboBox);


        semestersCardLayout->addLayout(filterLayout);

        semestersListWidget = new QListWidget(semestersCard);
        semestersListWidget->setObjectName("semestersListWidget");

        semestersCardLayout->addWidget(semestersListWidget);

        emptyStateLabel = new QLabel(semestersCard);
        emptyStateLabel->setObjectName("emptyStateLabel");
        emptyStateLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        semestersCardLayout->addWidget(emptyStateLabel);


        mainLayout->addWidget(semestersCard);


        retranslateUi(SemestersWindow);

        QMetaObject::connectSlotsByName(SemestersWindow);
    } // setupUi

    void retranslateUi(QWidget *SemestersWindow)
    {
        SemestersWindow->setWindowTitle(QCoreApplication::translate("SemestersWindow", "Semesters", nullptr));
        semestersTitleLabel->setText(QCoreApplication::translate("SemestersWindow", "Semesters", nullptr));
        semestersSubtitleLabel->setText(QCoreApplication::translate("SemestersWindow", "Manage your academic terms and choose the current semester.", nullptr));
        addSemesterButton->setText(QCoreApplication::translate("SemestersWindow", "+ Add Semester", nullptr));
        allSemestersLabel->setText(QCoreApplication::translate("SemestersWindow", "All Semesters", nullptr));
        semesterFilterComboBox->setItemText(0, QCoreApplication::translate("SemestersWindow", "All", nullptr));
        semesterFilterComboBox->setItemText(1, QCoreApplication::translate("SemestersWindow", "Current", nullptr));
        semesterFilterComboBox->setItemText(2, QCoreApplication::translate("SemestersWindow", "Completed", nullptr));
        semesterFilterComboBox->setItemText(3, QCoreApplication::translate("SemestersWindow", "Upcoming", nullptr));

        emptyStateLabel->setText(QCoreApplication::translate("SemestersWindow", "No semesters yet. Click \342\200\234Add Semester\342\200\235 to create one.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SemestersWindow: public Ui_SemestersWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEMESTERSWINDOW_H
