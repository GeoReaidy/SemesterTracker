#ifndef COURSE_H
#define COURSE_H

#include "assignment.h"

#include <string>
#include <vector>

enum class CourseStatus
{
    Planned,
    InProgress,
    Completed,
    Withdrawn
};

std::string courseStatusToStorage(CourseStatus status);
CourseStatus courseStatusFromStorage(const std::string &value);

class Course
{
private:
    std::string courseName;
    std::string courseCode;
    int creditCount;
    std::vector<Assignment> assignments;
    int courseID;
    CourseStatus status;
    bool retaken;
    int retakeOfCourseID;
    bool excludedFromCGPA;
    double targetGrade;

public:
    Course(int id,
           std::string name,
           std::string code,
           int credits,
           CourseStatus status = CourseStatus::InProgress,
           bool retaken = false,
           int retakeOfCourseID = -1,
           bool excludedFromCGPA = false,
           double targetGrade = -1.0);

    int getID() const;
    std::string getName() const;
    std::string getCode() const;
    int getCredits() const;
    CourseStatus getStatus() const;
    bool isPlanned() const;
    bool isInProgress() const;
    bool isCompleted() const;
    bool isWithdrawn() const;
    bool isRetaken() const;
    int getRetakeOfCourseID() const;
    bool isExcludedFromCGPA() const;
    double getTargetGrade() const;
    bool hasTargetGrade() const;
    const std::vector<Assignment> &getAssignments() const;

    void setName(const std::string &name);
    void setCode(const std::string &code);
    void setCredits(int credits);
    void setStatus(CourseStatus newStatus);
    void setRetaken(bool isRetaken);
    void setRetakeOfCourseID(int courseID);
    void setExcludedFromCGPA(bool excluded);
    void setTargetGrade(double grade);

    void addAssignment(const Assignment &assignment);
    bool removeAssignment(int assignmentID);
    Assignment *findAssignment(int assignmentID);

    bool hasGradedAssignments() const;
    int getTotalAssignmentWeight() const;
    double calculateCourseGrade() const;
    double calculateCourseGPA() const;
    double calculateQualityPoints() const;
};

#endif
