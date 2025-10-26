@echo off
REM Build-Skript für HS80 Projekt
REM HINWEIS: Für automatische MSVC-Detection verwende build.ps1
REM Dieses Skript funktioniert am besten in einer "Developer Command Prompt"

echo ========================================
echo HS80 Build-Skript
echo ========================================
echo.

REM Prüfe ob MSVC bereits im PATH verfügbar ist
where cl.exe >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [INFO] MSVC gefunden im PATH, verwende cl.exe
    goto :BUILD_MSVC
)

REM Hinweis: Wenn cl.exe nicht im PATH ist, verwende build.ps1 oder öffne Developer Command Prompt
echo [INFO] cl.exe nicht im PATH gefunden
echo [HINWEIS] Für automatischen Build verwende: powershell -ExecutionPolicy Bypass -File build.ps1
echo [HINWEIS] Oder öffne "Developer Command Prompt for VS 2022" und führe dieses Skript erneut aus
echo.

REM Prüfe ob MinGW verfügbar ist
where g++.exe >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [INFO] MinGW gefunden, verwende g++.exe
    goto :BUILD_MINGW
)

echo [FEHLER] Kein Compiler gefunden!
echo.
echo Bitte wähle eine der folgenden Optionen:
echo   1. Führe aus: powershell -ExecutionPolicy Bypass -File build.ps1
echo   2. Öffne "Developer Command Prompt for VS 2022" und führe build.bat aus
echo   3. Installiere MinGW-w64 und füge es zum PATH hinzu
pause
exit /b 1

:BUILD_MSVC
echo.
echo Kompiliere mit MSVC...
if not exist "HS80\Debug" mkdir "HS80\Debug"
cl.exe /Zi /EHsc /std:c++17 /nologo ^
    /Fe:"HS80\Debug\HS80.exe" ^
    /Fo:"HS80\Debug\" ^
    HS80\HS80.cpp ^
    hid.lib setupapi.lib

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [ERFOLG] Build erfolgreich!
    echo Executable: HS80\Debug\HS80.exe
) else (
    echo.
    echo [FEHLER] Build fehlgeschlagen!
)
goto :END

:BUILD_MINGW
echo.
echo Kompiliere mit MinGW g++...
if not exist "HS80\Debug" mkdir "HS80\Debug"
g++ -g -std=c++17 HS80\HS80.cpp -o HS80\Debug\HS80.exe -lhid -lsetupapi

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [ERFOLG] Build erfolgreich!
    echo Executable: HS80\Debug\HS80.exe
) else (
    echo.
    echo [FEHLER] Build fehlgeschlagen!
)
goto :END

:END
echo.
pause
