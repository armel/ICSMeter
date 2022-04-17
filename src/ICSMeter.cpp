// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <ICSMeter.h>
#include <image.h>
#include "functions.h"
#include "command.h"
#include "tasks.h"

// Setup
void setup()
{
  uint8_t loop = 0;

  // Debug
  Serial.begin(115200);

  // Init M5
  M5.begin(true, true, false, false);

  // Init Led
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  // Init Power
  power();

  // Preferences
  preferences.begin(NAME);
  option = preferences.getUInt("option", 2);
  brightness = preferences.getUInt("brightness", 64);

  // Bin Loader
  binLoader();

  // Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED && loop <= 10)
  {
    delay(250);
    loop += 1;
  }

  // Start server (for Web site Screen Capture)
  httpServer.begin();

// Let's go
#if BOARD == CORE2
  M5.Axp.SetLed(0);
#endif

  setBrightness(brightness);
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BACK);

  viewGUI();

  if(IC_MODEL == 705 && IC_CONNECT == BT)
  {
    CAT.register_callback(callbackBT);

    if (!CAT.begin(NAME))
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
      1);       // Core where the task should run
}

// Main loop
void loop()
{
  static uint8_t alternance = 0;
  static uint8_t tx = 0;

  if(checkConnection()) {
    tx = getTX();
    if(tx != 0) screensaver = millis();   // If transmit, refresh tempo

    if (screensaverMode == 0 && screenshot == false) {

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
     
      viewMenu();
      viewBattery();
      viewBaseline(alternance);

      getMode();
      getFrequency();

      switch (option)
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
    }
  }

  alternance = (alternance++ < 30) ? alternance : 0;

  // Manage Screen Saver
  wakeAndSleep();
}