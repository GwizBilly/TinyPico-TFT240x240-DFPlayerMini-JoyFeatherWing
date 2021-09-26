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

void vTaskSS(void *pvParameters) {
  char *pcTaskName;
  pcTaskName = (char *) pvParameters;
  for( ;; ) {
    //Serial.print(pcTaskName);
    if (ssInterrupt.pressed) {
      detachInterrupt(ssInterrupt.PIN);
      /* maybe detach the interrupt until done processing */
      uint32_t buttons = ss.digitalReadBulk(button_mask);
      
      if (! (buttons & (1 << BUTTON_RIGHT))) {
        Serial.println("Button A pressed");
				myDFPlayer.next();  //Play next mp3 every 3 second.
      }
      if (! (buttons & (1 << BUTTON_DOWN))) {
        Serial.println("Button B pressed");
				myDFPlayer.start();  //Play next mp3 every 3 second.
				img.draw(tft,0,0);
			}
      if (! (buttons & (1 << BUTTON_LEFT))) {
        Serial.println("Button Y pressed");
				myDFPlayer.pause();
				testfastlines(ST77XX_YELLOW, ST77XX_BLUE);
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
    if ((abs(x - last_x) > 3) || (abs(y - last_y) > 3)) {
      Serial.print(x); Serial.print(", "); Serial.println(y);
      last_x = x;
      last_y = y;
      detachInterrupt(ssInterrupt.PIN);
      attachInterruptArg(ssInterrupt.PIN, isr, &ssInterrupt, FALLING); 
      ssInterrupt.pressed = false;
      uint32_t buttons = ss.digitalReadBulk(button_mask);
    }
    vTaskDelay(10);
  }
}
static const char *pcTextForSS = "69420\r\n";

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
