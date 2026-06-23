#include "assignment.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace
{
constexpr std::size_t MaximumAssignmentNameLength = 120;

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

void validateAssignmentID(int id)
{
    if (id < -1)
    {
        throw std::invalid_argument(
            "Assignment ID cannot be less than -1."
        );
    }
}

std::string validateAssignmentName(
    const std::string &name)
{
    const std::string trimmed = trimCopy(name);

    if (trimmed.empty())
    {
        throw std::invalid_argument(
            "Assignment name cannot be empty."
        );
    }

    if (trimmed.size() > MaximumAssignmentNameLength)
    {
        throw std::invalid_argument(
            "Assignment name is too long."
        );
    }

    return trimmed;
}

void validateWeight(int weight)
{
    if (weight < 1 || weight > 100)
    {
        throw std::invalid_argument(
            "Assignment weight must be between 1 and 100."
        );
    }
}

void validateGrade(double grade)
{
    if (grade < -1.0 || grade > 100.0)
    {
        throw std::invalid_argument(
            "Assignment grade must be -1 for no grade, "
            "or between 0 and 100."
        );
    }
}

bool isLeapYear(int year)
{
    return (year % 400 == 0) ||
           ((year % 4 == 0) && (year % 100 != 0));
}

int daysInMonth(int year, int month)
{
    static const int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (month == 2 && isLeapYear(year))
    {
        return 29;
    }

    return days[month - 1];
}

bool isValidIsoDate(const std::string &date)
{
    if (date.size() != 10 ||
        date[4] != '-' ||
        date[7] != '-')
    {
        return false;
    }

    for (std::size_t index = 0;
         index < date.size();
         ++index)
    {
        if (index == 4 || index == 7)
        {
            continue;
        }

        if (!std::isdigit(
                static_cast<unsigned char>(date[index])))
        {
            return false;
        }
    }

    const int year = std::stoi(date.substr(0, 4));
    const int month = std::stoi(date.substr(5, 2));
    const int day = std::stoi(date.substr(8, 2));

    if (year < 1900 || year > 2100)
    {
        return false;
    }

    if (month < 1 || month > 12)
    {
        return false;
    }

    return day >= 1 &&
           day <= daysInMonth(year, month);
}

std::string validateDueDate(
    const std::string &date)
{
    const std::string trimmed = trimCopy(date);

    if (trimmed.empty())
    {
        return {};
    }

    if (!isValidIsoDate(trimmed))
    {
        throw std::invalid_argument(
            "Due date must be empty or use YYYY-MM-DD "
            "with a valid calendar date."
        );
    }

    return trimmed;
}
}

Assignment::Assignment(int id,
                       std::string name,
                       int weight,
                       double grade,
                       std::string dueDate)
    : assignmentName(validateAssignmentName(name)),
      assignmentWeight(weight),
      assignmentGrade(grade),
      assignmentID(id),
      dueDate(validateDueDate(dueDate))
{
    validateAssignmentID(assignmentID);
    validateWeight(assignmentWeight);
    validateGrade(assignmentGrade);
}

int Assignment::getID() const
{
    return assignmentID;
}

std::string Assignment::getName() const
{
    return assignmentName;
}

double Assignment::getWeight() const
{
    return assignmentWeight / 100.0;
}

double Assignment::getWeightPercentage() const
{
    return static_cast<double>(assignmentWeight);
}

double Assignment::getGrade() const
{
    return assignmentGrade;
}

bool Assignment::hasGrade() const
{
    return assignmentGrade >= 0.0;
}

std::string Assignment::getDueDate() const
{
    return dueDate;
}

bool Assignment::hasDueDate() const
{
    return !dueDate.empty();
}

void Assignment::setName(const std::string &name)
{
    assignmentName = validateAssignmentName(name);
}

void Assignment::setWeight(int weight)
{
    validateWeight(weight);
    assignmentWeight = weight;
}

void Assignment::setGrade(double grade)
{
    validateGrade(grade);
    assignmentGrade = grade;
}

void Assignment::setDueDate(
    const std::string &date)
{
    dueDate = validateDueDate(date);
}
