#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <SD.h>              // SD card library
#include <SPI.h>             // SPI library
#include <TouchScreen.h>

#define YP A1  // Analog pin
#define XM A2  // Analog pin
#define YM 7   // Digital pin, configured as analog input
#define XP 6   // Digital pin, configured as analog input

// Touchscreen calibration settings
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// Create TouchScreen object
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 100);

// Initialize TFT display
MCUFRIEND_kbv tft;

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// SD card pin configuration
#define SD_CS_PIN 10  // SD card chip select (SS) pin

// Function to read and display a BMP file from SD card
void displayBMP(File bmpFile, int16_t startX, int16_t startY) {
  // Check BMP file format
  if (bmpFile.read() != 'B' || bmpFile.read() != 'M') {
    Serial.println("Not a BMP file");
    return;
  }

  // Skip file size and reserved fields
  bmpFile.seek(18);

  // Read width and height
  int32_t width = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24); // stored in lil endian and we aim to store in 32 bits so nned to do left shift
  int32_t height = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);

  Serial.print("Width: ");
  Serial.println(width);
  Serial.print("Height: ");
  Serial.println(height);

  // Skip planes and bit depth
  bmpFile.seek(28);
  uint16_t depth = bmpFile.read() | (bmpFile.read() << 8);

  if (depth != 24) {
    Serial.println("Only 24-bit BMP files are supported");
    return;
  }

  // Skip compression and image size fields
  bmpFile.seek(54);

  Serial.println("Starting to draw BMP file...");
  tft.fillScreen(0x0000);

  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      byte b = bmpFile.read(); // Blue
      byte g = bmpFile.read(); // Green
      byte r = bmpFile.read(); // Red
      uint16_t color = tft.color565(r, g, b);
      tft.drawPixel(startX + x, startY + height - 1 - y, color);
    }
    // Skip padding bytes
    bmpFile.seek(bmpFile.position() + ((4 - (width * 3) % 4) % 4)); // Skip row padding only needed when size of the screen is not divisible by 12
  }
  
  Serial.println("BMP file drawing completed.");
}

void setup() {
  Serial.begin(9600);
  
  // Initialize TFT display
  uint16_t identifier = tft.readID();
  Serial.print("LCD driver chip identifier: ");
  Serial.println(identifier, HEX);

  if (identifier == 0x6814) {
    Serial.println("Found ILI6814 LCD driver");
  } else {
    Serial.print("Unknown driver: ");
    Serial.println(identifier, HEX);
  }

  tft.begin(identifier);
  tft.setRotation(-1);

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) { // SD card is connected to pin 10
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");
  
  // 1 whole file 480*320px
    File bmpFile = SD.open("A2.BMP");
    if (!bmpFile) {
      Serial.println("Failed to open BMP file: A2.BMP");
      return;
    }
    displayBMP(bmpFile, 0, 0);
    bmpFile.close();

    Serial.println("All BMP files displayed.");
  }

void loop() {
  TSPoint p = ts.getPoint();

  // Only read if touch is detected
  if (p.z > ts.pressureThreshhold) {
    // Map touch coordinates to screen coordinates
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());

    Serial.print("X = "); Serial.print(p.y);
    Serial.print("\tY = "); Serial.print(p.x);
    Serial.print("\tPressure = "); Serial.println(p.z);
  }

  delay(100);
}