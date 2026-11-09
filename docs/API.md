# API

[Zurück zur README](../README.md)

Öffentliche C++-Schnittstelle in `HS80_Library.h`, Namespace `HS80`. Alle Methoden liefern `bool` (true = Erfolg), sofern nicht anders angegeben. Grundlage ist die Variante in `HS80/`.

## HeadsetManager

| Funktion | Beschreibung |
|---|---|
| `bool connect(bool autoReconnect = true)` | Verbindet RGB- und Event-Interface |
| `void disconnect()` | Stoppt Monitoring, trennt beide Interfaces |
| `bool isConnected() const` | Beide Interfaces verbunden |
| `RGBController& rgb()` | Zugriff auf RGB |
| `EventMonitor& events()` | Zugriff auf Events |
| `bool setLEDs(RGBColor)` / `setLEDs(const LEDZones&)` | Farben setzen |
| `bool setZone(LEDZone, RGBColor)` | Einzelne Zone |
| `bool setBrightness(int percent)` | Helligkeit 0-100 |
| `bool startEventMonitoring(EventCallback)` | Monitoring starten |

## RGBController

| Funktion | Beschreibung |
|---|---|
| `connect(vid, pid)` / `disconnect()` / `isConnected()` | Verbindung |
| `initialize()` | Software-Modus aktivieren (3 Pakete) |
| `setColors(const LEDZones&)` / `setColor(RGBColor)` | Farben setzen |
| `setZone(LEDZone, RGBColor)`, `setLogoColor`, `setPowerColor`, `setMicColor` | Zonen |
| `setBrightness(int)` / `setBrightnessRaw(int)` | 0-100 bzw. 0-1000 |
| `int getBrightness() const` / `getBrightnessRaw() const` | Aktuelle Helligkeit |
| `setHardwareMode()` | Zurück in den Hardware-Modus |
| `startKeepAlive(int intervalMs = 5000)` / `stopKeepAlive()` / `isKeepAliveRunning()` | Keep-Alive |
| `rainbow(int durationMs = 10000, int stepMs = 100)` | Blockierender Effekt |
| `pulse(RGBColor, int cycles = 3, int stepMs = 50)` | Blockierender Effekt |
| `off()` | Alle LEDs aus |

Hinweis: `setColors` und `setBrightness` rufen bei Bedarf automatisch `initialize()` auf. `disconnect()` stellt den Hardware-Modus wieder her.

## EventMonitor

| Funktion | Beschreibung |
|---|---|
| `connect(vid, pid)` / `disconnect()` / `isConnected()` | Verbindung (Overlapped I/O) |
| `startMonitoring(EventCallback)` | Startet Lese-Thread; false, wenn bereits aktiv |
| `stopMonitoring()` | Beendet Thread (Wartezeit bis 2 s) |
| `isMonitoring() const` | Status |

## Datentypen

```cpp
struct RGBColor { unsigned char r, g, b; };
struct LEDZones { RGBColor logo, power, mic; };   // Standard: 0,155,222
enum class LEDZone { Logo = 0, Power = 1, Mic = 2, All = 255 };
enum class EventType { Unknown = 0x00, EventPacket = 0x03, Mute = 0xA6, Battery = 0x0F, Charging = 0x10 };
using EventCallback = std::function<void(const HeadsetEvent&)>;
```

### HeadsetEvent

| Mitglied | Beschreibung |
|---|---|
| `EventType type`, `unsigned char data[64]`, `size_t dataSize` | Rohdaten |
| `getActualEventType()` | Eventcode aus Byte 3 bei Header 0x03 |
| `isMuted()` / `isUnmuted()` | Mute-Status |
| `getBatteryLevel()` | Akku in %, -1 wenn kein Akku-Event |
| `getBatteryLevelRaw()` | Rohwert 0-1000 |
| `isCharging()` | true bei Wert 0x01 im Lade-Event |
| `getDescription()` | Deutsche Klartextbeschreibung |

Hinweis: `data` hat 64 Byte, der Lesepuffer 65 Byte; bei einem 65-Byte-Read wäre das ein Überlauf beim `memcpy` (Annahme: Reports sind 64 Byte).

## Device Discovery

```cpp
struct DeviceInfo { std::string path; unsigned short vendorId, productId, usagePage, usage; std::wstring manufacturer, product; };
std::vector<DeviceInfo> enumerateDevices(unsigned short vid = 0, unsigned short pid = 0);
bool findDeviceByUsage(unsigned short vid, unsigned short pid, unsigned short usagePage, unsigned short usage, DeviceInfo& outInfo);
```

`vid`/`pid` = 0 bedeutet kein Filter.
