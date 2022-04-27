// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Bluetooth callback
void callbackBT(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT)
  {
    screensaver = millis();
    M5.Lcd.wakeup();
    Serial.println("BT Client Connected");
    frequencyOld = "";
    btConnected = true;
  }
  if (event == ESP_SPP_CLOSE_EVT)
  {
    M5.Lcd.sleep();
    Serial.println("BT Client disconnected");
    btConnected = false;
  }
}

// Wifi callback Off
void callbackWifiOff(WiFiEvent_t event, WiFiEventInfo_t info)
{
  //M5.Lcd.sleep();
  Serial.println("Wifi Client disconnected");
  wifiConnected = false;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// Wifi callback On
void callbackWifiOn(WiFiEvent_t event, WiFiEventInfo_t info)
{
  screensaver = millis();
  //M5.Lcd.wakeup();
  Serial.println("Wifi Client Connected");
  frequencyOld = "";
  wifiConnected = true;
}

// Print battery
void viewBattery() {
  uint8_t batteryLevel;
  boolean batteryCharging;

  if(screensaverMode == false && settingsMode == false) {
    // On left, view battery level
    batteryLevel = map(getBatteryLevel(1), 0, 100, 0, 16);
    batteryCharging = isCharging();

    if(batteryLevel != batteryLevelOld || batteryCharging != batteryCharginglOld) {

      M5.Lcd.drawJpg(smeterTop, sizeof(smeterTop), 0, 0, 320, 20);

      // Settings
      M5.Lcd.setFont(0);
      M5.Lcd.setTextDatum(CC_DATUM);

      M5.Lcd.fillRoundRect(4, 4, 56, 13, 2, TFT_MODE_BACK);
      M5.Lcd.drawRoundRect(4, 4, 56, 13, 2, TFT_MODE_BORDER);
      M5.Lcd.setTextColor(TFT_WHITE);

      if (IC_CONNECT == BT)
        M5.Lcd.drawString(String(IC_MODEL) + " BT", 32, 11);
      else
        M5.Lcd.drawString(String(IC_MODEL) + " USB", 32, 11);

      //M5.Lcd.drawFastHLine(0, 20, 320, TFT_BLACK);

      batteryLevelOld = batteryLevel;
      batteryCharginglOld = batteryCharging;
      
      M5.Lcd.drawRect(294, 4, 20, 12, TFT_BLACK);
      M5.Lcd.drawRect(313, 7, 4, 6, TFT_BLACK);
      M5.Lcd.fillRect(296, 6, batteryLevel, 8, TFT_BLACK);
        
      if(batteryCharging) {
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.setFont(0);
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.setTextPadding(0);
        M5.Lcd.drawString("+", 290, 11);
      }
      else {
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.setFont(0);
        M5.Lcd.setTextDatum(CR_DATUM);
        M5.Lcd.setTextPadding(0);
        M5.Lcd.drawString(String(getBatteryLevel(1)) + "%", 290, 11);
      }
    }
  }
}

// View GUI
void viewGUI()
{
  M5.Lcd.drawJpg(smeterTop, sizeof(smeterTop), 0, 0, 320, 20);
  if(IC_MODEL == 705)
    M5.Lcd.drawJpg(smeterMiddle10, sizeof(smeterMiddle10), 0, 20, 320, 140);
  else
    M5.Lcd.drawJpg(smeterMiddle100, sizeof(smeterMiddle100), 0, 20, 320, 140);
  M5.Lcd.drawJpg(smeterBottom, sizeof(smeterBottom), 0, 160, 320, 80);
}

void clearData() 
{
  angleOld = 0;

  batteryLevelOld = 0;
  sOld = 255;
  SWROld = 255;
  powerOld = 255;

  filterOld = "";
  modeOld = "";
  valStringOld = "";
  subValStringOld = "";

  batteryCharginglOld = true;

  measureOld = 5;
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

  xNew = *x * cos(angle) - *y * sin(angle);
  yNew = *x * sin(angle) + *y * cos(angle);

  *x = xNew;
  *y = yNew;
}

// Manage map float
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Print needle
void needle(float_t angle, uint16_t a = 0, uint16_t b = 200, uint16_t c = 0, uint16_t d = 100)
{
  uint16_t x, y;

  if (angle != angleOld)
  {
    angleOld = angle;

    x = a;
    y = b;

    rotate(&x, &y, angle);

    a = 160 + x;
    b = 220 - y;

    x = c;
    y = d;

    rotate(&x, &y, angle);

    c = 160 + x;
    d = 220 - y;

  if(IC_MODEL == 705)
    M5.Lcd.drawJpg(smeterMiddle10, sizeof(smeterMiddle10), 0, 20, 320, 130);
  else
    M5.Lcd.drawJpg(smeterMiddle100, sizeof(smeterMiddle100), 0, 20, 320, 130);

    // M5.Lcd.drawFastHLine(0, 150, 320, TFT_BLACK);

    M5.Lcd.drawLine(a + 2, b, c + 3, d, TFT_NEDDLE_2);
    M5.Lcd.drawLine(a + 2, b, c + 2, d, TFT_NEDDLE_1);
    M5.Lcd.drawLine(a + 1, b, c + 1, d, TFT_RED);
    M5.Lcd.drawLine(a, b, c, d, TFT_RED);
    M5.Lcd.drawLine(a - 1, b, c - 1, d, TFT_RED);
    M5.Lcd.drawLine(a - 2, b, c - 2, d, TFT_NEDDLE_1);
    M5.Lcd.drawLine(a - 2, b, c - 3, d, TFT_NEDDLE_2);
  }
}

// Print value
void value(String valString, uint8_t x = 160, uint8_t y = 180)
{
  if (valString != valStringOld)
  {
    valStringOld = valString;

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(&stencilie16pt7b);
    M5.Lcd.setTextPadding(190);
    M5.Lcd.setTextColor(TFT_BLACK, TFT_BACK);
    valString.replace(".", ",");
    M5.Lcd.drawString(valString, x, y);
  }
}

// Print sub value
void subValue(String valString, uint8_t x = 160, uint8_t y = 206)
{
  if (valString != subValStringOld)
  {
    subValStringOld = valString;

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(&YELLOWCRE8pt7b);
    M5.Lcd.setTextPadding(140);
    M5.Lcd.setTextColor(TFT_BLACK, TFT_BACK);
    // valString.replace(".", ",");
    M5.Lcd.drawString(valString, x, y);
  }
}

// Print Measure
void viewMeasure()
{
  uint16_t i = 65;
  uint8_t j;

  if(measure != measureOld) {
    measureOld = measure;

    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setFont(&YELLOWCRE8pt7b);
    M5.Lcd.setTextPadding(0);

    for (j = 0; j <= 2; j++)
    {
      if (measure == j)
      {
        M5.Lcd.setTextColor(TFT_BLACK);
        reset = true;
      }
      else
      {
        M5.Lcd.setTextColor(TFT_DARKGREY);
      }

      M5.Lcd.drawString(choiceMeasures[j], i, 230);
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
      //Serial.println(type + "_" + entry.name());     
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

    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.drawString("Flash File System", 160, 20);
    M5.Lcd.drawString("needs to be formated.", 160, 50);
    M5.Lcd.drawString("It takes around 4 minutes.", 160, 100);
    M5.Lcd.drawString("Please, wait until ", 160, 150);
    M5.Lcd.drawString("the application starts !", 160, 180);

    Serial.println("SPIFFS Formating...");

    SPIFFS.format(); // Format SPIFFS...

    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextSize(0);

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
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(1);

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);

    for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--)
    {
      getButton();

      if (i % 10 == 0)
      {
        tmpName += ".";
        M5.Lcd.drawString(tmpName, 160, 20);
      }

      if (btnA || btnC)
      {
        SD.end(); // If not Bluetooth doesn't work !!!
        return;
      }
      else if (btnB)
      {
        click = 1;
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

    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.drawString("Bin Loader V0.2", 160, 20);

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
      if(binFilename[cursor].substring(0, 4) == "SP_/") {
        updateFromFS(SPIFFS, binFilename[cursor].substring(3));
      }
      else {
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
      M5.Lcd.setTextPadding(320);

      uint8_t i = 0;
      for (uint8_t j = (start * limit); j < stop; j++)
      {
        tmpName = binFilename[j].substring(4);

        if (cursor == j)
        {
          tmpName = ">> " + tmpName + " <<";

          if(binFilename[cursor].substring(0, 4) == "SP_/") {
            M5.Lcd.setTextSize(1);
            M5.Lcd.drawString("SPI Flash File Storage", 160, 50);
          }
          else {
            M5.Lcd.setTextSize(1);
            M5.Lcd.drawString("SD Card Storage", 160, 50);
          }
        }

        M5.Lcd.setTextSize(2);
        M5.Lcd.drawString(tmpName, 160, 80 + i * 20);
        i++;
      }
    }
    vTaskDelay(100);
  }
  SD.end(); // If not Bluetooth doesn't work !!!
}

// Send CI-V Command by Bluetooth
void sendCommandBt(char *request, size_t n, char *buffer, uint8_t limit)
{
  uint8_t byte1, byte2, byte3;
  uint8_t counter = 0;

  while (counter != limit)
  {
    for (uint8_t i = 0; i < n; i++)
    {
      CAT.write(request[i]);
    }

    vTaskDelay(100);

    while (CAT.available())
    {
      byte1 = CAT.read();
      byte2 = CAT.read();

      if (byte1 == 0xFE && byte2 == 0xFE)
      {
        counter = 0;
        byte3 = CAT.read();
        while (byte3 != 0xFD)
        {
          buffer[counter] = byte3;
          byte3 = CAT.read();
          counter++;
          if (counter > limit)
          {
            if (DEBUG)
            {
              Serial.print(" Overflow");
            }
            break;
          }
        }
      }
    }
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

  command += BAUDE_RATE + String(",") + SERIAL_DEVICE;

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

      if (DEBUG)
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
  if (IC_MODEL == 705 && IC_CONNECT == BT)
    sendCommandBt(request, n, buffer, limit);
  else
    sendCommandWifi(request, n, buffer, limit);
}

bool M5Screen24bmp()
{
  uint16_t image_height = M5.Lcd.height();
  uint16_t image_width = M5.Lcd.width();
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
    M5.Lcd.readRectRGB(0, y - 1, image_width, 1, line_data);
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

              screensaver = millis(); // Screensaver update !!!

              switch (htmlGetRequest)
              {
              case GET_index_page:
              {
                httpClient.println("HTTP/1.1 200 OK");
                httpClient.println("Content-type:text/html");
                httpClient.println();
                if(M5.getBoard() == m5::board_t::board_M5Stack) {
                  httpClient.write_P(index_m5stack_html, sizeof(index_m5stack_html));
                }
                else if(M5.getBoard() == m5::board_t::board_M5StackCore2) {
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

  if (screensaverMode == false && millis() - screensaver > TIMEOUT_SCREENSAVER)
  {
    screensaverMode = true;
    screensaver = 0;
    M5.Lcd.fillScreen(TFT_BLACK);
  }
  else if (screensaverMode == true && screensaver != 0)
  {
    M5.Lcd.fillScreen(TFT_BLACK);
    clearData();
    viewGUI();
    screensaverMode = false;

    vTaskDelay(100);
  }
  else if (screensaverMode == true) {
  
    M5.Lcd.fillRect(x, y, 44, 22, TFT_BLACK);

    if(xDir)
    {
      x += 1;
    }
    else {
      x -= 1;
    }

    if(yDir)
    {
      y += 1;
    }
    else {
      y -= 1;
    }

    if(x < 44) {
      xDir = true;
      x = 44;
    }
    else if(x > 232) {
      xDir = false;
      x = 232;
    }

    if(y < 22) {
      yDir = true;
      y = 22;
    }
    else if(y > 196) {
      yDir = false;
      y = 196;
    }

    M5.Lcd.drawJpg(logo, sizeof(logo), x, y, 44, 22);
    if(!btConnected) vTaskDelay(75);
  }

  // Debug trace
  if (DEBUG)
  {
    Serial.print(screensaverMode);
    Serial.print(" ");
    Serial.println(millis() - screensaver);
  }
}

// Manage connexion error
boolean checkConnection()
{
  HTTPClient http;

  uint16_t httpCode;

  String message = "";
  String command = "";
  String response = "";

  char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x03, 0xFD};

  char s[4];
  for (uint8_t i = 0; i < 6; i++)
  {
    sprintf(s, "%02x,", request[i]);
    command += String(s);
  }

  command += BAUDE_RATE + String(",") + SERIAL_DEVICE;

  if (screenshot == false)
  {
    if (IC_MODEL == 705 && IC_CONNECT == BT && btConnected == false)
      message = "Need Pairing";
    else if (IC_CONNECT == USB && wifiConnected == false)
      message = "Check Wifi";
    else if (IC_CONNECT == USB && (proxyConnected == false || txConnected == false))
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
          if(startup == false)
          {
            clearData();
            screensaver = millis();
            M5.Lcd.wakeup();
            Serial.println("TX connected");
          }

          txConnected = true;
          message = "";
        }
        else
        {
          if(startup == false)
          {
            M5.Lcd.sleep();
            Serial.println("TX disconnected");
          }

          message = "Check TX";
        }
      }
      else
      {
        message = "Check Proxy";
      }
      http.end(); // Free the resources
    }

    if (message != "")
    {
      if(screensaverMode == false && settingsMode == false) {
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.setFont(&stencilie16pt7b);
        M5.Lcd.setTextPadding(194);
        M5.Lcd.setTextColor(TFT_BLACK, TFT_BACK);
        M5.Lcd.drawString(message, 160, 180);
        vTaskDelay(500);
        M5.Lcd.drawString("", 160, 180);
        vTaskDelay(100);
        frequencyOld = "";
        return false;
      }
      else {
        vTaskDelay(1000);
      }
    }
  }

  return true;
}