# HS80 Library

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B) ![Plattform](https://img.shields.io/badge/Plattform-Windows-0078D6?logo=windows)

C++-Bibliothek zur Steuerung des Corsair HS80 RGB Wireless Gaming Headsets unter Windows (RGB-LEDs, Helligkeit, Events für Mute, Akku und Ladestatus) direkt über die Windows-HID-API.

## Features

- RGB-Steuerung der Zonen Logo, Power und Mic (einzeln oder alle gemeinsam)
- Helligkeitssteuerung (0-100 %)
- Software-Modus mit Keep-Alive-Thread, damit das Headset nicht in den Hardware-Modus zurückfällt
- Effekte: Regenbogen und Puls
- Rückkehr in den Hardware-Modus
- Asynchrones Event-Monitoring mit Callback: Mute, Akkustand, Ladestatus
- Automatische Erkennung des RGB-Interfaces (Usage 0x0001) und des Event-Interfaces (Usage 0x0002) auf Usage Page 0xFF42
- Beispielprogramme: interaktive Demo und Analyse-Tool

## Tech-Stack

- C++17
- Windows HID API (`hid.lib`, `setupapi.lib`, Windows SDK)
- Build: MSVC (Visual Studio 2022), MinGW-w64, CMake, Batch-/PowerShell-Skripte

## Schnellstart

### Voraussetzungen

- Windows 10/11
- Visual Studio 2022 (MSVC) oder MinGW-w64
- Windows SDK (hid.lib, setupapi.lib)
- Corsair HS80 RGB Wireless (VID 0x1B1C, PID 0x0A6B) mit Receiver
- iCUE sollte geschlossen sein (kann den Zugriff blockieren)

### Installation (Build)

```batch
build_library.bat
```

Erzeugt laut bisheriger Doku `HS80_Lib.lib`, `HS80.exe`, `HS80_Demo.exe` und `HS80_Analyzer.exe`.

Alternativ mit CMake:

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### Start

```powershell
.\HS80\Debug\HS80_Demo.exe
.\HS80\Debug\HS80_Analyzer.exe
```

### Minimalbeispiel

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    HeadsetManager manager;
    if (!manager.connect()) return 1;

    manager.rgb().initialize();
    manager.rgb().startKeepAlive(5000);
    manager.setLEDs(RGBColor(255, 0, 0));

    manager.startEventMonitoring([](const HeadsetEvent& e) {
        std::cout << e.getDescription() << std::endl;
    });

    manager.disconnect();
    return 0;
}
```

## Konfiguration

Das Projekt liest keine Umgebungsvariablen. Konfiguriert wird über Konstanten und Funktionsparameter (siehe [docs/KONFIGURATION.md](docs/KONFIGURATION.md)).

| Einstellung | Standard | Beschreibung |
|---|---|---|
| `CORSAIR_VID` | `0x1B1C` | Vendor-ID |
| `HS80_WIRELESS_PID` | `0x0A6B` | Product-ID |
| `startKeepAlive(intervalMs)` | `5000` | Keep-Alive-Intervall |
| `connect(autoReconnect)` | `true` | Auto-Reconnect-Flag (derzeit ohne Funktion) |

## Projektstruktur

```
.
├── HS80_Library.h/.cpp      # Bibliothek (Root-Kopie)
├── HS80_Demo.cpp            # Interaktive Demo
├── HS80_Analyzer.cpp        # Analyse-/Debug-Tool
├── HS80/                    # Visual-Studio-Projekt (HS80.sln), aktuellere Library-Kopie
│   ├── HS80.cpp, READme.txt # Original-Eventleser, Protokollnotizen
│   └── Corsair_Headset_Controller.js
├── base_file/               # SignalRGB-Referenzimplementierung (JS)
├── bak/                     # Altes Backup von HS80.cpp
├── CMakeLists.txt, CMakeLists_new.txt
├── build.bat, build.ps1, build_library.bat
├── .vscode/                 # VS-Code-Konfiguration
└── *.md                     # Weitere Guides (Build, Keep-Alive, Zonen, Events)
```

## Dokumentation

- [Architektur](docs/ARCHITEKTUR.md)
- [Installation](docs/INSTALLATION.md)
- [Konfiguration](docs/KONFIGURATION.md)
- [Entwicklung](docs/ENTWICKLUNG.md)
- [API](docs/API.md)

Weitere bestehende Guides im Repo: `BUILD_GUIDE.md`, `KEEP_ALIVE_GUIDE.md`, `BRIGHTNESS_ZONES_GUIDE.md`, `EVENT_DOCUMENTATION.md`, `LIBRARY_README.md`, `VSCODE_QUICKREF.md`.

## Fehlersuche

- Gerät nicht gefunden: Analyzer starten (Menüpunkt 2), ggf. als Administrator.
- RGB funktioniert nicht: iCUE schließen, `initialize()` prüfen.
- Keine Events: Headset einschalten, Mute-Taste testen.

## Hinweise des Autors

Dieses Projekt ist für Bildungs- und Entwicklungszwecke gedacht. Es basiert auf der SignalRGB-JavaScript-Implementierung (`Corsair_Headset_Controller.js`). Stand der Original-README: Oktober 2025, Version 1.0.
