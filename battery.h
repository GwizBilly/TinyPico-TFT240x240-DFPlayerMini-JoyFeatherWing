void setupBattery() {
  //check if battery is low and scold the user if recharging is necessary.
  float battery = analogRead(35);
  Serial.print("Value from pin(Battery): ");Serial.println(battery);
}

void vTaskBattery(void *pvParameters) {
  char *pcTaskName;
  pcTaskName = (char *) pvParameters;
  float battery; 
  for( ;; ) {
    battery = analogRead(35);
    Serial.print("Value from pin(Battery): ");Serial.println(battery);
    vTaskDelay(10000);
  }
}
static const char *pcTextForBattery = "69420\r\n";
