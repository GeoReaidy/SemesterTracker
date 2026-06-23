#include "semesterswindow.h"
#include "ui_semesterswindow.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QToolButton>

#include <exception>


namespace
{
bool semesterAlreadyExists(
    DatabaseManager &database,
    int userID,
    const QString &term,
    int year,
    int ignoredSemesterID = -1)
{
    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    const QString normalizedTerm =
        term.trimmed().toCaseFolded();

    for (const Semester &semester : semesters)
    {
        if (semester.getID() == ignoredSemesterID)
        {
            continue;
        }

        const QString existingTerm =
            QString::fromStdString(
                semester.getName()
            ).trimmed().toCaseFolded();

        if (existingTerm == normalizedTerm &&
            semester.getYear() == year)
        {
            return true;
        }
    }

    return false;
}
}

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
            (selectedFilter == "Current" && semester.isInProgress()) ||
            (selectedFilter == "Upcoming" && !semester.isInProgress());

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
            "No User",
            "No logged-in user is available."
        );
        return;
    }

    const QStringList terms{
        "Spring",
        "Summer",
        "Fall"
    };

    bool termAccepted = false;

    const QString term = QInputDialog::getItem(
        this,
        "Add Semester",
        "Semester term:",
        terms,
        0,
        false,
        &termAccepted
    );

    if (!termAccepted)
    {
        return;
    }

    bool yearAccepted = false;

    const int year = QInputDialog::getInt(
        this,
        "Add Semester",
        "Year:",
        2026,
        1900,
        2100,
        1,
        &yearAccepted
    );

    if (!yearAccepted)
    {
        return;
    }

    const QMessageBox::StandardButton completedAnswer =
        QMessageBox::question(
            this,
            "Semester Status",
            "Is this semester already completed?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    const bool isCompleted =
        completedAnswer == QMessageBox::Yes;

    int completedCredits = 0;
    double completedGPA = 0.0;
    bool isCurrent = false;

    if (isCompleted)
    {
        bool creditsAccepted = false;
        completedCredits = QInputDialog::getInt(
            this,
            "Completed Semester",
            "Total completed credits:",
            15,
            1,
            300,
            1,
            &creditsAccepted
        );

        if (!creditsAccepted)
        {
            return;
        }

        bool gpaAccepted = false;
        completedGPA = QInputDialog::getDouble(
            this,
            "Completed Semester",
            "Semester GPA:",
            3.00,
            0.00,
            4.00,
            2,
            &gpaAccepted
        );

        if (!gpaAccepted)
        {
            return;
        }
    }
    else
    {
        const QMessageBox::StandardButton currentAnswer =
            QMessageBox::question(
                this,
                "Current Semester",
                "Set this as the current semester?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );

        isCurrent = currentAnswer == QMessageBox::Yes;
    }

    try
    {
        const Semester candidate(
            -1,
            term.toStdString(),
            year,
            isCurrent,
            isCompleted,
            completedCredits,
            completedGPA
        );

        Q_UNUSED(candidate);
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            "Invalid Semester",
            error.what()
        );
        return;
    }

    if (semesterAlreadyExists(
            database,
            userID,
            term,
            year))
    {
        QMessageBox::warning(
            this,
            "Duplicate Semester",
            "This semester already exists."
        );
        return;
    }

    if (isCurrent &&
        !database.clearCurrentSemesterForUser(userID))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not clear the previous current semester."
        );
        return;
    }

    const bool saved = isCompleted
        ? database.addCompletedSemester(
              userID,
              term.toStdString(),
              year,
              completedCredits,
              completedGPA)
        : database.addSemester(
              userID,
              term.toStdString(),
              year,
              isCurrent);

    if (!saved)
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not save the semester."
        );
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

    const int semesterID =
        item->data(Qt::UserRole).toInt();

    const QString currentTerm =
        item->data(Qt::UserRole + 1).toString();

    const int currentYear =
        item->data(Qt::UserRole + 2).toInt();

    const bool currentlyCurrent =
        item->data(Qt::UserRole + 3).toBool();

    const bool summaryOnly =
        item->data(Qt::UserRole + 4).toBool();

    const QStringList terms{
        "Spring",
        "Summer",
        "Fall"
    };

    bool termAccepted = false;

    const int currentTermIndex =
        terms.contains(currentTerm)
            ? terms.indexOf(currentTerm)
            : 0;

    const QString newTerm = QInputDialog::getItem(
        this,
        "Edit Semester",
        "Semester term:",
        terms,
        currentTermIndex,
        false,
        &termAccepted
    );

    if (!termAccepted)
    {
        return;
    }

    bool yearAccepted = false;

    const int newYear = QInputDialog::getInt(
        this,
        "Edit Semester",
        "Year:",
        currentYear,
        1900,
        2100,
        1,
        &yearAccepted
    );

    if (!yearAccepted)
    {
        return;
    }

    bool newIsCurrent = false;

    if (!summaryOnly)
    {
        const QMessageBox::StandardButton currentAnswer =
            QMessageBox::question(
                this,
                "Current Semester",
                "Set this as the current semester?",
                QMessageBox::Yes | QMessageBox::No,
                currentlyCurrent
                    ? QMessageBox::Yes
                    : QMessageBox::No
            );

        newIsCurrent = currentAnswer == QMessageBox::Yes;
    }

    try
    {
        const Semester candidate(
            semesterID,
            newTerm.toStdString(),
            newYear,
            newIsCurrent
        );

        Q_UNUSED(candidate);
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            "Invalid Semester",
            error.what()
        );
        return;
    }

    if (semesterAlreadyExists(
            database,
            userID,
            newTerm,
            newYear,
            semesterID))
    {
        QMessageBox::warning(
            this,
            "Duplicate Semester",
            "Another semester with this term and year already exists."
        );
        return;
    }

    if (newIsCurrent &&
        !database.clearCurrentSemesterForUser(userID))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not clear the previous current semester."
        );
        return;
    }

    if (!database.updateSemester(
            semesterID,
            newTerm.toStdString(),
            newYear,
            newIsCurrent))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not update the semester."
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
