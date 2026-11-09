# Konfiguration

[Zurück zur README](../README.md)

Das Projekt verwendet keine Umgebungsvariablen, Konfigurationsdateien, Hooks oder Filter. Konfiguriert wird über Konstanten und Parameter.

## Konstanten (`HS80_Library.h`)

| Konstante | Wert | Beschreibung |
|---|---|---|
| `CORSAIR_VID` | `0x1B1C` | Vendor-ID Corsair |
| `HS80_WIRELESS_PID` | `0x0A6B` | Product-ID HS80 Wireless |
| `RGB_USAGE_PAGE` | `0xFF42` | Usage Page der Vendor-Interfaces |
| `RGB_USAGE` | `0x0001` | RGB-Interface |
| `EVENT_USAGE` | `0x0002` | Event-Interface |

## Laufzeitparameter

| Parameter | Standard | Beschreibung |
|---|---|---|
| `RGBController::connect(vid, pid)` | Corsair / Wireless-PID | Anderes Gerät wählen; bei `pid != HS80_WIRELESS_PID` wird Modus Wired (0x08) angenommen |
| `startKeepAlive(intervalMs)` | 5000 | Intervall des Farb-Refresh |
| `setBrightness(percent)` | 100 | 0-100 % (intern 0-1000) |
| `rainbow(durationMs, stepMs)` | 10000, 100 | Dauer und Schrittweite |
| `pulse(color, cycles, stepMs)` | Farbe, 3, 50 | Anzahl Zyklen, Schrittzeit |
| `HeadsetManager::connect(autoReconnect)` | true | Wird gespeichert, aber im Code nicht ausgewertet |
| `LEDZones()` Standardfarbe | RGB 0/155/222 | Corsair-Blau |

## Build-Konfiguration

| Option | Wert |
|---|---|
| C++-Standard | C++17 (`/std:c++17`, `-std=c++17`) |
| Link-Bibliotheken | `hid`, `setupapi` |
| MSVC-Flags in `build.ps1` | `/Zi /EHsc /nologo` |
| Ausgabeverzeichnis | `HS80\Debug` |

## Analyzer-Logging

Im Analyzer schaltet die Taste `L` das Logging in eine Datei `HS80_Log_<Datum>_<Zeit>.txt` im Arbeitsverzeichnis. Diese Dateien sollten nicht eingecheckt werden (ein Beispiel liegt im Repo; ggf. in `.gitignore` aufnehmen).

## Hooks / Filter

Nicht vorhanden. Einziger Erweiterungspunkt: `EventCallback` (`std::function<void(const HeadsetEvent&)>`).
