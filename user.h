#ifndef USER_H
#define USER_H

#include "semester.h"

#include <string>
#include <vector>

class User
{
private:
    int userID;
    std::string username;
    std::vector<Semester> semesters;
    int maxCredits;

public:
    User(
        int id,
        std::string username,
        int maxCredits = 120
        );

    int getID() const;
    std::string getUsername() const;
    const std::vector<Semester> &getSemesters() const;

    void setUsername(const std::string &username);

    void addSemester(const Semester &semester);
    bool removeSemester(int semesterID);
    Semester *findSemester(int semesterID);

    double calculateCGPA() const;

    int getMaxCredits() const;
    void setMaxCredits(int credits);
    int calculateCompletedCredits() const;
};

#endif
