#include "user.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace
{
constexpr int MinimumMaximumCredits = 1;
constexpr int MaximumMaximumCredits = 1000;
constexpr std::size_t MinimumUsernameLength = 3;
constexpr std::size_t MaximumUsernameLength = 40;

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

std::string normalizeUsername(
    const std::string &value)
{
    const std::string trimmed = trimCopy(value);

    if (trimmed.size() < MinimumUsernameLength ||
        trimmed.size() > MaximumUsernameLength)
    {
        throw std::invalid_argument(
            "Username must contain between 3 and 40 characters."
        );
    }

    for (unsigned char character : trimmed)
    {
        if (!(std::isalnum(character) ||
              character == '_' ||
              character == '-' ||
              character == '.'))
        {
            throw std::invalid_argument(
                "Username may only contain letters, numbers, "
                "periods, underscores, and hyphens."
            );
        }
    }

    return trimmed;
}

void validateUserID(int id)
{
    if (id < -1)
    {
        throw std::invalid_argument(
            "User ID cannot be less than -1."
        );
    }
}

void validateMaximumCredits(int credits)
{
    if (credits < MinimumMaximumCredits ||
        credits > MaximumMaximumCredits)
    {
        throw std::invalid_argument(
            "Maximum credits must be between 1 and 1000."
        );
    }
}
}

User::User(int id,
           std::string user,
           int maximumCredits)
    : userID(id),
      username(normalizeUsername(user)),
      maxCredits(maximumCredits)
{
    validateUserID(userID);
    validateMaximumCredits(maxCredits);
}

int User::getID() const
{
    return userID;
}

std::string User::getUsername() const
{
    return username;
}

const std::vector<Semester> &User::getSemesters() const
{
    return semesters;
}

void User::setUsername(
    const std::string &newUsername)
{
    username = normalizeUsername(newUsername);
}

void User::addSemester(const Semester &semester)
{
    for (const Semester &existingSemester : semesters)
    {
        if (semester.getID() >= 0 &&
            existingSemester.getID() == semester.getID())
        {
            throw std::invalid_argument(
                "A semester with this ID already exists."
            );
        }

        if (existingSemester.getName() == semester.getName() &&
            existingSemester.getYear() == semester.getYear())
        {
            throw std::invalid_argument(
                "This semester already exists."
            );
        }

        if (semester.isInProgress() &&
            existingSemester.isInProgress())
        {
            throw std::invalid_argument(
                "Only one semester can be current."
            );
        }
    }

    semesters.push_back(semester);
}

bool User::removeSemester(int semesterID)
{
    if (semesterID < 0)
    {
        return false;
    }

    for (auto iterator = semesters.begin();
         iterator != semesters.end();
         ++iterator)
    {
        if (iterator->getID() == semesterID)
        {
            semesters.erase(iterator);
            return true;
        }
    }

    return false;
}

Semester *User::findSemester(int semesterID)
{
    if (semesterID < 0)
    {
        return nullptr;
    }

    for (Semester &semester : semesters)
    {
        if (semester.getID() == semesterID)
        {
            return &semester;
        }
    }

    return nullptr;
}

double User::calculateCGPA() const
{
    double totalQualityPoints = 0.0;
    int totalCredits = 0;

    for (const Semester &semester : semesters)
    {
        if (semester.isSummaryOnly())
        {
            totalQualityPoints += semester.getSummaryGPA() * semester.getSummaryCredits();
            totalCredits += semester.getSummaryCredits();
            continue;
        }

        for (const Course &course : semester.getCourses())
        {
            if (!course.hasGradedAssignments())
            {
                continue;
            }

            totalQualityPoints +=
                course.calculateQualityPoints();

            totalCredits += course.getCredits();
        }
    }

    if (totalCredits == 0)
    {
        return 0.0;
    }

    return totalQualityPoints /
           static_cast<double>(totalCredits);
}

int User::calculateCompletedCredits() const
{
    int completedCredits = 0;

    for (const Semester &semester : semesters)
    {
        if (semester.isSummaryOnly())
        {
            completedCredits += semester.getSummaryCredits();
            continue;
        }

        for (const Course &course : semester.getCourses())
        {
            completedCredits += course.getCredits();
        }
    }

    return completedCredits;
}

int User::getMaxCredits() const
{
    return maxCredits;
}

void User::setMaxCredits(int credits)
{
    validateMaximumCredits(credits);

    const int registeredCredits =
        calculateCompletedCredits();

    if (credits < registeredCredits)
    {
        throw std::invalid_argument(
            "Maximum credits cannot be lower than "
            "the credits already registered."
        );
    }

    maxCredits = credits;
}
