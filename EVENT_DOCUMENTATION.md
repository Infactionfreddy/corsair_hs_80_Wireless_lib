# HS80 Event-System - Deutsche Übersetzung ✅

## 📊 Event-Format (HS80 Wireless)

### Paket-Struktur
```
Byte 0: 0x03        - Event-Header (Konstant)
Byte 1: 0x01        - Unbekannt
Byte 2: 0x01        - Unbekannt  
Byte 3: Event-Code  - 0xA6 (Mute), 0x0F (Battery), 0x10 (Charging)
Byte 4: 0x00        - Unbekannt
Byte 5: Wert        - Event-spezifischer Wert
Byte 6+: Padding
```

## 🎤 Mikrofon (Mute) Events

### Event-Code: `0xA6`

**Format:**
```
03 01 01 A6 00 [STATUS] 00 00 ...
```

**Status-Werte:**
- `0x00` = **Mikrofon AKTIV** (Unmuted)
- `0x01` = **Mikrofon STUMM** (Muted)

**Beispiele:**
```
03 01 01 A6 00 00 00 00  → "Mikrofon AKTIV"
03 01 01 A6 00 01 00 00  → "Mikrofon STUMM"
```

**Deutsche Ausgabe:**
```
+--------------------------------------------------------------+
| HEADSET EVENT                                                |
+--------------------------------------------------------------+
| Mikrofon AKTIV                                               |
| Mikrofon: AKTIV (Unmute)                                     |
+--------------------------------------------------------------+
| Raw: 03 01 01 A6 00 00 00 00 00 00 00 00                     |
+--------------------------------------------------------------+
```

## 🔋 Akku (Battery) Events

### Event-Code: `0x0F`

**Format:**
```
03 01 01 0F 00 [LEVEL] 00 00 ...
```

**Level-Werte:**
- `0x00` - `0x64` (0-100%) = Akku-Prozentsatz

**Beispiele:**
```
03 01 01 0F 00 64 00 00  → "Akku: 100%"  [|||||||||| VOLL]
03 01 01 0F 00 50 00 00  → "Akku: 80%"   [|||||||||| VOLL]
03 01 01 0F 00 32 00 00  → "Akku: 50%"   [|||||||--- MITTEL]
03 01 01 0F 00 14 00 00  → "Akku: 20%"   [||||----- NIEDRIG]
03 01 01 0F 00 05 00 00  → "Akku: 5%"    [|-------- KRITISCH]
```

**Deutsche Ausgabe:**
```
+--------------------------------------------------------------+
| HEADSET EVENT                                                |
+--------------------------------------------------------------+
| Akku: 80%                                                    |
| Akkustand: 80% [|||||||||| VOLL]                             |
+--------------------------------------------------------------+
| Raw: 03 01 01 0F 00 50 00 00 00 00 00 00                     |
+--------------------------------------------------------------+
```

## ⚡ Lade-Status (Charging) Events

### Event-Code: `0x10`

**Format:**
```
03 01 01 10 00 [STATUS] 00 00 ...
```

**Status-Werte:**
- `0x00` = **NICHT AM LADEN**
- `0x01` = **WIRD GELADEN**

**Beispiele:**
```
03 01 01 10 00 00 00 00  → "Nicht am Laden"
03 01 01 10 00 01 00 00  → "Wird geladen"
```

**Deutsche Ausgabe:**
```
+--------------------------------------------------------------+
| HEADSET EVENT                                                |
+--------------------------------------------------------------+
| Wird geladen                                                 |
| Ladestatus: WIRD GELADEN                                     |
+--------------------------------------------------------------+
| Raw: 03 01 01 10 00 01 00 00 00 00 00 00                     |
+--------------------------------------------------------------+
```

## 💻 API-Verwendung

### Event-Handler registrieren

```cpp
#include "HS80_Library.h"
using namespace HS80;

void onEvent(const HeadsetEvent& event) {
    // Deutsche Beschreibung
    std::cout << event.getDescription() << std::endl;
    
    // Event-Typ prüfen
    EventType actualType = event.getActualEventType();
    
    if (actualType == EventType::Mute) {
        if (event.isMuted()) {
            std::cout << "🔇 Mikrofon ist STUMM!" << std::endl;
        } else {
            std::cout << "🎤 Mikrofon ist AKTIV!" << std::endl;
        }
    }
    else if (actualType == EventType::Battery) {
        int level = event.getBatteryLevel();
        std::cout << "🔋 Akku: " << level << "%" << std::endl;
        
        if (level < 20) {
            std::cout << "⚠️  WARNUNG: Akkustand niedrig!" << std::endl;
        }
    }
    else if (actualType == EventType::Charging) {
        if (event.isCharging()) {
            std::cout << "⚡ Headset wird geladen..." << std::endl;
        }
    }
}

int main() {
    HeadsetManager manager;
    manager.connect();
    
    // Event-Monitoring starten
    manager.startEventMonitoring(onEvent);
    
    // Warten...
    Sleep(60000); // 60 Sekunden
    
    manager.disconnect();
    return 0;
}
```

### Verfügbare Hilfsfunktionen

```cpp
// In HeadsetEvent-Struktur:

// Event-Typ ermitteln (liest Byte 3)
EventType getActualEventType() const;

// Deutsche Beschreibung
std::string getDescription() const;

// Mikrofon-Status
bool isMuted() const;        // true wenn Byte 5 = 0x01
bool isUnmuted() const;      // true wenn Byte 5 = 0x00

// Akku-Level
int getBatteryLevel() const; // Wert von Byte 5 (0-100)

// Lade-Status
bool isCharging() const;     // true wenn Byte 5 = 0x01
```

## 🔍 Debug-Ausgabe (Analyzer)

Der Analyzer zeigt detaillierte Event-Informationen:

```
[EVENT] Paket=0x03, Event=0xA6 | Mikrofon AKTIV | Byte[5]=0x00 (AKTIV)
        Raw Data [8 bytes]: 03 01 01 A6 00 00 00 00
        Struktur: [Header=0x03][01][01][EventCode=0xA6][00][Wert=0x00]

[EVENT] Paket=0x03, Event=0x0F | Akku: 80% | Byte[5]=0x50 (80%)
        Raw Data [8 bytes]: 03 01 01 0F 00 50 02 00
        Struktur: [Header=0x03][01][01][EventCode=0x0F][00][Wert=0x50]

[EVENT] Paket=0x03, Event=0x10 | Wird geladen | Byte[5]=0x01 (LAEDT)
        Raw Data [8 bytes]: 03 01 01 10 00 01 00 00
        Struktur: [Header=0x03][01][01][EventCode=0x10][00][Wert=0x01]
```

## 📝 Event-Logging

Mit Logging aktiviert (Analyzer → L):

```
20:30:15.123 | [EVENT] Paket=0x03, Event=0xA6 | Mikrofon STUMM | Byte[5]=0x01 (STUMM)
20:30:15.123 |         Raw Data [8 bytes]: 03 01 01 A6 00 01 00 00
20:30:15.123 |         Struktur: [Header=0x03][01][01][EventCode=0xA6][00][Wert=0x01]
20:30:18.456 | [EVENT] Paket=0x03, Event=0xA6 | Mikrofon AKTIV | Byte[5]=0x00 (AKTIV)
20:30:18.456 |         Raw Data [8 bytes]: 03 01 01 A6 00 00 00 00
20:30:18.456 |         Struktur: [Header=0x03][01][01][EventCode=0xA6][00][Wert=0x00]
```

## 🎯 Praktische Beispiele

### Beispiel 1: Mute-LED synchronisieren

```cpp
void onEvent(const HeadsetEvent& event) {
    if (event.getActualEventType() == EventType::Mute) {
        if (event.isMuted()) {
            // Headset LED auf Rot setzen
            manager.setLEDs(RGBColor(255, 0, 0));
        } else {
            // Headset LED auf Grün setzen
            manager.setLEDs(RGBColor(0, 255, 0));
        }
    }
}
```

### Beispiel 2: Akku-Warnung mit RGB

```cpp
void onEvent(const HeadsetEvent& event) {
    if (event.getActualEventType() == EventType::Battery) {
        int level = event.getBatteryLevel();
        
        if (level < 10) {
            // Kritisch: Rot blinken
            for (int i = 0; i < 3; i++) {
                manager.setLEDs(RGBColor(255, 0, 0));
                Sleep(200);
                manager.setLEDs(RGBColor(0, 0, 0));
                Sleep(200);
            }
        }
        else if (level < 20) {
            // Niedrig: Orange
            manager.setLEDs(RGBColor(255, 128, 0));
        }
        else {
            // OK: Grün
            manager.setLEDs(RGBColor(0, 255, 0));
        }
    }
}
```

### Beispiel 3: Lade-Animation

```cpp
void onEvent(const HeadsetEvent& event) {
    if (event.getActualEventType() == EventType::Charging) {
        if (event.isCharging()) {
            // Lade-Animation (Puls Grün)
            manager.rgb().pulse(RGBColor(0, 255, 0), 99999, 50);
        } else {
            // Stoppe Animation
            manager.rgb().setHardwareMode();
        }
    }
}
```

## 📊 Event-Statistik

**Typische Event-Frequenz:**
- Mute-Events: Bei Tastendruck (sofort)
- Battery-Events: Alle ~30 Sekunden oder bei Statusänderung
- Charging-Events: Bei USB-Verbindung/Trennung

**Event-Größe:**
- Immer 65 Bytes (1 Byte Report-ID + 64 Bytes Daten)
- Nur erste 6-8 Bytes enthalten Nutzdaten

---

**Erstellt:** 26. Oktober 2025  
**Version:** 1.0  
**Status:** ✅ Vollständig implementiert und getestet
