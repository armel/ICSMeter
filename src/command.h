// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Get Smeter
void getSmeter()
{
  String valString;

  static char buffer[6];
  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x15, 0x02, 0xFD};
  char str[2];

  uint8_t val0 = 0;
  float_t val1 = 0;
  static uint8_t val2 = 0;

  float_t angle = 0;

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 6);

  sprintf(str, "%02x%02x", buffer[4], buffer[5]);
  val0 = atoi(str);

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
      angle = 42.50f;
      valString = "S " + String(int(round(val1)));
    }
    else if (val0 <= 120)
    {
      angle = mapFloat(val0, 14, 120, 42.50f, -6.50f); // SMeter image start at S1 so S0 is out of image on the left...
      valString = "S " + String(int(round(val1)));
    }
    else
    {
      angle = mapFloat(val0, 121, 241, -6.50f, -43.0f);
      if(int(round(val1) < 10))
        valString = "S 9 + 0" + String(int(round(val1))) + " dB";
      else
        valString = "S 9 + " + String(int(round(val1))) + " dB";
    }

    // Debug trace
    if (DEBUG)
    {
      Serial.print("Get S");
      Serial.print(val0);
      Serial.print(" ");
      Serial.print(val1);
      Serial.print(" ");
      Serial.println(angle);
    }
   
    // Draw line
    needle(angle);

    // Write Value
    value(valString);
  }
}

// Get SWR
void getSWR()
{
  String valString;

  static char buffer[6];
  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x15, 0x12, 0xFD};
  char str[2];

  uint8_t val0 = 0;
  float_t val1 = 0;
  static uint8_t val3 = 0;

  float_t angle = 0;

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 6);

  sprintf(str, "%02x%02x", buffer[4], buffer[5]);
  val0 = atoi(str);

  if (val0 != val3 || reset == true)
  {
    val3 = val0;
    reset = false;

    if (val0 <= 48)
    {
      angle = mapFloat(val0, 0, 48, 42.50f, 32.50f);
      val1 = mapFloat(val0, 0, 48, 1.0, 1.5);
    }
    else if (val0 <= 80)
    {
      angle = mapFloat(val0, 49, 80, 32.50f, 24.0f);
      val1 = mapFloat(val0, 49, 80, 1.5, 2.0);
    }
    else if (val0 <= 120)
    {
      angle = mapFloat(val0, 81, 120, 24.0f, 10.0f);
      val1 = mapFloat(val0, 81, 120, 2.0, 3.0);
    }
    else if (val0 <= 155)
    {
      angle = mapFloat(val0, 121, 155, 10.0f, 0.0f);
      val1 = mapFloat(val0, 121, 155, 3.0, 4.0);
    }
    else if (val0 <= 175)
    {
      angle = mapFloat(val0, 156, 175, 0.0f, -7.0f);
      val1 = mapFloat(val0, 156, 175, 4.0, 5.0);
    }
    else if (val0 <= 225)
    {
      angle = mapFloat(val0, 176, 225, -7.0f, -19.0f);
      val1 = mapFloat(val0, 176, 225, 5.0, 10.0);
    }
    else
    {
      angle = mapFloat(val0, 226, 255, -19.0f, -30.50f);
      val1 = mapFloat(val0, 226, 255, 10.0, 50.0);
    }

    valString = "SWR " + String(val1);

    // Debug trace
    if (DEBUG)
    {
      Serial.print("Get SWR");
      Serial.print(val0);
      Serial.print(" ");
      Serial.print(val1);
      Serial.print(" ");
      Serial.println(angle);
    }

    // Draw line
    needle(angle);

    // Write Value
    value(valString);
  }
}

// Get Power
void getPower()
{
  String valString;

  static char buffer[6];
  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x15, 0x11, 0xFD};
  char str[2];

  uint8_t val0 = 0;
  float_t val1 = 0;
  float_t val2 = 0;
  static uint8_t val3 = 0;

  float_t angle = 0;

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 6);

  sprintf(str, "%02x%02x", buffer[4], buffer[5]);
  val0 = atoi(str);

  if (val0 != val3 || reset == true)
  {
    val3 = val0;
    reset = false;

    if (val0 <= 27)
    {
      angle = mapFloat(val0, 0, 27, 42.50f, 30.50f);
      val1 = mapFloat(val0, 0, 27, 0, 0.5);
    }
    else if (val0 <= 49)
    {
      angle = mapFloat(val0, 28, 49, 30.50f, 23.50f);
      val1 = mapFloat(val0, 28, 49, 0.5, 1.0);
    }
    else if (val0 <= 78)
    {
      angle = mapFloat(val0, 50, 78, 23.50f, 14.50f);
      val1 = mapFloat(val0, 50, 78, 1.0, 2.0);
    }
    else if (val0 <= 104)
    {
      angle = mapFloat(val0, 79, 104, 14.50f, 6.30f);
      val1 = mapFloat(val0, 79, 104, 2.0, 3.0);
    }
    else if (val0 <= 143)
    {
      angle = mapFloat(val0, 105, 143, 6.30f, -6.50f);
      val1 = mapFloat(val0, 105, 143, 3.0, 5.0);
    }
    else if (val0 <= 175)
    {
      angle = mapFloat(val0, 144, 175, -6.50f, -17.50f);
      val1 = mapFloat(val0, 144, 175, 5.0, 7.0);
    }
    else
    {
      angle = mapFloat(val0, 176, 226, -17.50f, -30.50f);
      val1 = mapFloat(val0, 176, 226, 7.0, 10.0);
    }

    val2 = round(val1 * 10);
    if(IC_MODEL == 705)
      valString = "PWR " + String((val2 / 10)) + " W";
    else
      valString = "PWR " + String(val2) + " W";

    // Debug trace
    if (DEBUG)
    {
      Serial.print("Get PWR");
      Serial.print(val0);
      Serial.print(" ");
      Serial.print(val1);
      Serial.print(" ");
      Serial.println(angle);
    }

    // Draw line
    needle(angle);

    // Write Value
    value(valString);
  }
}

// Get Data Mode
void getDataMode()
{
  static char buffer[6];
  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x1A, 0x06, 0xFD};

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 6);

  dataMode = buffer[4];
}

// Get Frequency
void getFrequency()
{
  String valString;

  static char buffer[8];
  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x03, 0xFD};

  String val0;
  String val1;
  String val2;
  String val3;

  uint32_t frequency; // Current frequency in Hz
  const uint32_t decMulti[] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

  uint8_t lenght = 0;

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 8);

  frequency = 0;
  for (uint8_t i = 2; i < 7; i++)
  {
    frequency += (buffer[9 - i] >> 4) * decMulti[(i - 2) * 2];
    frequency += (buffer[9 - i] & 0x0F) * decMulti[(i - 2) * 2 + 1];
  }

  frequency += TRANSVERTER_LO;

  valString = String(frequency);
  lenght = valString.length();
  
  if(lenght <= 9) {
    val0 = valString.substring(lenght - 3, lenght);
    val1 = valString.substring(lenght - 6, lenght - 3);
    val2 = valString.substring(0, lenght - 6);
    subValue(val2 + "." + val1 + "." + val0);
  }
  else {
    val0 = valString.substring(lenght - 3, lenght);
    val1 = valString.substring(lenght - 6, lenght - 3);
    val2 = valString.substring(lenght - 9, lenght - 6);
    val3 = valString.substring(0, lenght - 9);
    subValue(val3 + "." + val2 + "." + val1 + "." + val0);    
  }
}

// Get Mode
void getMode()
{
  String valString;

  static char buffer[5];
  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x04, 0xFD};

  const char *mode[] = {"LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", "CW-R", "RTTY-R", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "DV"};

  size_t n = sizeof(request) / sizeof(request[0]);

  sendCommand(request, n, buffer, 5);

  M5.Lcd.setFont(0);
  M5.Lcd.setTextPadding(24);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextDatum(CC_DATUM);

  valString = "FIL" + String(uint8_t(buffer[4]));
  if (valString != filterOld)
  {
    filterOld = valString;
    M5.Lcd.fillRoundRect(46, 198, 40, 15, 2, TFT_MODE_BACK);
    M5.Lcd.drawRoundRect(46, 198, 40, 15, 2, TFT_MODE_BORDER);
    M5.Lcd.drawString(valString, 66, 206);
  }

  valString = String(mode[(uint8_t)buffer[3]]);

  getDataMode(); // Data ON or OFF ?

  if (dataMode == 1)
  {
    valString += "-D";
  }
  if (valString != modeOld)
  {
    modeOld = valString;
    M5.Lcd.fillRoundRect(234, 198, 40, 15, 2, TFT_MODE_BACK);
    M5.Lcd.drawRoundRect(234, 198, 40, 15, 2, TFT_MODE_BORDER);
    M5.Lcd.drawString(valString, 254, 206);
  }
}

// Get TX
uint8_t getTX()
{
  uint8_t value;

  static char buffer[5];
  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x1C, 0x00, 0xFD};

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