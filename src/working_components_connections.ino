#include <TFT_eSPI.h>        // Hardware-specific library for TFT
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library for I2C display
#include <Adafruit_SSD1306.h> // Library for I2C OLED display
#include <Encoder.h>        // Library for rotary encoder

// Define TFT pins
#define TFT_CS   18
#define TFT_RST  20
#define TFT_DC   21
#define TFT_WR   17
#define TFT_RD   16

#define TFT_D0   5
#define TFT_D1   4
#define TFT_D2   3
#define TFT_D3   2
#define TFT_D4   15
#define TFT_D5   14
#define TFT_D6   13
#define TFT_D7   12

// Create TFT object
TFT_eSPI tft = TFT_eSPI();

// I2C display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Define I2C pins
#define I2C_SDA 37
#define I2C_SCL 36

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define Encoder pins
#define ENCODER_DT 38
#define ENCODER_CLK 39
#define ENCODER_SW 40 // Button pin if present

Encoder encoder(ENCODER_DT, ENCODER_CLK);

void setup() {
  Serial.begin(115200);

  // Initialize TFT
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("TFT Display");

  // Initialize I2C display
  Wire.begin(I2C_SDA, I2C_SCL); // Initialize I2C with custom SDA and SCL pins
  if(!display.begin(SSD1306_PAGEADDR, 0x3C)) { // Adjust address if needed
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Infinite loop if the display is not found
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(F("Hello, I2C Display!"));
  display.display();

  // Initialize encoder
  pinMode(ENCODER_SW, INPUT_PULLUP); // Assuming button is active low
  Serial.println("Encoder initialized.");
}

void loop() {
  // Read encoder position
  long position = encoder.read();

  bool buttonPressed = digitalRead(ENCODER_SW) == LOW; // Button pressed if pin is LOW

  // Display on I2C OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("Updated I2C Display!"));
  display.setCursor(0,16);
  display.printf("Encoder: %ld", position);
  display.setCursor(0,32);
  display.printf("Button: %s", buttonPressed ? "Pressed" : "Released");
  display.display();

}
