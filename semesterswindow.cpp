#include "semesterswindow.h"
#include "semestereditordialog.h"
#include "ui_semesterswindow.h"

#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QColor>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QSize>
#include <QSizePolicy>
#include <QStringList>
#include <QToolButton>

#include <exception>

namespace
{
QIcon makePencilIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor("#2563eb"));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    painter.drawLine(6, 18, 17, 7);
    painter.drawLine(8, 20, 19, 9);
    painter.drawLine(6, 18, 8, 20);
    painter.drawLine(17, 7, 19, 9);
    painter.drawLine(5, 21, 9, 20);

    return QIcon(pixmap);
}

QIcon makeBinIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor("#dc2626"));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    painter.drawLine(6, 7, 18, 7);
    painter.drawLine(9, 4, 15, 4);
    painter.drawLine(10, 4, 10, 7);
    painter.drawLine(14, 4, 14, 7);
    painter.drawRoundedRect(7, 8, 10, 12, 2, 2);
    painter.drawLine(10, 11, 10, 17);
    painter.drawLine(14, 11, 14, 17);

    return QIcon(pixmap);
}

QString semesterStatusText(SemesterStatus status)
{
    switch (status)
    {
    case SemesterStatus::Planned:
        return QObject::tr("Planned");
    case SemesterStatus::Active:
        return QObject::tr("Active");
    case SemesterStatus::Completed:
        return QObject::tr("Completed");
    }

    return QObject::tr("Planned");
}

QString semesterStatusProperty(SemesterStatus status)
{
    switch (status)
    {
    case SemesterStatus::Planned:
        return QStringLiteral("planned");
    case SemesterStatus::Active:
        return QStringLiteral("active");
    case SemesterStatus::Completed:
        return QStringLiteral("completed");
    }

    return QStringLiteral("planned");
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

    setProperty("page", true);
    ui->headerCard->setProperty("card", true);
    ui->semestersCard->setProperty("card", true);
    ui->semestersTitleLabel->setProperty("role", "pageTitle");
    ui->semestersSubtitleLabel->setProperty("role", "pageSubtitle");
    ui->allSemestersLabel->setProperty("role", "sectionTitle");
    ui->emptyStateLabel->setProperty("role", "emptyState");
    ui->semestersListWidget->setProperty("cardList", true);
    ui->addSemesterButton->setProperty("role", "primary");

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
             semester.getStatus() == SemesterStatus::Active) ||
            (selectedFilter == "Completed" &&
             semester.getStatus() == SemesterStatus::Completed) ||
            (selectedFilter == "Upcoming" &&
             semester.getStatus() == SemesterStatus::Planned);

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
    item->setData(
        Qt::UserRole + 7,
        static_cast<int>(semester.getStatus())
    );

    auto *rowWidget = new QWidget(
        ui->semestersListWidget
    );
    rowWidget->setProperty("rowCard", true);
    rowWidget->setAttribute(Qt::WA_StyledBackground, true);
    rowWidget->setMinimumHeight(88);
    rowWidget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
    );

    auto *layout = new QHBoxLayout(rowWidget);
    layout->setContentsMargins(16, 14, 12, 14);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignVCenter);

    const QString term =
        QString::fromStdString(semester.getName());

    auto *semesterLabel = new QLabel(
        QString("%1 %2")
            .arg(term)
            .arg(semester.getYear()),
        rowWidget
    );

    semesterLabel->setProperty("role", "rowTitle");

    semesterLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    QLabel *summaryDetailsLabel = nullptr;

    if (semester.isSummaryOnly())
    {
        summaryDetailsLabel = new QLabel(
            QString("%1 cr · %2 GPA")
                .arg(semester.getSummaryCredits())
                .arg(semester.getSummaryGPA(), 0, 'f', 2),
            rowWidget
        );

        summaryDetailsLabel->setAlignment(Qt::AlignCenter);
        summaryDetailsLabel->setProperty("role", "summaryPill");
    }

    auto *statusButton = new QToolButton(rowWidget);
    statusButton->setText(
        semesterStatusText(semester.getStatus())
    );
    statusButton->setCursor(Qt::PointingHandCursor);
    statusButton->setToolButtonStyle(
        Qt::ToolButtonTextOnly
    );
    statusButton->setProperty(
        "status",
        semesterStatusProperty(semester.getStatus())
    );
    statusButton->setAccessibleName(
        tr("Semester status: %1")
            .arg(semesterStatusText(
                semester.getStatus()
            ))
    );

    if (semester.isSummaryOnly())
    {
        statusButton->setEnabled(false);
        statusButton->setToolTip(
            tr("Summary-only semesters are always completed")
        );
    }
    else
    {
        statusButton->setPopupMode(
            QToolButton::InstantPopup
        );
        statusButton->setToolTip(
            tr("Change semester status")
        );

        auto *statusMenu = new QMenu(statusButton);
        statusMenu->setMinimumWidth(165);

        auto *statusGroup =
            new QActionGroup(statusMenu);
        statusGroup->setExclusive(true);

        const struct StatusOption
        {
            SemesterStatus status;
            const char *label;
        } statusOptions[] = {
            {SemesterStatus::Planned, "Planned"},
            {SemesterStatus::Active, "Active"},
            {SemesterStatus::Completed, "Completed"}
        };

        for (const StatusOption &option : statusOptions)
        {
            QAction *action =
                statusMenu->addAction(
                    tr(option.label)
                );

            action->setCheckable(true);
            action->setChecked(
                option.status == semester.getStatus()
            );
            statusGroup->addAction(action);

            connect(
                action,
                &QAction::triggered,
                this,
                [this, item, option]()
                {
                    setSemesterStatus(
                        item,
                        option.status
                    );
                }
            );
        }

        statusButton->setMenu(statusMenu);
    }

    auto *editButton = new QToolButton(rowWidget);
    editButton->setIcon(makePencilIcon());
    editButton->setIconSize(QSize(20, 20));
    editButton->setToolTip("Edit semester");
    editButton->setAccessibleName("Edit semester");
    editButton->setCursor(Qt::PointingHandCursor);
    editButton->setFixedSize(36, 36);
    editButton->setStyleSheet(R"(
        QToolButton {
            border: none;
            border-radius: 8px;
            background-color: #eff6ff;
        }

        QToolButton:hover {
            background-color: #dbeafe;
        }
    )");

    auto *deleteButton = new QToolButton(rowWidget);
    deleteButton->setIcon(makeBinIcon());
    deleteButton->setIconSize(QSize(20, 20));
    deleteButton->setToolTip("Delete semester");
    deleteButton->setAccessibleName("Delete semester");
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setFixedSize(36, 36);
    deleteButton->setStyleSheet(R"(
        QToolButton {
            border: none;
            border-radius: 8px;
            background-color: #fef2f2;
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

    if (summaryDetailsLabel)
    {
        layout->addWidget(
            summaryDetailsLabel,
            0,
            Qt::AlignVCenter
        );
    }

    layout->addWidget(
        statusButton,
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

    item->setSizeHint(QSize(0, 96));

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

void SemestersWindow::setSemesterStatus(
    QListWidgetItem *item,
    SemesterStatus status)
{
    if (!item)
    {
        return;
    }

    const int semesterID =
        item->data(Qt::UserRole).toInt();

    const bool summaryOnly =
        item->data(Qt::UserRole + 4).toBool();

    const SemesterStatus currentStatus =
        static_cast<SemesterStatus>(
            item->data(Qt::UserRole + 7).toInt()
        );

    if (status == currentStatus)
    {
        return;
    }

    if (summaryOnly)
    {
        QMessageBox::information(
            this,
            tr("Completed Summary"),
            tr("A summary-only semester must remain completed.")
        );
        return;
    }

    if (status == SemesterStatus::Completed)
    {
        const std::vector<Course> courses =
            database.loadCoursesForSemester(semesterID);

        QStringList unfinishedCourses;

        for (const Course &course : courses)
        {
            if (!course.isCompleted() &&
                !course.isWithdrawn())
            {
                unfinishedCourses.append(
                    QString::fromStdString(
                        course.getCode()
                    )
                );
            }
        }

        if (!unfinishedCourses.isEmpty())
        {
            QMessageBox::information(
                this,
                tr("Courses Still Incomplete"),
                tr("Complete or withdraw every course before completing "
                   "this semester.\n\nStill incomplete: %1")
                    .arg(unfinishedCourses.join(", "))
            );
            return;
        }
    }

    if (!database.setSemesterStatus(
            semesterID,
            status))
    {
        QMessageBox::warning(
            this,
            tr("Database Error"),
            tr("The semester status could not be changed.")
        );
        return;
    }

    refreshSemesters();
    emit semestersChanged();
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
            item->data(Qt::UserRole + 6).toDouble(),
            static_cast<SemesterStatus>(
                item->data(Qt::UserRole + 7).toInt()
            )
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
