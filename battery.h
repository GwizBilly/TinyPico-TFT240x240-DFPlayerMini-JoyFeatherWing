#include <TinyPICO.h>
TinyPICO tp = TinyPICO();
bool batStat;
float setupBattery() {
  float battery = 0;
  if(!tp.IsChargingBattery()){
    battery = tp.GetBatteryVoltage();
  } else {
    battery = 1;
  }
  Serial.print(battery);
  return battery;
}

void vTaskLowBat (void *pvParameters) {
  char *pcTaskName;
  pcTaskName = (char *)pvParameters;
  float battery;
  for ( ;; ) {
    Serial.print(pcTaskName);
    if (!tp.IsChargingBattery()) {
      battery = tp.GetBatteryVoltage();
      Serial.print("Value from pin(Battery): ");Serial.println(battery);
    } else {
      Serial.println("Currently charging");
    }
    vTaskDelay(100000);
  }
}
