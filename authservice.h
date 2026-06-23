#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "databasemanager.h"
#include "user.h"

#include <string>

class AuthService
{
private:
    DatabaseManager &database;

    std::string hashPassword(const std::string &password) const;

    bool verifyPassword(const std::string &password,
                        const std::string &storedHash) const;

    std::string hashSessionToken(
        const std::string &token
    ) const;

public:
    explicit AuthService(DatabaseManager &database);

    bool registerUser(
        const std::string &username,
        const std::string &password,
        int maxCredits = 120
        );

    bool loginUser(const std::string &username,
                   const std::string &password,
                   User &loggedInUser);

    bool createPersistentSession(
        int userID,
        std::string &sessionToken
    );

    bool loginWithSession(
        const std::string &sessionToken,
        User &loggedInUser
    );

    void logoutSession(
        const std::string &sessionToken
    );

    bool changePassword(int userID,
                        const std::string &username,
                        const std::string &currentPassword,
                        const std::string &newPassword);
};

#endif
