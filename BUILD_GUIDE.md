# HS80 HID Device Reader

Ein C++ Programm zum Auslesen von HID-Geräten (speziell Corsair HS80 Headset) unter Windows.

## Voraussetzungen

### Option 1: MSVC (Microsoft Visual C++)
- Visual Studio 2019 oder neuer (oder Build Tools für Visual Studio)
- Windows SDK 10.0 oder neuer
- Developer Command Prompt für VS oder VS Code mit MSVC-Umgebung

### Option 2: MinGW/g++
- MinGW-w64 installiert
- g++ im PATH verfügbar

## Build in VS Code

### Schnellstart
1. **Öffne VS Code aus einer Developer Command Prompt** (für MSVC):
   - Starte "Developer Command Prompt for VS 2022" oder "Developer PowerShell for VS 2022"
   - Navigiere zum Projektordner: `cd "C:\Users\Freddy\Desktop\HS_80+\HS80"`
   - Öffne VS Code: `code .`

2. **Build starten**:
   - Drücke `Ctrl+Shift+B` (Standard-Build-Task)
   - Oder: `Strg+Shift+P` → "Tasks: Run Build Task" → Wähle gewünschte Build-Methode

### Build-Optionen

#### 1. Mit MSVC (empfohlen)
- Drücke `Ctrl+Shift+B` und wähle **"C++: Build HS80 (MSVC)"**
- Oder nutze das Build-Skript: `.\build.bat` oder `.\build.ps1`

#### 2. Mit MinGW/g++
- Drücke `Ctrl+Shift+B` und wähle **"C++: Build HS80 (g++)"**

#### 3. Mit CMake
- Drücke `Ctrl+Shift+B` und wähle **"CMake: Build"**
- Oder manuell:
  ```powershell
  cmake -S . -B build -G "Visual Studio 17 2022"
  cmake --build build --config Debug
  ```

### Debug
- Drücke `F5` um das Programm zu debuggen
- Wähle "Debug HS80 (MSVC)" oder "Debug HS80 (GDB)" je nach Compiler
- Das Programm wird automatisch kompiliert vor dem Debuggen

## Manueller Build

### MSVC
```powershell
cl.exe /Zi /EHsc /std:c++17 HS80\HS80.cpp hid.lib setupapi.lib /Fe:HS80\Debug\HS80.exe
```

### MinGW
```powershell
g++ -g -std=c++17 HS80\HS80.cpp -o HS80\Debug\HS80.exe -lhid -lsetupapi
```

## Ausführen

```powershell
.\HS80\Debug\HS80.exe
```

**Hinweis:** Das Programm benötigt möglicherweise Administrator-Rechte für den Zugriff auf HID-Geräte.

## Projektstruktur

```
HS80/
├── .vscode/
│   ├── tasks.json           # Build-Aufgaben
│   ├── launch.json          # Debug-Konfiguration
│   ├── c_cpp_properties.json # IntelliSense-Konfiguration
│   └── settings.json        # Workspace-Einstellungen
├── HS80/
│   ├── HS80.cpp            # Hauptprogramm
│   ├── HS80.vcxproj        # Visual Studio Projekt
│   └── Debug/              # Build-Ausgabe
└── BUILD_GUIDE.md          # Diese Datei
```

## Fehlerbehebung

### "cl.exe nicht gefunden"
- Öffne VS Code aus einer "Developer Command Prompt" oder "Developer PowerShell"
- Alternativ: Installiere die MSVC Build Tools

### "g++ nicht gefunden"
- Installiere MinGW-w64
- Füge den MinGW bin-Ordner zum PATH hinzu

### IntelliSense-Fehler
- Öffne `.vscode\c_cpp_properties.json`
- Passe die Pfade zu den Windows SDK Include-Verzeichnissen an

## Verwendete Bibliotheken

- Windows HID API (`hid.lib`)
- Setup API (`setupapi.lib`)
- Standard C++ Bibliotheken (C++17)
