#include "authservice.h"

#include <sodium.h>

#include <algorithm>
#include <cctype>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
constexpr std::size_t MinimumUsernameLength = 3;
constexpr std::size_t MaximumUsernameLength = 40;
constexpr std::size_t MinimumPasswordLength = 8;
constexpr std::size_t MaximumPasswordLength = 128;
constexpr int MinimumMaximumCredits = 1;
constexpr int MaximumMaximumCredits = 1000;

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

bool isValidUsername(const std::string &value)
{
    const std::string username = trimCopy(value);

    if (username.size() < MinimumUsernameLength ||
        username.size() > MaximumUsernameLength)
    {
        return false;
    }

    return std::all_of(
        username.begin(),
        username.end(),
        [](unsigned char character)
        {
            return std::isalnum(character) ||
                   character == '_' ||
                   character == '-' ||
                   character == '.';
        }
    );
}

std::string normalizeEmail(const std::string &value)
{
    std::string email = trimCopy(value);

    std::transform(
        email.begin(),
        email.end(),
        email.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::tolower(character)
            );
        }
    );

    return email;
}

bool isValidEmail(const std::string &value)
{
    const std::string email =
        normalizeEmail(value);

    if (email.size() < 5 ||
        email.size() > 254)
    {
        return false;
    }

    const std::size_t at =
        email.find('@');

    if (at == std::string::npos ||
        at == 0 ||
        at + 1 >= email.size() ||
        email.find('@', at + 1) !=
            std::string::npos)
    {
        return false;
    }

    const std::string domain =
        email.substr(at + 1);

    return domain.find('.') !=
               std::string::npos &&
           domain.front() != '.' &&
           domain.back() != '.';
}

bool isValidPassword(const std::string &password)
{
    if (password.size() < MinimumPasswordLength ||
        password.size() > MaximumPasswordLength)
    {
        return false;
    }

    bool hasLetter = false;
    bool hasNumber = false;

    for (unsigned char character : password)
    {
        if (std::isalpha(character))
        {
            hasLetter = true;
        }

        if (std::isdigit(character))
        {
            hasNumber = true;
        }
    }

    return hasLetter && hasNumber;
}
}

AuthService::AuthService(DatabaseManager &database)
    : database(database)
{
}

std::string AuthService::hashPassword(
    const std::string &password) const
{
    char hashedPassword[crypto_pwhash_STRBYTES];

    const int result = crypto_pwhash_str(
        hashedPassword,
        password.c_str(),
        static_cast<unsigned long long>(
            password.size()
        ),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
    );

    if (result != 0)
    {
        throw std::runtime_error(
            "Not enough memory to securely hash the password."
        );
    }

    return std::string(hashedPassword);
}

bool AuthService::verifyPassword(
    const std::string &password,
    const std::string &storedHash) const
{
    if (password.empty() || storedHash.empty())
    {
        return false;
    }

    return crypto_pwhash_str_verify(
               storedHash.c_str(),
               password.c_str(),
               static_cast<unsigned long long>(
                   password.size()
               ))
           == 0;
}

std::string AuthService::hashSessionToken(
    const std::string &token) const
{
    if (token.empty())
    {
        return {};
    }

    unsigned char digest[crypto_generichash_BYTES];

    if (crypto_generichash(
            digest,
            sizeof(digest),
            reinterpret_cast<const unsigned char *>(
                token.data()
            ),
            static_cast<unsigned long long>(
                token.size()
            ),
            nullptr,
            0) != 0)
    {
        return {};
    }

    std::vector<char> hexadecimal(
        sizeof(digest) * 2 + 1
    );

    sodium_bin2hex(
        hexadecimal.data(),
        hexadecimal.size(),
        digest,
        sizeof(digest)
    );

    sodium_memzero(digest, sizeof(digest));

    return std::string(hexadecimal.data());
}

bool AuthService::registerUser(
    const std::string &username,
    const std::string &password,
    int maxCredits)
{
    return registerUser(
        username,
        std::string(),
        password,
        maxCredits
    );
}

bool AuthService::registerUser(
    const std::string &username,
    const std::string &email,
    const std::string &password,
    int maxCredits)
{
    const std::string normalizedUsername =
        trimCopy(username);

    const std::string normalizedEmail =
        normalizeEmail(email);

    if (!isValidUsername(normalizedUsername))
    {
        std::cout
            << "Username must contain 3-40 valid characters."
            << std::endl;
        return false;
    }

    if (!normalizedEmail.empty() &&
        !isValidEmail(normalizedEmail))
    {
        std::cout
            << "A valid email address is required."
            << std::endl;
        return false;
    }

    if (!isValidPassword(password))
    {
        std::cout
            << "Password must contain 8-128 characters, "
               "including at least one letter and one number."
            << std::endl;
        return false;
    }

    if (maxCredits < MinimumMaximumCredits ||
        maxCredits > MaximumMaximumCredits)
    {
        std::cout
            << "Maximum credits must be between 1 and 1000."
            << std::endl;
        return false;
    }

    if (database.userExists(normalizedUsername) ||
        (!normalizedEmail.empty() &&
         database.emailExists(normalizedEmail)))
    {
        std::cout
            << "Username or email already exists."
            << std::endl;
        return false;
    }

    try
    {
        const User candidate(
            -1,
            normalizedUsername,
            maxCredits
        );

        (void)candidate;

        const std::string passwordHash =
            hashPassword(password);

        return database.addUser(
            normalizedUsername,
            normalizedEmail,
            passwordHash,
            maxCredits
        );
    }
    catch (const std::exception &error)
    {
        std::cout << "Registration failed: "
                  << error.what()
                  << std::endl;
        return false;
    }
}

bool AuthService::loginUser(
    const std::string &identifier,
    const std::string &password,
    User &loggedInUser)
{
    const std::string normalizedIdentifier =
        trimCopy(identifier);

    if (normalizedIdentifier.empty() ||
        password.empty())
    {
        std::cout
            << "An account identifier and password are required."
            << std::endl;
        return false;
    }

    std::string username =
        normalizedIdentifier;

    if (normalizedIdentifier.find('@') !=
        std::string::npos)
    {
        username =
            database.getUsernameByEmail(
                normalizeEmail(
                    normalizedIdentifier
                )
            );
    }

    if (!isValidUsername(username) ||
        !database.userExists(username))
    {
        std::cout << "User does not exist." << std::endl;
        return false;
    }

    const std::string storedHash =
        database.getPasswordHashByUsername(
            username
        );

    if (!verifyPassword(password, storedHash))
    {
        std::cout << "Incorrect password." << std::endl;
        return false;
    }

    try
    {
        loggedInUser =
            database.loadFullUserByUsername(
                username
            );

        return true;
    }
    catch (const std::exception &error)
    {
        std::cout << "Failed to load user data: "
                  << error.what()
                  << std::endl;
        return false;
    }
}

bool AuthService::createPersistentSession(
    int userID,
    std::string &sessionToken)
{
    sessionToken.clear();

    if (userID <= 0)
    {
        return false;
    }

    unsigned char tokenBytes[32];
    randombytes_buf(tokenBytes, sizeof(tokenBytes));

    std::vector<char> encodedToken(
        sodium_base64_ENCODED_LEN(
            sizeof(tokenBytes),
            sodium_base64_VARIANT_URLSAFE_NO_PADDING
        )
    );

    sodium_bin2base64(
        encodedToken.data(),
        encodedToken.size(),
        tokenBytes,
        sizeof(tokenBytes),
        sodium_base64_VARIANT_URLSAFE_NO_PADDING
    );

    sodium_memzero(
        tokenBytes,
        sizeof(tokenBytes)
    );

    const std::string rawToken(
        encodedToken.data()
    );

    const std::string tokenHash =
        hashSessionToken(rawToken);

    if (tokenHash.empty() ||
        !database.saveLoginSession(
            userID,
            tokenHash))
    {
        return false;
    }

    sessionToken = rawToken;
    return true;
}

bool AuthService::loginWithSession(
    const std::string &sessionToken,
    User &loggedInUser)
{
    const std::string tokenHash =
        hashSessionToken(sessionToken);

    if (tokenHash.empty())
    {
        return false;
    }

    const int userID =
        database.getUserIDBySessionTokenHash(
            tokenHash
        );

    if (userID <= 0)
    {
        return false;
    }

    try
    {
        const std::vector<Semester> semesters =
            database.loadSemestersForUser(userID);

        (void)semesters;

        // Resolve the user through the existing user table.
        sqlite3 *unusedDatabaseHandle = nullptr;
        (void)unusedDatabaseHandle;

        // The session stores the user ID; obtain the username by testing
        // the full user records already exposed by DatabaseManager.
        // A dedicated ID loader is cleaner, so use the new database method.
        loggedInUser =
            database.loadFullUserByID(userID);

        return true;
    }
    catch (const std::exception &error)
    {
        std::cout << "Stored login could not be restored: "
                  << error.what()
                  << std::endl;
        return false;
    }
}

void AuthService::logoutSession(
    const std::string &sessionToken)
{
    const std::string tokenHash =
        hashSessionToken(sessionToken);

    if (!tokenHash.empty())
    {
        database.deleteLoginSession(tokenHash);
    }
}

bool AuthService::changePassword(
    int userID,
    const std::string &username,
    const std::string &currentPassword,
    const std::string &newPassword)
{
    const std::string normalizedUsername =
        trimCopy(username);

    if (userID <= 0 ||
        !isValidUsername(normalizedUsername) ||
        currentPassword.empty() ||
        !isValidPassword(newPassword))
    {
        return false;
    }

    if (currentPassword == newPassword)
    {
        return false;
    }

    const std::string storedHash =
        database.getPasswordHashByUsername(
            normalizedUsername
        );

    if (!verifyPassword(currentPassword, storedHash))
    {
        return false;
    }

    try
    {
        const std::string newHash =
            hashPassword(newPassword);

        return database.updatePasswordHash(
            userID,
            newHash
        );
    }
    catch (const std::exception &error)
    {
        std::cout << "Password hashing failed: "
                  << error.what()
                  << std::endl;
        return false;
    }
}
