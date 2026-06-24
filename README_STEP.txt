AUTHENTICATION FLOW + EMAIL PASSWORD RESET — STEP 11

BACK UP FIRST
-------------
Copy semester_tracker.db before the first launch. The first launch adds an
email column and a case-insensitive unique email index to the users table.

ADD
---
passwordresetclient.h
passwordresetclient.cpp

REPLACE
-------
mainwindow.cpp
mainwindow.ui
guiapp.h
guiapp.cpp
authservice.h
authservice.cpp
databasemanager.h
databasemanager.cpp
settingswindow.h
settingswindow.cpp
settingswindow.ui
dashboard.cpp
CMakeLists.txt

DO NOT REPLACE
--------------
mainwindow.h
dashboard.h
user.h / user.cpp
main.cpp
appstyle.h / appstyle.cpp
any semester, course, or assignment files

NEW FLOW
--------
Welcome -> Get Started -> Sign In or Create Account

Sign In:
- username or email
- password
- forgot-password link

Create Account:
- username
- email
- password
- password confirmation
- degree credit target
- automatic login after successful registration

Forgot Password:
- email page
- six-digit code page
- new-password page
- no QInputDialog

EXISTING USERS
--------------
Existing accounts have an empty email after migration. Sign in normally, open
Settings, enter an email, and save the profile before testing password reset.

EMAIL SERVICE
-------------
The app does not contain SMTP credentials. For local testing, configure SMTP
environment variables and run:

    python password_reset_server.py

The client defaults to http://127.0.0.1:8787.

For another endpoint, set:

    SEMESTER_TRACKER_RESET_API_URL=https://your-service.example

Read PASSWORD_RESET_SETUP.md before publishing. The included Python service is
a development reference and must be placed behind HTTPS with persistent shared
storage and stronger deployment rate limiting for production.

BUILD
-----
1. Copy/add the listed files.
2. Delete the current build folder.
3. Reopen Qt Creator.
4. Run CMake.
5. Build.

TEST
----
- Get Started navigation
- Sign In with username
- Sign In with email
- Create account and automatic dashboard login
- Duplicate username
- Duplicate email
- Password confirmation validation
- Saved session restore
- Logout returns to Welcome
- Existing account adds email in Settings
- Send code, resend cooldown, wrong code, expired code
- Correct code and password reset
- Old password rejected after reset
- New password accepted
- Remembered sessions removed after password reset
