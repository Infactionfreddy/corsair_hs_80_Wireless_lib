@echo off
REM ============================================================================
REM HS80 Library Build Script - Baut alle Programme
REM ============================================================================

echo ====================================
echo   HS80 Library Build
echo ====================================
echo.

REM Prüfe ob MSVC verfügbar ist
where cl.exe >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [INFO] Suche Visual Studio...
    
    REM Suche VS 2022
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        echo [GEFUNDEN] Visual Studio 2022 Community
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        echo [GEFUNDEN] Visual Studio 2022 Professional
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        echo [GEFUNDEN] Visual Studio 2022 Enterprise
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo [FEHLER] Kein Visual Studio gefunden!
        echo Installieren Sie Visual Studio 2022 mit C++ Desktop Development
        pause
        exit /b 1
    )
)

echo.
echo [BUILD] Kompiliere Library und Programme...
echo.

REM Erstelle Debug-Verzeichnis
if not exist "HS80\Debug" mkdir "HS80\Debug"

REM Kompiliere Library
echo [1/4] Kompiliere HS80_Library.cpp...
cl.exe /c /EHsc /std:c++17 /Zi /Od /Fo"HS80\Debug\HS80_Library.obj" HS80\HS80_Library.cpp
if %ERRORLEVEL% NEQ 0 (
    echo [FEHLER] Library-Kompilierung fehlgeschlagen!
    pause
    exit /b 1
)

REM Erstelle statische Library
echo [2/4] Erstelle HS80_Lib.lib...
lib.exe /OUT:"HS80\Debug\HS80_Lib.lib" "HS80\Debug\HS80_Library.obj"
if %ERRORLEVEL% NEQ 0 (
    echo [FEHLER] Library-Erstellung fehlgeschlagen!
    pause
    exit /b 1
)

REM Kompiliere HS80.exe (Original)
echo [3/4] Kompiliere HS80.exe...
cl.exe /EHsc /std:c++17 /Zi /Od /Fe"HS80\Debug\HS80.exe" HS80\HS80.cpp "HS80\Debug\HS80_Lib.lib" hid.lib setupapi.lib
if %ERRORLEVEL% NEQ 0 (
    echo [FEHLER] HS80.exe Kompilierung fehlgeschlagen!
    pause
    exit /b 1
)

REM Kompiliere HS80_Demo.exe
echo [4/4] Kompiliere HS80_Demo.exe...
cl.exe /EHsc /std:c++17 /Zi /Od /Fe"HS80\Debug\HS80_Demo.exe" HS80\HS80_Demo.cpp "HS80\Debug\HS80_Lib.lib" hid.lib setupapi.lib
if %ERRORLEVEL% NEQ 0 (
    echo [WARNUNG] HS80_Demo.exe Kompilierung fehlgeschlagen!
)

REM Kompiliere HS80_Analyzer.exe
echo [5/5] Kompiliere HS80_Analyzer.exe...
cl.exe /EHsc /std:c++17 /Zi /Od /Fe"HS80\Debug\HS80_Analyzer.exe" HS80\HS80_Analyzer.cpp "HS80\Debug\HS80_Lib.lib" hid.lib setupapi.lib
if %ERRORLEVEL% NEQ 0 (
    echo [WARNUNG] HS80_Analyzer.exe Kompilierung fehlgeschlagen!
)

REM Aufräumen
del *.obj 2>nul

echo.
echo ====================================
echo   Build Abgeschlossen!
echo ====================================
echo.
echo Erstellt:
if exist "HS80\Debug\HS80_Lib.lib" (
    for %%F in ("HS80\Debug\HS80_Lib.lib") do echo   - HS80_Lib.lib       : %%~zF bytes
)
if exist "HS80\Debug\HS80.exe" (
    for %%F in ("HS80\Debug\HS80.exe") do echo   - HS80.exe            : %%~zF bytes
)
if exist "HS80\Debug\HS80_Demo.exe" (
    for %%F in ("HS80\Debug\HS80_Demo.exe") do echo   - HS80_Demo.exe       : %%~zF bytes
)
if exist "HS80\Debug\HS80_Analyzer.exe" (
    for %%F in ("HS80\Debug\HS80_Analyzer.exe") do echo   - HS80_Analyzer.exe   : %%~zF bytes
)
echo.
echo Zum Testen:
echo   HS80\Debug\HS80.exe          - Original Programm
echo   HS80\Debug\HS80_Demo.exe     - High-Level Demo
echo   HS80\Debug\HS80_Analyzer.exe - Analyse-Tool
echo.
pause
