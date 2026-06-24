#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <string>

class Assignment
{
private:
    std::string assignmentName;
    int assignmentWeight;
    double assignmentGrade;
    int assignmentID;
    std::string dueDate;
    bool completed;

public:
    Assignment(int id,
               std::string name,
               int weight,
               double grade,
               std::string dueDate = "",
               bool completed = false);

    int getID() const;
    std::string getName() const;
    double getWeight() const;
    double getWeightPercentage() const;
    double getGrade() const;
    bool hasGrade() const;
    std::string getDueDate() const;
    bool hasDueDate() const;
    bool isCompleted() const;

    void setName(const std::string &name);
    void setWeight(int weight);
    void setGrade(double grade);
    void setDueDate(const std::string &date);
    void setCompleted(bool completed);
};

#endif
