#define BL_PIN 32    //backlight

#include "battery.h"
#include "fileSystem.h"
#include "gfx.h"
#include "input.h"
#include "mp3.h"
#include "wifi.h"
#include "sleep.h"

void setup() {
  pinMode(BL_PIN, OUTPUT);  
  digitalWrite(BL_PIN, HIGH);
  tp.DotStar_Clear();
  tp.DotStar_SetPower( false );
  Serial.begin(115200);
    while(!Serial) {
    delay(1);
  }
    float bat;
    if ((bat = setupBattery()) > 0 ){   
        Serial.println(bat);
        setupFileSystem();
        setupGFX();
        setupInput();
        setupWifi();
        setupMp3();
        setupSleep();

        xTaskCreatePinnedToCore(vTaskSS,   "Task 1", 10000, (void*)pcTextForSS,   1, NULL, 0);
        xTaskCreatePinnedToCore(vTaskWifi, "Task 2", 10000, (void*)pcTextForWifi, 1, NULL, 1);
        xTaskCreatePinnedToCore(vTaskMp3,  "Task 3", 10000, (void*)pcTextForMp3, 1, NULL, 1);
        xTaskCreatePinnedToCore(vTaskLowBat,"Task 0", 10000, (void*)pcTextForLowBat, 1, NULL, 1);
    } else {
        xTaskCreatePinnedToCore(vTaskLowBat,"Task 0", 10000, (void*)pcTextForLowBat, 1, NULL, 1);
    }
}

void loop() {}
