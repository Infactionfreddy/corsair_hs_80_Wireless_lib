# Installation

[Zurück zur README](../README.md)

Das Projekt ist eine Bibliothek zum Selbstkompilieren; es gibt kein Installationspaket. Der Inhalt von `CMakeLists.txt` und `build_library.bat` wurde nicht geprüft; Details sind Annahmen auf Basis der bestehenden Doku.

## Voraussetzungen

- Windows 10/11
- Compiler: MSVC (Visual Studio 2022, Workload C++) oder MinGW-w64
- Windows SDK mit `hid.lib` und `setupapi.lib`
- C++17
- Optional: CMake
- Corsair HS80 RGB Wireless (VID 0x1B1C, PID 0x0A6B), iCUE beendet

## Schritt für Schritt

1. Repository klonen:
   ```powershell
   git clone https://github.com/Infactionfreddy/corsair_hs_80_Wireless_lib.git
   cd corsair_hs_80_Wireless_lib
   ```
2. Bauen, eine der Varianten:
   - **Batch:** `build_library.bat` (Developer-Eingabeaufforderung empfohlen)
   - **PowerShell:** `.\build.ps1` sucht MSVC (PATH, vswhere, Standardpfade) bzw. MinGW und baut nur `HS80\HS80.cpp` nach `HS80\Debug\HS80.exe`
   - **CMake:**
     ```powershell
     mkdir build; cd build
     cmake ..
     cmake --build . --config Debug
     ```
   - **Visual Studio:** `HS80.sln` öffnen und Projekt bauen
3. Headset/Receiver anschließen und iCUE schließen.
4. Testen:
   ```powershell
   .\HS80\Debug\HS80_Analyzer.exe
   .\HS80\Debug\HS80_Demo.exe
   ```
   Bei Zugriffsfehlern als Administrator starten.

## Einbindung in eigene Projekte

`HS80_Library.h` und `HS80_Library.cpp` ins Projekt aufnehmen (oder `HS80_Lib.lib` linken). `hid.lib` und `setupapi.lib` werden per `#pragma comment` eingebunden. Empfohlen ist die Variante aus `HS80/` (aktueller Akku-Parser).

## Update

```powershell
git pull
```
anschließend neu bauen.

## Deinstallation

Build-Ordner (`build/`, `HS80/Debug/`) und das Repository-Verzeichnis löschen. Es werden keine Systemänderungen vorgenommen. Hinweis: Falls ein Programm abstürzt, während der Software-Modus aktiv ist, Headset kurz aus- und einschalten bzw. `setHardwareMode()` aufrufen.
