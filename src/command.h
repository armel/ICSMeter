// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Send CI-V Command by Bluetooth
void sendCommandBt(char *request, size_t n, char *buffer, uint8_t limit)
{
  uint8_t byte1, byte2, byte3;
  uint8_t counter = 0;

  while (counter != limit)
  {
    for (uint8_t i = 0; i < n; i++)
    {
      serialBT.write(request[i]);
    }

    vTaskDelay(50);

    while (serialBT.available())
    {
      byte1 = serialBT.read();
      byte2 = serialBT.read();

      if (byte1 == 0xFE && byte2 == 0xFE)
      {
        counter = 0;
        byte3 = serialBT.read();
        while (byte3 != 0xFD)
        {
          buffer[counter] = byte3;
          byte3 = serialBT.read();
          counter++;
          if (counter > limit)
          {
            if (DEBUG == 1)
            {
              Serial.print(" Overflow");
            }
            break;
          }
        }
      }
    }
    startup = false;
  }
  // Serial.println(" Ok");
}

// Send CI-V Command by Wifi
void sendCommandWifi(char *request, size_t n, char *buffer, uint8_t limit)
{
  static uint8_t proxyError = 0;

  HTTPClient http;
  uint16_t httpCode;

  String command = "";
  String response = "";

  char s[4];

  for (uint8_t i = 0; i < n; i++)
  {
    sprintf(s, "%02x,", request[i]);
    command += String(s);
  }

  command += BAUD_RATE + String(",") + icSerialDevice;

  http.begin(civClient, PROXY_URL + String(":") + PROXY_PORT + String("/") + String("?civ=") + command); // Specify the URL
  http.addHeader("User-Agent", "M5Stack");                                                               // Specify header
  http.addHeader("Connection", "keep-alive");                                                            // Specify header
  http.setTimeout(100);                                                                                  // Set Time Out
  httpCode = http.GET();                                                                                 // Make the request

  if (httpCode == 200)
  {
    proxyConnected = true;
    proxyError = 0;

    response = http.getString(); // Get data
    response.trim();
    response = response.substring(4);

    if (response == "")
    {
      txConnected = false;
    }
    else
    {
      txConnected = true;
      startup = false;

      for (uint8_t i = 0; i < limit; i++)
      {
        buffer[i] = strtol(response.substring(i * 2, (i * 2) + 2).c_str(), NULL, 16);
      }

      if (DEBUG == 1)
      {
        Serial.println("-----");
        Serial.print(response);
        Serial.print(" ");
        Serial.println(response.length());

        for (uint8_t i = 0; i < limit; i++)
        {
          Serial.print(int(buffer[i]));
          Serial.print(" ");
        }
        Serial.println(" ");
        Serial.println("-----");
      }
    }
  }
  else
  {
    proxyError++;
    if (proxyError > 10)
    {
      proxyError = 10;
      proxyConnected = false;
    }
  }
  http.end(); // Free the resources
}

// Send CI-V Command dispatcher
void sendCommand(char *request, size_t n, char *buffer, uint8_t limit)
{
  if (icModel == 705 && icConnect == BT)
    sendCommandBt(request, n, buffer, limit);
  else
    sendCommandWifi(request, n, buffer, limit);
}

// Get Smeter
void getSmeter()
{
  char valString[32];  

  static char buffer[6];
  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x15, 0x02, 0xFD};
  char str[2];

  uint8_t val0 = 0;
  float_t val1 = 0;
  static uint8_t val2 = 0;

  float_t angle = 0;

  if(DEBUG == 2) {
    val0 = debugEncoder();
  }
  else
  {
    size_t n = sizeof(request) / sizeof(request[0]);
    sendCommand(request, n, buffer, 6);

    sprintf(str, "%02x%02x", buffer[4], buffer[5]);
    val0 = atoi(str);
  }

  if (val0 <= 120)
  { // 120 = S9 = 9 * (40/3)
    val1 = val0 / (40 / 3.0f);
  }
  else
  { // 240 = S9 + 60
    val1 = (val0 - 120) / 2.0f;
  }

  if (abs(val0 - val2) > 1 || reset == true)
  {
    val2 = val0;
    reset = false;

    if (val0 <= 13)
    {
      angle = -38.60f;
      snprintf(valString, 16, "%s %d", "S", int(round(val1)));
    }
    else if (val0 <= 120)
    {
      angle = mapFloat(val0, 14, 120, -38.60f, 6.50f); // SMeter image start at S1 so S0 is out of image on the left...
      snprintf(valString, 16, "%s %d", "S", int(round(val1)));
    }
    else
    {
      angle = mapFloat(val0, 121, 240, 6.50f, 39.40f);
      if (int(round(val1) < 10))
        snprintf(valString, 16, "%s%d %s", "S 9 + 0", int(round(val1)), "dB");
      else
        snprintf(valString, 16, "%s %d %s", "S 9 +", int(round(val1)), "dB");
    }

    // Debug trace
    if (DEBUG == 1)
    {
      Serial.printf("%s %d %f %f \n", valString, val0, val1, angle);
    }

    // Draw line
    needle(angle);

    // Write Value
    value(valString);

    // If M5GO
    if(strcmp(choiceLed[led], "MEASURES") == 0)
    {
      if(val0 <= 13) {
        for(uint8_t i = 0; i <= 4; i++)
        {
          leds[4 - i] = CRGB::WhiteSmoke;
          leds[5 + i] = CRGB::WhiteSmoke;
        }

        FastLED.setBrightness(16);
        FastLED.show();
      }
      else if(val0 <= 120) {
        uint8_t j = map(val0, 14, 120, 0, 4);
        for(uint8_t i = 0; i <= 4; i++)
        {
          if(i <= j)
          {
            leds[4 - i] = CRGB::Blue;
            leds[5 + i] = CRGB::Blue;
          }
          else
          {
            leds[4 - i] = CRGB::WhiteSmoke;
            leds[5 + i] = CRGB::WhiteSmoke;
          }
        }

        FastLED.setBrightness(16);
        FastLED.show();
      }
      else {
        uint8_t j = map(val0, 121, 240, 0, 4);

        for(uint8_t i = 0; i <= 4; i++)
        {
          if(i <= j)
          {
            leds[i] = CRGB::Red;
            leds[9 - i] = CRGB::Red;
          }
          else
          {
            leds[i] = CRGB::Blue;
            leds[9 - i] = CRGB::Blue;
          }
        }
        
        FastLED.setBrightness(16);
        FastLED.show();
      }
    }

    // If led strip...
    /*
    uint8_t limit = map(val0, 0, 241, 0, NUM_LEDS_STRIP);

    for (uint8_t i = 0; i < limit; i++)
    {
      if (i < NUM_LEDS_STRIP / 2)
      {
        strip[i] = CRGB::Blue;
      }
      else
      {
        strip[i] = CRGB::Red;
      }
    }

    for (uint8_t i = limit; i < NUM_LEDS_STRIP; i++)
    {
      strip[i] = CRGB::White;
    }
    FastLED.show();
    */
  }
}

// Get SWR
void getSWR()
{
  char valString[32];  

  static char buffer[6];
  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x15, 0x12, 0xFD};
  char str[2];

  uint8_t val0 = 0;
  float_t val1 = 0;
  static uint8_t val3 = 0;

  float_t angle = 0;

  if(DEBUG == 2) {
    val0 = debugEncoder();
  }
  else
  {
    size_t n = sizeof(request) / sizeof(request[0]);
    sendCommand(request, n, buffer, 6);

    sprintf(str, "%02x%02x", buffer[4], buffer[5]);
    val0 = atoi(str);
  }

  if (val0 != val3 || reset == true)
  {
    val3 = val0;
    reset = false;

    if (val0 <= 48)
    {
      angle = mapFloat(val0, 0, 48, -38.00f, -29.00f);
      val1 = mapFloat(val0, 0, 48, 1.0, 1.5);
    }
    else if (val0 <= 80)
    {
      angle = mapFloat(val0, 49, 80, -29.00f, -22.0f);
      val1 = mapFloat(val0, 49, 80, 1.5, 2.0);
    }
    else if (val0 <= 110)
    {
      angle = mapFloat(val0, 81, 110, -22.0f, -13.00f);
      val1 = mapFloat(val0, 81, 110, 2.0, 2.5);
    }
    else if (val0 <= 120)
    {
      angle = mapFloat(val0, 111, 120, -13.00f, -8.50f);
      val1 = mapFloat(val0, 111, 120, 2.5, 3.0);
    }
    else if (val0 <= 146)
    {
      angle = mapFloat(val0, 121, 146, -8.50f, -2.50f);
      val1 = mapFloat(val0, 121, 146, 3.0, 3.5);
    }
    else if (val0 <= 155)
    {
      angle = mapFloat(val0, 147, 155, -2.50f, 0.8f);
      val1 = mapFloat(val0, 147, 155, 3.5, 4.0);
    }
    else if (val0 <= 175)
    {
      angle = mapFloat(val0, 156, 175, 0.8f, 7.0f);
      val1 = mapFloat(val0, 156, 175, 4.0, 5.0);
    }
    else if (val0 <= 225)
    {
      angle = mapFloat(val0, 176, 225, 7.0f, 19.0f);
      val1 = mapFloat(val0, 176, 225, 5.0, 10.0);
    }
    else
    {
      angle = mapFloat(val0, 226, 255, 19.0f, 30.50f);
      val1 = mapFloat(val0, 226, 255, 10.0, 50.0);
    }

    snprintf(valString, 32, "%s %.2f", "SWR", val1);

    // Debug trace
    if (DEBUG == 1)
    {
      Serial.printf("%s %d %f %f \n", valString, val0, val1, angle);
    }

    // Draw line
    needle(angle);

    // Write Value
    value(valString);

    // If M5GO
    if(strcmp(choiceLed[led], "MEASURES") == 0)
    {
      if(val0 <= 110) {
        uint8_t j = map(val0, 0, 120, 0, 4);
        for(uint8_t i = 0; i <= 4; i++)
        {
          if(i <= j)
          {
            leds[4 - i] = CRGB::Green;
            leds[5 + i] = CRGB::Green;
          }
          else
          {
            leds[4 - i] = CRGB::WhiteSmoke;
            leds[5 + i] = CRGB::WhiteSmoke;
          }
        }

        FastLED.setBrightness(16);
        FastLED.show();
      }
      else {

        for(uint8_t i = 0; i <= 4; i++)
        {
          leds[i] = CRGB::Red;
          leds[9 - i] = CRGB::Red;
        }
        
        FastLED.setBrightness(16);
        FastLED.show();
      }
    }
  }
}

// Get Power
void getPower()
{
  char valString[32];  

  static char buffer[6];
  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x15, 0x11, 0xFD};
  char str[2];

  uint8_t val0 = 0;
  float_t val1 = 0;
  float_t val2 = 0;
  static uint8_t val3 = 0;

  float_t angle = 0;

  if(DEBUG == 2) {
    val0 = debugEncoder();
  }
  else
  {
    size_t n = sizeof(request) / sizeof(request[0]);
    sendCommand(request, n, buffer, 6);

    sprintf(str, "%02x%02x", buffer[4], buffer[5]);
    val0 = atoi(str);
  }

  if (val0 != val3 || reset == true)
  {
    val3 = val0;
    reset = false;

    if (val0 <= 27)
    {
      angle = mapFloat(val0, 0, 27, -38.60f, -27.25f);
      val1 = mapFloat(val0, 0, 27, 0, 0.5);
    }
    else if (val0 <= 51)
    {
      angle = mapFloat(val0, 28, 51, -27.25f, -21.25f);
      val1 = mapFloat(val0, 28, 51, 0.5, 1.0);
    }
    else if (val0 <= 80)
    {
      angle = mapFloat(val0, 51, 80, -21.25f, -13.00f);
      val1 = mapFloat(val0, 51, 80, 1.0, 2.0);
    }
    else if (val0 <= 106)
    {
      angle = mapFloat(val0, 81, 106, -13.00f, -6.50f);
      val1 = mapFloat(val0, 81, 106, 2.0, 3.0);
    }
    else if (val0 <= 124)
    {
      angle = mapFloat(val0, 107, 124, -6.50f, 0.0f);
      val1 = mapFloat(val0, 107, 124, 3.0, 4.0);
    }
    else if (val0 <= 144)
    {
      angle = mapFloat(val0, 125, 148, 0.0f, 6.00f);
      val1 = mapFloat(val0, 125, 148, 4.00, 5.0);
    }
    else
    {
      angle = mapFloat(val0, 149, 232, 6.00f, 28.50f);
      val1 = mapFloat(val0, 149, 232, 5.0, 10.0);
    }

    val2 = round(val1 * 10);
    if (icModel == 705)
      snprintf(valString, 16, "%s %.2f %s", "PWR", (val2 / 10), "W");
    else
      snprintf(valString, 16, "%s %.2f %s", "PWR", (val2), "W");

    // Debug trace
    if (DEBUG == 1)
    {
      Serial.printf("%s %d %f %f \n", valString, val0, val1, angle);
    }

      Serial.printf("%s %d %f %f \n", valString, val0, val1, angle);

    // Draw line
    needle(angle);

    // Write Value
    value(valString);

    // If M5GO
    if(strcmp(choiceLed[led], "MEASURES") == 0)
    {
      if(val0 <= 232) {
        uint8_t j = map(val0, 0, 232, 0, 4);
        for(uint8_t i = 0; i <= 4; i++)
        {
          if(i <= j)
          {
            leds[4 - i] = CRGB::Green;
            leds[5 + i] = CRGB::Green;
          }
          else
          {
            leds[4 - i] = CRGB::WhiteSmoke;
            leds[5 + i] = CRGB::WhiteSmoke;
          }
        }

        FastLED.setBrightness(16);
        FastLED.show();
      }
    }
  }
}

// Get Data Mode
void getDataMode()
{
  static char buffer[6];
  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x1A, 0x06, 0xFD};

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 6);

  dataMode = buffer[4];
}

// Get Frequency
void getFrequency()
{
  String frequency;
  String frequencyNew;

  static char buffer[8];
  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x03, 0xFD};

  double freq; // Current frequency in Hz
  const uint32_t decMulti[] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

  uint8_t lenght = 0;

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 8);

  freq = 0;
  for (uint8_t i = 2; i < 7; i++)
  {
    freq += (buffer[9 - i] >> 4) * decMulti[(i - 2) * 2];
    freq += (buffer[9 - i] & 0x0F) * decMulti[(i - 2) * 2 + 1];
  }

  if (transverter > 0)
    freq += double(choiceTransverter[transverter]);

  frequency = String(freq);
  lenght = frequency.length();

  if (frequency != "0")
  {
    int8_t i;

    for (i = lenght - 6; i >= 0; i -= 3)
    {
      frequencyNew = "." + frequency.substring(i, i + 3) + frequencyNew;
    }

    if (i == -3)
    {
      frequencyNew = frequencyNew.substring(1, frequencyNew.length());
    }
    else
    {
      frequencyNew = frequency.substring(0, i + 3) + frequencyNew;
    }
    subValue(frequencyNew);
  }
  else
  {
    subValue("-");
  }
}

// Get Mode
void getMode()
{
  char valString[16];  
  static char buffer[5];
  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x04, 0xFD};

  const char *mode[] = {"LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", "CW-R", "RTTY-R", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "DV"};

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 5);

  display.setFont(0);
  display.setTextPadding(24);
  display.setTextColor(TFT_WHITE);
  display.setTextDatum(CC_DATUM);

  snprintf(valString, 16, "%s%d", "FIL", uint8_t(buffer[4]));
  if (strcmp(valString, filterOld) != 0)
  {
    strncpy(filterOld, valString, 16);
    display.fillRoundRect(40 + offsetX, 198 + offsetY, 40, 15, 2, TFT_MODE_BACK);
    display.drawRoundRect(40 + offsetX, 198 + offsetY, 40, 15, 2, TFT_MODE_BORDER);
    display.drawString(valString, 60 + offsetX, 206 + offsetY);
  }

  snprintf(valString, 16, "%s", mode[(uint8_t)buffer[3]]);

  getDataMode(); // Data ON or OFF ?

  if (dataMode == 1)
  {
    snprintf(valString, 16, "%s%s", valString, "-D");
  }

  if (strcmp(valString, modeOld) != 0)
  {
    strncpy(modeOld, valString, 16);
    display.fillRoundRect(240 + offsetX, 198 + offsetY, 40, 15, 2, TFT_MODE_BACK);
    display.drawRoundRect(240 + offsetX, 198 + offsetY, 40, 15, 2, TFT_MODE_BORDER);
    display.drawString(valString, 260 + offsetX, 206 + offsetY);
  }
}

// Get TX
uint8_t getTX()
{
  uint8_t value;

  static char buffer[5];
  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x1C, 0x00, 0xFD};

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 5);

  if (buffer[4] <= 1)
  {
    value = buffer[4];
  }
  else
  {
    value = 0;
  }

  return value;
}