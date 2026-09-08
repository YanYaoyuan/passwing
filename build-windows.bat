@echo off
setlocal
cd /d "%~dp0"

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\build-windows.ps1" %*
set "PASSWING_BUILD_EXIT=%ERRORLEVEL%"

echo.
if not "%PASSWING_BUILD_EXIT%"=="0" (
    echo PassWing build/package failed with exit code %PASSWING_BUILD_EXIT%.
) else (
    echo PassWing build/package completed successfully.
)
pause
exit /b %PASSWING_BUILD_EXIT%
