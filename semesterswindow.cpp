#include "semesterswindow.h"
#include "semestereditordialog.h"
#include "ui_semesterswindow.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>

#include <exception>



SemestersWindow::SemestersWindow(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      userID(userID),
      ui(new Ui::SemestersWindow)
{
    ui->setupUi(this);

    ui->semestersListWidget->setStyleSheet(R"(
        QListWidget {
            background-color: #ffffff;
            color: #1f2937;
            border: none;
        }

        QListWidget::item {
            background-color: #ffffff;
            color: #1f2937;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            padding: 10px;
            margin: 4px;
        }

        QListWidget::item:selected {
            background-color: #eff6ff;
            color: #1f2937;
        }
    )");

    ui->semesterFilterComboBox->setStyleSheet(R"(
        QComboBox {
            background-color: #ffffff;
            color: #1f2937;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            padding: 6px 10px;
        }

        QComboBox QAbstractItemView {
            background-color: #ffffff;
            color: #1f2937;
            selection-background-color: #dbeafe;
            selection-color: #1f2937;
        }
    )");

    connect(
        ui->addSemesterButton,
        &QPushButton::clicked,
        this,
        &SemestersWindow::handleAddSemester
    );

    connect(
        ui->semesterFilterComboBox,
        &QComboBox::currentIndexChanged,
        this,
        &SemestersWindow::handleFilterChanged
    );

    refreshSemesters();
}

SemestersWindow::~SemestersWindow()
{
    delete ui;
}

void SemestersWindow::setUserID(int newUserID)
{
    userID = newUserID;
    refreshSemesters();
}

void SemestersWindow::refreshSemesters()
{
    ui->semestersListWidget->clear();

    if (userID < 0)
    {
        updateEmptyState();
        return;
    }

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    const QString selectedFilter =
        ui->semesterFilterComboBox->currentText();

    for (const Semester &semester : semesters)
    {
        const bool showSemester =
            selectedFilter == "All" ||
            (selectedFilter == "Current" &&
             semester.isInProgress()) ||
            (selectedFilter == "Completed" &&
             semester.isSummaryOnly()) ||
            (selectedFilter == "Upcoming" &&
             !semester.isInProgress() &&
             !semester.isSummaryOnly());

        if (showSemester)
        {
            addSemesterRow(semester);
        }
    }

    updateEmptyState();
}

void SemestersWindow::handleAddSemester()
{
    if (userID < 0)
    {
        QMessageBox::warning(
            this,
            tr("No User"),
            tr("No logged-in user is available.")
        );
        return;
    }

    SemesterEditorDialog dialog(
        database,
        userID,
        this
    );

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    refreshSemesters();
    emit semestersChanged();
}

void SemestersWindow::handleFilterChanged(int index)
{
    Q_UNUSED(index);
    refreshSemesters();
}

void SemestersWindow::updateEmptyState()
{
    const bool hasSemesters =
        ui->semestersListWidget->count() > 0;

    ui->emptyStateLabel->setVisible(!hasSemesters);
    ui->semestersListWidget->setVisible(hasSemesters);
}

void SemestersWindow::addSemesterRow(
    const Semester &semester)
{
    auto *item = new QListWidgetItem(
        ui->semestersListWidget
    );

    item->setData(Qt::UserRole, semester.getID());
    item->setData(
        Qt::UserRole + 1,
        QString::fromStdString(semester.getName())
    );
    item->setData(Qt::UserRole + 2, semester.getYear());
    item->setData(
        Qt::UserRole + 3,
        semester.isInProgress()
    );
    item->setData(Qt::UserRole + 4, semester.isSummaryOnly());
    item->setData(Qt::UserRole + 5, semester.getSummaryCredits());
    item->setData(Qt::UserRole + 6, semester.getSummaryGPA());

    auto *rowWidget = new QWidget(
        ui->semestersListWidget
    );

    auto *layout = new QHBoxLayout(rowWidget);
    layout->setContentsMargins(14, 0, 10, 0);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignVCenter);

    const QString term =
        QString::fromStdString(semester.getName());

    auto *semesterLabel = new QLabel(
        QString("%1 %2")
            .arg(term)
            .arg(semester.getYear()),
        rowWidget
    );

    semesterLabel->setStyleSheet(
        "color: #111827;"
        "font-size: 15px;"
        "font-weight: 600;"
    );

    semesterLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    const QString statusText = semester.isSummaryOnly()
        ? QString("Completed · %1 cr · %2 GPA")
              .arg(semester.getSummaryCredits())
              .arg(semester.getSummaryGPA(), 0, 'f', 2)
        : (semester.isInProgress() ? "Current" : "Upcoming");

    auto *statusLabel = new QLabel(statusText, rowWidget);

    statusLabel->setStyleSheet(
        semester.isInProgress()
            ? "color: #16a34a;"
              "background-color: #dcfce7;"
              "border-radius: 8px;"
              "padding: 4px 9px;"
              "font-weight: 600;"
            : semester.isSummaryOnly()
                ? "color: #1d4ed8;"
                  "background-color: #dbeafe;"
                  "border-radius: 8px;"
                  "padding: 4px 9px;"
                  "font-weight: 600;"
                : "color: #64748b;"
                  "background-color: #f1f5f9;"
                  "border-radius: 8px;"
                  "padding: 4px 9px;"
    );

    statusLabel->setAlignment(Qt::AlignCenter);

    auto *editButton = new QToolButton(rowWidget);
    editButton->setText("✎");
    editButton->setToolTip("Edit semester");
    editButton->setCursor(Qt::PointingHandCursor);
    editButton->setStyleSheet(R"(
        QToolButton {
            border: none;
            border-radius: 7px;
            padding: 6px;
            color: #2563eb;
            background-color: #eff6ff;
            font-size: 16px;
        }

        QToolButton:hover {
            background-color: #dbeafe;
        }
    )");

    auto *deleteButton = new QToolButton(rowWidget);
    deleteButton->setText("🗑");
    deleteButton->setToolTip("Delete semester");
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setStyleSheet(R"(
        QToolButton {
            border: none;
            border-radius: 7px;
            padding: 6px;
            color: #dc2626;
            background-color: #fef2f2;
            font-size: 15px;
        }

        QToolButton:hover {
            background-color: #fee2e2;
        }
    )");

    layout->addWidget(
        semesterLabel,
        0,
        Qt::AlignVCenter
    );

    layout->addStretch();

    layout->addWidget(
        statusLabel,
        0,
        Qt::AlignVCenter
    );

    layout->addWidget(
        editButton,
        0,
        Qt::AlignVCenter
    );

    layout->addWidget(
        deleteButton,
        0,
        Qt::AlignVCenter
    );

    item->setSizeHint(QSize(0, 64));

    ui->semestersListWidget->setItemWidget(
        item,
        rowWidget
    );

    connect(
        editButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            editSemesterRow(item);
        }
    );

    connect(
        deleteButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            deleteSemesterRow(item);
        }
    );
}

void SemestersWindow::editSemesterRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    try
    {
        const Semester semester(
            item->data(Qt::UserRole).toInt(),
            item->data(Qt::UserRole + 1)
                .toString()
                .toStdString(),
            item->data(Qt::UserRole + 2).toInt(),
            item->data(Qt::UserRole + 3).toBool(),
            item->data(Qt::UserRole + 4).toBool(),
            item->data(Qt::UserRole + 5).toInt(),
            item->data(Qt::UserRole + 6).toDouble()
        );

        SemesterEditorDialog dialog(
            database,
            userID,
            semester,
            this
        );

        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            tr("Invalid Semester"),
            QString::fromUtf8(error.what())
        );
        return;
    }

    refreshSemesters();
    emit semestersChanged();
}

void SemestersWindow::deleteSemesterRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    const int semesterID =
        item->data(Qt::UserRole).toInt();

    const QString term =
        item->data(Qt::UserRole + 1).toString();

    const int year =
        item->data(Qt::UserRole + 2).toInt();

    const QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Delete Semester",
            QString("Delete %1 %2?")
                .arg(term)
                .arg(year),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (answer != QMessageBox::Yes)
    {
        return;
    }

    if (!database.deleteSemester(semesterID))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not delete the semester."
        );
        return;
    }

    refreshSemesters();
    emit semestersChanged();
}
