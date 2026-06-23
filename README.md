[README.md](https://github.com/user-attachments/files/29241597/README.md)
# Semester Tracker

Semester Tracker is a Windows desktop application for university students to manage semesters, courses, assignments, grades, deadlines, GPA, and overall degree progress.

It was built in C++17 using Qt, SQLite, CMake, and libsodium.

## Features

### Academic Tracking

- Create and manage semesters
- Add courses with names, codes, and credit hours
- Track assignments with grades, weights, and optional due dates
- Calculate course averages automatically
- Calculate semester GPA and cumulative GPA
- Track total completed credits toward a degree
- Add completed courses directly using a final letter grade
- Handle retaken courses by replacing the previous attempt

### Calendar and Notifications

- Monthly calendar view
- Deadline highlighting
- Upcoming assignment list
- Configurable deadline reminders
- Windows system-tray notifications
- Clickable notifications that open the relevant calendar view

### Accounts and Settings

- Local account registration and login
- Secure password hashing with libsodium
- Persistent login sessions until logout
- Change username and password
- Configure required degree credits
- Enable or disable reminders
- Delete the local account and associated academic data

### Updates

- Automatic update checks at startup
- Manual update checks from Settings
- GitHub Releases integration
- Installer download and launch from inside the application

## Screenshots

For Later

## Technologies

- C++17
- Qt Widgets
- SQLite
- libsodium
- CMake
- Inno Setup
- GitHub Releases

## Project Structure

```text
SemesterTracker/
├── resources/
├── main.cpp
├── guiapp.cpp
├── guiapp.h
├── dashboard.cpp
├── dashboard.h
├── semesterswindow.cpp
├── courseswindow.cpp
├── assignmentswindow.cpp
├── gradeswindow.cpp
├── calendarwindow.cpp
├── settingswindow.cpp
├── updatemanager.cpp
├── databasemanager.cpp
├── authservice.cpp
├── user.cpp
├── semester.cpp
├── course.cpp
├── assignment.cpp
├── CMakeLists.txt
├── LICENSE
└── README.md
```

## Building from Source

### Requirements

- Qt 6
- CMake 3.16 or newer
- A C++17-compatible compiler
- MinGW or MSVC on Windows
- libsodium

### Build Steps

```bash
git clone https://github.com/GeoReaidy/SemesterTracker.git
cd SemesterTracker
cmake -S . -B build
cmake --build build --config Release
```

Depending on your Qt installation, you may need to configure CMake with the Qt path.

Example:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"
cmake --build build --config Release
```

## Running the Application

For most users, download the latest installer from the GitHub Releases page.

The installer asset is named:

```text
SemesterTrackerSetup.exe
```

## Data and Privacy

Semester Tracker stores account and academic data locally using SQLite.

- No academic data is uploaded to an external server.
- Passwords are stored as secure hashes, not plain text.
- A persistent session token is used to keep users logged in.
- Internet access is only used for update checks and installer downloads.

## Versioning

This project uses semantic versioning:

```text
MAJOR.MINOR.PATCH
```

Examples:

- `1.0.0` — first stable release
- `1.0.1` — bug fixes
- `1.1.0` — new backward-compatible features
- `2.0.0` — major changes

## Roadmap

Possible future improvements:

- Export grades and academic reports
- Multiple grading-scale presets
- Cloud synchronization
- Password recovery
- Dark mode
- Additional platforms

## License

This project is licensed under the MIT License.

See [LICENSE](LICENSE) for details.

## Author

Developed by Georges Reaidy.

GitHub: [GeoReaidy](https://github.com/GeoReaidy)
