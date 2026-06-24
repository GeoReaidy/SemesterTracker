cmake_minimum_required(VERSION 3.16)

project(
    SemesterTrackerGUI
    VERSION 1.1.1
    LANGUAGES C CXX
)

# --------------------------------------------------
# C++ configuration
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automatically process Qt .ui, signals/slots, and resources.
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --------------------------------------------------
# Qt
# --------------------------------------------------

find_package(
    QT NAMES Qt6 Qt5
    REQUIRED COMPONENTS Widgets Network
)

find_package(
    Qt${QT_VERSION_MAJOR}
    REQUIRED COMPONENTS Widgets Network
)

# --------------------------------------------------
# Project source files
# --------------------------------------------------

set(PROJECT_SOURCES
    # Qt application
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    # Temporary console controller
    consoleapp.cpp
    consoleapp.h

    # Authentication
    authservice.cpp
    authservice.h

    # Database
    databasemanager.cpp
    databasemanager.h

    # Models
    user.cpp
    user.h

    semester.cpp
    semester.h

    course.cpp
    course.h

    assignment.cpp
    assignment.h

    # SQLite
    sqlite3.c
    sqlite3.h

    # Add future GUI files here:
    # guiapp.cpp
    # guiapp.h
    # loginform.cpp
    # loginform.h
    # loginform.ui
)


# --------------------------------------------------
# Application resources and Windows executable icon
# --------------------------------------------------

set(APP_RESOURCE_FILES
    resources/app_resources.qrc
)

if(WIN32)
    list(
        APPEND APP_RESOURCE_FILES
        resources/app_icon.rc
    )
endif()

# --------------------------------------------------
# Executable
# --------------------------------------------------

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(
        SemesterTrackerGUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        guiapp.h guiapp.cpp
        dashboard.ui
        dashboard.h dashboard.cpp
        semesterswindow.ui
        semesterswindow.h semesterswindow.cpp
        semestereditordialog.ui
        semestereditordialog.h semestereditordialog.cpp
        courseswindow.h courseswindow.cpp
        courseswindow.ui
        courseeditordialog.h courseeditordialog.cpp
        courseeditordialog.ui
        assignmentswindow.ui
        assignmentswindow.h assignmentswindow.cpp
        assignmenteditordialog.ui
        assignmenteditordialog.h assignmenteditordialog.cpp
        gradeswindow.ui
        gradeswindow.h gradeswindow.cpp
        settingswindow.cpp settingswindow.h settingswindow.ui
        calendarwindow.cpp calendarwindow.h calendarwindow.ui
        updatemanager.cpp updatemanager.h
        ${APP_RESOURCE_FILES}
    )
else()
    if(ANDROID)
        add_library(
            SemesterTrackerGUI
            SHARED
            ${PROJECT_SOURCES}
            ${APP_RESOURCE_FILES}
        )
    else()
        add_executable(
            SemesterTrackerGUI
            ${PROJECT_SOURCES}
            ${APP_RESOURCE_FILES}
        )
    endif()
endif()

# --------------------------------------------------
# libsodium
# --------------------------------------------------

set(
    LIBSODIUM_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/external/libsodium-1.0.19-mingw/libsodium-win64"
)

target_include_directories(
    SemesterTrackerGUI
    PRIVATE
    "${LIBSODIUM_ROOT}/include"
)

# --------------------------------------------------
# Libraries
# --------------------------------------------------

target_link_libraries(
    SemesterTrackerGUI
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    "${LIBSODIUM_ROOT}/lib/libsodium.a"
)

# --------------------------------------------------
# Application version
# --------------------------------------------------

target_compile_definitions(
    SemesterTrackerGUI
    PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
)

# Statically link the MinGW runtime.
# Qt itself remains dynamically linked.
if(MINGW)
    target_link_options(
        SemesterTrackerGUI
        PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endif()

# --------------------------------------------------
# Application properties
# --------------------------------------------------

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(
        BUNDLE_ID_OPTION
        MACOSX_BUNDLE_GUI_IDENTIFIER
        com.example.SemesterTrackerGUI
    )
endif()

set_target_properties(
    SemesterTrackerGUI
    PROPERTIES
    ${BUNDLE_ID_OPTION}

    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE

    MACOSX_BUNDLE_BUNDLE_VERSION
        ${PROJECT_VERSION}

    MACOSX_BUNDLE_SHORT_VERSION_STRING
        "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)

# --------------------------------------------------
# Installation
# --------------------------------------------------

include(GNUInstallDirs)

install(
    TARGETS SemesterTrackerGUI

    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SemesterTrackerGUI)
endif()
