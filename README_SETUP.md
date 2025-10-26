# HS80 Projekt - VS Code Setup

✅ **Das Projekt ist jetzt build-fähig unter VS Code!**

## 📋 Was wurde eingerichtet

### VS Code Konfigurationsdateien
- ✅ `.vscode/tasks.json` - Build-Tasks für MSVC, g++ und CMake
- ✅ `.vscode/launch.json` - Debug-Konfigurationen
- ✅ `.vscode/c_cpp_properties.json` - IntelliSense-Konfiguration
- ✅ `.vscode/settings.json` - Workspace-Einstellungen

### Build-Dateien
- ✅ `CMakeLists.txt` - CMake-Konfiguration
- ✅ `build.bat` - Windows Batch Build-Skript
- ✅ `build.ps1` - PowerShell Build-Skript
- ✅ `.gitignore` - Git Ignore-Regeln

### Dokumentation
- ✅ `BUILD_GUIDE.md` - Detaillierte Build-Anleitung

## 🚀 Schnellstart

### Voraussetzungen

Du benötigst einen C++ Compiler. **Wähle eine Option:**

**Option 1: MSVC (empfohlen für Windows)**
- Installiere "Build Tools für Visual Studio 2022" von [visualstudio.microsoft.com](https://visualstudio.microsoft.com/de/downloads/)
- Oder installiere Visual Studio Community 2022 mit "Desktop-Entwicklung mit C++"

**Option 2: MinGW-w64**
- Lade MinGW-w64 von [winlibs.com](https://winlibs.com/) herunter
- Extrahiere es und füge den `bin`-Ordner zum PATH hinzu

### Build-Anleitung

#### Mit MSVC:
1. **Öffne "Developer Command Prompt for VS 2022"** (wichtig!)
   - Start → "Developer Command Prompt for VS 2022"
2. Navigiere zum Projekt:
   ```cmd
   cd "C:\Users\Freddy\Desktop\HS_80+\HS80"
   ```
3. Öffne VS Code:
   ```cmd
   code .
   ```
4. Bauen:
   - **Einfachste Methode:** Drücke `Ctrl+Shift+B`
   - **Oder:** Terminal → `.\build.bat`
   - **Oder:** Task ausführen → `Strg+Shift+P` → "Tasks: Run Build Task"

#### Mit MinGW:
1. Öffne PowerShell im Projektordner
2. Öffne VS Code:
   ```powershell
   code .
   ```
3. Drücke `Ctrl+Shift+B` und wähle "C++: Build HS80 (g++)"

#### Mit CMake:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
```

### Debuggen

1. Drücke `F5` in VS Code
2. Wähle "Debug HS80 (MSVC)" oder "Debug HS80 (GDB)"
3. Das Programm wird automatisch kompiliert und gestartet

**Hinweis:** Das Programm benötigt möglicherweise Administrator-Rechte für HID-Zugriff.

## 📁 Projektstruktur

```
HS80/
├── .vscode/                    # VS Code Konfiguration
│   ├── tasks.json             # Build-Tasks
│   ├── launch.json            # Debug-Konfiguration
│   ├── c_cpp_properties.json  # IntelliSense
│   └── settings.json          # Workspace-Einstellungen
├── HS80/
│   ├── HS80.cpp              # Hauptprogramm (HID Reader)
│   ├── HS80.vcxproj          # Visual Studio Projekt
│   └── Debug/                # Build-Ausgabe
├── bak/                       # Backup-Dateien
├── CMakeLists.txt            # CMake-Konfiguration
├── build.bat                 # Batch Build-Skript
├── build.ps1                 # PowerShell Build-Skript
├── BUILD_GUIDE.md            # Detaillierte Anleitung
├── .gitignore                # Git Ignore
└── README_SETUP.md           # Diese Datei
```

## 🎯 Verfügbare Build-Tasks (Ctrl+Shift+B)

1. **C++: Build HS80 (MSVC)** ⭐ - Standard, kompiliert mit MSVC
2. **C++: Build HS80 (g++)** - Kompiliert mit MinGW g++
3. **C++: Clean Build** - Löscht Build-Artefakte
4. **CMake: Configure** - Konfiguriert CMake
5. **CMake: Build** - Baut mit CMake

## 🔧 Fehlerbehebung

### "cl.exe nicht gefunden"
→ Öffne VS Code aus der "Developer Command Prompt for VS"
→ Oder installiere Visual Studio Build Tools

### "g++ nicht gefunden"
→ Installiere MinGW-w64 und füge es zum PATH hinzu

### IntelliSense zeigt Fehler
→ Öffne `.vscode/c_cpp_properties.json`
→ Passe die Windows SDK Pfade an deine Installation an

### "Cannot open include file 'windows.h'"
→ MSVC ist nicht richtig konfiguriert
→ Öffne VS Code aus der Developer Command Prompt

## 📚 Weitere Informationen

- **Detaillierte Build-Anleitung:** Siehe `BUILD_GUIDE.md`
- **Quellcode:** `HS80/HS80.cpp`
- **Projekt-Details:** Siehe `HS80/READme.txt`

## 💡 Tipps

- **Schneller Build:** Nutze die Tastenkombination `Ctrl+Shift+B`
- **Quick Debug:** Drücke `F5` für sofortiges Debuggen
- **Task-Übersicht:** `Strg+Shift+P` → "Tasks: Run Task"
- **Terminal öffnen:** `` Strg+` `` (Backtick)

## ✨ C++ Extension Features

Die C/C++ Extension ist bereits installiert und bietet:
- ✅ IntelliSense (Code-Vervollständigung)
- ✅ Code-Navigation (Gehe zu Definition)
- ✅ Syntax-Highlighting
- ✅ Debugging mit Breakpoints
- ✅ Code-Formatierung

---

**Status:** ✅ Build-fähig | 🔧 Bereit für Entwicklung | 🐛 Debugger konfiguriert

Bei Fragen oder Problemen siehe `BUILD_GUIDE.md` für detaillierte Informationen.
