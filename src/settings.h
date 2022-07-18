// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// IC Config
//  For each line, 
//  IC Model (705, 7300 or 9700), 
//  CI_V Address, 
//  IC Connect (USB or BT),
//  PC Serial Device (if USB) or 705 BT Address (if BT)
const char *choiceConfig[] = {
    "7300", "0xA4", "USB", "/dev/ttyUSB0",     
     "705", "0xA4", "USB", "/dev/ttyACM0",    
     "705", "0xA4", "BT", "01:A1:B2:C3:D4:10" 
};

// Wifi Config
#define WIFI_SSID       "YOUR WIFI SSID"        // Your WiFi SSID
#define WIFI_PASSWORD   "YOUR WIFI PASSWORD"    // Your WiFi Password

// Proxy Config
#define BAUD_RATE 115200                        // Proxy Serial Device Baud Rate
#define PROXY_URL "http://192.168.1.32"         // Proxy HTTP URL
#define PROXY_PORT 1234                         // Proxy HTTP Port

// Transverter
#define TRANSVERTER_LO_1    116000000           // LO Frequency if you're using a Transverter
#define TRANSVERTER_LO_2    118000000           // LO Frequency if you're using a Transverter
#define TRANSVERTER_LO_3    404000000           // LO Frequency if you're using a Transverter
#define TRANSVERTER_LO_4    406000000           // LO Frequency if you're using a Transverter
#define TRANSVERTER_LO_5   9968000000           // LO Frequency if you're using a Transverter