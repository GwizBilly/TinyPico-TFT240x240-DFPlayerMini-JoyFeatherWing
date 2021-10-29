#include "battery.h"
#include "RGB.h"
#include "fileSystem.h"
#include "gfx.h"
#include "input.h"
#include "mp3.h"
#include "wifi.h"
#include "sleep.h"

void setup() {
  setupRGB();
  Serial.begin(115200);
    while(!Serial) {
    delay(1);
  }
  float bat;
  const char *pcTextForLowBat = "I am lowBat function\r\n";
  if ((bat = setupBattery()) > 0) {   
    Serial.println(bat);
    setupFileSystem();
    setupGFX();
    setupInput();
    //setupWifi();
    setupMp3();
    setupSleep();
    xTaskCreatePinnedToCore(vTaskSS,   "Task 1", 10000, NULL,   1, NULL, 0);
    //xTaskCreatePinnedToCore(vTaskWifi, "Task 2", 10000, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(vTaskMp3,  "Task 3", 10000, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(vTaskLowBat,"Task 0", 10000, (void*)pcTextForLowBat, 1, NULL, 1);
  } else {
    const char *pcTextForOtherLowBat = "I am also lowBat function\r\n";
    xTaskCreatePinnedToCore(vTaskLowBat,"Task 0", 10000, (void*)pcTextForOtherLowBat, 1, NULL, 1);
  }
}

void loop() {}
