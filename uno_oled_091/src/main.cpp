#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels (0.91" OLED is 128x32)

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

#define BUTTON_PIN     2  // Button connected between digital pin 2 and GND

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 8x12 pixels bitmap for Reva (the child character)
const unsigned char PROGMEM reva_run1[] = {
  0x18, //   00011000  (Head)
  0x3c, //   00111100
  0x3c, //   00111100
  0x18, //   00011000
  0x7e, //   01111110  (Torso/Arms)
  0xdb, //   11011011  (Arms down)
  0x3c, //   00111100
  0x3c, //   00111100
  0x24, //   00100100  (Legs run frame 1)
  0x24, //   00100100
  0x42, //   01000010
  0x42  //   01000010
};

const unsigned char PROGMEM reva_run2[] = {
  0x18, //   00011000  (Head)
  0x3c, //   00111100
  0x3c, //   00111100
  0x18, //   00011000
  0x7e, //   01111110  (Torso/Arms)
  0xdb, //   11011011
  0x3c, //   00111100
  0x3c, //   00111100
  0x18, //   00011000  (Legs run frame 2)
  0x2c, //   00101100
  0x4c, //   01001100
  0x08  //   00001000
};

const unsigned char PROGMEM reva_jump[] = {
  0x18, //   00011000  (Head)
  0x3c, //   00111100
  0x3c, //   00111100
  0x18, //   00011000
  0x7e, //   01111110  (Torso/Arms up/jump)
  0xdb, //   11011011
  0x3c, //   00111100
  0x3c, //   00111100
  0x42, //   01000010  (Legs bent for jump)
  0x24, //   00100100
  0x18, //   00011000
  0x00  //   00000000
};

// Game state
enum GameState {
  STATE_INTRO,
  STATE_PLAYING,
  STATE_GAMEOVER
};

GameState currentState = STATE_INTRO;
unsigned long stateTimer = 0;
int score = 0;

// Reva physics
const int revaX = 15;
float revaY = 19; // ground y coordinate is 19 (31 - 12)
float revaVY = 0;

// High-feel jump physics settings
const float gravity = 0.58;         // Standard gravity
const float holdGravity = 0.28;     // Reduced gravity when button is held
const float initialJumpForce = -4.8; // High initial jump impulse
const int MAX_JUMP_HOLD = 14;       // Max frames you can float/hold the jump
int jumpHoldCounter = 0;
bool isJumping = false;

// Stars (background parallax)
struct Star {
  float x;
  int y;
};
Star stars[4] = {
  {30, 4},
  {65, 8},
  {95, 3},
  {120, 6}
};

// Clouds (midground parallax)
struct Cloud {
  float x;
  int y;
};
Cloud clouds[2] = {
  {10, 2},
  {80, 5}
};

// Houses (foreground obstacles)
struct House {
  float x;
  int w;
  int h;
  bool active;
  bool passed;
};
House houses[2] = {
  {130, 16, 12, true, false},
  {210, 20, 15, true, false}
};

// Animation properties
int frameCounter = 0;

// Reset the game to initial state
void resetGame() {
  revaY = 19;
  revaVY = 0;
  isJumping = false;
  jumpHoldCounter = 0;
  score = 0;
  houses[0] = {130, 16, 12, true, false};
  houses[1] = {210, 20, 15, true, false};
  frameCounter = 0;
}

// Bounding Box Collision Detection
bool checkCollision(int rx, int ry, int rw, int rh, int hx, int hy, int hw, int hh) {
  return (rx < hx + hw &&
          rx + rw > hx &&
          ry < hy + hh &&
          ry + rh > hy);
}

void setup() {
  Serial.begin(115200);

  // Set up button pin with internal pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.display();
  
  stateTimer = millis();
}

void updateIntro() {
  display.clearDisplay();

  // Draw elegant border
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.drawRect(2, 2, SCREEN_WIDTH - 4, SCREEN_HEIGHT - 4, SSD1306_WHITE);

  // Text setup
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Center text
  display.setCursor(10, 7);
  display.print(F("REVA'NIN ILK OYUNU"));
  
  display.setCursor(19, 18);
  display.print(F("BASLAMAK ICIN BAS"));

  display.display();

  // Check if button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    resetGame();
    currentState = STATE_PLAYING;
    delay(200); // debounce delay
  }
}

void updatePlaying() {
  frameCounter++;

  // 1. Jump Input Trigger
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!isJumping) {
      revaVY = initialJumpForce;
      isJumping = true;
      jumpHoldCounter = 0;
    }
  }

  // 2. Move Background Stars (Very slow)
  for (int i = 0; i < 4; i++) {
    stars[i].x -= 0.15;
    if (stars[i].x < -2) {
      stars[i].x = SCREEN_WIDTH + 2;
    }
  }

  // 3. Move Clouds (Medium speed)
  for (int i = 0; i < 2; i++) {
    clouds[i].x -= 0.35;
    if (clouds[i].x < -20) {
      clouds[i].x = SCREEN_WIDTH + 10;
    }
  }

  // 4. Move and Spawn Houses (Fast)
  for (int i = 0; i < 2; i++) {
    if (houses[i].active) {
      houses[i].x -= 1.8;
      
      // Check if house has successfully passed the player
      if (!houses[i].passed && (houses[i].x + houses[i].w < revaX)) {
        houses[i].passed = true;
        score++;
      }

      // Recycle/Respawn houses offscreen
      if (houses[i].x + houses[i].w < 0) {
        houses[i].x = SCREEN_WIDTH + random(10, 60);
        houses[i].w = random(12, 22);
        houses[i].h = random(10, 17); // Max height is 17 (31-17 = y=14)
        houses[i].passed = false;
      }
    }
  }

  // 5. Apply Jump/Gravity Physics (Hold to float longer)
  if (isJumping) {
    revaY += revaVY;
    
    // If holding button down while still ascending, apply lower gravity for a floatier jump
    if (digitalRead(BUTTON_PIN) == LOW && revaVY < 0 && jumpHoldCounter < MAX_JUMP_HOLD) {
      revaVY += holdGravity;
      jumpHoldCounter++;
    } else {
      revaVY += gravity;
    }

    if (revaY >= 19) {
      revaY = 19;
      revaVY = 0;
      isJumping = false;
    }
  }

  // 6. Check Collisions
  for (int i = 0; i < 2; i++) {
    if (houses[i].active) {
      int hx = (int)houses[i].x;
      int hw = houses[i].w;
      int hh = houses[i].h;
      int hy = 31 - hh;

      if (checkCollision(revaX, (int)revaY, 8, 12, hx, hy, hw, hh)) {
        currentState = STATE_GAMEOVER;
        stateTimer = millis();
        delay(200); // debounce delay
        return;
      }
    }
  }

  // Draw Screen
  display.clearDisplay();

  // Draw ground line
  display.drawFastHLine(0, 31, SCREEN_WIDTH, SSD1306_WHITE);

  // Draw Stars
  for (int i = 0; i < 4; i++) {
    display.drawPixel((int)stars[i].x, stars[i].y, SSD1306_WHITE);
  }

  // Draw Clouds
  for (int i = 0; i < 2; i++) {
    int cx = (int)clouds[i].x;
    int cy = clouds[i].y;
    display.fillRoundRect(cx, cy, 14, 6, 2, SSD1306_WHITE);
    display.fillRoundRect(cx + 4, cy - 2, 8, 4, 2, SSD1306_WHITE);
  }

  // Draw Houses
  for (int i = 0; i < 2; i++) {
    if (houses[i].active) {
      int hx = (int)houses[i].x;
      int hw = houses[i].w;
      int hh = houses[i].h;
      int hy = 31 - hh;

      // Draw the house body
      display.drawRect(hx, hy, hw, hh, SSD1306_WHITE);
      
      // Draw a door
      display.fillRect(hx + (hw / 2) - 2, hy + hh - 6, 4, 6, SSD1306_WHITE);

      // Draw a window
      if (hw > 14) {
        display.drawRect(hx + 3, hy + 3, 3, 3, SSD1306_WHITE);
        display.drawRect(hx + hw - 6, hy + 3, 3, 3, SSD1306_WHITE);
      }
      
      // Draw roof lines (triangular roof outline)
      display.drawLine(hx, hy, hx + (hw / 2), hy - 4, SSD1306_WHITE);
      display.drawLine(hx + (hw / 2), hy - 4, hx + hw - 1, hy, SSD1306_WHITE);
    }
  }

  // Draw Reva Sprite
  const unsigned char* currentSprite;
  if (isJumping) {
    currentSprite = reva_jump;
  } else {
    // Toggle frame every 6 ticks
    if ((frameCounter / 6) % 2 == 0) {
      currentSprite = reva_run1;
    } else {
      currentSprite = reva_run2;
    }
  }
  display.drawBitmap(revaX, (int)revaY, currentSprite, 8, 12, SSD1306_WHITE);

  // Draw Score in Top-Right
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(105, 2);
  display.print(score);

  display.display();
}

void updateGameOver() {
  display.clearDisplay();

  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(35, 4);
  display.print(F("OYUN BITTI"));
  
  display.setCursor(32, 14);
  display.print(F("SKOR: "));
  display.print(score);

  display.setCursor(17, 23);
  display.print(F("TEKRAR ICIN BAS"));

  display.display();

  // Restart check
  if (digitalRead(BUTTON_PIN) == LOW && (millis() - stateTimer > 500)) {
    resetGame();
    currentState = STATE_PLAYING;
    delay(200); // debounce delay
  }
}

void loop() {
  switch (currentState) {
    case STATE_INTRO:
      updateIntro();
      break;
    case STATE_PLAYING:
      updatePlaying();
      break;
    case STATE_GAMEOVER:
      updateGameOver();
      break;
  }
  delay(30); // ~33 FPS
}
