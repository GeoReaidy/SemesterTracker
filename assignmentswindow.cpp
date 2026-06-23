#include "assignmentswindow.h"
#include "ui_assignmentswindow.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
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

bool askForDueDate(
    QWidget *parent,
    const QString &title,
    const QDate &initialDate,
    QDate &selectedDate)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setModal(true);

    auto *layout = new QFormLayout(&dialog);

    auto *dateEdit = new QDateEdit(
        initialDate.isValid()
            ? initialDate
            : QDate::currentDate(),
        &dialog
    );

    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd MMM yyyy");
    dateEdit->setMinimumDate(QDate(2000, 1, 1));
    dateEdit->setMaximumDate(QDate(2100, 12, 31));

    layout->addRow("Due date:", dateEdit);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        &dialog
    );

    QObject::connect(
        buttons,
        &QDialogButtonBox::accepted,
        &dialog,
        &QDialog::accept
    );

    QObject::connect(
        buttons,
        &QDialogButtonBox::rejected,
        &dialog,
        &QDialog::reject
    );

    layout->addRow(buttons);

    if (dialog.exec() != QDialog::Accepted)
    {
        return false;
    }

    selectedDate = dateEdit->date();
    return selectedDate.isValid();
}

QString assignmentCountText(int count)
{
    return QString("%1 assignment%2")
        .arg(count)
        .arg(count == 1 ? "" : "s");
}

bool assignmentNameAlreadyExists(
    DatabaseManager &database,
    int courseID,
    const QString &name,
    int ignoredAssignmentID = -1)
{
    const std::vector<Assignment> assignments =
        database.loadAssignmentsForCourse(courseID);

    const QString normalizedName =
        name.trimmed().toCaseFolded();

    for (const Assignment &assignment : assignments)
    {
        if (assignment.getID() == ignoredAssignmentID)
        {
            continue;
        }

        const QString existingName =
            QString::fromStdString(
                assignment.getName()
            ).trimmed().toCaseFolded();

        if (existingName == normalizedName)
        {
            return true;
        }
    }

    return false;
}

int totalAssignmentWeight(
    DatabaseManager &database,
    int courseID,
    int ignoredAssignmentID = -1)
{
    int total = 0;

    const std::vector<Assignment> assignments =
        database.loadAssignmentsForCourse(courseID);

    for (const Assignment &assignment : assignments)
    {
        if (assignment.getID() == ignoredAssignmentID)
        {
            continue;
        }

        total += static_cast<int>(
            assignment.getWeightPercentage()
        );
    }

    return total;
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
            "No Course",
            "Select a course before adding an assignment."
        );
        return;
    }

    bool nameAccepted = false;

    const QString name =
        QInputDialog::getText(
            this,
            "Add Assignment",
            "Assignment name:",
            QLineEdit::Normal,
            QString(),
            &nameAccepted
        ).trimmed();

    if (!nameAccepted)
    {
        return;
    }

    if (name.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Invalid Assignment",
            "Assignment name cannot be empty."
        );
        return;
    }

    bool weightAccepted = false;

    const int weight =
        QInputDialog::getInt(
            this,
            "Add Assignment",
            "Weight percentage:",
            10,
            1,
            100,
            1,
            &weightAccepted
        );

    if (!weightAccepted)
    {
        return;
    }

    double grade = -1.0;

    const QMessageBox::StandardButton gradeAnswer =
        QMessageBox::question(
            this,
            "Assignment Grade",
            "Has the grade for this assignment been released?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (gradeAnswer == QMessageBox::Yes)
    {
        bool gradeAccepted = false;

        grade = QInputDialog::getDouble(
            this,
            "Add Assignment",
            "Grade percentage:",
            0.0,
            0.0,
            100.0,
            2,
            &gradeAccepted
        );

        if (!gradeAccepted)
        {
            return;
        }
    }

    std::string dueDateValue;

    const QMessageBox::StandardButton dueDateAnswer =
        QMessageBox::question(
            this,
            "Assignment Due Date",
            "Does this assignment have a due date?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::Yes
        );

    if (dueDateAnswer == QMessageBox::Yes)
    {
        QDate dueDate;

        if (!askForDueDate(
                this,
                "Assignment Due Date",
                QDate::currentDate(),
                dueDate))
        {
            return;
        }

        dueDateValue =
            dueDate.toString(Qt::ISODate).toStdString();
    }

    try
    {
        const Assignment candidate(
            -1,
            name.toStdString(),
            weight,
            grade,
            dueDateValue
        );

        Q_UNUSED(candidate);
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            "Invalid Assignment",
            error.what()
        );
        return;
    }

    if (assignmentNameAlreadyExists(
            database,
            courseID,
            name))
    {
        QMessageBox::warning(
            this,
            "Duplicate Assignment",
            "An assignment with this name already exists in the selected course."
        );
        return;
    }

    const int existingWeight =
        totalAssignmentWeight(
            database,
            courseID
        );

    if (existingWeight + weight > 100)
    {
        QMessageBox::warning(
            this,
            "Invalid Weight",
            QString(
                "The total assignment weight would become %1%. "
                "A course cannot exceed 100%."
            ).arg(existingWeight + weight)
        );
        return;
    }

    if (!database.addAssignment(
            courseID,
            name.toStdString(),
            grade,
            weight,
            dueDateValue))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not save the assignment."
        );
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
    item->setData(
        Qt::UserRole + 5,
        assignment.hasGrade()
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

    const int assignmentID =
        item->data(Qt::UserRole).toInt();

    const QString currentName =
        item->data(Qt::UserRole + 1).toString();

    const int currentWeight =
        static_cast<int>(
            item->data(Qt::UserRole + 2).toDouble()
        );

    const double currentGrade =
        item->data(Qt::UserRole + 3).toDouble();

    const QString currentDueDateText =
        item->data(Qt::UserRole + 4).toString();

    const bool currentlyHasGrade =
        item->data(Qt::UserRole + 5).toBool();

    const QDate currentDueDate =
        QDate::fromString(
            currentDueDateText,
            Qt::ISODate
        );

    bool nameAccepted = false;

    const QString newName =
        QInputDialog::getText(
            this,
            "Edit Assignment",
            "Assignment name:",
            QLineEdit::Normal,
            currentName,
            &nameAccepted
        ).trimmed();

    if (!nameAccepted)
    {
        return;
    }

    if (newName.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Invalid Assignment",
            "Assignment name cannot be empty."
        );
        return;
    }

    bool weightAccepted = false;

    const int newWeight =
        QInputDialog::getInt(
            this,
            "Edit Assignment",
            "Weight percentage:",
            currentWeight,
            1,
            100,
            1,
            &weightAccepted
        );

    if (!weightAccepted)
    {
        return;
    }

    double newGrade = -1.0;

    const QMessageBox::StandardButton gradeAnswer =
        QMessageBox::question(
            this,
            "Assignment Grade",
            "Has the grade for this assignment been released?",
            QMessageBox::Yes | QMessageBox::No,
            currentlyHasGrade
                ? QMessageBox::Yes
                : QMessageBox::No
        );

    if (gradeAnswer == QMessageBox::Yes)
    {
        bool gradeAccepted = false;

        newGrade = QInputDialog::getDouble(
            this,
            "Edit Assignment",
            "Grade percentage:",
            currentlyHasGrade ? currentGrade : 0.0,
            0.0,
            100.0,
            2,
            &gradeAccepted
        );

        if (!gradeAccepted)
        {
            return;
        }
    }

    std::string newDueDateValue;

    const QMessageBox::StandardButton dueDateAnswer =
        QMessageBox::question(
            this,
            "Assignment Due Date",
            "Does this assignment have a due date?",
            QMessageBox::Yes | QMessageBox::No,
            currentDueDate.isValid()
                ? QMessageBox::Yes
                : QMessageBox::No
        );

    if (dueDateAnswer == QMessageBox::Yes)
    {
        QDate newDueDate;

        if (!askForDueDate(
                this,
                "Edit Assignment Due Date",
                currentDueDate.isValid()
                    ? currentDueDate
                    : QDate::currentDate(),
                newDueDate))
        {
            return;
        }

        newDueDateValue =
            newDueDate.toString(Qt::ISODate).toStdString();
    }

    try
    {
        const Assignment candidate(
            assignmentID,
            newName.toStdString(),
            newWeight,
            newGrade,
            newDueDateValue
        );

        Q_UNUSED(candidate);
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            "Invalid Assignment",
            error.what()
        );
        return;
    }

    const int courseID = selectedCourseID();

    if (assignmentNameAlreadyExists(
            database,
            courseID,
            newName,
            assignmentID))
    {
        QMessageBox::warning(
            this,
            "Duplicate Assignment",
            "Another assignment with this name already exists in the selected course."
        );
        return;
    }

    const int otherAssignmentsWeight =
        totalAssignmentWeight(
            database,
            courseID,
            assignmentID
        );

    if (otherAssignmentsWeight + newWeight > 100)
    {
        QMessageBox::warning(
            this,
            "Invalid Weight",
            QString(
                "The total assignment weight would become %1%. "
                "A course cannot exceed 100%."
            ).arg(otherAssignmentsWeight + newWeight)
        );
        return;
    }

    if (!database.updateAssignment(
            assignmentID,
            newName.toStdString(),
            newGrade,
            newWeight,
            newDueDateValue))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not update the assignment."
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
