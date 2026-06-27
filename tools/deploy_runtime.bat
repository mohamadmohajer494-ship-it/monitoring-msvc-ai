@echo off
setlocal EnableExtensions

set "ROOT=%~dp0.."
set "QT_ROOT=C:\Qt\6.11.1\msvc2022_64"
set "QT_DEPLOY=%ROOT%\third_party\qt-msvc"
set "VCPKG_BIN=%ROOT%\third_party\vcpkg-msvc\bin"
set "DEBUG_DIR=%ROOT%\build\Desktop_Qt_6_11_1_MSVC2022_64bit_Debug\debug"
set "RELEASE_DIR=%ROOT%\build\Desktop_Qt_6_11_1_MSVC2022_64bit_Release\release"

call :deploy "%DEBUG_DIR%" "--debug"
call :deploy "%RELEASE_DIR%" "--release"
exit /b 0

:deploy
set "DEST=%~1"
set "MODE=%~2"
if not exist "%DEST%\Monitoring.exe" exit /b 0

if not exist "%QT_DEPLOY%\platforms\qwindows.dll" (
    if exist "%QT_ROOT%\bin\windeployqt.exe" (
        if not exist "%QT_DEPLOY%" mkdir "%QT_DEPLOY%"
        "%QT_ROOT%\bin\windeployqt.exe" %MODE% --dir "%QT_DEPLOY%" "%DEST%\Monitoring.exe"
    )
)

if exist "%QT_DEPLOY%\" robocopy "%QT_DEPLOY%" "%DEST%" /E >nul
if exist "%VCPKG_BIN%\*.dll" copy /Y "%VCPKG_BIN%\*.dll" "%DEST%\" >nul

exit /b 0
