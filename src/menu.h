// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

void viewMenu() {
    M5.Lcd.fillRoundRect(64, 4, 192, 192, 8, TFT_MODE_BACK);
    M5.Lcd.drawRoundRect(64, 4, 192, 192, 8, TFT_MODE_BORDER);

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(&YELLOWCRE8pt7b);
    M5.Lcd.setTextPadding(196);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString("SETTINGS", 160, 20);

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(0);
    M5.Lcd.setTextPadding(188);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString(String(NAME) + " V" + String(VERSION) + " by " + String(AUTHOR), 160, 34);

    M5.Lcd.drawFastHLine(66, 42, 188, TFT_WHITE);
}

void viewOption(int8_t settingsChoice, boolean settingsSelect) {
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(&tahoma8pt7b);
    M5.Lcd.setTextPadding(188);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_MODE_BACK);

    size_t stop = sizeof(settings) / sizeof(settings[0]);
    for(uint8_t i = 0; i < stop; i++) {
        if(settingsChoice == i && settingsSelect == false) {
            M5.Lcd.setTextColor(TFT_MODE_BACK, TFT_WHITE);
            M5.Lcd.drawString(settings[i], 160, 56 + (i * 18));
        }
        else if(settingsChoice == i && settingsSelect == true) {
            M5.Lcd.setTextColor(TFT_MODE_BACK, TFT_WHITE);
            M5.Lcd.drawString(settings[i], 160, 56 + (i * 18));
        }
        else {
            M5.Lcd.setTextColor(TFT_WHITE, TFT_MODE_BACK);
            M5.Lcd.drawString(settings[i], 160, 56 + (i * 18));
        }
    }
}