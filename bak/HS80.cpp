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

struct HIDDeviceInfo {
    unsigned short vendorId;
    unsigned short productId;
    std::string devicePath;
    std::wstring productName;
    std::wstring manufacturerName;
    unsigned short usagePage;
    unsigned short usage;
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
    std::cout << "Lese Daten vom Geraet..." << std::endl;
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
    CloseHandle(device);
    return 0;
}
