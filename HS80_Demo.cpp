// ============================================================================
// HS80 Library Demo - Zeigt alle Features der Library
// ============================================================================

#include "HS80_Library.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <conio.h>

using namespace HS80;

// Event-Handler mit deutscher Übersetzung
void onHeadsetEvent(const HeadsetEvent& event) {
    std::cout << "\n+--------------------------------------------------------------+" << std::endl;
    std::cout << "| HEADSET EVENT                                                |" << std::endl;
    std::cout << "+--------------------------------------------------------------+" << std::endl;
    
    // Event-Beschreibung
    std::string desc = event.getDescription();
    std::cout << "| " << desc;
    for (size_t i = desc.length(); i < 60; i++) std::cout << " ";
    std::cout << " |" << std::endl;
    
    // Event-Details
    EventType actualType = event.getActualEventType();
    
    if (actualType == EventType::Mute) {
        std::string status = event.isMuted() ? "STUMM (Mute)" : "AKTIV (Unmute)";
        std::cout << "| Mikrofon: " << status;
        for (size_t i = status.length(); i < 48; i++) std::cout << " ";
        std::cout << " |" << std::endl;
    }
    else if (actualType == EventType::Battery) {
        int level = event.getBatteryLevel();
        std::cout << "| Akkustand: " << level << "%";
        std::string indicator;
        if (level >= 80) indicator = " [|||||||||| VOLL]";
        else if (level >= 60) indicator = " [||||||||--  GUT]";
        else if (level >= 40) indicator = " [|||||||--- MITTEL]";
        else if (level >= 20) indicator = " [||||----- NIEDRIG]";
        else indicator = " [|-------- KRITISCH]";
        std::cout << indicator;
        for (size_t i = indicator.length() + std::to_string(level).length(); i < 48; i++) std::cout << " ";
        std::cout << " |" << std::endl;
    }
    else if (actualType == EventType::Charging) {
        std::string status = event.isCharging() ? "WIRD GELADEN" : "NICHT AM LADEN";
        std::cout << "| Ladestatus: " << status;
        for (size_t i = status.length(); i < 47; i++) std::cout << " ";
        std::cout << " |" << std::endl;
    }
    
    // Zeige Raw-Daten für Debugging
    std::cout << "+--------------------------------------------------------------+" << std::endl;
    std::cout << "| Raw: ";
    for (size_t i = 0; i < (event.dataSize < 12 ? event.dataSize : 12); i++) {
        printf("%02X ", event.data[i]);
    }
    if (event.dataSize > 12) std::cout << "...";
    size_t padLen = 54 - (event.dataSize < 12 ? event.dataSize : 12) * 3;
    for (size_t i = 0; i < padLen; i++) std::cout << " ";
    std::cout << "|" << std::endl;
    std::cout << "+--------------------------------------------------------------+" << std::endl;
}

// Menü
void printMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  HS80 RGB & Event Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "RGB-Kontrolle:" << std::endl;
    std::cout << "  1 - Rot" << std::endl;
    std::cout << "  2 - Gruen" << std::endl;
    std::cout << "  3 - Blau" << std::endl;
    std::cout << "  4 - Cyan" << std::endl;
    std::cout << "  5 - Magenta" << std::endl;
    std::cout << "  6 - Gelb" << std::endl;
    std::cout << "  7 - Weiss" << std::endl;
    std::cout << "  8 - Corsair Blau" << std::endl;
    std::cout << "  9 - Aus" << std::endl;
    std::cout << "\nEffekte:" << std::endl;
    std::cout << "  R - Regenbogen" << std::endl;
    std::cout << "  P - Puls (Rot)" << std::endl;
    std::cout << "\nZonen-Test:" << std::endl;
    std::cout << "  Z - Verschiedene Farben pro Zone" << std::endl;
    std::cout << "  L - Nur Logo (Rot)" << std::endl;
    std::cout << "  O - Nur Power-LED (Gruen)" << std::endl;
    std::cout << "  M - Nur Mikrofon (Blau)" << std::endl;
    std::cout << "\nHelligkeit:" << std::endl;
    std::cout << "  + - Heller (+10%)" << std::endl;
    std::cout << "  - - Dunkler (-10%)" << std::endl;
    std::cout << "  B - Helligkeit setzen (0-100%)" << std::endl;
    std::cout << "\nSystem:" << std::endl;
    std::cout << "  H - Hardware-Modus" << std::endl;
    std::cout << "  Q - Beenden" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n[!] Keep-Alive AKTIV - Software-Modus bleibt erhalten!" << std::endl;
    std::cout << "Event-Monitoring laeuft im Hintergrund..." << std::endl;
    std::cout << "Druecke Mute/Akku-Taste am Headset zum Testen!" << std::endl;
    std::cout << "\nWaehlen Sie: ";
}

int main() {
    std::cout << "HS80 Library Demo v1.0" << std::endl;
    std::cout << "======================" << std::endl;
    
    // Manager erstellen
    HeadsetManager manager;
    
    // Verbinden
    if (!manager.connect()) {
        std::cerr << "\nFEHLER: Konnte nicht mit Headset verbinden!" << std::endl;
        std::cerr << "Stellen Sie sicher, dass:" << std::endl;
        std::cerr << "  - Das HS80 Headset angeschlossen ist" << std::endl;
        std::cerr << "  - Die Treiber installiert sind" << std::endl;
        std::cerr << "  - Das Programm als Administrator laeuft" << std::endl;
        
        std::cout << "\nDruecke eine Taste zum Beenden..." << std::endl;
        _getch();
        return 1;
    }
    
    // RGB initialisieren
    std::cout << "\n[INIT] Initialisiere RGB..." << std::endl;
    if (!manager.rgb().initialize()) {
        std::cerr << "WARNUNG: RGB-Initialisierung fehlgeschlagen!" << std::endl;
    }
    
    // Keep-Alive aktivieren (verhindert Rückfall in Hardware-Modus)
    std::cout << "[INIT] Aktiviere Keep-Alive (haelt Software-Modus aktiv)..." << std::endl;
    if (!manager.rgb().startKeepAlive(5000)) { // Alle 5 Sekunden
        std::cerr << "WARNUNG: Keep-Alive konnte nicht gestartet werden!" << std::endl;
    }
    
    // Event-Monitoring starten
    std::cout << "[INIT] Starte Event-Monitoring..." << std::endl;
    if (!manager.startEventMonitoring(onHeadsetEvent)) {
        std::cerr << "WARNUNG: Event-Monitoring konnte nicht gestartet werden!" << std::endl;
    }
    
    // Begrüßung mit Corsair Blau
    manager.setLEDs(RGBColor(0, 155, 222));
    
    // Hauptschleife
    bool running = true;
    while (running) {
        printMenu();
        
        char choice = _getch();
        choice = toupper(choice);
        
        std::cout << choice << std::endl;
        
        switch (choice) {
        case '1': 
            std::cout << "[RGB] Setze ROT..." << std::endl;
            manager.setLEDs(RGBColor(255, 0, 0)); 
            break;
            
        case '2': 
            std::cout << "[RGB] Setze GRUEN..." << std::endl;
            manager.setLEDs(RGBColor(0, 255, 0)); 
            break;
            
        case '3': 
            std::cout << "[RGB] Setze BLAU..." << std::endl;
            manager.setLEDs(RGBColor(0, 0, 255)); 
            break;
            
        case '4': 
            std::cout << "[RGB] Setze CYAN..." << std::endl;
            manager.setLEDs(RGBColor(0, 255, 255)); 
            break;
            
        case '5': 
            std::cout << "[RGB] Setze MAGENTA..." << std::endl;
            manager.setLEDs(RGBColor(255, 0, 255)); 
            break;
            
        case '6': 
            std::cout << "[RGB] Setze GELB..." << std::endl;
            manager.setLEDs(RGBColor(255, 255, 0)); 
            break;
            
        case '7': 
            std::cout << "[RGB] Setze WEISS..." << std::endl;
            manager.setLEDs(RGBColor(255, 255, 255)); 
            break;
            
        case '8': 
            std::cout << "[RGB] Setze CORSAIR BLAU..." << std::endl;
            manager.setLEDs(RGBColor(0, 155, 222)); 
            break;
            
        case '9': 
            std::cout << "[RGB] LEDs AUS..." << std::endl;
            manager.rgb().off(); 
            break;
            
        case 'R': 
            std::cout << "[EFFEKT] Starte Regenbogen..." << std::endl;
            manager.rgb().rainbow(10000, 50); 
            break;
            
        case 'P': 
            std::cout << "[EFFEKT] Starte Puls..." << std::endl;
            manager.rgb().pulse(RGBColor(255, 0, 0), 3, 30); 
            break;
            
        case 'Z':
            std::cout << "[ZONEN] Logo=Rot, Power=Gruen, Mic=Blau..." << std::endl;
            {
                LEDZones zones;
                zones.logo = RGBColor(255, 0, 0);   // Rot
                zones.power = RGBColor(0, 255, 0);  // Grün
                zones.mic = RGBColor(0, 0, 255);    // Blau
                manager.setLEDs(zones);
            }
            break;
            
        case 'L':
            std::cout << "[ZONE] Nur LOGO auf Rot..." << std::endl;
            manager.setZone(LEDZone::Logo, RGBColor(255, 0, 0));
            break;
            
        case 'O':
            std::cout << "[ZONE] Nur POWER-LED auf Gruen..." << std::endl;
            manager.setZone(LEDZone::Power, RGBColor(0, 255, 0));
            break;
            
        case 'M':
            std::cout << "[ZONE] Nur MIKROFON auf Blau..." << std::endl;
            manager.setZone(LEDZone::Mic, RGBColor(0, 0, 255));
            break;
            
        case '+':
        case '=':
            {
                int current = manager.rgb().getBrightness();
                int newBrightness = current + 10;
                if (newBrightness > 100) newBrightness = 100;
                std::cout << "[HELLIGKEIT] " << current << "% -> " << newBrightness << "%" << std::endl;
                manager.setBrightness(newBrightness);
            }
            break;
            
        case '-':
        case '_':
            {
                int current = manager.rgb().getBrightness();
                int newBrightness = current - 10;
                if (newBrightness < 0) newBrightness = 0;
                std::cout << "[HELLIGKEIT] " << current << "% -> " << newBrightness << "%" << std::endl;
                manager.setBrightness(newBrightness);
            }
            break;
            
        case 'B':
            {
                std::cout << "\n[HELLIGKEIT] Aktuelle Helligkeit: " << manager.rgb().getBrightness() << "%" << std::endl;
                std::cout << "Neue Helligkeit eingeben (0-100): ";
                int brightness;
                std::cin >> brightness;
                
                if (brightness < 0) brightness = 0;
                if (brightness > 100) brightness = 100;
                
                std::cout << "[HELLIGKEIT] Setze auf " << brightness << "%..." << std::endl;
                manager.setBrightness(brightness);
            }
            break;
            
        case 'H':
            std::cout << "[SYSTEM] Stelle Hardware-Modus wieder her..." << std::endl;
            manager.rgb().setHardwareMode();
            break;
            
        case 'Q':
            std::cout << "[SYSTEM] Beende..." << std::endl;
            running = false;
            break;
            
        default:
            std::cout << "Ungueltige Auswahl!" << std::endl;
            break;
        }
        
        Sleep(100);
    }
    
    // Aufräumen
    std::cout << "\n[CLEANUP] Trenne Verbindung..." << std::endl;
    manager.disconnect();
    
    std::cout << "\nProgramm beendet." << std::endl;
    return 0;
}
