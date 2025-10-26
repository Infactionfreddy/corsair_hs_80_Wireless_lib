// ============================================================================
// HS80 Analyzer - Detaillierte Analyse von RGB & Event Interfaces
// ============================================================================

#include "HS80_Library.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <conio.h>

using namespace HS80;

// Log-Datei
std::ofstream logFile;
bool enableLogging = false;

void logEvent(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm;
    localtime_s(&tm, &time);
    
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", &tm);
    
    std::string fullMessage = std::string(timestamp) + "." + 
                              std::to_string(ms.count()) + " | " + message;
    
    std::cout << fullMessage << std::endl;
    
    if (enableLogging && logFile.is_open()) {
        logFile << fullMessage << std::endl;
        logFile.flush();
    }
}

void logHexData(const std::string& prefix, const unsigned char* data, size_t size) {
    std::stringstream ss;
    ss << prefix << " [" << size << " bytes]: ";
    
    for (size_t i = 0; i < size; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << (int)data[i] << " ";
        
        if ((i + 1) % 16 == 0 && i < size - 1) {
            ss << "\n" << std::string(prefix.length() + 14, ' ');
        }
    }
    
    logEvent(ss.str());
}

// Event-Handler mit detailliertem Logging und deutscher Übersetzung
void analyzeEvent(const HeadsetEvent& event) {
    EventType actualType = event.getActualEventType();
    
    std::stringstream ss;
    ss << "[EVENT] Paket=0x" << std::hex << std::uppercase 
       << std::setw(2) << std::setfill('0')
       << static_cast<int>(event.type);
    
    if (event.dataSize >= 4 && event.data[0] == 0x03) {
        ss << ", Event=0x" << std::setw(2) << std::setfill('0')
           << static_cast<int>(event.data[3]) << std::dec << " | ";
    } else {
        ss << std::dec << " | ";
    }
    
    // Deutsche Beschreibung
    ss << event.getDescription();
    
    // Detaillierte Werte
    if (actualType == EventType::Mute) {
        ss << " | Byte[5]=" << (event.isMuted() ? "0x01 (STUMM)" : "0x00 (AKTIV)");
    }
    else if (actualType == EventType::Battery) {
        int level = event.getBatteryLevel();
        ss << " | Byte[5]=0x" << std::hex << std::setw(2) << std::setfill('0') 
           << level << std::dec << " (" << level << "%)";
    }
    else if (actualType == EventType::Charging) {
        ss << " | Byte[5]=" << (event.isCharging() ? "0x01 (LAEDT)" : "0x00 (NICHT LAEDT)");
    }
    
    logEvent(ss.str());
    logHexData("        Raw Data", event.data, event.dataSize);
    
    // Event-Struktur erklären
    if (event.dataSize >= 6 && event.data[0] == 0x03) {
        std::stringstream structure;
        structure << "        Struktur: [Header=0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)event.data[0] << "][" << (int)event.data[1] << "][" 
                  << (int)event.data[2] << "][EventCode=0x" << (int)event.data[3]
                  << "][" << (int)event.data[4] << "][Wert=0x" << (int)event.data[5] << "]";
        logEvent(structure.str());
    }
}

// Zeige alle verfügbaren Devices
void showAllDevices() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Alle HID-Geraete" << std::endl;
    std::cout << "========================================" << std::endl;
    
    auto devices = enumerateDevices();
    
    int index = 1;
    for (const auto& dev : devices) {
        std::cout << "\n[" << index++ << "] ";
        std::wcout << dev.manufacturer << L" - " << dev.product << std::endl;
        std::cout << "    VID:PID   = 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << dev.vendorId << ":0x" << std::setw(4) << dev.productId << std::dec << std::endl;
        std::cout << "    Usage     = 0x" << std::hex << std::uppercase
                  << dev.usagePage << ":0x" << dev.usage << std::dec << std::endl;
        std::cout << "    Path      = " << dev.path.substr(0, 80) << "..." << std::endl;
    }
    
    std::cout << "\nGesamt: " << devices.size() << " Geraete gefunden." << std::endl;
}

// Zeige nur HS80 Devices
void showHS80Interfaces() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  HS80 Interfaces" << std::endl;
    std::cout << "========================================" << std::endl;
    
    auto devices = enumerateDevices(CORSAIR_VID, HS80_WIRELESS_PID);
    
    if (devices.empty()) {
        std::cout << "\nKEIN HS80 gefunden!" << std::endl;
        std::cout << "Suche nach VID=0x" << std::hex << CORSAIR_VID 
                  << ", PID=0x" << HS80_WIRELESS_PID << std::dec << std::endl;
        return;
    }
    
    for (const auto& dev : devices) {
        std::wcout << L"\n" << dev.product << std::endl;
        std::cout << "  VID:PID      = 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << dev.vendorId << ":0x" << std::setw(4) << dev.productId << std::dec << std::endl;
        std::cout << "  Usage Page   = 0x" << std::hex << std::uppercase << dev.usagePage << std::dec;
        
        if (dev.usagePage == 0x000C) std::cout << " (Consumer Control)";
        else if (dev.usagePage == 0xFF42) std::cout << " (Corsair RGB/Events)";
        else if (dev.usagePage == 0xFF58) std::cout << " (Vendor Specific)";
        std::cout << std::endl;
        
        std::cout << "  Usage        = 0x" << std::hex << std::uppercase << dev.usage << std::dec;
        
        if (dev.usagePage == 0xFF42) {
            if (dev.usage == RGB_USAGE) {
                std::cout << " >>> RGB-INTERFACE <<<";
            } else if (dev.usage == EVENT_USAGE) {
                std::cout << " >>> EVENT-INTERFACE <<<";
            }
        }
        std::cout << std::endl;
        
        std::cout << "  Path         = " << dev.path << std::endl;
    }
    
    std::cout << "\nGesamt: " << devices.size() << " HS80-Interfaces gefunden." << std::endl;
}

// RGB Test-Suite
void rgbTestSuite(RGBController& rgb) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  RGB Test-Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (!rgb.isConnected()) {
        std::cout << "\n[FEHLER] RGB nicht verbunden!" << std::endl;
        return;
    }
    
    std::cout << "\n[TEST 1] Initialisierung..." << std::endl;
    if (rgb.initialize()) {
        logEvent("[PASS] RGB initialisiert");
    } else {
        logEvent("[FAIL] RGB Initialisierung fehlgeschlagen");
        return;
    }
    
    Sleep(500);
    
    std::cout << "\n[TEST 2] Primaerfarben..." << std::endl;
    logEvent("[TEST] Rot");
    rgb.setColor(RGBColor(255, 0, 0));
    Sleep(1000);
    
    logEvent("[TEST] Gruen");
    rgb.setColor(RGBColor(0, 255, 0));
    Sleep(1000);
    
    logEvent("[TEST] Blau");
    rgb.setColor(RGBColor(0, 0, 255));
    Sleep(1000);
    
    std::cout << "\n[TEST 3] Helligkeit..." << std::endl;
    for (int brightness = 255; brightness >= 0; brightness -= 51) {
        logEvent("[TEST] Helligkeit: " + std::to_string(brightness));
        rgb.setColor(RGBColor(brightness, brightness, brightness));
        Sleep(500);
    }
    
    std::cout << "\n[TEST 4] Zonen..." << std::endl;
    LEDZones zones;
    zones.logo = RGBColor(255, 0, 0);
    zones.power = RGBColor(0, 255, 0);
    zones.mic = RGBColor(0, 0, 255);
    logEvent("[TEST] Logo=Rot, Power=Gruen, Mic=Blau");
    rgb.setColors(zones);
    Sleep(2000);
    
    std::cout << "\n[TEST 5] Regenbogen..." << std::endl;
    logEvent("[TEST] Regenbogen-Effekt");
    rgb.rainbow(5000, 50);
    
    std::cout << "\n[TEST 6] Puls..." << std::endl;
    logEvent("[TEST] Puls-Effekt");
    rgb.pulse(RGBColor(0, 155, 222), 2, 30);
    
    std::cout << "\n[TEST 7] Hardware-Modus..." << std::endl;
    logEvent("[TEST] Stelle Hardware-Modus wieder her");
    rgb.setHardwareMode();
    
    std::cout << "\nAlle RGB-Tests abgeschlossen!" << std::endl;
}

// Event Monitor Test
void eventMonitorTest(EventMonitor& events) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Event Monitor Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (!events.isConnected()) {
        std::cout << "\n[FEHLER] Event-Interface nicht verbunden!" << std::endl;
        return;
    }
    
    std::cout << "\nStarte Event-Monitoring fuer 30 Sekunden..." << std::endl;
    std::cout << "Druecke am Headset:" << std::endl;
    std::cout << "  - Mute-Taste" << std::endl;
    std::cout << "  - Akku-Status-Taste" << std::endl;
    std::cout << "  - Schliesse/Oeffne USB-Kabel (fuer Charging-Event)" << std::endl;
    std::cout << "\nDruecke 'Q' zum Abbrechen...\n" << std::endl;
    
    logEvent("[EVENT MONITOR] Starte 30s Test");
    
    if (!events.startMonitoring(analyzeEvent)) {
        std::cout << "[FEHLER] Konnte Monitoring nicht starten!" << std::endl;
        return;
    }
    
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count();
        
        if (elapsed >= 30) {
            std::cout << "\n30 Sekunden abgelaufen!" << std::endl;
            break;
        }
        
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q') {
                std::cout << "\nAbgebrochen!" << std::endl;
                break;
            }
        }
        
        Sleep(100);
    }
    
    events.stopMonitoring();
    logEvent("[EVENT MONITOR] Test beendet");
}

// Hauptmenü
void printMainMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  HS80 Analyzer v1.0" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Zeige alle HID-Geraete" << std::endl;
    std::cout << "2. Zeige HS80 Interfaces" << std::endl;
    std::cout << "3. RGB Test-Suite" << std::endl;
    std::cout << "4. Event Monitor Test" << std::endl;
    std::cout << "5. Vollstaendiger Test (RGB + Events)" << std::endl;
    std::cout << "L. Logging " << (enableLogging ? "AUS" : "AN") << std::endl;
    std::cout << "Q. Beenden" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nWaehlen Sie: ";
}

int main() {
    std::cout << "HS80 Analyzer v1.0" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "\nDieses Tool analysiert die RGB- und Event-Interfaces" << std::endl;
    std::cout << "des Corsair HS80 RGB Wireless Gaming Headsets.\n" << std::endl;
    
    HeadsetManager manager;
    bool connected = false;
    
    while (true) {
        printMainMenu();
        
        char choice = _getch();
        choice = toupper(choice);
        std::cout << choice << std::endl;
        
        switch (choice) {
        case '1':
            showAllDevices();
            break;
            
        case '2':
            showHS80Interfaces();
            break;
            
        case '3':
            if (!connected) {
                std::cout << "\nVerbinde mit HS80..." << std::endl;
                connected = manager.connect();
            }
            if (connected) {
                rgbTestSuite(manager.rgb());
            }
            break;
            
        case '4':
            if (!connected) {
                std::cout << "\nVerbinde mit HS80..." << std::endl;
                connected = manager.connect();
            }
            if (connected) {
                eventMonitorTest(manager.events());
            }
            break;
            
        case '5':
            if (!connected) {
                std::cout << "\nVerbinde mit HS80..." << std::endl;
                connected = manager.connect();
            }
            if (connected) {
                rgbTestSuite(manager.rgb());
                std::cout << "\n\nDruecke eine Taste fuer Event-Test..." << std::endl;
                _getch();
                eventMonitorTest(manager.events());
            }
            break;
            
        case 'L':
            enableLogging = !enableLogging;
            if (enableLogging) {
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                std::tm tm;
                localtime_s(&tm, &time);
                
                char filename[64];
                strftime(filename, sizeof(filename), "HS80_Log_%Y%m%d_%H%M%S.txt", &tm);
                
                logFile.open(filename);
                if (logFile.is_open()) {
                    std::cout << "\nLogging aktiviert: " << filename << std::endl;
                    logEvent("=== HS80 Analyzer Log gestartet ===");
                } else {
                    std::cout << "\nFEHLER: Konnte Log-Datei nicht erstellen!" << std::endl;
                    enableLogging = false;
                }
            } else {
                if (logFile.is_open()) {
                    logEvent("=== HS80 Analyzer Log beendet ===");
                    logFile.close();
                }
                std::cout << "\nLogging deaktiviert." << std::endl;
            }
            break;
            
        case 'Q':
            std::cout << "\nBeende..." << std::endl;
            if (logFile.is_open()) {
                logEvent("=== Programm beendet ===");
                logFile.close();
            }
            manager.disconnect();
            return 0;
            
        default:
            std::cout << "Ungueltige Auswahl!" << std::endl;
            break;
        }
        
        Sleep(500);
    }
    
    return 0;
}
