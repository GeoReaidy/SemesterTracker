#include "semesterswindow.h"
#include "semestereditordialog.h"
#include "ui_semesterswindow.h"

#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QToolButton>

#include <exception>

namespace
{
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

QString semesterStatusButtonStyle(SemesterStatus status)
{
    switch (status)
    {
    case SemesterStatus::Planned:
        return R"(
            QToolButton {
                color: #475569;
                background-color: #f1f5f9;
                border: 1px solid #cbd5e1;
                border-radius: 8px;
                padding: 5px 24px 5px 10px;
                font-size: 12px;
                font-weight: 600;
            }

            QToolButton:hover {
                background-color: #e2e8f0;
            }

            QToolButton::menu-indicator {
                subcontrol-origin: padding;
                subcontrol-position: center right;
                right: 7px;
            }
        )";

    case SemesterStatus::Active:
        return R"(
            QToolButton {
                color: #166534;
                background-color: #dcfce7;
                border: 1px solid #bbf7d0;
                border-radius: 8px;
                padding: 5px 24px 5px 10px;
                font-size: 12px;
                font-weight: 600;
            }

            QToolButton:hover {
                background-color: #bbf7d0;
            }

            QToolButton::menu-indicator {
                subcontrol-origin: padding;
                subcontrol-position: center right;
                right: 7px;
            }
        )";

    case SemesterStatus::Completed:
        return R"(
            QToolButton {
                color: #1d4ed8;
                background-color: #dbeafe;
                border: 1px solid #bfdbfe;
                border-radius: 8px;
                padding: 5px 24px 5px 10px;
                font-size: 12px;
                font-weight: 600;
            }

            QToolButton:hover {
                background-color: #bfdbfe;
            }

            QToolButton:disabled {
                color: #1d4ed8;
                background-color: #dbeafe;
                border: 1px solid #bfdbfe;
            }

            QToolButton::menu-indicator {
                subcontrol-origin: padding;
                subcontrol-position: center right;
                right: 7px;
            }
        )";
    }

    return {};
}

QString statusMenuStyle()
{
    return R"(
        QMenu {
            background-color: white;
            color: #1f2937;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            padding: 5px;
            font-size: 13px;
            font-weight: 600;
        }

        QMenu::item {
            color: #1f2937;
            background-color: transparent;
            border-radius: 6px;
            padding: 9px 14px;
            margin: 2px;
        }

        QMenu::item:selected {
            color: white;
            background-color: #2563eb;
        }

        QMenu::item:disabled {
            color: #94a3b8;
            background-color: transparent;
        }

        QMenu::indicator {
            width: 14px;
            height: 14px;
        }
    )";
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
        summaryDetailsLabel->setStyleSheet(R"(
            QLabel {
                color: #475569;
                background-color: #f8fafc;
                border: 1px solid #e2e8f0;
                border-radius: 8px;
                padding: 4px 9px;
                font-size: 12px;
            }
        )");
    }

    auto *statusButton = new QToolButton(rowWidget);
    statusButton->setText(
        semesterStatusText(semester.getStatus())
    );
    statusButton->setCursor(Qt::PointingHandCursor);
    statusButton->setToolButtonStyle(
        Qt::ToolButtonTextOnly
    );
    statusButton->setMinimumWidth(108);
    statusButton->setFixedHeight(32);
    statusButton->setStyleSheet(
        semesterStatusButtonStyle(
            semester.getStatus()
        )
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
        statusMenu->setStyleSheet(statusMenuStyle());

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
