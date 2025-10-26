# HS80 Software-Modus Keep-Alive 🔄

## 🎯 Problem

Der Corsair HS80 RGB Wireless **wechselt nach ca. 30-60 Sekunden automatisch zurück in den Hardware-Modus**, wenn keine RGB-Kommandos mehr gesendet werden. Das führt dazu, dass:

- ✗ Deine RGB-Farben verschwinden
- ✗ Das Headset zur Hardware-Beleuchtung zurückkehrt
- ✗ Software-gesteuerte Effekte stoppen

## ✅ Lösung: Keep-Alive Mechanismus

Die Library enthält jetzt einen **automatischen Keep-Alive Thread**, der den Software-Modus aktiv hält:

### Wie funktioniert es?

```
┌──────────────────────────────────────────────┐
│  Keep-Alive Thread (Hintergrund)             │
│                                              │
│  while (aktiv) {                             │
│      Sleep(5000);  // 5 Sekunden warten     │
│      SendColorsAgain();  // RGB auffrischen │
│  }                                           │
└──────────────────────────────────────────────┘
```

Der Thread sendet **alle 5 Sekunden** die aktuellen RGB-Farben erneut, sodass das Headset im Software-Modus bleibt.

## 💻 API-Verwendung

### Automatische Aktivierung (Empfohlen)

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    HeadsetManager manager;
    manager.connect();
    
    // RGB initialisieren
    manager.rgb().initialize();
    
    // Keep-Alive starten (alle 5 Sekunden)
    manager.rgb().startKeepAlive(5000);
    
    // Farbe setzen
    manager.setLEDs(RGBColor(255, 0, 0));  // Rot
    
    // Jetzt bleibt die Farbe dauerhaft erhalten!
    Sleep(300000);  // 5 Minuten - Farbe bleibt ROT!
    
    manager.disconnect();  // Keep-Alive wird automatisch gestoppt
    return 0;
}
```

### Manuelle Kontrolle

```cpp
RGBController rgb;
rgb.connect();
rgb.initialize();

// Keep-Alive manuell starten
rgb.startKeepAlive(3000);  // Alle 3 Sekunden

// Farben ändern (Keep-Alive sendet automatisch neue Farben)
rgb.setColor(RGBColor(0, 255, 0));  // Grün
Sleep(60000);  // 1 Minute - bleibt GRÜN

rgb.setColor(RGBColor(0, 0, 255));  // Blau
Sleep(60000);  // 1 Minute - bleibt BLAU

// Keep-Alive stoppen (Headset kehrt zum Hardware-Modus zurück)
rgb.stopKeepAlive();

rgb.disconnect();
```

### Status prüfen

```cpp
if (rgb.isKeepAliveRunning()) {
    std::cout << "Keep-Alive ist aktiv!" << std::endl;
} else {
    std::cout << "Keep-Alive ist inaktiv!" << std::endl;
}
```

## ⚙️ Konfiguration

### Intervall anpassen

```cpp
// Standard: 5000ms (5 Sekunden)
rgb.startKeepAlive();

// Schneller: 3000ms (3 Sekunden)
rgb.startKeepAlive(3000);

// Langsamer: 10000ms (10 Sekunden)
rgb.startKeepAlive(10000);
```

**Empfehlung:**
- **3-5 Sekunden**: Sicher und zuverlässig
- **< 3 Sekunden**: Unnötige Belastung
- **> 10 Sekunden**: Risiko, dass Headset zurückschaltet

### Thread-Sicherheit

Die `setColors()` Funktion ist **thread-safe** - der Keep-Alive Thread und dein Hauptprogramm können parallel RGB-Kommandos senden:

```cpp
rgb.startKeepAlive(5000);

// Hauptprogramm kann weiterhin Farben ändern
for (int i = 0; i < 100; i++) {
    rgb.setColor(RGBColor(i * 2, 0, 255 - i * 2));
    Sleep(100);  // Keep-Alive läuft parallel!
}

rgb.stopKeepAlive();
```

## 🔍 Technische Details

### Implementierung

```cpp
class RGBController {
private:
    HANDLE m_keepAliveThread;
    bool m_keepAliveRunning;
    LEDZones m_currentZones;
    CRITICAL_SECTION m_lock;  // Thread-Synchronisation
    
public:
    bool startKeepAlive(int intervalMs = 5000);
    void stopKeepAlive();
    bool isKeepAliveRunning() const;
};
```

### Funktionsweise

1. **Thread erstellen**: `CreateThread()` startet Hintergrund-Thread
2. **Farben speichern**: Aktuelle RGB-Werte werden in `m_currentZones` gespeichert
3. **Periodisches Senden**: Thread sendet alle X Millisekunden die gespeicherten Farben
4. **Automatisches Cleanup**: Bei `disconnect()` wird Thread automatisch beendet

### Synchronisation

```cpp
// Beim Setzen neuer Farben:
EnterCriticalSection(&m_lock);
m_currentZones = zones;  // Neue Farben speichern
LeaveCriticalSection(&m_lock);

// Im Keep-Alive Thread:
EnterCriticalSection(&m_lock);
LEDZones zones = m_currentZones;  // Aktuelle Farben lesen
LeaveCriticalSection(&m_lock);
sendColorsInternal(zones);  // Senden
```

## 📊 Vergleich: Mit vs. Ohne Keep-Alive

### ❌ OHNE Keep-Alive

```
0s:   RGB = ROT (Software)
10s:  RGB = ROT (Software)
20s:  RGB = ROT (Software)
30s:  RGB = ROT (Software)
60s:  RGB = Hardware-Farbe ⚠️  <-- Headset wechselt zurück!
```

### ✅ MIT Keep-Alive (5s Intervall)

```
0s:   RGB = ROT (Software) → Initial gesetzt
5s:   RGB = ROT (Software) → Keep-Alive sendet
10s:  RGB = ROT (Software) → Keep-Alive sendet
15s:  RGB = ROT (Software) → Keep-Alive sendet
...
300s: RGB = ROT (Software) → Farbe bleibt dauerhaft!
```

## 🎮 Demo-Programme

### HS80_Demo.exe

Das Demo-Programm aktiviert **automatisch** Keep-Alive:

```
[INIT] Initialisiere RGB...
[INIT] Aktiviere Keep-Alive (haelt Software-Modus aktiv)...
[RGB] Starte Keep-Alive Thread (Intervall: 5000ms)...
[RGB] Keep-Alive Thread gestartet!

========================================
  HS80 RGB & Event Demo
========================================
...
[!] Keep-Alive AKTIV - Software-Modus bleibt erhalten!
```

### Eigene Programme

Minimales Beispiel:

```cpp
#include "HS80_Library.h"
using namespace HS80;

int main() {
    RGBController rgb;
    
    if (!rgb.connect()) return 1;
    if (!rgb.initialize()) return 1;
    
    // Keep-Alive starten
    rgb.startKeepAlive();
    
    // Farbe setzen und 10 Minuten warten
    rgb.setColor(RGBColor(255, 0, 255));  // Magenta
    
    std::cout << "Farbe bleibt jetzt dauerhaft Magenta!" << std::endl;
    Sleep(600000);  // 10 Minuten
    
    // Cleanup
    rgb.stopKeepAlive();
    rgb.disconnect();
    return 0;
}
```

## ⚠️ Wichtige Hinweise

### 1. Automatisches Stoppen

Keep-Alive wird **automatisch gestoppt** bei:
- `disconnect()` - Trennung vom Headset
- `setHardwareMode()` - Rückkehr zum Hardware-Modus
- Programmende (Destruktor `~RGBController()`)

### 2. Resource-Management

```cpp
// ✅ KORREKT - Keep-Alive wird automatisch aufgeräumt
{
    RGBController rgb;
    rgb.connect();
    rgb.startKeepAlive();
    // ...
} // Destruktor stoppt Keep-Alive automatisch

// ❌ FALSCH - Programm beenden ohne disconnect()
rgb.connect();
rgb.startKeepAlive();
exit(0);  // Keep-Alive Thread läuft weiter! (bis Headset timeout)
```

### 3. Mehrfache Aufrufe

```cpp
rgb.startKeepAlive(5000);
rgb.startKeepAlive(3000);  // Ignoriert - Thread läuft bereits
// Output: "[RGB] Keep-Alive laeuft bereits!"
```

Um Intervall zu ändern:
```cpp
rgb.stopKeepAlive();
rgb.startKeepAlive(3000);  // Neues Intervall
```

### 4. Performance

- **CPU-Last**: Minimal (~0.01% - nur Sleep + WriteFile alle 5s)
- **Memory**: ~4 KB (Thread-Stack)
- **USB-Traffic**: 64 Bytes alle 5 Sekunden = 12.8 bytes/s

## 🔧 Troubleshooting

### Problem: Farbe verschwindet immer noch

```
Prüfen:
1. Keep-Alive gestartet?
   → rgb.isKeepAliveRunning() == true?

2. Intervall zu groß?
   → Versuche 3000ms statt 5000ms

3. Thread läuft?
   → Prüfe Debug-Ausgabe "[RGB] Keep-Alive Thread gestartet!"
```

### Problem: RGB-Befehle langsam

```
Ursache: Intervall zu klein (z.B. 100ms)
Lösung:  Erhöhe auf 5000ms
```

### Problem: Thread stoppt nicht

```
Prüfe:
1. disconnect() aufgerufen?
2. Destruktor wurde ausgeführt?
3. Warte-Timeout erhöhen (max 10s):
   → Siehe WaitForSingleObject(m_keepAliveThread, 10000)
```

## 📝 Changelog

### v1.0 - Keep-Alive Feature

**Hinzugefügt:**
- `startKeepAlive(int intervalMs)` - Startet Keep-Alive Thread
- `stopKeepAlive()` - Stoppt Keep-Alive Thread
- `isKeepAliveRunning()` - Status-Abfrage
- Thread-sichere Farb-Verwaltung mit `CRITICAL_SECTION`
- Automatisches Cleanup bei Disconnect

**Geändert:**
- `setColors()` speichert Farben für Keep-Alive
- `disconnect()` stoppt Keep-Alive automatisch
- `~RGBController()` räumt Keep-Alive auf

**Intern:**
- Neue Member: `m_keepAliveThread`, `m_keepAliveRunning`, `m_currentZones`, `m_lock`
- Neue Funktion: `sendColorsInternal()` (private)

---

**Erstellt:** 26. Oktober 2025  
**Version:** 1.0  
**Status:** ✅ Produktionsbereit
