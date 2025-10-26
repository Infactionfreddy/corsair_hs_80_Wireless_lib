# Build-Skript für HS80 Projekt (PowerShell)
# Dieses Skript versucht automatisch den richtigen Compiler zu finden

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "HS80 Build-Skript (PowerShell)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Wechsle ins Projektverzeichnis
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptPath

# Prüfe ob MSVC bereits im PATH verfügbar ist
$clExists = Get-Command cl.exe -ErrorAction SilentlyContinue
if ($clExists) {
    Write-Host "[INFO] MSVC gefunden im PATH, verwende cl.exe" -ForegroundColor Green
    $useMSVC = $true
}
else {
    # Suche nach Visual Studio Installation
    Write-Host "[INFO] Suche Visual Studio Installation..." -ForegroundColor Yellow
    
    # Versuche vswhere.exe zu verwenden
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $vsPath = $null
    
    if (Test-Path $vswhere) {
        $vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
    }
    
    # Fallback: Suche an Standard-Pfaden
    if (-not $vsPath) {
        $possiblePaths = @(
            "C:\Program Files\Microsoft Visual Studio\2022\Community",
            "C:\Program Files\Microsoft Visual Studio\2022\Professional",
            "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
            "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community",
            "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional"
        )
        
        foreach ($path in $possiblePaths) {
            if (Test-Path "$path\VC\Auxiliary\Build\vcvars64.bat") {
                $vsPath = $path
                break
            }
        }
    }
    
    # Wenn Visual Studio gefunden wurde, aktiviere die Umgebung
    if ($vsPath) {
        Write-Host "[INFO] Visual Studio gefunden: $vsPath" -ForegroundColor Green
        Write-Host "[INFO] Aktiviere MSVC Build-Umgebung..." -ForegroundColor Yellow
        
        $vcvarsPath = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
        
        # Führe vcvars64.bat aus und baue mit MSVC im selben cmd-Kontext
        $useMSVC = $true
        $vcvarsActivated = $true
    }
    else {
        # Prüfe ob MinGW verfügbar ist
        if (Get-Command g++.exe -ErrorAction SilentlyContinue) {
            Write-Host "[INFO] MinGW gefunden, verwende g++.exe" -ForegroundColor Green
            $useMSVC = $false
        }
        else {
            Write-Host "[FEHLER] Kein Compiler gefunden!" -ForegroundColor Red
            Write-Host "Bitte installiere Visual Studio Build Tools oder MinGW-w64" -ForegroundColor Yellow
            Write-Host ""
            Write-Host "Option 1: Installiere Visual Studio 2022 (Community, Professional oder Enterprise)" -ForegroundColor Cyan
            Write-Host "Option 2: Installiere MinGW-w64 und füge es zum PATH hinzu" -ForegroundColor Cyan
            exit 1
        }
    }
}

# Erstelle Debug-Verzeichnis falls nicht vorhanden
if (-not (Test-Path "HS80\Debug")) {
    New-Item -ItemType Directory -Path "HS80\Debug" | Out-Null
}

# Build mit MSVC
if ($useMSVC) {
    Write-Host ""
    Write-Host "Kompiliere mit MSVC..." -ForegroundColor Yellow
    
    if ($vcvarsActivated) {
        # Erstelle temporäres Batch-Skript für den Build  
        $tempBat = Join-Path $PSScriptRoot "temp_build.bat"
        $currentPath = (Get-Location).Path
        
        # Schreibe Batch-File mit korrektem Encoding
        @"
@echo off
call "$vcvarsPath"
if errorlevel 1 exit /b 1
cd /d "$currentPath"
cl.exe /Zi /EHsc /std:c++17 /nologo /Fe:HS80\Debug\HS80.exe /Fo:HS80\Debug\ HS80\HS80.cpp hid.lib setupapi.lib
"@ | Set-Content -Path $tempBat -Encoding ASCII -NoNewline
        
        # Führe Batch-Skript aus
        $output = & cmd.exe /c "`"$tempBat`" 2>&1"
        $exitCode = $LASTEXITCODE
        
        # Lösche temporäres Batch-Skript
        if (Test-Path $tempBat) {
            Remove-Item $tempBat -Force -ErrorAction SilentlyContinue
        }
        
        # Zeige Build-Ausgaben
        if ($output) {
            $output | ForEach-Object { 
                $line = $_.ToString()
                # Filtere vcvars Copyright und Environment initialized Nachrichten
                if ($line -and $line.Trim() -ne "" -and 
                    $line -notmatch "Visual Studio" -and 
                    $line -notmatch "Copyright.*Microsoft" -and
                    $line -notmatch "Environment initialized" -and
                    $line -notmatch "^\*+$" -and
                    $line -notmatch "\[vcvarsall\.bat\]") {
                    Write-Host $line
                }
            }
        }
    }
    else {
        # cl.exe ist bereits im PATH
        & cl.exe /Zi /EHsc /std:c++17 /nologo `
            /Fe:"HS80\Debug\HS80.exe" `
            /Fo:"HS80\Debug\" `
            HS80\HS80.cpp `
            hid.lib setupapi.lib
        
        $exitCode = $LASTEXITCODE
    }
    
    if ($exitCode -eq 0) {
        Write-Host ""
        Write-Host "[ERFOLG] Build erfolgreich!" -ForegroundColor Green
        Write-Host "Executable: HS80\Debug\HS80.exe" -ForegroundColor Cyan
        
        if (Test-Path "HS80\Debug\HS80.exe") {
            $exeInfo = Get-Item "HS80\Debug\HS80.exe"
            Write-Host "Größe: $([math]::Round($exeInfo.Length/1KB, 2)) KB" -ForegroundColor Gray
            Write-Host "Erstellt: $($exeInfo.LastWriteTime)" -ForegroundColor Gray
        }
    }
    else {
        Write-Host ""
        Write-Host "[FEHLER] Build fehlgeschlagen!" -ForegroundColor Red
        exit 1
    }
}
# Build mit MinGW
else {
    Write-Host ""
    Write-Host "Kompiliere mit MinGW g++..." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Kompiliere mit MinGW g++..." -ForegroundColor Yellow
    
    & g++ -g -std=c++17 HS80\HS80.cpp -o HS80\Debug\HS80.exe -lhid -lsetupapi
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "[ERFOLG] Build erfolgreich!" -ForegroundColor Green
        Write-Host "Executable: HS80\Debug\HS80.exe" -ForegroundColor Cyan
        
        if (Test-Path "HS80\Debug\HS80.exe") {
            $exeInfo = Get-Item "HS80\Debug\HS80.exe"
            Write-Host "Größe: $([math]::Round($exeInfo.Length/1KB, 2)) KB" -ForegroundColor Gray
            Write-Host "Erstellt: $($exeInfo.LastWriteTime)" -ForegroundColor Gray
        }
    }
    else {
        Write-Host ""
        Write-Host "[FEHLER] Build fehlgeschlagen!" -ForegroundColor Red
        exit 1
    }
}

Write-Host ""
