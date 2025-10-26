
# HS80 Library - Corsair HS80 RGB Wireless Gaming Headset

Eine C++ Library zur Steuerung des Corsair HS80 RGB Wireless Gaming Headsets unter Windows.

## 📦 Features

### RGB-Steuerung
- ✅ Vollständige RGB-LED Kontrolle (Logo, Power, Mic)
- ✅ Software-Modus Initialisierung
- ✅ Einzelne Farben oder Zonen-basierte Beleuchtung
- ✅ Vordefinierte Effekte (Regenbogen, Puls)
- ✅ Hardware-Modus Wiederherstellung

### Event-Monitoring
- ✅ Echtzeit-Event-Überwachung
- ✅ Mute-Status (On/Off)
- ✅ Batterie-Level (0-100%)
- ✅ Lade-Status
- ✅ Callback-basiertes Event-System

### Automatische Interface-Erkennung
- ✅ Automatisches Finden des RGB-Interfaces (Usage 0x0001)
- ✅ Automatisches Finden des Event-Interfaces (Usage 0x0002)
- ✅ Kein manuelles Interface-Wählen notwendig

## 🏗️ Architektur

```
HS80_Library.h/cpp
├── RGBController        - RGB-LED Steuerung
├── EventMonitor         - Event-Überwachung (async)
├── HeadsetManager       - High-Level Interface
└── Device Discovery     - HID-Geräteerkennung
```

### Interface-Mapping

**HS80 Wireless (PID 0x0A6B):**
```
Interface 6: Usage Page 0xFF42, Usage 0x0001 → RGB-Kommandos
Interface 7: Usage Page 0xFF42, Usage 0x0002 → Events (Mute, Battery)
```

## 🚀 Quick Start

### 1. Library nutzen

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    // Manager erstellen (verbindet automatisch beide Interfaces)
    HeadsetManager manager;
    
    if (!manager.connect()) {
        std::cerr << "Fehler beim Verbinden!" << std::endl;
        return 1;
    }
    
    // RGB initialisieren
    manager.rgb().initialize();
    
    // Farbe setzen
    manager.setLEDs(RGBColor(255, 0, 0)); // Rot
    
    // Event-Monitoring starten
    manager.startEventMonitoring([](const HeadsetEvent& event) {
        if (event.type == EventType::Mute) {
            std::cout << "Mute: " << event.isMuted() << std::endl;
        }
    });
    
    // Aufräumen
    manager.disconnect();
    return 0;
}
```

### 2. Build

#### Methode 1: Batch-Script (Einfach)
```batch
build_library.bat
```

Erstellt:
- `HS80_Lib.lib` - Statische Library
- `HS80.exe` - Original Programm mit Library
- `HS80_Demo.exe` - Demo mit High-Level API
- `HS80_Analyzer.exe` - Analyse-Tool

#### Methode 2: CMake
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

## 📚 API Referenz

### HeadsetManager (High-Level)

```cpp
HeadsetManager manager;

// Verbindung
bool connect(bool autoReconnect = true);
void disconnect();
bool isConnected() const;

// RGB-Zugriff
RGBController& rgb();

// Event-Zugriff
EventMonitor& events();

// Schnellzugriff
bool setLEDs(RGBColor color);
bool setLEDs(const LEDZones& zones);
bool startEventMonitoring(EventCallback callback);
```

### RGBController

```cpp
RGBController rgb;

// Verbindung
bool connect(unsigned short vid = CORSAIR_VID, 
             unsigned short pid = HS80_WIRELESS_PID);
void disconnect();

// RGB-Kontrolle
bool initialize();                  // Aktiviert Software-Modus
bool setColors(const LEDZones& zones);
bool setColor(RGBColor color);
bool setHardwareMode();            // Zurück zu Hardware-Steuerung

// Effekte
bool rainbow(int durationMs = 10000, int stepMs = 100);
bool pulse(RGBColor color, int cycles = 3, int stepMs = 50);
bool off();
```

### EventMonitor

```cpp
EventMonitor events;

// Verbindung
bool connect(unsigned short vid = CORSAIR_VID,
             unsigned short pid = HS80_WIRELESS_PID);
void disconnect();

// Monitoring
bool startMonitoring(EventCallback callback);
void stopMonitoring();
bool isMonitoring() const;
```

### Datenstrukturen

```cpp
// RGB-Farbe
struct RGBColor {
    unsigned char r, g, b;
    RGBColor(unsigned char red, unsigned char green, unsigned char blue);
};

// LED-Zonen (Logo, Power LED, Mic LED)
struct LEDZones {
    RGBColor logo;
    RGBColor power;
    RGBColor mic;
};

// Headset-Event
struct HeadsetEvent {
    EventType type;
    unsigned char data[64];
    size_t dataSize;
    
    bool isMuted() const;
    int getBatteryLevel() const;  // -1 wenn nicht verfügbar
    bool isCharging() const;
};

// Event-Typen
enum class EventType {
    Unknown  = 0x00,
    Mute     = 0xA6,
    Battery  = 0x0F,
    Charging = 0x10
};
```

## 🎯 Beispielprogramme

### HS80_Demo.exe
Interaktives Demo-Programm mit Menü:
- RGB-Farbwahl (1-9)
- Effekte (Regenbogen, Puls)
- Zonen-Test (verschiedene Farben pro LED)
- Live Event-Monitoring

### HS80_Analyzer.exe
Analyse- und Debugging-Tool:
- Alle HID-Geräte anzeigen
- HS80-Interfaces im Detail
- RGB Test-Suite
- Event Monitor Test (30s Recording)
- Optional: Logging in Datei

### Verwendung

```powershell
# Demo starten
.\HS80\Debug\HS80_Demo.exe

# Analyzer starten
.\HS80\Debug\HS80_Analyzer.exe
```

## 🔧 Low-Level Details

### RGB-Protokoll

**Initialisierung:**
```cpp
// Paket 1: Software-Modus
[0x02, 0x09, 0x00, ...] // 0x09 = Wireless, 0x08 = Wired

// Paket 2: Lighting öffnen
[0x02, 0x01, 0x00, ...]

// Paket 3: Helligkeit
[0x02, 0x03, 0x02, 0x01, 0x64, ...] // 0x64 = 100%
```

**RGB setzen:**
```cpp
[0x02, 0x09, 0x06, 0x00, 0x09, 0x00, 0x00, 0x00,
 R, R, R,    // Logo Rot (3x)
 G, G, G,    // Logo Grün (3x)
 B, B, B,    // Logo Blau (3x)
 R, R, R,    // Power Rot (3x)
 G, G, G,    // Power Grün (3x)
 B, B, B,    // Power Blau (3x)
 R, R, R,    // Mic Rot (3x)
 G, G, G,    // Mic Grün (3x)
 B, B, B]    // Mic Blau (3x)
```

### Event-Format

```cpp
// Mute Event
[0xA6, 0x01] // Muted
[0xA6, 0x00] // Unmuted

// Battery Event
[0x0F, <level>] // level = 0-100

// Charging Event
[0x10, 0x01] // Charging
[0x10, 0x00] // Not charging
```

## 🐛 Debugging

### Gerät wird nicht gefunden
```powershell
# Als Administrator ausführen
.\HS80\Debug\HS80_Analyzer.exe

# Option 2: Zeige HS80 Interfaces
# Prüft ob beide Interfaces gefunden werden
```

### RGB funktioniert nicht
1. Prüfe dass das richtige Interface geöffnet wird (Usage 0x0001)
2. Stelle sicher dass iCUE geschlossen ist (verhindert Zugriff)
3. Überprüfe dass `initialize()` erfolgreich war

### Events werden nicht empfangen
1. Prüfe dass das Event-Interface geöffnet wird (Usage 0x0002)
2. Stelle sicher dass das Headset eingeschaltet ist
3. Teste mit Mute-Taste am Headset

## 📋 Anforderungen

- **OS:** Windows 10/11
- **Compiler:** MSVC (Visual Studio 2022) oder MinGW-w64
- **C++ Standard:** C++17 oder höher
- **Libraries:** hid.lib, setupapi.lib (Windows SDK)

## 🔗 Referenzen

Basiert auf der SignalRGB JavaScript-Implementierung:
- `Corsair_Headset_Controller.js`
- HS80 Wireless Endpoint-Definition (Interface 3, Collections 0x0004/0x0005)

## 📝 Lizenz

Dieses Projekt ist für Bildungs- und Entwicklungszwecke.

---

**Erstellt:** Oktober 2025  
**Version:** 1.0  
**Hardware:** Corsair HS80 RGB Wireless Gaming Receiver (VID 0x1B1C, PID 0x0A6B)

