// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Bluetooth callback
void callbackBT(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT)
  {
    btClient = true;
    Serial.println("BT Client Connected");
  }
  if (event == ESP_SPP_CLOSE_EVT)
  {
    btClient = false;
    Serial.println("BT Client Disconnected");
  }
}

// Wifi callback On
void callbackWifiOn(WiFiEvent_t event, WiFiEventInfo_t info)
{
  wifiConnected = true;
  Serial.println("Wifi Client Connected");
}

// Wifi callback Off
void callbackWifiOff(WiFiEvent_t event, WiFiEventInfo_t info)
{
  wifiConnected = false;
  Serial.println("Wifi Client Disconnected");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// Reset color
void resetColor()
{
  switch (theme)
  {
  case 0:
    TFT_BACK = display.color565(TFT_BACK_CLASSIC.r, TFT_BACK_CLASSIC.g, TFT_BACK_CLASSIC.b);
    TFT_FRONT = display.color565(TFT_FRONT_CLASSIC.r, TFT_FRONT_CLASSIC.g, TFT_FRONT_CLASSIC.b);
    break;

  case 1:
    TFT_BACK = display.color565(TFT_BACK_DARK.r, TFT_BACK_DARK.g, TFT_BACK_DARK.b);
    TFT_FRONT = display.color565(TFT_FRONT_DARK.r, TFT_FRONT_DARK.g, TFT_FRONT_DARK.b);
    break;
  }
}

// Print settings
void viewSettings()
{
  char buf[16];

  // Settings
  display.setFont(0);
  display.setTextDatum(CC_DATUM);

  display.fillRoundRect(4 + offsetX, 4 + offsetY, 56, 13, 2, TFT_MODE_BACK);
  display.drawRoundRect(4 + offsetX, 4 + offsetY, 56, 13, 2, TFT_MODE_BORDER);
  display.setTextColor(TFT_WHITE);

  snprintf(buf, 16, "%lu %s", icModel, "BT");

  if (icConnect == BT)
    snprintf(buf, 16, "%lu %s", icModel, "BT");
  else
    snprintf(buf, 16, "%lu %s", icModel, "USB");

  display.drawString(buf, 32 + offsetX, 11 + offsetY);

  if (transverter > 0)
  {
    snprintf(buf, 16, "%s %d", "LO", transverter);

    display.fillRoundRect(62 + offsetX, 4 + offsetY, 26, 13, 2, TFT_MODE_BACK);
    display.drawRoundRect(62 + offsetX, 4 + offsetY, 26, 13, 2, TFT_MODE_BORDER);
    display.setTextColor(TFT_WHITE);
    display.drawString(buf, 76 + offsetX, 11 + offsetY);
  }
}

// Print battery
void viewBattery()
{
  char buf[8];

  uint8_t batteryLevel;
  boolean batteryCharging;

  if (screensaverMode == false && settingsMode == false)
  {
    // On left, view battery level
    batteryLevel = map(getBatteryLevel(1), 0, 100, 0, 16);
    batteryCharging = isCharging();

    if (batteryLevel != batteryLevelOld || batteryCharging != batteryCharginglOld)
    {
      if(theme == 0)
      {
        display.drawJpg(smeterTopClassic, sizeof(smeterTopClassic), 0, 0, 320, 20);
      }
      else
      {
        display.drawJpg(smeterTopDark, sizeof(smeterTopDark), 0, 0, 320, 20);
      }
      
      viewSettings();

      // display.drawFastHLine(0, 20, 320, TFT_BLACK);

      batteryLevelOld = batteryLevel;
      batteryCharginglOld = batteryCharging;

      display.drawRect(294, 4, 20, 12, TFT_FRONT);
      display.drawRect(313, 7, 4, 6, TFT_FRONT);
      display.fillRect(296, 6, batteryLevel, 8, TFT_FRONT);

      if (batteryCharging)
      {
        display.setTextColor(TFT_FRONT);
        display.setFont(0);
        display.setTextDatum(CC_DATUM);
        display.setTextPadding(0);
        snprintf(buf, 8, "%s", "+");
      }
      else
      {
        display.setTextColor(TFT_FRONT);
        display.setFont(0);
        display.setTextDatum(CR_DATUM);
        display.setTextPadding(0);
        snprintf(buf, 8, "%d%s", getBatteryLevel(1), "%");
      }
      display.drawString(buf, 290, 11);
    }
  }
}

// View GUI
void viewGUI()
{
  resetColor();

  display.fillScreen(TFT_BLACK);

  if(theme == 0) 
  {
    display.drawJpg(smeterTopClassic, sizeof(smeterTopClassic), 0 + offsetX, 0 + offsetY, 320, 20);

    if (icModel == 705)
      display.drawJpg(smeterMiddleClassic10, sizeof(smeterMiddleClassic10), 0 + offsetX, 20 + offsetY, 320, 140);
    else
      display.drawJpg(smeterMiddleClassic100, sizeof(smeterMiddleClassic100), 0 + offsetX, 20 + offsetY, 320, 140);
    display.drawJpg(smeterBottomClassic, sizeof(smeterBottomClassic), 0 + offsetX, 160 + offsetY, 320, 80);
  }
  else
  {
    display.drawJpg(smeterTopDark, sizeof(smeterTopDark), 0 + offsetX, 0 + offsetY, 320, 20);

    if (icModel == 705)
      display.drawJpg(smeterMiddleDark10, sizeof(smeterMiddleDark10), 0 + offsetX, 20 + offsetY, 320, 140);
    else
      display.drawJpg(smeterMiddleDark100, sizeof(smeterMiddleDark100), 0 + offsetX, 20 + offsetY, 320, 140);
    display.drawJpg(smeterBottomDark, sizeof(smeterBottomDark), 0 + offsetX, 160 + offsetY, 320, 80);
  }

  viewSettings();
}

void clearData()
{
  angleOld = 0;

  batteryLevelOld = 0;
  sOld = 255;
  SWROld = 255;
  powerOld = 255;

  memset(filterOld, 0, sizeof(filterOld));
  memset(modeOld, 0, sizeof(modeOld));
  memset(valStringOld, 0, sizeof(valStringOld));

  subValStringOld = "";

  batteryCharginglOld = true;

  measureOld = 5;

  for(uint8_t i = 0; i <= 4; i++)
  {
    leds[4 - i] = CRGB::Black;
    leds[5 + i] = CRGB::Black;
  }

  FastLED.setBrightness(16);
  FastLED.show();
}

// Manage rotation
void rotate(uint16_t *x, uint16_t *y, float angle)
{
  float xNew;
  float yNew;

  // Check that these are not null pointer
  assert(x);
  assert(y);

  angle = angle * PI / 180;

  xNew = *x * cos(angle) + *y * sin(angle);
  yNew = -*x * sin(angle) + *y * cos(angle);

  *x = xNew;
  *y = yNew;
}

// Manage map float
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void needleCalc(float_t angle, uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
  uint16_t x, y;
  uint16_t aa, bb, cc, dd;

  x = a;
  y = b;

  rotate(&x, &y, angle);

  aa = 160 + x;
  bb = 220 - y;

  x = c;
  y = d;

  rotate(&x, &y, angle);

  cc = 160 + x;
  dd = 220 - y;

  if(icConnect == USB || ESP.getPsramSize() > 0) // Sprite mode
  {
    if(theme == 0) 
    {
      if (icModel == 705)
        needleSprite.drawJpg(smeterMiddleClassic10, sizeof(smeterMiddleClassic10), 0, 0, 320, 130);
      else
        needleSprite.drawJpg(smeterMiddleClassic100, sizeof(smeterMiddleClassic100), 0, 0, 320, 130);
    }
    else
    {
      if (icModel == 705)
        needleSprite.drawJpg(smeterMiddleDark10, sizeof(smeterMiddleDark10), 0, 0, 320, 130);
      else
        needleSprite.drawJpg(smeterMiddleDark100, sizeof(smeterMiddleDark100), 0, 0, 320, 130);
    }

    needleSprite.drawLine(aa + 2, bb, cc + 3, dd, TFT_NEDDLE_2);
    needleSprite.drawLine(aa + 2, bb, cc + 2, dd, TFT_NEDDLE_1);
    needleSprite.drawLine(aa + 1, bb, cc + 1, dd, TFT_RED);
    needleSprite.drawLine(aa, bb, cc, dd, TFT_RED);
    needleSprite.drawLine(aa - 1, bb, cc - 1, dd, TFT_RED);
    needleSprite.drawLine(aa - 2, bb, cc - 2, dd, TFT_NEDDLE_1);
    needleSprite.drawLine(aa - 2, bb, cc - 3, dd, TFT_NEDDLE_2);

    needleSprite.pushSprite(0 + offsetX, 20 + offsetY, TFT_TRANSPARENT);
  }
  else // Display mode
  {
    if(theme == 0) 
    {
      if (icModel == 705)
        display.drawJpg(smeterMiddleClassic10, sizeof(smeterMiddleClassic10), 0 + offsetX, 20 + offsetY, 320, 140);
      else
        display.drawJpg(smeterMiddleClassic100, sizeof(smeterMiddleClassic100), 0 + offsetX, 20 + offsetY, 320, 140);
    }
    else
    {
      if (icModel == 705)
        display.drawJpg(smeterMiddleDark10, sizeof(smeterMiddleDark10), 0 + offsetX, 20 + offsetY, 320, 140);
      else
        display.drawJpg(smeterMiddleDark100, sizeof(smeterMiddleDark100), 0 + offsetX, 20 + offsetY, 320, 140);
    }

    bb += 20;
    dd += 20;

    display.drawLine(aa + 2 + offsetX, bb + offsetY, cc + 3 + offsetX, dd + offsetY, TFT_NEDDLE_2);
    display.drawLine(aa + 2 + offsetX, bb + offsetY, cc + 2 + offsetX, dd + offsetY, TFT_NEDDLE_1);
    display.drawLine(aa + 1 + offsetX, bb + offsetY, cc + 1 + offsetX, dd + offsetY, TFT_RED);
    display.drawLine(aa + offsetX, bb + offsetY, cc + offsetX, dd + offsetY, TFT_RED);
    display.drawLine(aa - 1 + offsetX, bb + offsetY, cc - 1 + offsetX, dd + offsetY, TFT_RED);
    display.drawLine(aa - 2 + offsetX, bb + offsetY, cc - 2 + offsetX, dd + offsetY, TFT_NEDDLE_1);
    display.drawLine(aa - 2 + offsetX, bb + offsetY, cc - 3 + offsetX, dd + offsetY, TFT_NEDDLE_2);
  }

  if (DEBUG == 1)
  {
    Serial.printf("%d %d %d %d / %d %d %d %d\n", a, b, c, d, aa, bb, cc, dd);
  }
}

// Print needle
void needle(float_t angle, uint16_t a = 0, uint16_t b = 220, uint16_t c = 0, uint16_t d = 115)
{
  uint8_t speed;
  float move, shift;
  float offset, reason;

  if(NEEDLE == ARITHMETIC)  // Arithmetic
  {
    if(icConnect == USB || ESP.getPsramSize() > 0) // Sprite mode
      speed = 8;
    else
      speed = 2;

    if (angle != angleOld)
    {
      if(angle > angleOld)
      {
        shift = (angle - angleOld) / speed;
      }
      else
      {
        shift = -(angleOld - angle) / speed;
      }

      move = angleOld;

      for(uint8_t i = 1; i <= speed; i+= 1)
      {
        move += shift;
        needleCalc(move, a, b, c, d);

        if (DEBUG == 1)
        {
          Serial.printf("--> %f %f %f %f\n", move, angle, angleOld, shift);
        }
      }
    }
  }
  else if(NEEDLE == GEOMETRIC)  // Geometric
  {
    if(icConnect == USB || ESP.getPsramSize() > 0) // Sprite mode
      speed = 8;
    else
      speed = 2;

    offset = 90;
    reason = (angle + offset)/(angleOld + offset);
    reason = pow(reason, (float) 1 / speed);
    move = (angleOld + offset);

    for(uint8_t i = 1; i <= speed; i+= 1)
    {
      needleCalc((move * pow(reason, i)) - offset, a, b, c, d);

      if (DEBUG == 1)
      {
        Serial.printf("--> %f %f %f %f\n", (move * pow(reason, i) - offset), angle, angleOld, reason);
      }
    }
  }
  angleOld = angle;
}

// Print value
void value(char* valString, uint8_t x = 160, uint8_t y = 180)
{
  if (strcmp(valString, valStringOld) != 0)
  {
    strncpy(valStringOld, valString, 32);

    display.setTextDatum(CC_DATUM);
    display.setFont(&stencilie16pt7b);
    //display.setFont(&YELLOWCRE8pt7b);
    for(uint8_t i = 0; i < strlen(valString); i++)
    {
      if(valString[i] == '.') valString[i] = ';';
    }
    display.setTextPadding(192);
    display.setTextColor(TFT_FRONT, TFT_BACK);
    display.drawString(valString, x + offsetX, y + offsetY);
  }
}

// Print sub value
void subValue(String valString, uint8_t x = 160, uint8_t y = 206)
{
  if (valString != subValStringOld)
  {
    subValStringOld = valString;

    display.setTextDatum(CC_DATUM);
    display.setFont(&YELLOWCRE8pt7b);
    display.setTextPadding(160);
    display.setTextColor(TFT_FRONT, TFT_BACK);
    // valString.replace(".", ",");
    display.drawString(valString, x + offsetX, y + offsetY);
  }
}

// Print Measure
void viewMeasure()
{
  uint16_t i = 65;
  uint8_t j;

  if (measure != measureOld)
  {
    measureOld = measure;

    display.setTextDatum(CC_DATUM);
    display.setFont(&YELLOWCRE8pt7b);
    display.setTextPadding(0);

    for (j = 0; j <= 2; j++)
    {
      if (measure == j)
      {
        display.setTextColor(TFT_FRONT);
        reset = true;
      }
      else
      {
        display.setTextColor(TFT_DARKGREY);
      }

      display.drawString(choiceMeasures[j], i + offsetX, 230 + offsetY);
      i += 95;
    }
  }
}

// List files on SPIFFS or SD
void getBinaryList(File dir, String type)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }

    if (strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".bin") != NULL)
    {
      // Serial.println(type + "_" + entry.name());
      binFilename[binIndex] = type + "_" + entry.name();
      binIndex++;
    }

    if (entry.isDirectory() && strstr(entry.name(), "/.") == NULL)
    {
      getBinaryList(entry, type);
    }

    entry.close();
  }
}

// Bin Loader
void binLoader()
{
  boolean click = 0;
  int8_t cursor = 0;
  int8_t start = 0;
  int8_t stop = 0;
  int8_t limit = 8;
  int8_t change = 255;
  String tmpName;

  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");

    display.setTextFont(1);
    display.setTextSize(2);

    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextDatum(CC_DATUM);
    display.drawString("Flash File System", 160, 20);
    display.drawString("needs to be formated.", 160, 50);
    display.drawString("It takes around 4 minutes.", 160, 100);
    display.drawString("Please, wait until ", 160, 150);
    display.drawString("the application starts !", 160, 180);

    Serial.println("SPIFFS Formating...");

    SPIFFS.format(); // Format SPIFFS...

    display.setTextFont(0);
    display.setTextSize(0);

    return;
  }

  root = SPIFFS.open("/");
  getBinaryList(root, "SP");

  if (SD.begin(GPIO_NUM_4, SPI, 25000000))
  {
    root = SD.open("/");
    getBinaryList(root, "SD");
  }

  if (binIndex != 0)
  {
    // QRCode
    display.qrcode("https://github.com/armel/ICSMeter", 90, 80, 140, 6);

    display.setTextFont(1);
    display.setTextSize(1);

    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextDatum(CC_DATUM);

    for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--)
    {
      getButton();

      if (i % 10 == 0)
      {
        tmpName += ".";
        display.drawString(tmpName, 160, 20);
      }

      if (btnA || btnC)
      {
        SD.end(); // If not Bluetooth doesn't work !!!
        return;
      }
      else if (btnB)
      {
        click = 1;
        display.fillRect(0, 0, 320, 240, TFT_BLACK);
        break;
      }

      vTaskDelay(100);
    }
  }

  while (click == 1)
  {
    while (btnB != 0)
    {
      getButton();
      vTaskDelay(100);
    }

    display.setTextFont(1);
    display.setTextSize(2);

    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextDatum(CC_DATUM);
    display.drawString("Bin Loader V0.2", 160, 20);

    getButton();

    if (btnA)
    {
      cursor--;
    }
    else if (btnC)
    {
      cursor++;
    }
    else if (btnB)
    {
      if (binFilename[cursor].substring(0, 4) == "SP_/")
      {
        updateFromFS(SPIFFS, binFilename[cursor].substring(3));
      }
      else
      {
        updateFromFS(SD, binFilename[cursor].substring(3));
      }
      ESP.restart();
    }

    cursor = (cursor < 0) ? binIndex - 1 : cursor;
    cursor = (cursor > binIndex - 1) ? 0 : cursor;

    start = cursor / limit;

    stop = (start * limit) + limit;

    /*
    Serial.print(cursor);
    Serial.print("-");
    Serial.print(start);
    Serial.print("-");
    Serial.print(stop);
    Serial.println("----------");
    */

    if (change != cursor)
    {
      change = cursor;
      display.setTextPadding(320);

      uint8_t i = 0;
      for (uint8_t j = (start * limit); j < stop; j++)
      {
        tmpName = binFilename[j].substring(4);

        if (cursor == j)
        {
          tmpName = ">> " + tmpName + " <<";

          if (binFilename[cursor].substring(0, 4) == "SP_/")
          {
            display.setTextSize(1);
            display.drawString("SPI Flash File Storage", 160, 50);
          }
          else
          {
            display.setTextSize(1);
            display.drawString("SD Card Storage", 160, 50);
          }
        }

        display.setTextSize(2);
        display.drawString(tmpName, 160, 80 + i * 20);
        i++;
      }
    }
    vTaskDelay(100);
  }
  SD.end(); // If not Bluetooth doesn't work !!!
}

bool M5Screen24bmp()
{
  uint16_t image_height = display.height();
  uint16_t image_width = display.width();
  const uint16_t pad = (4 - (3 * image_width) % 4) % 4;
  uint32_t filesize = 54 + (3 * image_width + pad) * image_height;
  unsigned char swap;
  unsigned char line_data[image_width * 3 + pad];
  unsigned char header[54] = {
      'B', 'M',    // BMP signature (Windows 3.1x, 95, NT, …)
      0, 0, 0, 0,  // Image file size in bytes
      0, 0, 0, 0,  // Reserved
      54, 0, 0, 0, // Start of pixel array
      40, 0, 0, 0, // Info header size
      0, 0, 0, 0,  // Image width
      0, 0, 0, 0,  // Image height
      1, 0,        // Number of color planes
      24, 0,       // Bits per pixel
      0, 0, 0, 0,  // Compression
      0, 0, 0, 0,  // Image size (can be 0 for uncompressed images)
      0, 0, 0, 0,  // Horizontal resolution (dpm)
      0, 0, 0, 0,  // Vertical resolution (dpm)
      0, 0, 0, 0,  // Colors in color table (0 = none)
      0, 0, 0, 0}; // Important color count (0 = all colors are important)

  // Fill filesize, width and heigth in the header array
  for (uint8_t i = 0; i < 4; i++)
  {
    header[2 + i] = (char)((filesize >> (8 * i)) & 255);
    header[18 + i] = (char)((image_width >> (8 * i)) & 255);
    header[22 + i] = (char)((image_height >> (8 * i)) & 255);
  }

  // Write the header to the file
  httpClient.write(header, 54);

  // To keep the required memory low, the image is captured line by line
  // initialize padded pixel with 0
  for (uint16_t i = (image_width - 1) * 3; i < (image_width * 3 + pad); i++)
  {
    line_data[i] = 0;
  }
  // The coordinate origin of a BMP image is at the bottom left.
  // Therefore, the image must be read from bottom to top.
  for (uint16_t y = image_height; y > 0; y--)
  {
    // Get one line of the screen content
    display.readRectRGB(0, y - 1, image_width, 1, line_data);
    // BMP color order is: Blue, Green, Red
    // Return values from readRectRGB is: Red, Green, Blue
    // Therefore: R und B need to be swapped
    for (uint16_t x = 0; x < image_width; x++)
    {
      swap = line_data[x * 3];
      line_data[x * 3] = line_data[x * 3 + 2];
      line_data[x * 3 + 2] = swap;
    }
    // Write the line to the file
    httpClient.write(line_data, (image_width * 3) + pad);
  }
  return true;
}

// Get screenshot
void getScreenshot()
{
  unsigned long timeout_millis = millis() + 3000;
  String currentLine = "";

  if (WiFi.status() == WL_CONNECTED)
  {
    httpClient = httpServer.available();
    // httpClient.setNoDelay(1);
    if (httpClient)
    {
      // Force a disconnect after 3 seconds
      // Serial.println("New Client.");
      // Loop while the client's connected
      while (httpClient.connected())
      {
        // If the client is still connected after 3 seconds,
        // Something is wrong. So kill the connection
        if (millis() > timeout_millis)
        {
          // Serial.println("Force Client stop!");
          httpClient.stop();
        }
        // If there's bytes to read from the client,
        if (httpClient.available())
        {
          screenshot = true;
          char c = httpClient.read();
          Serial.write(c);
          // If the byte is a newline character
          if (c == '\n')
          {
            // Two newline characters in a row (empty line) are indicating
            // The end of the client HTTP request, so send a response:
            if (currentLine.length() == 0)
            {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line,
              // followed by the content:

              screensaverTimer = millis(); // Screensaver update !!!

              switch (htmlGetRequest)
              {
              case GET_index_page:
              {
                httpClient.println("HTTP/1.1 200 OK");
                httpClient.println("Content-type:text/html");
                httpClient.println();
                if (M5.getBoard() == m5::board_t::board_M5Stack)
                {
                  httpClient.write_P(index_m5stack_html, sizeof(index_m5stack_html));
                }
                else if (M5.getBoard() == m5::board_t::board_M5StackCore2)
                {
                  httpClient.write_P(index_core2_html, sizeof(index_core2_html));
                }
                break;
              }
              case GET_screenshot:
              {
                httpClient.println("HTTP/1.1 200 OK");
                httpClient.println("Content-type:image/bmp");
                httpClient.println();
                M5Screen24bmp();
                vTaskDelay(1000);
                screenshot = false;
                break;
              }
              default:
                httpClient.println("HTTP/1.1 404 Not Found");
                httpClient.println("Content-type:text/html");
                httpClient.println();
                httpClient.print("404 Page not found.<br>");
                break;
              }
              // The HTTP response ends with another blank line:
              // httpClient.println();
              // Break out of the while loop:
              break;
            }
            else
            { // if a newline is found
              // Analyze the currentLine:
              // detect the specific GET requests:
              if (currentLine.startsWith("GET /"))
              {
                htmlGetRequest = GET_unknown;
                // If no specific target is requested
                if (currentLine.startsWith("GET / "))
                {
                  htmlGetRequest = GET_index_page;
                }
                // If the screenshot image is requested
                if (currentLine.startsWith("GET /screenshot.bmp"))
                {
                  htmlGetRequest = GET_screenshot;
                }
                // If the button left was pressed on the HTML page
                if (currentLine.startsWith("GET /buttonLeft"))
                {
                  buttonLeftPressed = true;
                  htmlGetRequest = GET_index_page;
                }
                // If the button center was pressed on the HTML page
                if (currentLine.startsWith("GET /buttonCenter"))
                {
                  buttonCenterPressed = true;
                  htmlGetRequest = GET_index_page;
                }
                // If the button right was pressed on the HTML page
                if (currentLine.startsWith("GET /buttonRight"))
                {
                  buttonRightPressed = true;
                  htmlGetRequest = GET_index_page;
                }
              }
              currentLine = "";
            }
          }
          else if (c != '\r')
          {
            // Add anything else than a carriage return
            // character to the currentLine
            currentLine += c;
          }
        }
      }
      // Close the connection
      httpClient.stop();
      // Serial.println("Client Disconnected.");
      vTaskDelay(100);
    }
  }
}

// Manage screensaver
void wakeAndSleep()
{
  static uint16_t x = rand() % 232;
  static uint16_t y = rand() % 196;
  static boolean xDir = rand() & 1;
  static boolean yDir = rand() & 1;

  if (screensaverMode == false && millis() - screensaverTimer > screensaver * 60 * 1000)
  {
    settingsMode = false;
    screensaverMode = true;
    screensaverTimer = 0;
    display.fillScreen(TFT_BLACK);
  }
  else if (screensaverMode == true && screensaverTimer != 0)
  {
    display.fillScreen(TFT_BLACK);
    clearData();
    viewGUI();
    screensaverMode = false;
    settingsMode = false;

    vTaskDelay(100);
  }
  else if (screensaverMode == true)
  {
    display.fillRect(x + offsetX, y + offsetY, 44, 22, TFT_BLACK);

    if (xDir)
    {
      x += 1;
    }
    else
    {
      x -= 1;
    }

    if (yDir)
    {
      y += 1;
    }
    else
    {
      y -= 1;
    }

    if (x < 44)
    {
      xDir = true;
      x = 44;
    }
    else if (x > 232)
    {
      xDir = false;
      x = 232;
    }

    if (y < 22)
    {
      yDir = true;
      y = 22;
    }
    else if (y > 196)
    {
      yDir = false;
      y = 196;
    }

    logoSprite.pushSprite(x + offsetX, y + offsetY, TFT_TRANSPARENT);
    vTaskDelay(100);

    if (icModel == 705 && icConnect == BT && btConnected == false)
      vTaskDelay(75);
    else if (icConnect == USB && wifiConnected == false)
      vTaskDelay(75);
  }

  // Debug trace
  if (DEBUG == 1)
  {
    Serial.printf("%d %ld\n", screensaverMode, millis() - screensaverTimer);
  }
}

// Manage connexion error
boolean checkConnection()
{
  HTTPClient http;

  uint16_t httpCode;

  char message[24] = "";
  String command = "";
  String response = "";

  char request[] = {0xFE, 0xFE, icCIVAddress, 0xE0, 0x03, 0xFD};

  char s[4];

  settingLock = false;

  for (uint8_t i = 0; i < 6; i++)
  {
    sprintf(s, "%02x,", request[i]);
    command += String(s);
  }

  command += BAUD_RATE + String(",") + icSerialDevice;

  if (screenshot == false)
  {
    if(btClient)
    {
      btConnected = true;
    }
    else 
    {
      btConnected = false;
    }

    if (icModel == 705 && icConnect == BT && btConnected == false)
      snprintf(message, 24, "%s", "Need Pairing");
    else if (icConnect == USB && wifiConnected == false)
      snprintf(message, 24, "%s", "Check Wifi");
    else if (icConnect == USB && (proxyConnected == false || txConnected == false))
    {
      http.begin(civClient, PROXY_URL + String(":") + PROXY_PORT + String("/") + String("?civ=") + command); // Specify the URL
      http.addHeader("User-Agent", "M5Stack");                                                               // Specify header
      http.addHeader("Connection", "keep-alive");                                                            // Specify header
      http.setTimeout(100);                                                                                  // Set Time Out
      httpCode = http.GET();                                                                                 // Make the request
      if (httpCode == 200)
      {
        proxyConnected = true;

        response = http.getString(); // Get data
        response.trim();

        if (response != "")
        {
          Serial.println("TX connected");
          txConnected = true;
          snprintf(message, 24, "%s", "");
        }
        else
        {
          Serial.println("TX disconnected");
          txConnected = false;
          snprintf(message, 24, "%s %lu", "Check", icModel);
        }
      }
      else
      {
        snprintf(message, 24, "%s", "Check Proxy");
      }
      http.end();
    }

    // Shutdown screen if no TX connexion
    if (wakeup == true && startup == false)
    {
      if ((icConnect == BT && btConnected == false) || (icConnect == USB && txConnected == false))
      {
        display.sleep();
        wakeup = false;

        for(uint8_t i = 0; i <= 4; i++)
        {
          leds[4 - i] = CRGB::Black;
          leds[5 + i] = CRGB::Black;
        }

        FastLED.setBrightness(16);
        FastLED.show();
      }
    }
    else if (wakeup == false && startup == false)
    {
      if ((icConnect == BT && btConnected == true) || (icConnect == USB && txConnected == true))
      {
        clearData();
        viewGUI();
        display.wakeup();
        wakeup = true;
        screensaverTimer = millis();
      }
    }

    // View message

    if (strcmp(message, "") != 0)
    {
      settingLock = true;

      if (screensaverMode == false && settingsMode == false)
      {
        value(message);
        vTaskDelay(750);
        snprintf(message, 24, "%s", "");
        value(message);
        frequencyOld = "";
        settingLock = false;
        vTaskDelay(250);
        return false;
      }
      else
      {
        settingLock = false;
        vTaskDelay(1000);
        return false;
      }
    }
  }
  return true;
}