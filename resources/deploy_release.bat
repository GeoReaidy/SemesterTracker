@echo off
setlocal

REM ---------- EDIT THESE THREE PATHS ----------
set "QT_BIN=C:\Qt\6.11.1\mingw_64\bin"
set "BUILD_EXE=C:\Users\96171\OneDrive\Documents\QtProjects\SemesterTrackerGUI\build\Desktop_Qt_6_11_1_MinGW_64_bit-Release\SemesterTrackerGUI.exe"
set "DEPLOY_DIR=C:\SemesterTrackerDeploy"
REM -------------------------------------------

if not exist "%BUILD_EXE%" (
    echo ERROR: The Release EXE was not found:
    echo %BUILD_EXE%
    echo.
    echo Update BUILD_EXE near the top of this script.
    pause
    exit /b 1
)

if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
mkdir "%DEPLOY_DIR%"

copy /y "%BUILD_EXE%" "%DEPLOY_DIR%\SemesterTrackerGUI.exe" >nul

"%QT_BIN%\windeployqt.exe" ^
    --release ^
    --compiler-runtime ^
    --no-translations ^
    "%DEPLOY_DIR%\SemesterTrackerGUI.exe"

REM Copy third-party DLLs not handled by windeployqt.
REM Uncomment and adjust these lines if the DLLs are dynamically linked:
REM copy /y "C:\path\to\libsodium-23.dll" "%DEPLOY_DIR%\"
REM copy /y "C:\path\to\sqlite3.dll" "%DEPLOY_DIR%\"

copy /y "%~dp0semestertracker.ico" "%DEPLOY_DIR%\" >nul
copy /y "%~dp0semestertracker_icon.png" "%DEPLOY_DIR%\" >nul

echo.
echo Deployment created at:
echo %DEPLOY_DIR%
echo.
echo Test it by temporarily renaming your Qt folder, or by copying
echo the deploy folder to another Windows PC.
pause
