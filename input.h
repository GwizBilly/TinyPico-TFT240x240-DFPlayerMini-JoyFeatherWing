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
    bool pressed;
};
SSI ssInterrupt = {5, 0, false};
void IRAM_ATTR isr(void* arg) {
    SSI* s = static_cast<SSI*>(arg);
    s->pressed = true;
}
void IRAM_ATTR isr() {
    ssInterrupt.pressed = true;
}

int last_x = 0, last_y = 0;

void vTaskSS(void *pvParameters) {
  //char *pcTaskName;
  //pcTaskName = (char *) pvParameters;
	bool DETACHED = false;
  for( ;; ) {
    if (ssInterrupt.pressed) {
      detachInterrupt(ssInterrupt.PIN);
      DETACHED = true;
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
    }
    int x = ss.analogRead(2);
    int y = ss.analogRead(3);
    if ((abs(x - last_x) > 3) || (abs(y - last_y) > 3)) {
      Serial.print(x); Serial.print(", "); Serial.println(y);
      last_x = x;
      last_y = y;
    }
		if () {
			ssInterrupt.pressed = false;
			attachInterruptArg(ssInterrupt.PIN, isr, &ssInterrupt, FALLING);    
			vTaskDelay(10);
		}
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
