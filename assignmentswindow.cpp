#include "assignmentswindow.h"
#include "assignmenteditordialog.h"
#include "ui_assignmentswindow.h"

#include <QColor>
#include <QComboBox>
#include <QDialog>
#include <QDate>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QSize>
#include <QToolButton>
#include <QVBoxLayout>

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

QString assignmentCountText(int count)
{
    return QString("%1 assignment%2")
        .arg(count)
        .arg(count == 1 ? "" : "s");
}
}

AssignmentsWindow::AssignmentsWindow(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      userID(userID),
      ui(new Ui::AssignmentsWindow)
{
    ui->setupUi(this);

    const QString comboPopupStyle = R"(
        QListView {
            background-color: #ffffff;
            color: #1f2937;
            border: 1px solid #cbd5e1;
            outline: none;
            padding: 4px;
        }

        QListView::item {
            min-height: 30px;
            padding: 4px 8px;
            color: #1f2937;
            background-color: #ffffff;
        }

        QListView::item:hover {
            background-color: #eff6ff;
            color: #1f2937;
        }

        QListView::item:selected {
            background-color: #dbeafe;
            color: #1f2937;
        }
    )";

    ui->semesterComboBox->view()->setStyleSheet(
        comboPopupStyle
    );

    ui->courseComboBox->view()->setStyleSheet(
        comboPopupStyle
    );

    ui->semesterComboBox->setMaxVisibleItems(10);
    ui->courseComboBox->setMaxVisibleItems(10);

    connect(
        ui->semesterComboBox,
        &QComboBox::currentIndexChanged,
        this,
        &AssignmentsWindow::handleSemesterChanged
    );

    connect(
        ui->courseComboBox,
        &QComboBox::currentIndexChanged,
        this,
        &AssignmentsWindow::handleCourseChanged
    );

    connect(
        ui->addAssignmentButton,
        &QPushButton::clicked,
        this,
        &AssignmentsWindow::handleAddAssignment
    );

    refreshSemesters();
}

AssignmentsWindow::~AssignmentsWindow()
{
    delete ui;
}

void AssignmentsWindow::setUserID(int newUserID)
{
    userID = newUserID;
    refreshSemesters();
}

void AssignmentsWindow::refreshSemesters()
{
    const int previousSemesterID = selectedSemesterID();

    ui->semesterComboBox->blockSignals(true);
    ui->semesterComboBox->clear();

    if (userID < 0)
    {
        ui->semesterComboBox->addItem("No user selected", -1);
        ui->semesterComboBox->setEnabled(false);
        ui->semesterComboBox->blockSignals(false);

        refreshCourses();
        return;
    }

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    int previousIndex = -1;
    int currentIndex = -1;

    for (const Semester &semester : semesters)
    {
        const QString label =
            QString("%1 %2")
                .arg(QString::fromStdString(semester.getName()))
                .arg(semester.getYear());

        ui->semesterComboBox->addItem(label, semester.getID());

        const int index = ui->semesterComboBox->count() - 1;

        if (semester.getID() == previousSemesterID)
        {
            previousIndex = index;
        }

        if (semester.isInProgress())
        {
            currentIndex = index;
        }
    }

    if (semesters.empty())
    {
        ui->semesterComboBox->addItem(
            "Create a semester first",
            -1
        );
        ui->semesterComboBox->setEnabled(false);
    }
    else
    {
        ui->semesterComboBox->setEnabled(true);

        if (previousIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(previousIndex);
        }
        else if (currentIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(currentIndex);
        }
        else
        {
            ui->semesterComboBox->setCurrentIndex(0);
        }
    }

    ui->semesterComboBox->blockSignals(false);
    refreshCourses();
}

void AssignmentsWindow::refreshCourses()
{
    const int previousCourseID = selectedCourseID();
    const int semesterID = selectedSemesterID();

    ui->courseComboBox->blockSignals(true);
    ui->courseComboBox->clear();

    if (semesterID < 0)
    {
        ui->courseComboBox->addItem("Select a semester first", -1);
        ui->courseComboBox->setEnabled(false);
        ui->addAssignmentButton->setEnabled(false);
        ui->courseComboBox->blockSignals(false);

        refreshAssignments();
        return;
    }

    const std::vector<Course> courses =
        database.loadCoursesForSemester(semesterID);

    int previousIndex = -1;

    for (const Course &course : courses)
    {
        const QString label =
            QString("%1 — %2")
                .arg(QString::fromStdString(course.getCode()))
                .arg(QString::fromStdString(course.getName()));

        ui->courseComboBox->addItem(label, course.getID());

        if (course.getID() == previousCourseID)
        {
            previousIndex = ui->courseComboBox->count() - 1;
        }
    }

    if (courses.empty())
    {
        ui->courseComboBox->addItem(
            "Add a course to this semester first",
            -1
        );
        ui->courseComboBox->setEnabled(false);
        ui->addAssignmentButton->setEnabled(false);
    }
    else
    {
        ui->courseComboBox->setEnabled(true);
        ui->addAssignmentButton->setEnabled(true);

        ui->courseComboBox->setCurrentIndex(
            previousIndex >= 0 ? previousIndex : 0
        );
    }

    ui->courseComboBox->blockSignals(false);
    refreshAssignments();
}

void AssignmentsWindow::refreshAssignments()
{
    ui->assignmentsListWidget->clear();

    const int courseID = selectedCourseID();

    if (courseID < 0)
    {
        updateEmptyState();
        return;
    }

    const std::vector<Assignment> assignments =
        database.loadAssignmentsForCourse(courseID);

    for (const Assignment &assignment : assignments)
    {
        addAssignmentRow(assignment);
    }

    updateEmptyState();
}

void AssignmentsWindow::handleSemesterChanged(int index)
{
    Q_UNUSED(index);
    refreshCourses();
}

void AssignmentsWindow::handleCourseChanged(int index)
{
    Q_UNUSED(index);
    refreshAssignments();
}

void AssignmentsWindow::handleAddAssignment()
{
    const int courseID = selectedCourseID();

    if (courseID < 0)
    {
        QMessageBox::information(
            this,
            tr("No Course"),
            tr("Select a course before adding an assignment.")
        );
        return;
    }

    AssignmentEditorDialog dialog(
        database,
        courseID,
        ui->courseComboBox->currentText(),
        this
    );

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    refreshAssignments();
    emit assignmentsChanged();
}

int AssignmentsWindow::selectedSemesterID() const
{
    if (!ui ||
        !ui->semesterComboBox ||
        ui->semesterComboBox->count() == 0)
    {
        return -1;
    }

    return ui->semesterComboBox->currentData().toInt();
}

int AssignmentsWindow::selectedCourseID() const
{
    if (!ui ||
        !ui->courseComboBox ||
        ui->courseComboBox->count() == 0)
    {
        return -1;
    }

    return ui->courseComboBox->currentData().toInt();
}

void AssignmentsWindow::addAssignmentRow(
    const Assignment &assignment)
{
    auto *item =
        new QListWidgetItem(ui->assignmentsListWidget);

    item->setData(Qt::UserRole, assignment.getID());
    item->setData(
        Qt::UserRole + 1,
        QString::fromStdString(assignment.getName())
    );
    item->setData(
        Qt::UserRole + 2,
        assignment.getWeightPercentage()
    );
    item->setData(
        Qt::UserRole + 3,
        assignment.getGrade()
    );
    item->setData(
        Qt::UserRole + 4,
        QString::fromStdString(assignment.getDueDate())
    );

    auto *rowWidget =
        new QWidget(ui->assignmentsListWidget);

    rowWidget->setMinimumHeight(78);
    rowWidget->setObjectName("assignmentRow");
    rowWidget->setStyleSheet(R"(
        QWidget#assignmentRow {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 9px;
        }

        QWidget#assignmentRow QLabel {
            background-color: transparent;
            border: none;
        }

        QWidget#assignmentRow QWidget {
            background-color: transparent;
            border: none;
        }
    )");

    auto *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(14, 8, 10, 8);
    rowLayout->setSpacing(10);

    auto *textContainer = new QWidget(rowWidget);
    textContainer->setStyleSheet(
        "background: transparent;"
        "border: none;"
    );

    auto *textLayout = new QVBoxLayout(textContainer);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(4);

    auto *nameLabel = new QLabel(
        QString::fromStdString(assignment.getName()),
        textContainer
    );

    nameLabel->setStyleSheet(
        "color: #111827;"
        "font-size: 15px;"
        "font-weight: 600;"
    );

    const QString dueDateText =
        QString::fromStdString(assignment.getDueDate());

    const QDate dueDate =
        QDate::fromString(dueDateText, Qt::ISODate);

    const QString formattedDueDate =
        assignment.hasDueDate() && dueDate.isValid()
            ? dueDate.toString("dd MMM yyyy")
            : "No due date";

    const QString gradeText =
        assignment.hasGrade()
            ? QString("%1%")
                  .arg(assignment.getGrade(), 0, 'f', 2)
            : "Not graded";

    auto *detailsLabel = new QLabel(
        QString("Weight: %1%   •   Grade: %2   •   Due: %3")
            .arg(assignment.getWeightPercentage(), 0, 'f', 0)
            .arg(gradeText)
            .arg(formattedDueDate),
        textContainer
    );

    detailsLabel->setStyleSheet(
        "color: #64748b;"
        "font-size: 13px;"
    );

    textLayout->addWidget(nameLabel);
    textLayout->addWidget(detailsLabel);

    auto *editButton = new QToolButton(rowWidget);
    editButton->setIcon(makePencilIcon());
    editButton->setIconSize(QSize(20, 20));
    editButton->setToolTip("Edit assignment");
    editButton->setAccessibleName("Edit assignment");
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
    deleteButton->setToolTip("Delete assignment");
    deleteButton->setAccessibleName("Delete assignment");
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

    rowLayout->addWidget(
        textContainer,
        1,
        Qt::AlignVCenter
    );
    rowLayout->addStretch();
    rowLayout->addWidget(
        editButton,
        0,
        Qt::AlignVCenter
    );
    rowLayout->addWidget(
        deleteButton,
        0,
        Qt::AlignVCenter
    );

    item->setSizeHint(QSize(0, 88));

    ui->assignmentsListWidget->setItemWidget(
        item,
        rowWidget
    );

    connect(
        editButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            editAssignmentRow(item);
        }
    );

    connect(
        deleteButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            deleteAssignmentRow(item);
        }
    );
}

void AssignmentsWindow::editAssignmentRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    try
    {
        const Assignment assignment(
            item->data(Qt::UserRole).toInt(),
            item->data(Qt::UserRole + 1)
                .toString()
                .toStdString(),
            static_cast<int>(
                item->data(Qt::UserRole + 2).toDouble()
            ),
            item->data(Qt::UserRole + 3).toDouble(),
            item->data(Qt::UserRole + 4)
                .toString()
                .toStdString()
        );

        AssignmentEditorDialog dialog(
            database,
            selectedCourseID(),
            ui->courseComboBox->currentText(),
            assignment,
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
            tr("Invalid Assignment"),
            QString::fromUtf8(error.what())
        );
        return;
    }

    refreshAssignments();
    emit assignmentsChanged();
}

void AssignmentsWindow::deleteAssignmentRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    const int assignmentID =
        item->data(Qt::UserRole).toInt();

    const QString assignmentName =
        item->data(Qt::UserRole + 1).toString();

    const QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Delete Assignment",
            QString("Delete \"%1\"?").arg(assignmentName),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (answer != QMessageBox::Yes)
    {
        return;
    }

    if (!database.deleteAssignment(assignmentID))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not delete the assignment."
        );
        return;
    }

    refreshAssignments();
    emit assignmentsChanged();
}

void AssignmentsWindow::updateEmptyState()
{
    const int count =
        ui->assignmentsListWidget->count();

    ui->assignmentCountLabel->setText(
        assignmentCountText(count)
    );

    const bool hasAssignments = count > 0;
    const bool hasSemester = selectedSemesterID() >= 0;
    const bool hasCourse = selectedCourseID() >= 0;

    ui->assignmentsListWidget->setVisible(hasAssignments);
    ui->emptyStateTitleLabel->setVisible(!hasAssignments);
    ui->emptyStateLabel->setVisible(!hasAssignments);

    if (!hasSemester)
    {
        ui->emptyStateTitleLabel->setText(
            "No semester selected"
        );
        ui->emptyStateLabel->setText(
            "Create or select a semester first."
        );
    }
    else if (!hasCourse)
    {
        ui->emptyStateTitleLabel->setText(
            "No course selected"
        );
        ui->emptyStateLabel->setText(
            "Add or select a course for this semester first."
        );
    }
    else
    {
        ui->emptyStateTitleLabel->setText(
            "No assignments yet"
        );
        ui->emptyStateLabel->setText(
            "Click \"Add Assignment\" to create the first assignment for this course."
        );
    }
}
