#ifndef COURSE_H
#define COURSE_H

#include "assignment.h"

#include <string>
#include <vector>

class Course
{
private:
    std::string courseName;
    std::string courseCode;
    int creditCount;
    std::vector<Assignment> assignments;
    int courseID;

public:
    Course(int id,
           std::string name,
           std::string code,
           int credits);

    int getID() const;
    std::string getName() const;
    std::string getCode() const;
    int getCredits() const;
    const std::vector<Assignment> &getAssignments() const;

    void setName(const std::string &name);
    void setCode(const std::string &code);
    void setCredits(int credits);

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
