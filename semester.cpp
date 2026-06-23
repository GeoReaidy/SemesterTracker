#include "semester.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace
{
constexpr int MinimumSemesterYear = 1900;
constexpr int MaximumSemesterYear = 2100;
constexpr std::size_t MaximumSemesterNameLength = 40;

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

void validateSemesterID(int id)
{
    if (id < -1)
    {
        throw std::invalid_argument(
            "Semester ID cannot be less than -1."
        );
    }
}

std::string validateSemesterName(
    const std::string &name)
{
    const std::string trimmed = trimCopy(name);

    if (trimmed.empty())
    {
        throw std::invalid_argument(
            "Semester name cannot be empty."
        );
    }

    if (trimmed.size() > MaximumSemesterNameLength)
    {
        throw std::invalid_argument(
            "Semester name is too long."
        );
    }

    return trimmed;
}

void validateSemesterYear(int year)
{
    if (year < MinimumSemesterYear ||
        year > MaximumSemesterYear)
    {
        throw std::invalid_argument(
            "Semester year must be between 1900 and 2100."
        );
    }
}
}

Semester::Semester()
    : semesterYear(MinimumSemesterYear),
      semesterID(-1),
      inProgress(false)
{
}

Semester::Semester(int id,
                   std::string name,
                   int year,
                   bool inProgress)
    : semesterName(validateSemesterName(name)),
      semesterYear(year),
      semesterID(id),
      inProgress(inProgress)
{
    validateSemesterID(semesterID);
    validateSemesterYear(semesterYear);
}

int Semester::getID() const
{
    return semesterID;
}

std::string Semester::getName() const
{
    return semesterName;
}

int Semester::getYear() const
{
    return semesterYear;
}

bool Semester::isInProgress() const
{
    return inProgress;
}

const std::vector<Course> &Semester::getCourses() const
{
    return courses;
}

void Semester::setName(const std::string &name)
{
    semesterName = validateSemesterName(name);
}

void Semester::setYear(int year)
{
    validateSemesterYear(year);
    semesterYear = year;
}

void Semester::setInProgress(bool status)
{
    inProgress = status;
}

void Semester::addCourse(const Course &course)
{
    for (const Course &existingCourse : courses)
    {
        if (course.getID() >= 0 &&
            existingCourse.getID() == course.getID())
        {
            throw std::invalid_argument(
                "A course with this ID already exists "
                "in the semester."
            );
        }

        if (upperCopy(existingCourse.getCode()) ==
            upperCopy(course.getCode()))
        {
            throw std::invalid_argument(
                "A course with this code already exists "
                "in the semester."
            );
        }
    }

    courses.push_back(course);
}

bool Semester::removeCourse(int courseID)
{
    if (courseID < 0)
    {
        return false;
    }

    for (auto iterator = courses.begin();
         iterator != courses.end();
         ++iterator)
    {
        if (iterator->getID() == courseID)
        {
            courses.erase(iterator);
            return true;
        }
    }

    return false;
}

Course *Semester::findCourse(int courseID)
{
    if (courseID < 0)
    {
        return nullptr;
    }

    for (Course &course : courses)
    {
        if (course.getID() == courseID)
        {
            return &course;
        }
    }

    return nullptr;
}

double Semester::calculateSemesterGPA() const
{
    double totalQualityPoints = 0.0;
    int totalCredits = 0;

    for (const Course &course : courses)
    {
        if (!course.hasGradedAssignments())
        {
            continue;
        }

        totalQualityPoints += course.calculateQualityPoints();
        totalCredits += course.getCredits();
    }

    if (totalCredits == 0)
    {
        return 0.0;
    }

    return totalQualityPoints /
           static_cast<double>(totalCredits);
}
