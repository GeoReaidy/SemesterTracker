#include "courseswindow.h"
#include "ui_courseswindow.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QStringList>
#include <QToolButton>
#include <QVBoxLayout>

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

double percentageForLetterGrade(
    const QString &letterGrade)
{
    if (letterGrade == "A")  return 90.0;
    if (letterGrade == "A-") return 87.0;
    if (letterGrade == "B+") return 83.0;
    if (letterGrade == "B")  return 80.0;
    if (letterGrade == "B-") return 77.0;
    if (letterGrade == "C+") return 73.0;
    if (letterGrade == "C")  return 70.0;
    if (letterGrade == "C-") return 67.0;
    if (letterGrade == "D+") return 63.0;
    if (letterGrade == "D")  return 60.0;
    if (letterGrade == "F")  return 0.0;

    return -1.0;
}
}

CoursesWindow::CoursesWindow(
    DatabaseManager &database,
    int userID,
    QWidget *parent)
    : QWidget(parent),
      database(database),
      userID(userID),
      ui(new Ui::CoursesWindow)
{
    ui->setupUi(this);

    ui->coursesListWidget->setStyleSheet(R"(
        QListWidget {
            background-color: transparent;
            color: #1f2937;
            border: none;
            outline: none;
        }

        QListWidget::item {
            background-color: transparent;
            color: #1f2937;
            border: none;
            padding: 0;
            margin: 4px 0;
        }

        QListWidget::item:hover,
        QListWidget::item:selected {
            background-color: transparent;
            color: #1f2937;
            border: none;
        }
    )");

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
            background-color: #ffffff;
            color: #1f2937;
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

    ui->semesterComboBox->setMaxVisibleItems(10);

    connect(
        ui->semesterComboBox,
        &QComboBox::currentIndexChanged,
        this,
        &CoursesWindow::handleSemesterChanged
    );

    connect(
        ui->addCourseButton,
        &QPushButton::clicked,
        this,
        &CoursesWindow::handleAddCourse
    );

    refreshSemesters();
}

CoursesWindow::~CoursesWindow()
{
    delete ui;
}

void CoursesWindow::setUserID(int newUserID)
{
    userID = newUserID;
    refreshSemesters();
}

void CoursesWindow::refreshSemesters()
{
    const int previousSemesterID =
        selectedSemesterID();

    ui->semesterComboBox->blockSignals(true);
    ui->semesterComboBox->clear();

    if (userID < 0)
    {
        ui->semesterComboBox->addItem(
            "No user selected",
            -1
        );

        ui->semesterComboBox->setEnabled(false);
        ui->addCourseButton->setEnabled(false);
        ui->semesterComboBox->blockSignals(false);

        refreshCourses();
        return;
    }

    const std::vector<Semester> semesters =
        database.loadSemestersForUser(userID);

    int currentSemesterIndex = -1;
    int previousSemesterIndex = -1;

    for (const Semester &semester : semesters)
    {
        const QString label =
            QString("%1 %2")
                .arg(QString::fromStdString(
                    semester.getName()
                ))
                .arg(semester.getYear());

        ui->semesterComboBox->addItem(
            label,
            semester.getID()
        );

        const int index =
            ui->semesterComboBox->count() - 1;

        if (semester.isInProgress())
        {
            currentSemesterIndex = index;
        }

        if (semester.getID() == previousSemesterID)
        {
            previousSemesterIndex = index;
        }
    }

    if (semesters.empty())
    {
        ui->semesterComboBox->addItem(
            "Create a semester first",
            -1
        );

        ui->semesterComboBox->setEnabled(false);
        ui->addCourseButton->setEnabled(false);
    }
    else
    {
        ui->semesterComboBox->setEnabled(true);
        ui->addCourseButton->setEnabled(true);

        if (previousSemesterIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(
                previousSemesterIndex
            );
        }
        else if (currentSemesterIndex >= 0)
        {
            ui->semesterComboBox->setCurrentIndex(
                currentSemesterIndex
            );
        }
        else
        {
            ui->semesterComboBox->setCurrentIndex(0);
        }
    }

    ui->semesterComboBox->blockSignals(false);

    refreshCourses();
}

void CoursesWindow::refreshCourses()
{
    ui->coursesListWidget->clear();

    const int semesterID = selectedSemesterID();

    if (semesterID < 0)
    {
        updateEmptyState();
        return;
    }

    const std::vector<Course> courses =
        database.loadCoursesForSemester(semesterID);

    for (const Course &course : courses)
    {
        addCourseRow(course);
    }

    updateEmptyState();
}

void CoursesWindow::handleSemesterChanged(int index)
{
    Q_UNUSED(index);
    refreshCourses();
}

void CoursesWindow::handleAddCourse()
{
    const int semesterID = selectedSemesterID();

    if (semesterID < 0)
    {
        QMessageBox::warning(
            this,
            "No Semester Selected",
            "Select or create a semester before adding a course."
        );
        return;
    }

    bool codeAccepted = false;

    const QString courseCode =
        QInputDialog::getText(
            this,
            "Add Course",
            "Course code:",
            QLineEdit::Normal,
            QString(),
            &codeAccepted
        ).trimmed();

    if (!codeAccepted)
    {
        return;
    }

    if (courseCode.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Invalid Course",
            "Course code cannot be empty."
        );
        return;
    }

    bool nameAccepted = false;

    const QString courseName =
        QInputDialog::getText(
            this,
            "Add Course",
            "Course name:",
            QLineEdit::Normal,
            QString(),
            &nameAccepted
        ).trimmed();

    if (!nameAccepted)
    {
        return;
    }

    if (courseName.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Invalid Course",
            "Course name cannot be empty."
        );
        return;
    }

    bool creditsAccepted = false;

    const int credits =
        QInputDialog::getInt(
            this,
            "Add Course",
            "Credit hours:",
            3,
            1,
            30,
            1,
            &creditsAccepted
        );

    if (!creditsAccepted)
    {
        return;
    }

    try
    {
        const Course candidate(
            -1,
            courseName.toStdString(),
            courseCode.toStdString(),
            credits
        );

        (void)candidate;
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(
            this,
            "Invalid Course",
            error.what()
        );
        return;
    }

    const QStringList gradeOptions = {
        "No final grade yet",
        "A",
        "A-",
        "B+",
        "B",
        "B-",
        "C+",
        "C",
        "C-",
        "D+",
        "D",
        "F"
    };

    bool gradeAccepted = false;

    const QString selectedGrade =
        QInputDialog::getItem(
            this,
            "Course Final Grade",
            "Select the final letter grade if it is already available:",
            gradeOptions,
            0,
            false,
            &gradeAccepted
        );

    if (!gradeAccepted)
    {
        return;
    }

    const bool hasFinalGrade =
        selectedGrade != "No final grade yet";

    const double finalPercentage =
        hasFinalGrade
            ? percentageForLetterGrade(selectedGrade)
            : -1.0;

    const int existingCourseID =
        database.findCourseByCodeForUser(
            userID,
            courseCode.toStdString()
        );

    bool saved = false;

    if (existingCourseID >= 0)
    {
        const QMessageBox::StandardButton retakeAnswer =
            QMessageBox::question(
                this,
                "Course Already Exists",
                QString(
                    "%1 already exists in your academic history.\n\n"
                    "Are you adding it as a retaken course?\n\n"
                    "If you continue, its previous assignments and grade "
                    "will be removed and replaced by this new attempt."
                ).arg(courseCode),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );

        if (retakeAnswer != QMessageBox::Yes)
        {
            QMessageBox::information(
                this,
                "Course Not Added",
                "Duplicate courses are blocked unless the course is being retaken."
            );
            return;
        }

        saved = database.retakeCourse(
            existingCourseID,
            semesterID,
            courseCode.toStdString(),
            courseName.toStdString(),
            credits,
            hasFinalGrade,
            selectedGrade.toStdString(),
            finalPercentage
        );
    }
    else if (hasFinalGrade)
    {
        saved = database.addCompletedCourse(
            semesterID,
            courseCode.toStdString(),
            courseName.toStdString(),
            credits,
            selectedGrade.toStdString(),
            finalPercentage
        );
    }
    else
    {
        saved = database.addCourse(
            semesterID,
            courseCode.toStdString(),
            courseName.toStdString(),
            credits
        );
    }

    if (!saved)
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not save the course or complete the retake operation."
        );
        return;
    }

    refreshSemesters();
    refreshCourses();
    emit coursesChanged();
}

int CoursesWindow::selectedSemesterID() const
{
    if (!ui ||
        !ui->semesterComboBox ||
        ui->semesterComboBox->count() == 0)
    {
        return -1;
    }

    return ui->semesterComboBox
        ->currentData()
        .toInt();
}

void CoursesWindow::addCourseRow(
    const Course &course)
{
    auto *item =
        new QListWidgetItem(ui->coursesListWidget);

    item->setData(Qt::UserRole, course.getID());
    item->setData(
        Qt::UserRole + 1,
        QString::fromStdString(course.getCode())
    );
    item->setData(
        Qt::UserRole + 2,
        QString::fromStdString(course.getName())
    );
    item->setData(
        Qt::UserRole + 3,
        course.getCredits()
    );

    auto *rowWidget =
        new QWidget(ui->coursesListWidget);

    rowWidget->setMinimumHeight(88);
    rowWidget->setObjectName("courseRow");
    rowWidget->setStyleSheet(R"(
        QWidget#courseRow {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 9px;
        }

        QWidget#courseRow QLabel {
            background-color: transparent;
            border: none;
        }

        QWidget#courseRow QWidget {
            background-color: transparent;
            border: none;
        }
    )");

    auto *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(14, 0, 10, 0);
    rowLayout->setSpacing(10);
    rowLayout->setAlignment(Qt::AlignVCenter);

    auto *textContainer = new QWidget(rowWidget);
    textContainer->setStyleSheet(
        "background: transparent;"
        "border: none;"
    );

    auto *textLayout = new QVBoxLayout(textContainer);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);

    auto *courseTitleLabel = new QLabel(
        QString("%1 — %2")
            .arg(QString::fromStdString(
                course.getCode()
            ))
            .arg(QString::fromStdString(
                course.getName()
            )),
        textContainer
    );

    courseTitleLabel->setStyleSheet(
        "color: #111827;"
        "font-size: 15px;"
        "font-weight: 600;"
    );

    auto *creditsLabel = new QLabel(
        QString("%1 credit%2")
            .arg(course.getCredits())
            .arg(course.getCredits() == 1 ? "" : "s"),
        textContainer
    );

    creditsLabel->setStyleSheet(
        "color: #64748b;"
        "font-size: 13px;"
    );

    textLayout->addWidget(courseTitleLabel);
    textLayout->addWidget(creditsLabel);

    auto *editButton = new QToolButton(rowWidget);
    editButton->setIcon(makePencilIcon());
    editButton->setIconSize(QSize(20, 20));
    editButton->setToolTip("Edit course");
    editButton->setAccessibleName("Edit course");
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
    deleteButton->setToolTip("Delete course");
    deleteButton->setAccessibleName("Delete course");
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

    ui->coursesListWidget->setItemWidget(
        item,
        rowWidget
    );

    connect(
        editButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            editCourseRow(item);
        }
    );

    connect(
        deleteButton,
        &QToolButton::clicked,
        this,
        [this, item]()
        {
            deleteCourseRow(item);
        }
    );
}

void CoursesWindow::editCourseRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    const int courseID =
        item->data(Qt::UserRole).toInt();

    const QString currentCode =
        item->data(Qt::UserRole + 1).toString();

    const QString currentName =
        item->data(Qt::UserRole + 2).toString();

    const int currentCredits =
        item->data(Qt::UserRole + 3).toInt();

    bool codeAccepted = false;

    const QString newCode =
        QInputDialog::getText(
            this,
            "Edit Course",
            "Course code:",
            QLineEdit::Normal,
            currentCode,
            &codeAccepted
        ).trimmed();

    if (!codeAccepted)
    {
        return;
    }

    if (newCode.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Invalid Course",
            "Course code cannot be empty."
        );
        return;
    }

    bool nameAccepted = false;

    const QString newName =
        QInputDialog::getText(
            this,
            "Edit Course",
            "Course name:",
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
            "Invalid Course",
            "Course name cannot be empty."
        );
        return;
    }

    bool creditsAccepted = false;

    const int newCredits =
        QInputDialog::getInt(
            this,
            "Edit Course",
            "Credit hours:",
            currentCredits,
            1,
            30,
            1,
            &creditsAccepted
        );

    if (!creditsAccepted)
    {
        return;
    }

    if (!database.updateCourse(
            courseID,
            newCode.toStdString(),
            newName.toStdString(),
            newCredits))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not update the course."
        );
        return;
    }

    refreshCourses();
    emit coursesChanged();
}

void CoursesWindow::deleteCourseRow(
    QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    const int courseID =
        item->data(Qt::UserRole).toInt();

    const QString courseCode =
        item->data(Qt::UserRole + 1).toString();

    const QString courseName =
        item->data(Qt::UserRole + 2).toString();

    const QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Delete Course",
            QString("Delete %1 — %2?")
                .arg(courseCode)
                .arg(courseName),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (answer != QMessageBox::Yes)
    {
        return;
    }

    if (!database.deleteCourse(courseID))
    {
        QMessageBox::warning(
            this,
            "Database Error",
            "Could not delete the course."
        );
        return;
    }

    refreshCourses();
    emit coursesChanged();
}

void CoursesWindow::updateEmptyState()
{
    const int count =
        ui->coursesListWidget->count();

    ui->courseCountLabel->setText(
        QString::number(count)
    );

    const bool hasCourses = count > 0;
    const bool hasSemester =
        selectedSemesterID() >= 0;

    ui->coursesListWidget->setVisible(hasCourses);

    ui->emptyStateTitleLabel->setVisible(
        !hasCourses
    );

    ui->emptyStateLabel->setVisible(
        !hasCourses
    );

    if (!hasSemester)
    {
        ui->emptyStateTitleLabel->setText(
            "No semester selected"
        );

        ui->emptyStateLabel->setText(
            "Create a semester first, then add courses to it."
        );
    }
    else
    {
        ui->emptyStateTitleLabel->setText(
            "No courses yet"
        );

        ui->emptyStateLabel->setText(
            "Add a course to the selected semester."
        );
    }
}
