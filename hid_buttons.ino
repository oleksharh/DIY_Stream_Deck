#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <Keyboard.h>

// Define touchscreen pins
#define YP A1  // Analog pin
#define XM A2  // Analog pin
#define YM 7   // Digital pin, configured as analog input
#define XP 6   // Digital pin, configured as analog input

// Define touchscreen dimensions
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// Define grid dimensions
#define GRID_ROWS 3
#define GRID_COLS 2

// Define touchscreen object
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 100);
MCUFRIEND_kbv tft;

void setup() {
  Serial.begin(9600);
  Keyboard.begin(); // Start keyboard functionality
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(-1); // Adjust rotation if necessary
  
  // Draw 3x2 grid
  drawGrid();
}

void drawGrid() {
  int gridWidth = 160;
  int gridHeight = 160;

  tft.fillScreen(WHITE);

  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 3; col++) {
      int x1 = col * gridWidth;
      int y1 = row * gridHeight;

      // Draw cell borders
      tft.drawRect(x1, y1, gridWidth, gridHeight, BLACK);
    }
  }
}

void loop() {
  // Get touch coordinates
  TSPoint p = ts.getPoint();

  if (p.z > ts.pressureThreshhold) {
    // Map the touch coordinates to your screen
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());

    int gridWidth = 160;
    int gridHeight = 160;

    // Determine which grid cell is touched
    int row = p.y / gridHeight;
    int col = 0;

    // Calculate the index of the key to press based on grid position
    char key = 'a' + row;

    Serial.print("X = "); Serial.print(p.y);
    Serial.print("\tY = "); Serial.print(p.x);
    Serial.print("\tPressure = "); Serial.println(p.z);
    Serial.print("Key Pressed: "); Serial.println(key);

    // Send the appropriate keyboard output
    Keyboard.press(key);
    delay(100); // Hold the key for a short duration
    Keyboard.release(key);
    delay(500); // Debounce delay to avoid multiple key presses
  }
  delay(100);
}