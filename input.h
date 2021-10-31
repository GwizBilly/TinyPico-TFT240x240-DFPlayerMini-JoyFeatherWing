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

void doChoice(int choice);
void handleSelection(int choice);
struct SSI {
  const uint8_t PIN;
  bool pressed;
};
SSI ssInterrupt = {5, false};

void IRAM_ATTR isr(void* arg) {
  SSI* s = static_cast<SSI*>(arg);
  s->pressed = true;
}

void IRAM_ATTR isr() {
  ssInterrupt.pressed = true;
}

int last_x = 0, last_y = 0;
int started = 0;
char lastDir = 'u';
int l = 0;
int r = 0;
int u = 0;
int d = 0;

bool choiceOne = false;
bool choiceTwo = false;
bool chooseNow = false;
int choice = 0;
void vTaskSS(void *pvParameters) {
  (void) pvParameters;
  for( ;; ) {
		// ssInterrupt.pressed
		// make interrupt a function that returns true if interrupt has been triggered recently (woke from sleep)
    if (ssInterrupt.pressed) {
      // maybe detach the interrupt until done processing
			if (started && chooseNow) { handleSelection(choice); }
      if (!started) { 
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
				detachInterrupt(ssInterrupt.PIN);
      }
      uint32_t buttons = ss.digitalReadBulk(button_mask);
      if (!(buttons & (1 << BUTTON_RIGHT))) {
        Serial.println("Button A pressed");
				if (started) { chooseNow = true; }
      }
      if (!(buttons & (1 << BUTTON_DOWN))) {
        Serial.println("Button B pressed");
      }
      if (!(buttons & (1 << BUTTON_LEFT))) {
        Serial.println("Button Y pressed");
      }
      if (!(buttons & (1 << BUTTON_UP))) {
        Serial.println("Button X pressed");
        //digitalWrite(BL_PIN, HIGH);
      }
      if (!(buttons & (1 << BUTTON_SEL))) {
        Serial.println("Button SEL pressed");
        //digitalWrite(BL_PIN, LOW);
      }
    }
    int x = ss.analogRead(2);
    int y = ss.analogRead(3);
		if (started) {
			if (x > 600 && last_x < 600) {
				// down selects "Game" 
				choice = 1;
			} else if (last_x > 600 && x < 600) {
				// not left
			}
			if (x < 400 && last_x > 400) {
				// up selects "MP3 Player"
				choice = 2;
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
			doChoice(choice); 
		}
    if ((abs(x - last_x) > 3) || (abs(y - last_y) > 3)) {
			// detect joystick movement
			Serial.println("Joy movement");
      //Serial.print(x); Serial.print(", "); Serial.println(y);
      last_x = x;
      last_y = y;
      //detachInterrupt(ssInterrupt.PIN);
      //ssInterrupt.pressed = false;
      //attachInterruptArg(ssInterrupt.PIN, isr, &ssInterrupt, FALLING);
    }
		//ssInterrupt.pressed = false;
		//attachInterruptArg(ssInterrupt.PIN, isr, &ssInterrupt, FALLING);    
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

int lastChoice = 0;
void doChoice(int choice) {
	if (choice != lastChoice) {
		switch (choice) {
			case 1:
				// down selects "Game"
				lastChoice = 1;
				tft.drawRect(tft.width()/2 -240/2, tft.height()/2 -240/2 , 240, 240, ST77XX_BLACK);
				tft.drawRect(75, 110, 80, 45, ST77XX_GREEN); 
				tft.setTextColor(ST77XX_GREEN);
				tft.setCursor(80, 120);
				tft.print("Game");

				tft.drawRect(30, 45, 190, 30, ST77XX_BLACK); 
				tft.setCursor(35, 50);
				tft.setTextColor(ST77XX_WHITE);
				tft.print("MP3 Player");
				break;
			case 2:
				// up selects "MP3 Player"
				lastChoice = 2;
				tft.drawRect(tft.width()/2 -240/2, tft.height()/2 -240/2 , 240, 240, ST77XX_BLACK);
				tft.drawRect(30, 45, 190, 30, ST77XX_GREEN); 
				tft.setCursor(35, 50);
				tft.setTextColor(ST77XX_GREEN);
				tft.print("MP3 Player");

				tft.drawRect(75, 110, 80, 45, ST77XX_BLACK); 
				tft.setTextColor(ST77XX_WHITE);
				tft.setCursor(80, 120);
				tft.print("Game");
		}
	}
}

void handleSelection(int choice) {
	switch (choice) {
		case 1:
			Serial.println("You chose Game");
			break;
		case 2:
			Serial.println("You chose MP3 Player");
			break;
		default:
			Serial.println("No selection!");
	}
	chooseNow = false;
	vTaskDelay(500);
}
