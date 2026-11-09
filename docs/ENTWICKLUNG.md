# Entwicklung

[Zurück zur README](../README.md)

## Lokale Umgebung

- Windows mit Visual Studio 2022 oder VS Code (`.vscode/` mit c_cpp_properties, launch und tasks vorhanden, Inhalt nicht geprüft)
- Windows SDK, optional MinGW-w64 und CMake
- Echtes HS80-Headset nötig; es gibt keinen Hardware-Mock

Build siehe [Installation](INSTALLATION.md).

## Code-Struktur

| Datei | Inhalt |
|---|---|
| `HS80_Library.h/.cpp` | Library: Discovery, `RGBController`, `EventMonitor`, `HeadsetManager` |
| `HS80_Demo.cpp` | Menügesteuerte Demo |
| `HS80_Analyzer.cpp` | Geräteliste, Interface-Anzeige, RGB-Testsuite, Event-Monitor, Logging |
| `HS80/HS80.cpp` | Eigenständiger Event-Leser mit Protokollausgabe |
| `HS80/READme.txt` | Mitschnitte und Protokollnotizen |
| `base_file/Corsair_Headset_Controller.js` | Referenz (SignalRGB) |

Achtung: Library-Dateien existieren doppelt (Root und `HS80/`) und sind nicht identisch. Vor Änderungen die Quelle der Wahrheit festlegen und Duplikate zusammenführen; `bak/` und `CMakeLists_new.txt` sind vermutlich Altstände.

## Tests

Es gibt keine automatisierten Tests. Manuelle Prüfung mit `HS80_Analyzer.exe`:

1. Menü 2: beide Interfaces (0x0001 und 0x0002) sichtbar
2. Menü 3: RGB-Testsuite
3. Menü 4: Event-Test (Mute, Akku, Laden)

Empfehlung: Event-Dekodierung (`HeadsetEvent`) mit Byte-Arrays aus `HS80/READme.txt` per Unit-Test absichern.

## Coding-Standards (aus dem Code abgeleitet)

- Namespace `HS80`, Klassen in PascalCase, Member mit Präfix `m_`
- Kommentare und Konsolenausgaben auf Deutsch
- Keine Exceptions, Fehler über `bool`-Rückgabe und `std::cerr`
- Thread-Sicherheit über `CRITICAL_SECTION`

Verbesserungsvorschläge: `snprintf` statt `sprintf`, `std::atomic<bool>` für Laufzeit-Flags, undefinierte Deklarationen (`KeepAliveThreadProc`, `keepAliveLoop`) entfernen, Logging statt direkter `std::cout`-Ausgaben, Callback-Aufruf vor Race beim Disconnect schützen.

## Release-Ablauf (Vorschlag, nicht im Repo definiert)

1. Debug- und Release-Build mit MSVC und MinGW prüfen
2. Manuelle Tests mit Hardware durchführen
3. Version in README/Demo anpassen (derzeit 1.0)
4. Git-Tag setzen und ggf. GitHub-Release mit `HS80_Lib.lib` und Headern erstellen
