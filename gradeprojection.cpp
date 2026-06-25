#include "gradeprojection.h"

#include <algorithm>

namespace
{
double clampPercentage(double value)
{
    return std::clamp(value, 0.0, 100.0);
}

bool courseGradeForSemester(
    const Course &course,
    bool useProjections,
    double &gpa)
{
    if (course.isWithdrawn())
    {
        return false;
    }

    const CourseProjectionResult result =
        GradeProjection::calculateCourse(course);

    if (course.isCompleted() &&
        result.hasActualGrades)
    {
        gpa = GradeProjection::percentageToGpa(
            result.currentGrade
        );
        return true;
    }

    if (useProjections)
    {
        if (!result.hasProjectedResult)
        {
            return false;
        }

        gpa = result.projectedGpa;
        return true;
    }

    if (!result.hasActualGrades)
    {
        return false;
    }

    gpa = GradeProjection::percentageToGpa(
        result.currentGrade
    );
    return true;
}

bool courseGradeForCgpa(
    const Course &course,
    bool useProjections,
    double &gpa)
{
    if (course.isExcludedFromCGPA() ||
        course.isWithdrawn())
    {
        return false;
    }

    const CourseProjectionResult result =
        GradeProjection::calculateCourse(course);

    if (course.isCompleted() &&
        result.hasActualGrades)
    {
        gpa = GradeProjection::percentageToGpa(
            result.currentGrade
        );
        return true;
    }

    if (useProjections &&
        result.hasProjectedResult)
    {
        gpa = result.projectedGpa;
        return true;
    }

    return false;
}
}

CourseProjectionResult GradeProjection::calculateCourse(
    const Course &course)
{
    CourseProjectionResult result;

    double actualPoints = 0.0;
    double projectedPoints = 0.0;

    for (const Assignment &assignment :
         course.getAssignments())
    {
        const int weight = static_cast<int>(
            assignment.getWeightPercentage()
        );

        if (weight <= 0)
        {
            continue;
        }

        result.configuredWeight += weight;

        if (assignment.hasGrade())
        {
            actualPoints +=
                assignment.getGrade() * weight;
            result.actualWeight += weight;
            continue;
        }

        if (assignment.hasProjectedGrade())
        {
            projectedPoints +=
                assignment.getProjectedGrade() * weight;
            result.projectedWeight += weight;
        }
    }

    result.configuredWeight =
        std::clamp(result.configuredWeight, 0, 100);

    result.actualWeight =
        std::clamp(result.actualWeight, 0, 100);

    result.projectedWeight =
        std::clamp(result.projectedWeight, 0, 100);

    result.coveredWeight =
        std::clamp(
            result.actualWeight +
                result.projectedWeight,
            0,
            100
        );

    result.unresolvedWeight =
        std::max(0, 100 - result.coveredWeight);

    result.hasActualGrades =
        result.actualWeight > 0;

    result.hasProjectedGrades =
        result.projectedWeight > 0;

    if (result.hasActualGrades)
    {
        result.currentGrade =
            clampPercentage(
                actualPoints /
                static_cast<double>(
                    result.actualWeight
                )
            );
    }

    if (result.coveredWeight > 0)
    {
        result.hasProjectedResult = true;
        result.projectedGrade =
            clampPercentage(
                (actualPoints + projectedPoints) /
                static_cast<double>(
                    result.coveredWeight
                )
            );

        result.projectedGpa =
            percentageToGpa(
                result.projectedGrade
            );
    }

    result.hasTarget = course.hasTargetGrade();

    if (!result.hasTarget)
    {
        result.targetState =
            ProjectionTargetState::NoTarget;
        return result;
    }

    result.targetGrade =
        course.getTargetGrade();

    const int remainingWeight =
        std::max(0, 100 - result.actualWeight);

    if (remainingWeight == 0)
    {
        result.targetState =
            ProjectionTargetState::Complete;

        result.requiredRemainingAverage =
            result.currentGrade;
        return result;
    }

    const double requiredPoints =
        result.targetGrade * 100.0 -
        actualPoints;

    result.requiredRemainingAverage =
        requiredPoints /
        static_cast<double>(remainingWeight);

    if (result.requiredRemainingAverage <= 0.0)
    {
        result.requiredRemainingAverage = 0.0;
        result.targetState =
            ProjectionTargetState::AlreadyReached;
    }
    else if (result.requiredRemainingAverage <= 100.0)
    {
        result.targetState =
            ProjectionTargetState::Achievable;
    }
    else
    {
        result.targetState =
            ProjectionTargetState::Impossible;
    }

    return result;
}

double GradeProjection::calculateSemesterGpa(
    const Semester &semester,
    bool useProjections,
    bool *hasValue)
{
    if (semester.isSummaryOnly())
    {
        if (hasValue)
        {
            *hasValue =
                semester.getSummaryCredits() > 0;
        }

        return semester.getSummaryGPA();
    }

    double qualityPoints = 0.0;
    int credits = 0;

    for (const Course &course :
         semester.getCourses())
    {
        double courseGpa = 0.0;

        if (!courseGradeForSemester(
                course,
                useProjections,
                courseGpa))
        {
            continue;
        }

        const int courseCredits =
            std::max(course.getCredits(), 0);

        qualityPoints +=
            courseGpa *
            static_cast<double>(courseCredits);

        credits += courseCredits;
    }

    if (hasValue)
    {
        *hasValue = credits > 0;
    }

    if (credits <= 0)
    {
        return 0.0;
    }

    return qualityPoints /
           static_cast<double>(credits);
}

double GradeProjection::calculateCgpa(
    const User &user,
    bool useProjections,
    bool *hasValue)
{
    double qualityPoints = 0.0;
    int credits = 0;

    for (const Semester &semester :
         user.getSemesters())
    {
        if (semester.isSummaryOnly())
        {
            if (semester.getSummaryCredits() > 0)
            {
                qualityPoints +=
                    semester.getSummaryGPA() *
                    static_cast<double>(
                        semester.getSummaryCredits()
                    );

                credits +=
                    semester.getSummaryCredits();
            }

            continue;
        }

        for (const Course &course :
             semester.getCourses())
        {
            double courseGpa = 0.0;

            if (!courseGradeForCgpa(
                    course,
                    useProjections,
                    courseGpa))
            {
                continue;
            }

            const int courseCredits =
                std::max(
                    course.getCredits(),
                    0
                );

            qualityPoints +=
                courseGpa *
                static_cast<double>(
                    courseCredits
                );

            credits += courseCredits;
        }
    }

    if (hasValue)
    {
        *hasValue = credits > 0;
    }

    if (credits <= 0)
    {
        return 0.0;
    }

    return qualityPoints /
           static_cast<double>(credits);
}

std::vector<GpaTrendPoint>
GradeProjection::buildTrend(const User &user)
{
    std::vector<GpaTrendPoint> points;

    double cumulativeQualityPoints = 0.0;
    int cumulativeCredits = 0;

    for (const Semester &semester :
         user.getSemesters())
    {
        GpaTrendPoint point;

        point.label =
            semester.getName() + " " +
            std::to_string(semester.getYear());

        const bool useProjection =
            !semester.isCompleted();

        point.semesterGpa =
            calculateSemesterGpa(
                semester,
                useProjection,
                &point.hasSemesterGpa
            );

        point.projected =
            useProjection &&
            point.hasSemesterGpa;

        if (semester.isSummaryOnly())
        {
            if (semester.getSummaryCredits() > 0)
            {
                cumulativeQualityPoints +=
                    semester.getSummaryGPA() *
                    static_cast<double>(
                        semester.getSummaryCredits()
                    );

                cumulativeCredits +=
                    semester.getSummaryCredits();
            }
        }
        else
        {
            for (const Course &course :
                 semester.getCourses())
            {
                double courseGpa = 0.0;

                if (!courseGradeForCgpa(
                        course,
                        useProjection,
                        courseGpa))
                {
                    continue;
                }

                const int courseCredits =
                    std::max(
                        course.getCredits(),
                        0
                    );

                cumulativeQualityPoints +=
                    courseGpa *
                    static_cast<double>(
                        courseCredits
                    );

                cumulativeCredits += courseCredits;

                if (!course.isCompleted())
                {
                    point.projected = true;
                }
            }
        }

        if (cumulativeCredits > 0)
        {
            point.hasCumulativeCgpa = true;
            point.cumulativeCgpa =
                cumulativeQualityPoints /
                static_cast<double>(
                    cumulativeCredits
                );
        }

        if (point.hasSemesterGpa ||
            point.hasCumulativeCgpa)
        {
            points.push_back(point);
        }
    }

    return points;
}

double GradeProjection::percentageToGpa(
    double percentage)
{
    if (percentage >= 90.0) return 4.0;
    if (percentage >= 87.0) return 3.7;
    if (percentage >= 83.0) return 3.3;
    if (percentage >= 80.0) return 3.0;
    if (percentage >= 77.0) return 2.7;
    if (percentage >= 73.0) return 2.3;
    if (percentage >= 70.0) return 2.0;
    if (percentage >= 67.0) return 1.7;
    if (percentage >= 63.0) return 1.3;
    if (percentage >= 60.0) return 1.0;
    return 0.0;
}

std::string GradeProjection::percentageToLetterGrade(
    double percentage)
{
    if (percentage >= 90.0) return "A";
    if (percentage >= 87.0) return "A-";
    if (percentage >= 83.0) return "B+";
    if (percentage >= 80.0) return "B";
    if (percentage >= 77.0) return "B-";
    if (percentage >= 73.0) return "C+";
    if (percentage >= 70.0) return "C";
    if (percentage >= 67.0) return "C-";
    if (percentage >= 63.0) return "D+";
    if (percentage >= 60.0) return "D";
    return "F";
}
