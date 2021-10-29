#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN  7
#define BUTTON_LEFT  9
#define BUTTON_UP    10
#define BUTTON_SEL   14
uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) | 
                       (1 << BUTTON_LEFT)  | (1 << BUTTON_UP)   |
                       (1 << BUTTON_SEL);

struct SSI {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};
SSI ssInterrupt = {5, 0, false};

void IRAM_ATTR isr(void* arg) {
    SSI* s = static_cast<SSI*>(arg);
    s->numberKeyPresses += 1;
    s->pressed = true;
}
void IRAM_ATTR isr() {
    ssInterrupt.numberKeyPresses += 1;
    ssInterrupt.pressed = true;
}

int last_x = 0, last_y = 0;
int started = 0;
char lastDir = 'u';
int l = 0;
int r = 0;
int u = 0;
int d = 0;

void vTaskSS(void *pvParameters) {
  (void) pvParameters;
  for( ;; ) {
    if (ssInterrupt.pressed) {
      if (started == 0) { 
        started = 1; 
        tft.fillScreen(ST77XX_BLACK);
        tft.drawRect(tft.width()/2 -240/2, tft.height()/2 -240/2 , 240, 240, ST77XX_GREEN);
        tft.setCursor(35, 50);
        tft.setTextSize(3);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextWrap(false);
        tft.print("MP3 Player");
        tft.setCursor(80, 120);
        tft.print("Game");
      }
      
      detachInterrupt(ssInterrupt.PIN);
      // maybe detach the interrupt until done processing
      uint32_t buttons = ss.digitalReadBulk(button_mask);
      if (! (buttons & (1 << BUTTON_RIGHT))) {
        Serial.println("Button A pressed");
      }
      if (! (buttons & (1 << BUTTON_DOWN))) {
        Serial.println("Button B pressed");
      }
      if (! (buttons & (1 << BUTTON_LEFT))) {
        Serial.println("Button Y pressed");
      }
      if (! (buttons & (1 << BUTTON_UP))) {
        Serial.println("Button X pressed");
        digitalWrite(BL_PIN, HIGH);
      }
      if (! (buttons & (1 << BUTTON_SEL))) {
        Serial.println("Button SEL pressed");
        digitalWrite(BL_PIN, LOW);
      }
      ssInterrupt.pressed = false;
      attachInterruptArg(ssInterrupt.PIN, isr, &ssInterrupt, FALLING);    
    }
    int x = ss.analogRead(2);
    int y = ss.analogRead(3);
    if (x > 600 && last_x < 600) {
      tft.drawRect(tft.width()/2 -240/2, tft.height()/2 -240/2 , 240, 240, ST77XX_BLACK);
      tft.drawRect(30, 45, 190, 30, ST77XX_GREEN); 
      tft.setCursor(35, 50);
      tft.setTextColor(ST77XX_GREEN);
      tft.print("MP3 Player");

      tft.drawRect(75, 110, 80, 45, ST77XX_BLACK); 
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(80, 120);
      tft.print("Game");
      vTaskDelay(300);
    } else if (last_x > 600 && x < 600) {
      // not left
    }
    if (x < 400 && last_x > 400) {
      tft.drawRect(tft.width()/2 -240/2, tft.height()/2 -240/2 , 240, 240, ST77XX_BLACK);
      tft.drawRect(75, 110, 80, 45, ST77XX_GREEN); 
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(80, 120);
      tft.print("Game");

      tft.drawRect(30, 45, 190, 30, ST77XX_BLACK); 
      tft.setCursor(35, 50);
      tft.setTextColor(ST77XX_WHITE);
      tft.print("MP3 Player");
      vTaskDelay(300);
    } else if (last_x < 400 && x > 400) {
      // not right
    }
    if (y > 600 && last_y < 600) {
      // down
    } else if (last_y > 600 && y < 600) {
      // not down
    }
    if (y < 400 && last_y > 400) {
      // up
    } else if (last_y < 400 && y > 400) {
      // not up
    }
    if ((abs(x - last_x) > 3) || (abs(y - last_y) > 3)) {
      Serial.print(x); Serial.print(", "); Serial.println(y);
      last_x = x;
      last_y = y;
      detachInterrupt(ssInterrupt.PIN);
      ssInterrupt.pressed = false;
      attachInterruptArg(ssInterrupt.PIN, isr, &ssInterrupt, FALLING);
    }
  }
}

void setupInput(void) {
  if (!ss.begin(0x49)) {
      Serial.println("ERROR! seesaw not found");
      while(1);
    } else {
      Serial.println("seesaw started");
      Serial.print("version: ");
      Serial.println(ss.getVersion(), HEX);
    }
    ss.pinModeBulk(button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(button_mask, 1);
    pinMode(ssInterrupt.PIN, INPUT_PULLUP);
    attachInterruptArg(ssInterrupt.PIN, isr, &ssInterrupt, FALLING);    
}
