# HS80 Brightness & Zonen-Kontrolle 🎨💡

## 🎯 Neue Features

Die Library unterstützt jetzt:

1. **Helligkeit (Brightness)** - Kontrolle von 0-100%
2. **Einzelne Zonen** - Logo, Power-LED, Mikrofon separat ansteuern

---

## 💡 Brightness-Kontrolle

### Helligkeit setzen (0-100%)

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    HeadsetManager manager;
    manager.connect();
    manager.rgb().initialize();
    manager.rgb().startKeepAlive();
    
    // Helligkeit setzen
    manager.setBrightness(100);  // 100% - Volle Helligkeit
    manager.setBrightness(50);   // 50%  - Mittlere Helligkeit
    manager.setBrightness(10);   // 10%  - Gedimmt
    manager.setBrightness(0);    // 0%   - Aus (aber Software-Modus aktiv)
    
    return 0;
}
```

### Raw-Wert nutzen (0-1000)

Der HS80 verwendet intern Werte von 0-1000:

```cpp
RGBController rgb;
rgb.connect();
rgb.initialize();

// Raw-Wert setzen (Corsair-Protokoll)
rgb.setBrightnessRaw(1000);  // 100%
rgb.setBrightnessRaw(500);   // 50%
rgb.setBrightnessRaw(0);     // 0%

// Raw-Wert auslesen
int raw = rgb.getBrightnessRaw();  // 0-1000
std::cout << "Brightness (Raw): " << raw << std::endl;
```

### Helligkeit auslesen

```cpp
// Prozent auslesen (0-100%)
int percent = manager.rgb().getBrightness();
std::cout << "Helligkeit: " << percent << "%" << std::endl;

// Raw-Wert auslesen (0-1000)
int raw = manager.rgb().getBrightnessRaw();
std::cout << "Helligkeit (Raw): " << raw << "/1000" << std::endl;
```

### Interaktive Helligkeit

```cpp
// Heller (+10%)
int current = rgb.getBrightness();
rgb.setBrightness(current + 10);

// Dunkler (-10%)
int current = rgb.getBrightness();
rgb.setBrightness(current - 10);

// Automatische Begrenzung auf 0-100%
rgb.setBrightness(150);  // Wird auf 100% begrenzt
rgb.setBrightness(-50);  // Wird auf 0% begrenzt
```

---

## 🎨 Einzelne Zonen-Kontrolle

### Die 3 LED-Zonen

```
┌────────────────────────────────────┐
│                                    │
│   [LOGO]                           │  ← Logo-LED (oben am Headset)
│                                    │
│   [POWER]                          │  ← Power-LED (Seite)
│                                    │
│   [MIC]                            │  ← Mikrofon-LED (Mikrofonarm)
│                                    │
└────────────────────────────────────┘
```

### Einzelne Zone setzen

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    HeadsetManager manager;
    manager.connect();
    manager.rgb().initialize();
    manager.rgb().startKeepAlive();
    
    // Nur Logo-LED ändern (andere bleiben unverändert)
    manager.setZone(LEDZone::Logo, RGBColor(255, 0, 0));  // Logo → Rot
    
    // Nur Power-LED ändern
    manager.setZone(LEDZone::Power, RGBColor(0, 255, 0)); // Power → Grün
    
    // Nur Mikrofon-LED ändern
    manager.setZone(LEDZone::Mic, RGBColor(0, 0, 255));   // Mic → Blau
    
    // Alle Zonen auf einmal
    manager.setZone(LEDZone::All, RGBColor(255, 255, 0)); // Alle → Gelb
    
    return 0;
}
```

### Convenience-Funktionen

```cpp
RGBController rgb;
rgb.connect();
rgb.initialize();

// Direkte Zonen-Funktionen
rgb.setLogoColor(RGBColor(255, 0, 0));   // Logo → Rot
rgb.setPowerColor(RGBColor(0, 255, 0));  // Power → Grün
rgb.setMicColor(RGBColor(0, 0, 255));    // Mic → Blau
```

### LEDZone Enum

```cpp
enum class LEDZone {
    Logo = 0,    // Logo-LED (oben)
    Power = 1,   // Power-LED (Seite)
    Mic = 2,     // Mikrofon-LED
    All = 255    // Alle Zonen gleichzeitig
};
```

---

## 🔄 Kombination: Zonen + Helligkeit

### Beispiel 1: Status-Anzeige mit Helligkeit

```cpp
RGBController rgb;
rgb.connect();
rgb.initialize();
rgb.startKeepAlive();

// Gedimmte Status-LEDs
rgb.setBrightness(30);  // 30% Helligkeit

// Logo = Blau (Online)
rgb.setLogoColor(RGBColor(0, 0, 255));

// Power = Grün (Eingeschaltet)
rgb.setPowerColor(RGBColor(0, 255, 0));

// Mikrofon = Rot (Stumm)
rgb.setMicColor(RGBColor(255, 0, 0));

std::cout << "Status-Anzeige aktiv mit 30% Helligkeit" << std::endl;
```

### Beispiel 2: Mute-Indikator

```cpp
void onMuteEvent(const HeadsetEvent& event) {
    if (event.isMuted()) {
        // Muted: Mikrofon-LED ROT, volle Helligkeit
        manager.setBrightness(100);
        manager.setZone(LEDZone::Mic, RGBColor(255, 0, 0));
    } else {
        // Unmuted: Mikrofon-LED GRÜN, gedimmt
        manager.setBrightness(50);
        manager.setZone(LEDZone::Mic, RGBColor(0, 255, 0));
    }
}
```

### Beispiel 3: Akku-Warnung

```cpp
void onBatteryEvent(const HeadsetEvent& event) {
    int level = event.getBatteryLevel();
    
    if (level < 10) {
        // Kritisch: Rot, blinkend (durch Helligkeit)
        for (int i = 0; i < 5; i++) {
            manager.setBrightness(100);
            Sleep(200);
            manager.setBrightness(0);
            Sleep(200);
        }
        manager.setLEDs(RGBColor(255, 0, 0));
    }
    else if (level < 20) {
        // Niedrig: Orange, gedimmt
        manager.setBrightness(50);
        manager.setLEDs(RGBColor(255, 128, 0));
    }
    else {
        // OK: Grün, normal
        manager.setBrightness(70);
        manager.setLEDs(RGBColor(0, 255, 0));
    }
}
```

### Beispiel 4: Zonen-Animation

```cpp
void zonenAnimation() {
    RGBController rgb;
    rgb.connect();
    rgb.initialize();
    rgb.startKeepAlive();
    
    // Helligkeit für Effekt
    rgb.setBrightness(80);
    
    // Zone für Zone durchgehen
    for (int i = 0; i < 3; i++) {
        // Logo
        rgb.setLogoColor(RGBColor(255, 0, 0));
        Sleep(300);
        rgb.setLogoColor(RGBColor(0, 0, 0));
        
        // Power
        rgb.setPowerColor(RGBColor(0, 255, 0));
        Sleep(300);
        rgb.setPowerColor(RGBColor(0, 0, 0));
        
        // Mic
        rgb.setMicColor(RGBColor(0, 0, 255));
        Sleep(300);
        rgb.setMicColor(RGBColor(0, 0, 0));
    }
    
    // Alle zusammen
    rgb.setZone(LEDZone::All, RGBColor(255, 255, 255));
}
```

---

## 🎮 Demo-Programm

Das `HS80_Demo.exe` wurde erweitert:

### Neue Tastenkombinationen

```
Zonen-Test:
  Z - Verschiedene Farben pro Zone (Logo=Rot, Power=Grün, Mic=Blau)
  L - Nur Logo (Rot)
  O - Nur Power-LED (Grün)
  M - Nur Mikrofon (Blau)

Helligkeit:
  + - Heller (+10%)
  - - Dunkler (-10%)
  B - Helligkeit setzen (0-100%)
```

### Demo-Session

```
[INIT] Aktiviere Keep-Alive...
[RGB] Keep-Alive Thread gestartet!

========================================
  HS80 RGB & Event Demo
========================================
...
Zonen-Test:
  Z - Verschiedene Farben pro Zone
  L - Nur Logo (Rot)
  O - Nur Power-LED (Grün)
  M - Nur Mikrofon (Blau)

Helligkeit:
  + - Heller (+10%)
  - - Dunkler (-10%)
  B - Helligkeit setzen (0-100%)
...

Wählen Sie: L
[ZONE] Nur LOGO auf Rot...

Wählen Sie: +
[HELLIGKEIT] 100% -> 100%

Wählen Sie: -
[HELLIGKEIT] 100% -> 90%

Wählen Sie: B
[HELLIGKEIT] Aktuelle Helligkeit: 90%
Neue Helligkeit eingeben (0-100): 50
[HELLIGKEIT] Setze auf 50%...
```

---

## 📋 API-Referenz

### HeadsetManager (High-Level)

```cpp
class HeadsetManager {
public:
    // Helligkeit
    bool setBrightness(int percent);  // 0-100%
    
    // Einzelne Zonen
    bool setZone(LEDZone zone, RGBColor color);
    
    // Kombiniert (alle Zonen)
    bool setLEDs(RGBColor color);
    bool setLEDs(const LEDZones& zones);
};
```

### RGBController (Low-Level)

```cpp
class RGBController {
public:
    // Helligkeit
    bool setBrightness(int percent);           // 0-100%
    bool setBrightnessRaw(int brightness);     // 0-1000 (raw)
    int getBrightness() const;                 // Gibt 0-100% zurück
    int getBrightnessRaw() const;              // Gibt 0-1000 zurück
    
    // Einzelne Zonen
    bool setZone(LEDZone zone, RGBColor color);
    bool setLogoColor(RGBColor color);
    bool setPowerColor(RGBColor color);
    bool setMicColor(RGBColor color);
    
    // Alle Zonen
    bool setColors(const LEDZones& zones);
    bool setColor(RGBColor color);
};
```

---

## ⚙️ Technische Details

### Brightness-Paket (HID)

```cpp
unsigned char packet[64] = {0};
packet[0] = 0x02;                     // Command
packet[1] = 0x09;                     // Headset Mode (Wireless)
packet[2] = 0x01;                     // Sub-command
packet[3] = 0x02;                     // Brightness command
packet[4] = 0x00;                     // Reserved
packet[5] = brightness & 0xFF;        // Low byte (0-1000)
packet[6] = (brightness >> 8) & 0xFF; // High byte
```

### Zonen-Speicherung

Die Library speichert die aktuellen Farben jeder Zone:

```cpp
struct LEDZones {
    RGBColor logo;   // Zone 0
    RGBColor power;  // Zone 1
    RGBColor mic;    // Zone 2
};

// Bei setZone() wird nur die gewünschte Zone aktualisiert:
m_currentZones.logo = color;  // Andere bleiben unverändert
```

### Thread-Sicherheit

Brightness und Zonen sind thread-safe mit Keep-Alive:

```cpp
// Thread 1: Helligkeit ändern
rgb.setBrightness(50);

// Thread 2: Keep-Alive sendet automatisch mit neuer Helligkeit
// (Kein Konflikt dank CRITICAL_SECTION)
```

---

## 🔧 Troubleshooting

### Problem: Helligkeit ändert sich nicht

```
Prüfen:
1. Software-Modus aktiv?
   → rgb.initialize() aufgerufen?
   
2. Wert im gültigen Bereich?
   → 0-100% oder 0-1000 (raw)
   
3. Keep-Alive aktiv?
   → Ohne Keep-Alive kann Headset zurückschalten
```

### Problem: Zone bleibt schwarz

```
Ursache: Helligkeit auf 0%
Lösung:  setBrightness(100) vor setZone()
```

### Problem: Alle Zonen ändern sich

```
Ursache: setColor() statt setZone() verwendet
Lösung:  setZone(LEDZone::Logo, color) für einzelne Zone
```

---

## 🎯 Best Practices

### 1. Helligkeit vor Farben setzen

```cpp
// ✅ RICHTIG
rgb.setBrightness(80);
rgb.setColor(RGBColor(255, 0, 0));

// ❌ FALSCH (Farbe unsichtbar bei Brightness=0)
rgb.setBrightness(0);
rgb.setColor(RGBColor(255, 0, 0));
```

### 2. Keep-Alive für Persistenz

```cpp
// ✅ RICHTIG - Einstellungen bleiben erhalten
rgb.startKeepAlive();
rgb.setBrightness(50);
rgb.setZone(LEDZone::Logo, RGBColor(255, 0, 0));
// → Bleibt 50% hell, Logo rot

// ❌ FALSCH - Headset kehrt zu Hardware-Modus zurück
rgb.setBrightness(50);
rgb.setZone(LEDZone::Logo, RGBColor(255, 0, 0));
// → Nach 60s: Brightness + Zone zurückgesetzt!
```

### 3. Zonen-Status verfolgen

```cpp
// Aktuelle Zonen-Farben speichern
LEDZones currentZones;
currentZones.logo = RGBColor(255, 0, 0);
currentZones.power = RGBColor(0, 255, 0);
currentZones.mic = RGBColor(0, 0, 255);

// Bei Änderung einer Zone: Alle anderen beibehalten
rgb.setColors(currentZones);  // Vollständiges Update
```

---

## 📊 Vergleich: Vorher vs. Nachher

### ❌ VORHER (Nur alle Zonen gleichzeitig)

```cpp
// Alle Zonen ROT
rgb.setColor(RGBColor(255, 0, 0));

// Problem: Kann nicht nur Logo ändern!
```

### ✅ NACHHER (Einzelne Zonen + Helligkeit)

```cpp
// Logo ROT, Rest bleibt
rgb.setLogoColor(RGBColor(255, 0, 0));

// Power GRÜN, Rest bleibt
rgb.setPowerColor(RGBColor(0, 255, 0));

// Alles auf 50% dimmen
rgb.setBrightness(50);

// Ergebnis: Logo rot + Power grün, beide 50% hell!
```

---

## 📝 Code-Beispiele

### Minimal-Beispiel: Helligkeit

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    HeadsetManager mgr;
    mgr.connect();
    mgr.rgb().initialize();
    mgr.rgb().startKeepAlive();
    
    mgr.setBrightness(50);  // 50% Helligkeit
    mgr.setLEDs(RGBColor(255, 0, 0));  // Rot
    
    Sleep(10000);
    mgr.disconnect();
    return 0;
}
```

### Minimal-Beispiel: Zonen

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    HeadsetManager mgr;
    mgr.connect();
    mgr.rgb().initialize();
    mgr.rgb().startKeepAlive();
    
    mgr.setZone(LEDZone::Logo, RGBColor(255, 0, 0));   // Logo → Rot
    mgr.setZone(LEDZone::Power, RGBColor(0, 255, 0));  // Power → Grün
    mgr.setZone(LEDZone::Mic, RGBColor(0, 0, 255));    // Mic → Blau
    
    Sleep(10000);
    mgr.disconnect();
    return 0;
}
```

---

**Erstellt:** 26. Oktober 2025  
**Version:** 1.1  
**Status:** ✅ Produktionsbereit

**Neue Features:**
- Brightness-Kontrolle (0-100% oder 0-1000 raw)
- Einzelne Zonen-Kontrolle (Logo, Power, Mic)
- Thread-sichere Implementierung mit Keep-Alive
- Erweiterte Demo mit +/- und B-Tasten
