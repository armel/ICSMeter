// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Get Button
void button(void *pvParameters)
{
  uint8_t optionOld = 1;
  uint8_t brightnessOld = 64;

  for (;;)
  {

    // Get screenshot
    getScreenshot();

    // Get button
    getButton();
        
    if(btnA || btnB || btnC) {
      screensaver = millis();
      optionOld = preferences.getUInt("option");
      brightnessOld = preferences.getUInt("brightness");
    }

    if(screensaverMode == false)
    {
      if (btnA == 1 || buttonLeftPressed == 1)
      {
        option = 0;
        buttonLeftPressed = 0;
        if(optionOld != option)
          preferences.putUInt("option", option);
      }
      else if (btnB == 1 || buttonCenterPressed == 1)
      {
        option = 1;
        buttonCenterPressed = 0;
        if(optionOld != option)
          preferences.putUInt("option", option);
      }
      else if (btnC == 1 || buttonRightPressed == 1)
      {
        option = 2;
        buttonRightPressed = 0;
        if(optionOld != option)
          preferences.putUInt("option", option);
      }
      else if (btnL == 1) {
        brightness -= 1;
        if(brightness < 1) {
          brightness = 1;
        }
        setBrightness(brightness);
        if(brightnessOld != brightness)
          preferences.putUInt("brightness", brightness);
        vTaskDelay(pdMS_TO_TICKS(50));
      }
      else if (btnM == 1) {
        shutdown();
      }
      else if (btnR == 1) {
        brightness += 1;
        if(brightness > 254) {
          brightness = 254;
        }
        setBrightness(brightness);
        if(brightnessOld != brightness)
          preferences.putUInt("brightness", brightness);
        vTaskDelay(pdMS_TO_TICKS(50));
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}