# HS80 - VS Code Schnellreferenz

## 🎯 Wichtigste Tastenkombinationen

### Build & Run
| Aktion | Tastenkombination | Beschreibung |
|--------|------------------|--------------|
| **Build starten** | `Ctrl+Shift+B` | Startet die Standard-Build-Task (MSVC) |
| **Debuggen** | `F5` | Kompiliert und startet den Debugger |
| **Ohne Debug ausführen** | `Ctrl+F5` | Startet ohne Debugger |
| **Build stoppen** | `Ctrl+C` (im Terminal) | Stoppt laufenden Build |

### Tasks
| Aktion | Tastenkombination | Beschreibung |
|--------|------------------|--------------|
| **Task ausführen** | `Strg+Shift+P` → "Tasks: Run Task" | Zeigt alle verfügbaren Tasks |
| **Build-Task** | `Strg+Shift+P` → "Tasks: Run Build Task" | Zeigt nur Build-Tasks |
| **Standard-Build** | `Ctrl+Shift+B` | Führt Standard-Build aus (MSVC) |

### Editor
| Aktion | Tastenkombination | Beschreibung |
|--------|------------------|--------------|
| **Gehe zu Definition** | `F12` | Springt zur Funktions-/Variablen-Definition |
| **Alle Referenzen** | `Shift+F12` | Zeigt alle Verwendungen eines Symbols |
| **Symbol umbenennen** | `F2` | Benennt Symbol im gesamten Projekt um |
| **Fehler anzeigen** | `F8` | Springt zum nächsten Fehler |
| **Quick Fix** | `Ctrl+.` | Zeigt Lösungsvorschläge |

### Terminal
| Aktion | Tastenkombination | Beschreibung |
|--------|------------------|--------------|
| **Terminal öffnen** | `` Strg+` `` | Öffnet/schließt integriertes Terminal |
| **Neues Terminal** | `Strg+Shift+` `` | Öffnet neues Terminal |
| **Terminal wechseln** | Im Terminal-Dropdown auswählen | - |

### Debugging
| Aktion | Tastenkombination | Beschreibung |
|--------|------------------|--------------|
| **Debuggen starten** | `F5` | Startet Debugger |
| **Breakpoint setzen** | `F9` | Setzt/entfernt Breakpoint in aktueller Zeile |
| **Schritt über** | `F10` | Führt nächste Zeile aus |
| **Schritt hinein** | `F11` | Springt in Funktion hinein |
| **Schritt heraus** | `Shift+F11` | Springt aus Funktion heraus |
| **Weiter** | `F5` | Führt bis zum nächsten Breakpoint aus |
| **Stoppen** | `Shift+F5` | Stoppt Debugger |

### Navigation
| Aktion | Tastenkombination | Beschreibung |
|--------|------------------|--------------|
| **Datei öffnen** | `Strg+P` | Schnelles Öffnen von Dateien |
| **Symbol suchen** | `Strg+T` | Sucht nach Funktionen, Klassen, etc. |
| **Gehe zu Zeile** | `Strg+G` | Springt zu bestimmter Zeile |
| **Zurück** | `Alt+←` | Zur vorherigen Position |
| **Vorwärts** | `Alt+→` | Zur nächsten Position |

## 🔨 Verfügbare Build-Tasks

Drücke `Ctrl+Shift+B` und wähle eine der folgenden Tasks:

1. **C++: Build HS80 (MSVC)** ⭐ [Standard]
   - Kompiliert mit Microsoft Visual C++
   - Benötigt: Developer Command Prompt

2. **C++: Build HS80 (g++)**
   - Kompiliert mit MinGW g++
   - Benötigt: MinGW-w64 im PATH

3. **C++: Clean Build**
   - Löscht alle Build-Artefakte
   - Bereinigt Debug-Ordner

4. **CMake: Configure**
   - Konfiguriert CMake-Projekt
   - Erstellt Build-System

5. **CMake: Build**
   - Baut Projekt mit CMake
   - Führt automatisch Configure aus

## 🐛 Debug-Konfigurationen

Drücke `F5` und wähle:

- **Debug HS80 (MSVC)** - Mit MSVC Debugger (cppvsdbg)
- **Debug HS80 (GDB)** - Mit GDB (MinGW)

## 💻 Terminal-Befehle

### Manueller Build
```powershell
# Mit PowerShell Build-Skript
.\build.ps1

# Mit Batch-Skript
.\build.bat

# Oder direkt mit MSVC (in Developer Command Prompt)
cl.exe /Zi /EHsc /std:c++17 HS80\HS80.cpp hid.lib setupapi.lib /Fe:HS80\Debug\HS80.exe

# Oder mit g++ (wenn MinGW installiert)
g++ -g -std=c++17 HS80\HS80.cpp -o HS80\Debug\HS80.exe -lhid -lsetupapi
```

### Ausführen
```powershell
# Programm starten
.\HS80\Debug\HS80.exe

# Als Administrator (falls HID-Zugriff benötigt)
Start-Process -FilePath ".\HS80\Debug\HS80.exe" -Verb RunAs
```

## 📝 Tipps & Tricks

### IntelliSense
- Tippe `Ctrl+Space` für Code-Vervollständigung
- Hovere über Code für Dokumentation
- `Ctrl+Shift+Space` zeigt Parameter-Hilfe

### Probleme Panel
- `Ctrl+Shift+M` öffnet Probleme-Panel
- Zeigt Compile-Fehler und Warnungen

### Multi-Cursor
- `Alt+Click` für mehrere Cursor
- `Ctrl+D` wählt nächstes Vorkommen aus
- `Ctrl+Shift+L` wählt alle Vorkommen aus

### Kommentare
- `Ctrl+K Ctrl+C` - Kommentiert Zeile(n)
- `Ctrl+K Ctrl+U` - Entfernt Kommentar
- `Ctrl+/` - Toggle Zeilen-Kommentar

## 🔧 Wichtige Dateien

| Datei | Zweck |
|-------|-------|
| `HS80/HS80.cpp` | Hauptquellcode |
| `.vscode/tasks.json` | Build-Task-Definitionen |
| `.vscode/launch.json` | Debug-Konfigurationen |
| `.vscode/c_cpp_properties.json` | IntelliSense-Einstellungen |
| `CMakeLists.txt` | CMake-Konfiguration |
| `build.ps1` / `build.bat` | Build-Skripte |

## ⚙️ Einstellungen anpassen

- `Ctrl+,` - Öffnet Einstellungen
- `Strg+Shift+P` → "Preferences: Open Workspace Settings (JSON)"

## 🆘 Häufige Probleme

### Build schlägt fehl mit "cl.exe nicht gefunden"
**Lösung:** Öffne VS Code aus der Developer Command Prompt:
```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
code "C:\Users\Freddy\Desktop\HS_80+\HS80"
```

### IntelliSense zeigt Fehler bei `#include <windows.h>`
**Lösung:** 
1. Öffne `.vscode/c_cpp_properties.json`
2. Prüfe die Pfade zu den Windows SDK Include-Verzeichnissen
3. Ggf. anpassen an deine Installation

### Programm benötigt Admin-Rechte
**Lösung:** Starte VS Code als Administrator oder führe die .exe manuell als Admin aus

---

**Tipp:** Drucke diese Referenz aus oder halte sie als Tab offen! 📌
