// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "settings.h"
#include "ICSMeter.h"
#include "debug.h"
#include "font.h"
#include "image.h"
#include "tools.h"
#include "webIndex.h"
#include "functions.h"
#include "command.h"
#include "menu.h"
#include "tasks.h"

// Setup
void setup()
{
  uint8_t loop = 0;

  // Init M5
  auto cfg = M5.config();
  M5.begin(cfg);

  pinMode(32, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);

  // Init Encoder
  if(DEBUG == 2)
  {
    sensor.begin();
  }

  // Init Display
  display.begin();

  offsetX = (display.width() - 320) / 2; 
  offsetY = (display.height() - 240) / 2;

  // Preferences
  preferences.begin(NAME);
  measure = preferences.getUInt("measure", 1);
  brightness = preferences.getUInt("brightness", 64);
  transverter = preferences.getUInt("transverter", 0);
  beep = preferences.getUInt("beep", 0);
  screensaver = preferences.getUInt("screensaver", 60);
  theme = preferences.getUInt("theme", 0);
  led = preferences.getUInt("led", 0);
  config = preferences.getUInt("config", 0);

  // Init Setting
  icModel = strtol(choiceConfig[(config * 4) + 0], 0, 10);
  icCIVAddress = strtol(String(choiceConfig[(config * 4) + 1]).substring(2, 4).c_str(), 0, 16);
  if(String(choiceConfig[(config * 4) + 2]) == "USB")
  {
    icConnect = USB;
    icSerialDevice = choiceConfig[(config * 4) + 3];
  }
  else
  {
    icConnect = BT;
    uint8_t i = 0;
    while(i <= 15)
    {
      icAddress[i/3] = strtol(String(choiceConfig[(config * 4) + 3]).substring(i, i + 2).c_str(), 0, 16);
      Serial.println(icAddress[i/3]);
      i += 3;
    }
  }
  icConnectOld = icConnect;

  // Init Sprite
  if(icConnect == USB || ESP.getPsramSize() > 0) // Sprite mode
  {
    needleSprite.setPsram(true);
    needleSprite.createSprite(320, 130);
  }

  logoSprite.setColorDepth(8);
  logoSprite.createSprite(44, 22);
  logoSprite.drawJpg(logo, sizeof(logo), 0, 0, 44, 22);

  // Init Led
  if(M5.getBoard() == m5::board_t::board_M5Stack) {
    FastLED.addLeds<NEOPIXEL, 15>(leds, NUM_LEDS);  // GRB ordering is assumed
  }
  else if(M5.getBoard() == m5::board_t::board_M5StackCore2) {
    FastLED.addLeds<NEOPIXEL, 25>(leds, NUM_LEDS);  // GRB ordering is assumed
  }

  // If led strip...
  /*
  FastLED.addLeds<WS2811,Neopixel_PIN,GRB>(strip, NUM_LEDS_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(32);
  */
 
  // Bin Loader
  binLoader();

  // Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED && loop <= 10)
  {
    vTaskDelay(250);
    loop += 1;
  }

  // Start server (for Web site Screen Capture)
  httpServer.begin();

  display.setBrightness(map(brightness, 1, 100, 1, 254));
  display.setRotation(1);

  viewGUI();

  serialBT.register_callback(callbackBT);

  if(icModel == 705 && icConnect == BT)
  {
    uint8_t attempt = 0;
    char valString[24] = "Connexion";
    char dot[4] = "";

    value(strcat(valString, dot));

    serialBT.begin(NAME, true);
    btClient = serialBT.connect(icAddress);

    while(!btClient && attempt < 3) 
    {
      Serial.printf("Attempt %d - Make sure IC-705 is available and in range.", attempt + 1);

      sprintf(dot, "%.*s", 1, ",,,,,");
      value(strcat(valString, dot));

      btClient = serialBT.connect(icAddress);
      attempt++;
    }
  
    if(!btClient) 
    {
      if (!serialBT.begin(NAME))
      {
        Serial.println("An error occurred initializing Bluetooth");
      }
      else
      {
        Serial.println("Bluetooth initialized");
      }
    }
    else
    {
      display.setTextDatum(CC_DATUM);
      display.setFont(&stencilie16pt7b);
      display.setTextPadding(194);
      display.setTextColor(TFT_FRONT, TFT_BACK);
      display.drawString("", 160 + offsetX, 180 + offsetY);
    }
  }
  else
  {
    if (WiFi.status() == WL_CONNECTED) wifiConnected = true;
  }

  // Multitasking task for retreive button
  xTaskCreatePinnedToCore(
      button,   // Function to implement the task
      "button", // Name of the task
      8192,     // Stack size in words
      NULL,     // Task input parameter
      4,        // Priority of the task
      NULL,     // Task handle
      0);       // Core where the task should run
}

// Main loop
void loop()
{
  static uint8_t alternance = 0;
  static uint8_t tx = 0;
  
  if(checkConnection()) {
    if(alternance == 8) tx = getTX();

    if(tx != 0) screensaverTimer = millis();   // If transmit, refresh tempo

    if (screensaverMode == false && screenshot == false && settingsMode == false)
    {
      settingLock = true;

      if(alternance == 0) getFrequency();
      if(alternance == 4) getMode();

      if(strcmp(choiceLed[led], "TX") == 0)
      {
        if(tx == 0) {
          for(uint8_t i = 0; i <= 9; i++){
            leds[i] = CRGB::Black;
          }
          FastLED.setBrightness(16);
          FastLED.show();
        }
        else {
          for(uint8_t i = 0; i <= 9; i++){
            leds[i] = CRGB::Red;
          }
          FastLED.setBrightness(16);
          FastLED.show();
        }
      }
     
      switch (measure)
      {
      case 0:
        getPower();
        break;

      case 1:
        getSmeter();
        break;

      case 2:
        getSWR();
        break;
      }

      viewMeasure();
      viewBattery();

      settingLock = false;
    }
  }

  alternance = (alternance++ < 12) ? alternance : 0;

  // Manage Screen Saver
  wakeAndSleep();

  if(DEBUG == 1)
  {
    Serial.printf("%d kb %d kb %d kb %d kb\n", 
      ESP.getHeapSize() / 1024,
      ESP.getFreeHeap() / 1024, 
      ESP.getPsramSize() / 1024, 
      ESP.getFreePsram() / 1024
    );

    Serial.printf("%d %ld %ld\n", screensaverMode, millis() - screensaverTimer, long(screensaver * 60 * 1000));
  }
}