// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Get Button
void button(void *pvParameters)
{
  int8_t beepOld = 0;
  int8_t transverterOld = 0;
  int8_t screensaverOld = 0;
  uint8_t brightnessOld = 64;
  uint8_t themeOld = 0;
  static int8_t settingsChoice = 0;
  static boolean settingsSelect = false;

  uint8_t x = 44;
  uint8_t y = 4;
  uint16_t w = 320 - (x * 2);
  uint8_t h = 185;

  for (;;)
  {
    // Get screenshot
    getScreenshot();

    // Get button
    getButton();
            
    if(btnA || btnB || btnC) {

      if (M5.Speaker.isEnabled() && beep > 0)
      {
        // set master volume (0~255)
        M5.Speaker.setVolume(beep);
        if(btnA || btnC)
        {
          // play beep sound 1000Hz 100msec (background task)
          M5.Speaker.tone(1000, 50);
        }
        else if(btnB)
        {
          // play beep sound 2000Hz 100msec (background task)
          M5.Speaker.tone(2000, 50);
        }
        // wait done
        while (M5.Speaker.isPlaying()) { vTaskDelay(1); }
      }

      screensaverTimer = millis();
      brightnessOld = preferences.getUInt("brightness");
      transverterOld = preferences.getUInt("transverter");
      screensaverOld = preferences.getUInt("screensaver");
      themeOld = preferences.getUInt("theme");
    }

    if(settingsMode == false)
    {
      // Enter settings
      if(btnB) {
        settingsMode = true;
        while(settingLock == true) {
          vTaskDelay(10);
        }
        viewMenu(x, y, w, h);
        viewOption(settingsChoice, settingsSelect, x, y, w);
        vTaskDelay(300);
      }
      vTaskDelay(100);
    }
    // Select settings
    else if(settingsMode == true)
    {
      if(settingsSelect == false) {
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

          viewOption(settingsChoice, settingsSelect, x, y, w);
        }
        else if(btnB) {
          settingsSelect = true;
          viewOption(settingsChoice, settingsSelect, x, y, w);

          String settingsString = String(settings[settingsChoice]);
          if(settingsString == "Shutdown") {
            shutdown();
          }
          else if(settingsString == "Exit") {
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }
      // Manage settings
      else if(settingsSelect == true) {
        String settingsString = String(settings[settingsChoice]);

        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.setFont(&YELLOWCRE8pt7b);
        M5.Lcd.setTextPadding(w - 2);
        M5.Lcd.setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);

        // Measured Values
        if(settingsString == "Measured Values")
        {
          M5.Lcd.drawString(String(choiceMeasures[measure]), 160, h - 6);

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
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Themes
        if(settingsString == "Themes")
        {
          M5.Lcd.drawString(String(choiceThemes[theme]), 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              theme -= 1;
              if(theme < 0) {
                theme = 1;
              }
            }
            else if(btnC == 1) {
              theme += 1;
              if(theme > 1) {
                theme = 0;
              }
            }
          }
          else if(btnB == 1) {
            if(themeOld != theme)
              preferences.putUInt("theme", theme);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Brightness
        if(settingsString == "Brightness")
        {
          M5.Lcd.drawString(String(choiceBrightness[0]) + " " + String(brightness) + "%", 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              brightness -= 1;
              if(brightness < 1) {
                brightness = 1;
              }
            }
            else if(btnC == 1) {
              brightness += 1;
              if(brightness > 100) {
                brightness = 100;
              }
            }
          }
          else if(btnB == 1) {
            if(brightnessOld != brightness)
              preferences.putUInt("brightness", brightness);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          setBrightness(map(brightness, 1, 100, 1, 254));
          vTaskDelay(pdMS_TO_TICKS(25));
        }

        // Transverter
        else if(settingsString == "Transverter Mode")
        {
          if(transverter == 0)
          {
            M5.Lcd.drawString("OFF", 160, h - 6);
          }
          else
          {
            String transverterStringOld = String(choiceTransverter[transverter]);
            String transverterStringNew = "";
            uint8_t lenght = transverterStringOld.length();
            int8_t i;

            for(i = lenght - 6; i >= 0; i -= 3) 
            {
              transverterStringNew = "." + transverterStringOld.substring(i, i + 3) + transverterStringNew;
            }

            if(i == -3) 
            {
              transverterStringNew = transverterStringNew.substring(1, transverterStringNew.length());
            }
            else 
            {
              transverterStringNew = transverterStringOld.substring(0, i + 3) + transverterStringNew;
            }

            M5.Lcd.drawString(transverterStringNew, 160, h - 6);
          }

          size_t stop = sizeof(choiceTransverter) / sizeof(choiceTransverter[0]);
          stop--;

          if(btnA || btnC) {
            if(btnA == 1) {
              transverter -= 1;
              if(transverter < 0) {
                transverter = stop;
              }
            }
            else if(btnC == 1) {
              transverter += 1;
              if(transverter > stop) {
                transverter = 0;
              }
            }
          }
          else if(btnB == 1) {
            if(transverterOld != transverter)
              preferences.putUInt("transverter", transverter);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Beep
        else if(settingsString == "Beep")
        {
          M5.Lcd.drawString(String(choiceBeep[0]) + " " + String(beep) + "%", 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              beep -= 1;
              if(beep < 0) {
                beep = 0;
              }
            }
            else if(btnC == 1) {
              beep += 1;
              if(beep > 100) {
                beep = 100;
              }
            }
          }
          else if(btnB == 1) {
            if(beepOld != beep)
              preferences.putUInt("beep", beep);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(25));
        }

        // Screensaver
        else if(settingsString == "Screensaver")
        {
          M5.Lcd.drawString(String(choiceScreensaver[0]) + " " + String(screensaver) + " MIN", 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              screensaver -= 1;
              if(screensaver < 1) {
                screensaver = 1;
              }
            }
            else if(btnC == 1) {
              screensaver += 1;
              if(screensaver > 60) {
                screensaver = 60;
              }
            }
          }
          else if(btnB == 1) {
            if(screensaverOld != screensaver)
              preferences.putUInt("screensaver", screensaver);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(25));
        }

        // IP Address
        else if(settingsString == "IP Address")
        {
          M5.Lcd.drawString(String(WiFi.localIP().toString().c_str()), 160, h - 6);

          if(btnB == 1) {
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));  
  }    
}