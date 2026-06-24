#include "course.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace
{
constexpr int MinimumCourseCredits = 1;
constexpr int MaximumCourseCredits = 30;
constexpr std::size_t MaximumCourseNameLength = 120;
constexpr std::size_t MaximumCourseCodeLength = 30;

std::string trimCopy(const std::string &value)
{
    const auto first = std::find_if_not(
        value.begin(),
        value.end(),
        [](unsigned char character)
        {
            return std::isspace(character);
        }
    );

    if (first == value.end())
    {
        return {};
    }

    const auto last = std::find_if_not(
        value.rbegin(),
        value.rend(),
        [](unsigned char character)
        {
            return std::isspace(character);
        }
    ).base();

    return std::string(first, last);
}

std::string upperCopy(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::toupper(character)
            );
        }
    );

    return value;
}

std::string lowerCopy(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::tolower(character)
            );
        }
    );

    return value;
}

void validateCourseID(int id)
{
    if (id < -1)
    {
        throw std::invalid_argument(
            "Course ID cannot be less than -1."
        );
    }
}

std::string validateCourseName(
    const std::string &name)
{
    const std::string trimmed = trimCopy(name);

    if (trimmed.empty())
    {
        throw std::invalid_argument(
            "Course name cannot be empty."
        );
    }

    if (trimmed.size() > MaximumCourseNameLength)
    {
        throw std::invalid_argument(
            "Course name is too long."
        );
    }

    return trimmed;
}

std::string validateCourseCode(
    const std::string &code)
{
    const std::string trimmed = trimCopy(code);

    if (trimmed.empty())
    {
        throw std::invalid_argument(
            "Course code cannot be empty."
        );
    }

    if (trimmed.size() > MaximumCourseCodeLength)
    {
        throw std::invalid_argument(
            "Course code is too long."
        );
    }

    const bool containsVisibleCharacter =
        std::any_of(
            trimmed.begin(),
            trimmed.end(),
            [](unsigned char character)
            {
                return std::isalnum(character);
            }
        );

    if (!containsVisibleCharacter)
    {
        throw std::invalid_argument(
            "Course code must contain a letter or number."
        );
    }

    return trimmed;
}

void validateCredits(int credits)
{
    if (credits < MinimumCourseCredits ||
        credits > MaximumCourseCredits)
    {
        throw std::invalid_argument(
            "Course credits must be between 1 and 30."
        );
    }
}

void validateOptionalCourseID(int id)
{
    if (id == 0 || id < -1)
    {
        throw std::invalid_argument(
            "Retake source course ID must be -1 or a positive value."
        );
    }
}
}

std::string courseStatusToStorage(CourseStatus status)
{
    switch (status)
    {
    case CourseStatus::Planned:
        return "planned";
    case CourseStatus::InProgress:
        return "in_progress";
    case CourseStatus::Completed:
        return "completed";
    case CourseStatus::Withdrawn:
        return "withdrawn";
    }

    return "in_progress";
}

CourseStatus courseStatusFromStorage(
    const std::string &value)
{
    const std::string normalized =
        lowerCopy(trimCopy(value));

    if (normalized == "planned")
    {
        return CourseStatus::Planned;
    }

    if (normalized == "completed")
    {
        return CourseStatus::Completed;
    }

    if (normalized == "withdrawn")
    {
        return CourseStatus::Withdrawn;
    }

    return CourseStatus::InProgress;
}

Course::Course(int id,
               std::string name,
               std::string code,
               int credits,
               CourseStatus courseStatus,
               bool isRetaken,
               int sourceCourseID,
               bool excluded)
    : courseName(validateCourseName(name)),
      courseCode(validateCourseCode(code)),
      creditCount(credits),
      courseID(id),
      status(courseStatus),
      retaken(isRetaken),
      retakeOfCourseID(sourceCourseID),
      excludedFromCGPA(excluded)
{
    validateCourseID(courseID);
    validateCredits(creditCount);
    validateOptionalCourseID(retakeOfCourseID);
}

int Course::getID() const
{
    return courseID;
}

std::string Course::getName() const
{
    return courseName;
}

std::string Course::getCode() const
{
    return courseCode;
}

int Course::getCredits() const
{
    return creditCount;
}

CourseStatus Course::getStatus() const
{
    return status;
}

bool Course::isPlanned() const
{
    return status == CourseStatus::Planned;
}

bool Course::isInProgress() const
{
    return status == CourseStatus::InProgress;
}

bool Course::isCompleted() const
{
    return status == CourseStatus::Completed;
}

bool Course::isWithdrawn() const
{
    return status == CourseStatus::Withdrawn;
}

bool Course::isRetaken() const
{
    return retaken;
}

int Course::getRetakeOfCourseID() const
{
    return retakeOfCourseID;
}

bool Course::isExcludedFromCGPA() const
{
    return excludedFromCGPA;
}

const std::vector<Assignment> &
Course::getAssignments() const
{
    return assignments;
}

void Course::setName(const std::string &name)
{
    courseName = validateCourseName(name);
}

void Course::setCode(const std::string &code)
{
    courseCode = validateCourseCode(code);
}

void Course::setCredits(int credits)
{
    validateCredits(credits);
    creditCount = credits;
}

void Course::setStatus(CourseStatus newStatus)
{
    status = newStatus;
}

void Course::setRetaken(bool isRetaken)
{
    retaken = isRetaken;
}

void Course::setRetakeOfCourseID(int courseID)
{
    validateOptionalCourseID(courseID);
    retakeOfCourseID = courseID;
}

void Course::setExcludedFromCGPA(bool excluded)
{
    excludedFromCGPA = excluded;
}

void Course::addAssignment(
    const Assignment &assignment)
{
    for (const Assignment &existingAssignment : assignments)
    {
        if (assignment.getID() >= 0 &&
            existingAssignment.getID() ==
                assignment.getID())
        {
            throw std::invalid_argument(
                "An assignment with this ID already exists "
                "in the course."
            );
        }

        if (upperCopy(existingAssignment.getName()) ==
            upperCopy(assignment.getName()))
        {
            throw std::invalid_argument(
                "An assignment with this name already exists "
                "in the course."
            );
        }
    }

    const int newTotalWeight =
        getTotalAssignmentWeight() +
        static_cast<int>(
            assignment.getWeightPercentage()
        );

    if (newTotalWeight > 100)
    {
        throw std::invalid_argument(
            "Total assignment weight cannot exceed 100%."
        );
    }

    assignments.push_back(assignment);
}

bool Course::removeAssignment(int assignmentID)
{
    if (assignmentID < 0)
    {
        return false;
    }

    for (auto iterator = assignments.begin();
         iterator != assignments.end();
         ++iterator)
    {
        if (iterator->getID() == assignmentID)
        {
            assignments.erase(iterator);
            return true;
        }
    }

    return false;
}

Assignment *Course::findAssignment(int assignmentID)
{
    if (assignmentID < 0)
    {
        return nullptr;
    }

    for (Assignment &assignment : assignments)
    {
        if (assignment.getID() == assignmentID)
        {
            return &assignment;
        }
    }

    return nullptr;
}

bool Course::hasGradedAssignments() const
{
    return std::any_of(
        assignments.begin(),
        assignments.end(),
        [](const Assignment &assignment)
        {
            return assignment.hasGrade() &&
                   assignment.getWeightPercentage() > 0.0;
        }
    );
}

int Course::getTotalAssignmentWeight() const
{
    int totalWeight = 0;

    for (const Assignment &assignment : assignments)
    {
        totalWeight += static_cast<int>(
            assignment.getWeightPercentage()
        );
    }

    return totalWeight;
}

double Course::calculateCourseGrade() const
{
    double weightedGradeTotal = 0.0;
    int gradedWeightTotal = 0;

    for (const Assignment &assignment : assignments)
    {
        if (!assignment.hasGrade())
        {
            continue;
        }

        const int weight = static_cast<int>(
            assignment.getWeightPercentage()
        );

        if (weight <= 0)
        {
            continue;
        }

        weightedGradeTotal +=
            assignment.getGrade() * weight;

        gradedWeightTotal += weight;
    }

    if (gradedWeightTotal == 0)
    {
        return 0.0;
    }

    return weightedGradeTotal /
           static_cast<double>(gradedWeightTotal);
}

double Course::calculateCourseGPA() const
{
    const double grade = calculateCourseGrade();

    if (grade >= 90.0) return 4.0;
    if (grade >= 87.0) return 3.7;
    if (grade >= 83.0) return 3.3;
    if (grade >= 80.0) return 3.0;
    if (grade >= 77.0) return 2.7;
    if (grade >= 73.0) return 2.3;
    if (grade >= 70.0) return 2.0;
    if (grade >= 67.0) return 1.7;
    if (grade >= 63.0) return 1.3;
    if (grade >= 60.0) return 1.0;

    return 0.0;
}

double Course::calculateQualityPoints() const
{
    if (!isCompleted() ||
        !hasGradedAssignments())
    {
        return 0.0;
    }

    return calculateCourseGPA() *
           static_cast<double>(creditCount);
}
