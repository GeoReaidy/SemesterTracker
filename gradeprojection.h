#ifndef GRADEPROJECTION_H
#define GRADEPROJECTION_H

#include "course.h"
#include "semester.h"
#include "user.h"

#include <string>
#include <vector>

enum class ProjectionTargetState
{
    NoTarget,
    AlreadyReached,
    Achievable,
    Impossible,
    Complete
};

struct CourseProjectionResult
{
    bool hasActualGrades = false;
    bool hasProjectedGrades = false;
    bool hasProjectedResult = false;
    bool hasTarget = false;

    int configuredWeight = 0;
    int actualWeight = 0;
    int projectedWeight = 0;
    int coveredWeight = 0;
    int unresolvedWeight = 100;

    double currentGrade = 0.0;
    double projectedGrade = 0.0;
    double projectedGpa = 0.0;
    double targetGrade = 0.0;
    double requiredRemainingAverage = 0.0;

    ProjectionTargetState targetState =
        ProjectionTargetState::NoTarget;
};

struct GpaTrendPoint
{
    std::string label;
    double semesterGpa = 0.0;
    double cumulativeCgpa = 0.0;
    bool hasSemesterGpa = false;
    bool hasCumulativeCgpa = false;
    bool projected = false;
};

class GradeProjection final
{
public:
    static CourseProjectionResult calculateCourse(
        const Course &course
    );

    static double calculateSemesterGpa(
        const Semester &semester,
        bool useProjections,
        bool *hasValue = nullptr
    );

    static double calculateCgpa(
        const User &user,
        bool useProjections,
        bool *hasValue = nullptr
    );

    static std::vector<GpaTrendPoint> buildTrend(
        const User &user
    );

    static double percentageToGpa(double percentage);
    static std::string percentageToLetterGrade(
        double percentage
    );

private:
    GradeProjection() = delete;
};

#endif // GRADEPROJECTION_H
