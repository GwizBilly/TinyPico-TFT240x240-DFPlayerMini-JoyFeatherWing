#include <driver/rtc_io.h>
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define DURATION_OF_SLEEP  7       /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setupSleep() {
  bootCount++;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();
}

void sleepNow() {
  Serial.println("sleep now...");
  tft.enableDisplay(0);
  tp.DotStar_SetPower( false );
  myDFPlayer.outputSetting(false, 0);
  //Configure GPIO33 as wake up source with HIGH logic level (normally pulled down).
  rtc_gpio_pulldown_en(GPIO_NUM_33);
  //now configure to wake up esp32 on button press
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1);
  //now configure wake up by itself after a while
  esp_sleep_enable_timer_wakeup(DURATION_OF_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
