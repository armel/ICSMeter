// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Get Button
void button(void *pvParameters)
{
  int8_t measureOld = 1;
  int8_t transverterOld = 0;
  uint8_t brightnessOld = 64;
  static int8_t settingsChoice = 0;
  static boolean settingsSelect = false;

  for (;;)
  {
    // Get screenshot
    getScreenshot();

    // Get button
    getButton();
            
    if(btnA || btnB || btnC) {
      screensaver = millis();
      measureOld = preferences.getUInt("measure");
      brightnessOld = preferences.getUInt("brightness");
      transverterOld = preferences.getUInt("transverter");
    }

    if(settingsMode == false && btnB) {
      viewMenu();
      viewOption(settingsChoice, settingsSelect);
      settingsMode = true;
    }
    else if(settingsMode == true && settingsSelect == false) {
      if(btnA || btnC) {
        if(btnA) {
          settingsChoice--;
        }
        else if(btnC) {
          settingsChoice++;
        }

        size_t stop = sizeof(settings) / sizeof(settings[0]);
        stop--;

        settingsChoice = (settingsChoice < 0) ? stop : settingsChoice;
        settingsChoice = (settingsChoice > stop) ? 0 : settingsChoice;

        viewOption(settingsChoice, settingsSelect);
      }
      else if(btnB) {
        settingsSelect = true;
        viewOption(settingsChoice, settingsSelect);

        String settingsString = String(settings[settingsChoice]);
        if(settingsString == "Shutdown") {
          shutdown();
        }
        else if(settingsString == "Exit") {
          clearData();
          viewGUI();
          settingsMode = false;
          settingsSelect = false;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
      }
    }
    
    else if(settingsMode == true && settingsSelect == true) {
      String settingsString = String(settings[settingsChoice]);

      M5.Lcd.setTextDatum(CC_DATUM);
      M5.Lcd.setFont(&YELLOWCRE8pt7b);
      M5.Lcd.setTextPadding(188);
      //M5.Lcd.setTextColor(TFT_WHITE, TFT_MODE_BACK);

      // Measured Values
      if(settingsString == "Measured Values")
      {
        M5.Lcd.drawString(String(choiceMeasures[measure]), 160, 180);

        if(btnA || btnC) {
          if(btnA == 1) {
            measure -= 1;
            if(measure < 0) {
              measure = 2;
            }
          }
          else if(btnC == 1) {
            measure += 1;
            if(measure > 2) {
              measure = 0;
            }
          }
        }
        else if(btnB == 1) {
          if(measureOld != measure)
            preferences.putUInt("measure", measure);
          clearData();
          viewGUI();
          settingsMode = false;
          settingsSelect = false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
      }

      // Brightness
      else if(settingsString == "Brightness")
      {
        M5.Lcd.drawString(String(choiceBrightness[0]) + " " + String(map(brightness, 1, 254, 1, 100)) + "%", 160, 184);

        if(btnA || btnC) {
          if(btnA == 1) {
            brightness -= 1;
            if(brightness < 1) {
              brightness = 1;
            }
          }
          else if(btnC == 1) {
            brightness += 1;
            if(brightness > 254) {
              brightness = 254;
            }
          }
        }
        else if(btnB == 1) {
          if(brightnessOld != brightness)
            preferences.putUInt("brightness", brightness);
          clearData();
          viewGUI();
          settingsMode = false;
          settingsSelect = false;
        }
        setBrightness(brightness);
      }

      // Transverter
      else if(settingsString == "Transverter Mode")
      {
        M5.Lcd.drawString(String(choiceTransverter[transverter]), 160, 184);

        if(btnA || btnC) {
          if(btnA == 1) {
            transverter -= 1;
            if(transverter < 0) {
              transverter = 1;
            }
          }
          else if(btnC == 1) {
            transverter += 1;
            if(transverter > 1) {
              transverter = 0;
            }
          }
        }
        else if(btnB == 1) {
          if(transverterOld != transverter)
            preferences.putUInt("transverter", transverter);
          clearData();
          viewGUI();
          settingsMode = false;
          settingsSelect = false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
      }

      // IP Address
      else if(settingsString == "IP Address")
      {
        M5.Lcd.drawString(String(WiFi.localIP().toString().c_str()), 160, 184);

        if(btnB == 1) {
          clearData();
          viewGUI();
          settingsMode = false;
          settingsSelect = false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}