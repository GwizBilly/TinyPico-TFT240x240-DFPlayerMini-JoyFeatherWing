#define BL_PIN 32    //backlight

#include "fileSystem.h"
#include "gfx.h"
#include "mp3.h"
#include "input.h"
//#include "wifi.h"

void setup() {
  pinMode(BL_PIN, OUTPUT);  
  digitalWrite(BL_PIN, HIGH);
  
  Serial.begin(115200);
    while(!Serial) {
    delay(1);
  }
  
  setupFileSystem();
  setupGFX();
  setupInput();
  //setupWifi();
  setupMp3();
  
  xTaskCreatePinnedToCore(vTaskSS,   "Task 1", 10000, (void*)pcTextForSS,   1, NULL, 0);
  //xTaskCreatePinnedToCore(vTaskWifi, "Task 2", 10000, (void*)pcTextForWifi, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskMp3,  "Task 3", 10000, (void*)pcTextForMp3, 1, NULL, 1);	
}

void loop() {}
