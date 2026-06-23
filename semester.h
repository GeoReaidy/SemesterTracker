#ifndef SEMESTER_H
#define SEMESTER_H

#include "course.h"

#include <string>
#include <vector>

class Semester
{
private:
    std::string semesterName;
    int semesterYear;
    std::vector<Course> courses;
    int semesterID;
    bool inProgress;
    bool summaryOnly;
    int summaryCredits;
    double summaryGPA;

public:
    Semester();
    Semester(int id,
             std::string name,
             int year,
             bool inProgress = false,
             bool summaryOnly = false,
             int summaryCredits = 0,
             double summaryGPA = 0.0);

    int getID() const;
    std::string getName() const;
    int getYear() const;
    bool isInProgress() const;
    bool isSummaryOnly() const;
    int getSummaryCredits() const;
    double getSummaryGPA() const;

    const std::vector<Course> &getCourses() const;

    void setName(const std::string &name);
    void setYear(int year);
    void setInProgress(bool status);
    void setSummaryData(bool summaryOnly, int credits, double gpa);

    void addCourse(const Course &course);
    bool removeCourse(int courseID);
    Course *findCourse(int courseID);

    double calculateSemesterGPA() const;
};

#endif
