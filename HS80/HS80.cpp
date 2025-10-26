// main.cpp - Portierung von main.rs (HID-Lesen) nach C++ (C++17)
// Verwendet Windows HID API (keine externen Abhängigkeiten)

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <string>
#include <map>
#include <windows.h>
#include <hidsdi.h>
#include <setupapi.h>

// Link gegen diese Bibliotheken (in Projekteigenschaften unter Linker -> Eingabe)
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

static const unsigned short CORSAIR_VID = 0x1B1C;
static const unsigned short HS80_PID = 0x0A6B;
static const unsigned short HS80_USAGE_PAGE = 0xFF42;  // Vendor-Specific Usage Page
static const unsigned short HS80_USAGE = 0x0002;       // Vendor-Specific Usage

// RGB LED Indizes (basierend auf JS: Logo, Power, Mic)
static const unsigned char LED_LOGO_R = 0;
static const unsigned char LED_POWER_R = 1;
static const unsigned char LED_MIC_R = 2;

// RGB Struktur (RGBColor statt RGB wegen Windows.h Konflikt)
struct RGBColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    
    RGBColor() : r(0), g(0), b(0) {}
    RGBColor(unsigned char red, unsigned char green, unsigned char blue) 
        : r(red), g(green), b(blue) {}
};

struct HIDDeviceInfo {
    unsigned short vendorId;
    unsigned short productId;
    std::string devicePath;
    std::wstring productName;
    std::wstring manufacturerName;
    unsigned short usagePage;
    unsigned short usage;
    unsigned short interfaceNumber;
    unsigned short collectionNumber;
};

			std::vector<HIDDeviceInfo> EnumerateHIDDevices() {
    std::vector<HIDDeviceInfo> devices;
    
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);

    HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(&hidGuid, nullptr, nullptr, 
                                                    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device info set" << std::endl;
        return devices;
    }

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &hidGuid, 
                                                    i, &deviceInterfaceData); i++) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetailA(deviceInfoSet, &deviceInterfaceData, 
                                          nullptr, 0, &requiredSize, nullptr);

        std::vector<char> buffer(requiredSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_A detailData = 
            reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_A>(buffer.data());
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

        if (SetupDiGetDeviceInterfaceDetailA(deviceInfoSet, &deviceInterfaceData, 
                                              detailData, requiredSize, nullptr, nullptr)) {
            HANDLE hDevice = CreateFileA(detailData->DevicePath, 
                                         GENERIC_READ | GENERIC_WRITE,
                                         FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                         nullptr, OPEN_EXISTING, 0, nullptr);

            if (hDevice != INVALID_HANDLE_VALUE) {
                HIDD_ATTRIBUTES attributes;
                attributes.Size = sizeof(HIDD_ATTRIBUTES);
                
                if (HidD_GetAttributes(hDevice, &attributes)) {
                    HIDDeviceInfo info;
                    info.vendorId = attributes.VendorID;
                    info.productId = attributes.ProductID;
                    info.devicePath = detailData->DevicePath;
                    info.usagePage = 0;
                    info.usage = 0;
                    info.interfaceNumber = 0;
                    info.collectionNumber = 0;
                    
                    // Produktname abrufen
                    wchar_t productBuffer[256] = { 0 };
                    if (HidD_GetProductString(hDevice, productBuffer, sizeof(productBuffer))) {
                        info.productName = productBuffer;
                    }
                    
                    // Herstellername abrufen
                    wchar_t manufacturerBuffer[256] = { 0 };
                    if (HidD_GetManufacturerString(hDevice, manufacturerBuffer, sizeof(manufacturerBuffer))) {
                        info.manufacturerName = manufacturerBuffer;
                    }
                    
                    // WICHTIG: Usage Page und Usage abrufen
                    PHIDP_PREPARSED_DATA preparsedData;
                    if (HidD_GetPreparsedData(hDevice, &preparsedData)) {
                        HIDP_CAPS caps;
                        if (HidP_GetCaps(preparsedData, &caps) == HIDP_STATUS_SUCCESS) {
                            info.usagePage = caps.UsagePage;
                            info.usage = caps.Usage;
                        }
                        HidD_FreePreparsedData(preparsedData);
                    }
                    
                    devices.push_back(info);
                }
                CloseHandle(hDevice);
            }
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return devices;
}

HANDLE OpenHIDDeviceByPath(const std::string& devicePath) {
    HANDLE hDevice = CreateFileA(devicePath.c_str(), 
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                 nullptr, OPEN_EXISTING, 
                                 FILE_FLAG_OVERLAPPED,
                                 nullptr);
    return hDevice;
}

// Öffne Gerät für synchrones Schreiben (RGB-Befehle)
HANDLE OpenHIDDeviceForWrite(const std::string& devicePath) {
    HANDLE hDevice = CreateFileA(devicePath.c_str(), 
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                 nullptr, OPEN_EXISTING, 
                                 0, // Kein FILE_FLAG_OVERLAPPED
                                 nullptr);
    return hDevice;
}

// Öffne spezifisches HID Interface mit Collection (wichtig für RGB!)
HANDLE OpenHIDDeviceWithCollection(unsigned short vid, unsigned short pid, 
                                     unsigned short usagePage, unsigned short usage,
                                     unsigned short collection) {
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(&hidGuid, nullptr, nullptr, 
                                                    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        return INVALID_HANDLE_VALUE;
    }
    
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &hidGuid, 
                                                    i, &deviceInterfaceData); i++) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetailA(deviceInfoSet, &deviceInterfaceData, 
                                          nullptr, 0, &requiredSize, nullptr);
        
        std::vector<char> buffer(requiredSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_A detailData = 
            reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_A>(buffer.data());
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
        
        if (SetupDiGetDeviceInterfaceDetailA(deviceInfoSet, &deviceInterfaceData, 
                                              detailData, requiredSize, nullptr, nullptr)) {
            HANDLE hDevice = CreateFileA(detailData->DevicePath, 
                                         GENERIC_READ | GENERIC_WRITE,
                                         FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                         nullptr, OPEN_EXISTING, 0, nullptr);
            
            if (hDevice != INVALID_HANDLE_VALUE) {
                HIDD_ATTRIBUTES attributes;
                attributes.Size = sizeof(HIDD_ATTRIBUTES);
                
                if (HidD_GetAttributes(hDevice, &attributes) &&
                    attributes.VendorID == vid && attributes.ProductID == pid) {
                    
                    PHIDP_PREPARSED_DATA preparsedData;
                    if (HidD_GetPreparsedData(hDevice, &preparsedData)) {
                        HIDP_CAPS caps;
                        if (HidP_GetCaps(preparsedData, &caps) == HIDP_STATUS_SUCCESS) {
                            // Prüfe Usage Page und Usage
                            if (caps.UsagePage == usagePage && caps.Usage == usage) {
                                // Collection-Matching (vereinfacht - in echter HID API komplexer)
                                HidD_FreePreparsedData(preparsedData);
                                SetupDiDestroyDeviceInfoList(deviceInfoSet);
                                
                                std::cout << "[DEBUG] Geoeffnet: UsagePage=0x" << std::hex << caps.UsagePage
                                          << " Usage=0x" << caps.Usage << std::dec << std::endl;
                                return hDevice;
                            }
                        }
                        HidD_FreePreparsedData(preparsedData);
                    }
                }
                CloseHandle(hDevice);
            }
        }
    }
    
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return INVALID_HANDLE_VALUE;
}

// Basierend auf modernDirectLightingMode() aus JS
bool InitializeRGBMode(HANDLE device, bool isWireless) {
    const unsigned char headsetMode = isWireless ? 0x09 : 0x08;
    std::vector<unsigned char> packet(64, 0);
    DWORD bytesWritten = 0;

    std::cout << "\n=== Initialisiere RGB Software-Modus ===" << std::endl;

    // Enable Software Mode
    packet[0] = 0x02;
    packet[1] = headsetMode;
    packet[2] = 0x01;
    packet[3] = 0x03;
    packet[4] = 0x00;
    packet[5] = 0x02;
    
    if (!WriteFile(device, packet.data(), 64, &bytesWritten, nullptr)) {
        std::cerr << "Fehler beim Aktivieren des Software-Modus! Error: " << GetLastError() << std::endl;
        return false;
    }
    Sleep(100);

    // Open lighting endpoint
    std::fill(packet.begin(), packet.end(), 0);
    packet[0] = 0x02;
    packet[1] = headsetMode;
    packet[2] = 0x0D;
    packet[3] = 0x00;
    packet[4] = 0x01;
    
    if (!WriteFile(device, packet.data(), 64, &bytesWritten, nullptr)) {
        std::cerr << "Fehler beim Öffnen des Lighting-Endpoints!" << std::endl;
        return false;
    }
    Sleep(100);

    // Set Hardware Brightness to 100%
    std::fill(packet.begin(), packet.end(), 0);
    packet[0] = 0x02;
    packet[1] = headsetMode;
    packet[2] = 0x01;
    packet[3] = 0x02;
    packet[4] = 0x00;
    packet[5] = 0xE8; // 1000 = 100% (little endian)
    packet[6] = 0x03;
    
    if (!WriteFile(device, packet.data(), 64, &bytesWritten, nullptr)) {
        std::cerr << "Fehler beim Setzen der Helligkeit!" << std::endl;
        return false;
    }
    Sleep(100);

    std::cout << "RGB Software-Modus erfolgreich aktiviert!" << std::endl;
    return true;
}

// Basierend auf writeRGB() aus JS
bool SendRGBColors(HANDLE device, bool isWireless, RGBColor logo, RGBColor power, RGBColor mic) {
    const unsigned char headsetMode = isWireless ? 0x09 : 0x08;
    std::vector<unsigned char> packet(64, 0);
    
    // Paket-Header (aus JS: [0x02, headsetMode, 0x06, 0x00, 0x09, 0x00, 0x00, 0x00])
    packet[0] = 0x02;
    packet[1] = headsetMode;
    packet[2] = 0x06;
    packet[3] = 0x00;
    packet[4] = 0x09; // 9 Bytes RGB-Daten
    packet[5] = 0x00;
    packet[6] = 0x00;
    packet[7] = 0x00;
    
    // RGB-Daten (aus JS: RGBData mit Index 0,1,2 für Logo, Power, Mic)
    // R-Werte an Index 0, 1, 2 (+ offset 8)
    packet[8]  = logo.r;   // LED_LOGO_R
    packet[9]  = power.r;  // LED_POWER_R
    packet[10] = mic.r;    // LED_MIC_R
    
    // G-Werte an Index 3, 4, 5 (+ offset 8)
    packet[11] = logo.g;   // LED_LOGO_R + 3
    packet[12] = power.g;  // LED_POWER_R + 3
    packet[13] = mic.g;    // LED_MIC_R + 3
    
    // B-Werte an Index 6, 7, 8 (+ offset 8)
    packet[14] = logo.b;   // LED_LOGO_R + 6
    packet[15] = power.b;  // LED_POWER_R + 6
    packet[16] = mic.b;    // LED_MIC_R + 6
    
    DWORD bytesWritten = 0;
    if (!WriteFile(device, packet.data(), 64, &bytesWritten, nullptr)) {
        std::cerr << "Fehler beim Senden der RGB-Daten! Error: " << GetLastError() << std::endl;
        return false;
    }
    
    return true;
}

// Setze alle LEDs auf Hardware-Modus zurück beim Beenden
bool SetHardwareMode(HANDLE device, bool isWireless) {
    const unsigned char headsetMode = isWireless ? 0x09 : 0x08;
    std::vector<unsigned char> packet(64, 0);
    
    packet[0] = 0x02;
    packet[1] = headsetMode;
    packet[2] = 0x01;
    packet[3] = 0x03;
    packet[4] = 0x00;
    packet[5] = 0x01; // Hardware mode
    
    DWORD bytesWritten = 0;
    bool result = WriteFile(device, packet.data(), 64, &bytesWritten, nullptr);
    Sleep(100);
    
    return result;
}

int main() {
    std::cout << "=== HID-Geraete werden gesucht ===" << std::endl << std::endl;
    
    std::vector<HIDDeviceInfo> devices = EnumerateHIDDevices();
    
    if (devices.empty()) {
        std::cerr << "Keine HID-Geraete gefunden!" << std::endl;
        return 1;
    }
    
    // Gruppiere nach VID/PID für bessere Übersicht
    std::map<std::pair<unsigned short, unsigned short>, std::vector<size_t>> deviceGroups;
    for (size_t i = 0; i < devices.size(); ++i) {
        auto key = std::make_pair(devices[i].vendorId, devices[i].productId);
        deviceGroups[key].push_back(i);
    }
    
    std::cout << "Gefundene HID-Geraete:" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    int displayIndex = 1;
    for (const auto& group : deviceGroups) {
        unsigned short vid = group.first.first;
        unsigned short pid = group.first.second;
        
        // Zeige Geräteinformationen nur einmal pro VID/PID
        const HIDDeviceInfo& firstDevice = devices[group.second[0]];
        
        std::wcout << L"Geraet: " << firstDevice.manufacturerName;
        if (!firstDevice.productName.empty()) {
            std::wcout << L" - " << firstDevice.productName;
        }
        std::cout << std::endl;
        std::cout << "VID: 0x" << std::hex << std::uppercase << std::setw(4) 
                  << std::setfill('0') << vid
                  << ", PID: 0x" << std::setw(4) << pid << std::dec << std::endl;
        
        if (vid == CORSAIR_VID) {
            std::cout << ">>> CORSAIR GERAET <<<";
            if (pid == HS80_PID) {
                std::cout << " >>> HS80 MATCH! <<<";
            }
            std::cout << std::endl;
        }
        
        std::cout << "\nInterfaces:" << std::endl;
        for (size_t idx : group.second) {
            std::cout << "  [" << displayIndex++ << "] ";
            std::cout << "Usage Page: 0x" << std::hex << std::uppercase << std::setw(4) 
                      << std::setfill('0') << devices[idx].usagePage;
            std::cout << ", Usage: 0x" << std::setw(4) << devices[idx].usage << std::dec;
            
            // Erkläre bekannte Usage Pages
            if (devices[idx].usagePage == 0x0C) {
                std::cout << " (Consumer Control)";
            } else if (devices[idx].usagePage == 0x01) {
                std::cout << " (Generic Desktop)";
            } else if (devices[idx].usagePage == 0xFF00 || devices[idx].usagePage >= 0xFF00) {
                std::cout << " (Vendor-Specific)";
                // Prüfe auf exaktes HS80 Interface Match
                if (devices[idx].usagePage == HS80_USAGE_PAGE && devices[idx].usage == HS80_USAGE) {
                    std::cout << " <<< HS80 INTERFACE! >>>";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::string(80, '-') << std::endl;
    }
    
    std::cout << "Bitte waehlen Sie ein Interface (1-" << (displayIndex - 1) 
              << ") oder 0 zum Beenden: ";
    
    int selection;
    std::cin >> selection;
    
    if (selection <= 0 || selection > static_cast<int>(devices.size())) {
        std::cout << "Programm beendet." << std::endl;
        return 0;
    }
    
    const HIDDeviceInfo& selectedDevice = devices[selection - 1];
    std::cout << std::endl << "Oeffne Interface: ";
    std::wcout << selectedDevice.manufacturerName << L" - " << selectedDevice.productName << std::endl;
    std::cout << "Usage Page: 0x" << std::hex << std::uppercase << selectedDevice.usagePage
              << ", Usage: 0x" << selectedDevice.usage << std::dec << std::endl;
    
    HANDLE device = OpenHIDDeviceByPath(selectedDevice.devicePath);
    if (device == INVALID_HANDLE_VALUE) {
        std::cerr << "Fehler beim Oeffnen des Geraets! Error Code: " << GetLastError() << std::endl;
        std::cerr << "Versuchen Sie das Programm als Administrator auszufuehren." << std::endl;
        return 1;
    }

    std::cout << "Geraet erfolgreich geoeffnet!" << std::endl;
    
    // Prüfe ob es ein HS80 Wireless ist (PID 0x0A6B)
    bool isWireless = (selectedDevice.productId == HS80_PID);
    
    // Versuche RGB-Endpoint zu öffnen
    // Laut JS: HS80 Wireless braucht Interface 3, Usage 0x0001, UsagePage 0xFF42
    // Das ist ein ANDERES Interface als das Event-Interface (Usage 0x0002)!
    
    std::cout << "\n=== Suche RGB-Endpoint (Usage 0x0001) ===" << std::endl;
    
    HANDLE deviceWrite = INVALID_HANDLE_VALUE;
    
    // Suche das Interface mit Usage 0x0001 (RGB-Kommandos)
    for (const auto& dev : devices) {
        if (dev.vendorId == CORSAIR_VID && dev.productId == HS80_PID &&
            dev.usagePage == 0xFF42 && dev.usage == 0x0001) {
            
            std::cout << "RGB-Endpoint gefunden: Usage Page=0x" << std::hex << dev.usagePage
                      << ", Usage=0x" << dev.usage << std::dec << std::endl;
            std::cout << "Pfad: " << dev.devicePath << std::endl;
            
            deviceWrite = OpenHIDDeviceForWrite(dev.devicePath);
            
            if (deviceWrite != INVALID_HANDLE_VALUE) {
                std::cout << "RGB-Endpoint erfolgreich geoeffnet!" << std::endl;
                break;
            } else {
                std::cout << "Fehler beim Oeffnen! Error: " << GetLastError() << std::endl;
            }
        }
    }
    
    if (deviceWrite == INVALID_HANDLE_VALUE) {
        std::cout << "WARNUNG: Kein RGB-Endpoint gefunden! RGB wird nicht funktionieren." << std::endl;
        std::cout << "Nutzen Sie das Event-Interface nur zum Lesen." << std::endl;
    }
    
    // RGB-Modus initialisieren
    if (deviceWrite != INVALID_HANDLE_VALUE && !InitializeRGBMode(deviceWrite, isWireless)) {
        std::cerr << "WARNUNG: RGB-Initialisierung fehlgeschlagen!" << std::endl;
    }
    
    // Zeige RGB-Optionen
    std::cout << "\n=== RGB-Steuerung ===" << std::endl;
    std::cout << "Moechten Sie RGB-Farben setzen? (j/n): ";
    char rgbChoice;
    std::cin >> rgbChoice;
    
    if (rgbChoice == 'j' || rgbChoice == 'J') {
        std::cout << "\nVordefinierte Farben:" << std::endl;
        std::cout << "1. Rot" << std::endl;
        std::cout << "2. Gruen" << std::endl;
        std::cout << "3. Blau" << std::endl;
        std::cout << "4. Cyan" << std::endl;
        std::cout << "5. Magenta" << std::endl;
        std::cout << "6. Gelb" << std::endl;
        std::cout << "7. Weiss" << std::endl;
        std::cout << "8. Regenbogen-Animation" << std::endl;
        std::cout << "9. Aus (Schwarz)" << std::endl;
        std::cout << "Waehlen Sie (1-9): ";
        
        int colorChoice;
        std::cin >> colorChoice;
        
        RGBColor logo, power, mic;
        
        switch (colorChoice) {
        case 1: logo = power = mic = RGBColor(255, 0, 0); break;      // Rot
        case 2: logo = power = mic = RGBColor(0, 255, 0); break;      // Grün
        case 3: logo = power = mic = RGBColor(0, 0, 255); break;      // Blau
        case 4: logo = power = mic = RGBColor(0, 255, 255); break;    // Cyan
        case 5: logo = power = mic = RGBColor(255, 0, 255); break;    // Magenta
        case 6: logo = power = mic = RGBColor(255, 255, 0); break;    // Gelb
        case 7: logo = power = mic = RGBColor(255, 255, 255); break;  // Weiß
        case 9: logo = power = mic = RGBColor(0, 0, 0); break;        // Aus
        case 8: // Regenbogen-Animation
            std::cout << "\nStarte Regenbogen-Animation (10 Sekunden)..." << std::endl;
            for (int i = 0; i < 100; i++) {
                float hue = (i * 3.6f); // 0-360 Grad
                // Einfache HSV zu RGB Konvertierung für Regenbogen
                float r = 0, g = 0, b = 0;
                if (hue < 60) { r = 255; g = (hue / 60) * 255; }
                else if (hue < 120) { r = ((120 - hue) / 60) * 255; g = 255; }
                else if (hue < 180) { g = 255; b = ((hue - 120) / 60) * 255; }
                else if (hue < 240) { g = ((240 - hue) / 60) * 255; b = 255; }
                else if (hue < 300) { b = 255; r = ((hue - 240) / 60) * 255; }
                else { b = ((360 - hue) / 60) * 255; r = 255; }
                
                logo = power = mic = RGBColor((unsigned char)r, (unsigned char)g, (unsigned char)b);
                SendRGBColors(deviceWrite, isWireless, logo, power, mic);
                Sleep(100);
            }
            logo = power = mic = RGBColor(0, 155, 222); // Corsair Blau
            break;
        default:
            logo = power = mic = RGBColor(0, 155, 222); // Corsair Blau (Standard)
        }
        
        if (colorChoice != 8) {
            if (SendRGBColors(deviceWrite, isWireless, logo, power, mic)) {
                std::cout << "RGB-Farben erfolgreich gesendet!" << std::endl;
            }
        }
        
        Sleep(500);
    }
    
    std::cout << "\nLese Daten vom Geraet..." << std::endl;
    std::cout << "(Druecken Sie Ctrl+C zum Beenden)" << std::endl << std::endl;

    // Event für Overlapped I/O
    OVERLAPPED overlapped = { 0 };
    overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    
    if (overlapped.hEvent == nullptr) {
        std::cerr << "Fehler beim Erstellen des Events!" << std::endl;
        CloseHandle(device);
        return 1;
    }

    std::vector<unsigned char> buffer(65);
    DWORD bytesRead = 0;
    int readCount = 0;

    while (true) {
        ResetEvent(overlapped.hEvent);
        bytesRead = 0;

        BOOL result = ReadFile(device, buffer.data(), static_cast<DWORD>(buffer.size()), 
                               &bytesRead, &overlapped);
        
        DWORD error = GetLastError();
        
        if (!result && error != ERROR_IO_PENDING) {
            std::cerr << "ReadFile fehlgeschlagen! Error: " << error << std::endl;
            break;
        }

        DWORD waitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
        
        if (waitResult == WAIT_OBJECT_0) {
            if (!GetOverlappedResult(device, &overlapped, &bytesRead, FALSE)) {
                std::cerr << "GetOverlappedResult fehlgeschlagen! Error: " << GetLastError() << std::endl;
                break;
            }

            if (bytesRead > 0) {
                readCount++;
                
                std::cout << "\n[" << readCount << "] Empfangen [" << bytesRead << " bytes]: ";
                for (DWORD i = 0; i < bytesRead && i < 16; ++i) {
                    std::cout << std::uppercase << std::setfill('0') << std::setw(2)
                              << std::hex << static_cast<int>(buffer[i]) << " ";
                }
                if (bytesRead > 16) std::cout << "...";
                std::cout << std::dec << std::endl;

                // Unterscheide Pakettypen
                if (bytesRead == 2) {
                    std::cout << ">>> LAUTSTAERKE-UPDATE (2 Bytes) <<<" << std::endl;
                    std::cout << "Lautstaerke: " << static_cast<int>(buffer[1]) << std::endl;
                }
                else if (bytesRead >= 4 && buffer[0] == 0x03 && buffer[1] == 0x01 && buffer[2] == 0x01) {
                    unsigned char event = buffer[3];
                    std::cout << "[PROTOKOLL OK] Header: 03 01 01, Event: 0x" 
                              << std::hex << static_cast<int>(event) << std::dec << std::endl;
                    
                    switch (event) {
                    case 0x0f:
                        std::cout << ">>> BATTERIE-UPDATE <<<" << std::endl;
                        if (bytesRead >= 7) {
                            uint16_t value = buffer[5] | (static_cast<uint16_t>(buffer[6]) << 8);
                            double percentage = static_cast<double>(value) / 10.0;
                            std::cout << "Batteriestand: " << std::fixed << std::setprecision(1) 
                                      << percentage << "%" << std::endl;
                        }
                        break;
                    case 0x10:
                        std::cout << ">>> LADESTATUS-UPDATE <<<" << std::endl;
                        if (bytesRead >= 6) {
                            unsigned char chargingByte = buffer[5];
                            std::cout << "Ladevorgang: ";
                            if (chargingByte == 0x01 ) {
                                std::cout << "Start" << std::endl;
                            }
                            else if (chargingByte == 0x02) {
                                std::cout << "END" << std::endl;
                            }
                        }
                        break;
                    case 0xA6:
                        std::cout << ">>> MUTE-STATUS-UPDATE <<<" << std::endl;
                        if (bytesRead >= 6) {
                            unsigned char muteByte = buffer[5];
                            std::cout << "Status: ";
                            if (muteByte == 0x00) {
                                std::cout << "UNMUTE (Mikrofon aktiv)" << std::endl;
                            }
                            else if (muteByte == 0x01) {
                                std::cout << "MUTE (Mikrofon stumm)" << std::endl;
                            }
                            else {
                                std::cout << "Unbekannt (0x" << std::hex << std::uppercase 
                                          << static_cast<int>(muteByte) << std::dec << ")" << std::endl;
                            }
                        }
                        break;
                    default:
                        std::cout << "[UNBEKANNTES EVENT] Code: 0x" << std::hex 
                                  << static_cast<int>(event) << std::dec << std::endl;
                        break;
                    }
                }
                std::cout << std::endl;
            }
        }
        else {
            std::cerr << "WaitForSingleObject fehlgeschlagen! Result: " << waitResult << std::endl;
            break;
        }
    }

    CloseHandle(overlapped.hEvent);
    
    // Setze Hardware-Modus zurück beim Beenden
    if (deviceWrite != INVALID_HANDLE_VALUE) {
        std::cout << "\n\nSetze Hardware-Modus zurueck..." << std::endl;
        SetHardwareMode(deviceWrite, isWireless);
        CloseHandle(deviceWrite);
    }
    
    CloseHandle(device);
    std::cout << "Geraet geschlossen. Programm beendet." << std::endl;
    return 0;
}
