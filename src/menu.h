// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

void viewMenu() {
    uint8_t x = 44;
    uint8_t y = 4;
    uint8_t w = 320 - (x * 2);
    uint8_t h = 190;

    M5.Lcd.fillRoundRect(x, y, w, h, 8, TFT_MENU_BACK);
    M5.Lcd.drawRoundRect(x, y, w, h, 8, TFT_MODE_BORDER);

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(&YELLOWCRE8pt7b);
    M5.Lcd.setTextPadding(w - 2);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString("SETTINGS", 160, 20 + y);

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(0);
    M5.Lcd.setTextPadding(w - 2);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString(String(NAME) + " V" + String(VERSION) + " by " + String(AUTHOR), 160, 34 + y);

    M5.Lcd.drawFastHLine(x, 42 + y, w, TFT_WHITE);
}

void viewOption(int8_t settingsChoice, boolean settingsSelect) {
    uint8_t x = 44;
    uint8_t y = 4;
    uint8_t w = 320 - (x * 2);

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(&tahoma8pt7b);
    M5.Lcd.setTextPadding(w - 2);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_MENU_BACK);

    size_t stop = sizeof(settings) / sizeof(settings[0]);
    for(uint8_t i = 0; i < stop; i++) {
        if(settingsChoice == i && settingsSelect == false) {
            M5.Lcd.setTextColor(TFT_MENU_BACK, TFT_WHITE);
            M5.Lcd.drawString(settings[i], 160, 56 + y + (i * 18));
        }
        else if(settingsChoice == i && settingsSelect == true) {
            M5.Lcd.setTextColor(TFT_MENU_BACK, TFT_WHITE);
            M5.Lcd.drawString(settings[i], 160, 56 + y + (i * 18));
        }
        else {
            M5.Lcd.setTextColor(TFT_WHITE, TFT_MENU_BACK);
            M5.Lcd.drawString(settings[i], 160, 56 + y + (i * 18));
        }
    }
}