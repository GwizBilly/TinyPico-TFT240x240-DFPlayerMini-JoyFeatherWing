#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions
#define USE_SD_CARD
#define SD_CS    4 // SD card select pin
#if defined(USE_SD_CARD)
  SdFat                SD;         // SD card filesystem
  Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
#else
  // SPI or QSPI flash filesystem (i.e. CIRCUITPY drive)
  #if defined(__SAMD51__) || defined(NRF52840_XXAA)
    Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
      PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
  #else
    #if (SPI_INTERFACES_COUNT == 1)
      Adafruit_FlashTransport_SPI flashTransport(SS, &SPI);
    #else
      Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
    #endif
  #endif
  Adafruit_SPIFlash    flash(&flashTransport);
  FatFileSystem        filesys;
  Adafruit_ImageReader reader(filesys); // Image-reader, pass in flash filesys
#endif
Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;
                     
#define LEDC_CHANNEL_0 0
#define LEDC_TIMER_13_BIT 13
#define LEDC_BASE_FREQ 5000
#define LED_PIN 32
int brightness = 200;    // how bright the LED is
int fadeAmount = 1;    // how many points to fade the LED by
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);
  // write duty to LEDC
  ledcWrite(channel, duty);
}

#include <driver/rtc_io.h>
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR char joyState = '0';
#include <TinyPICO.h>
unsigned long next_temp_read = 0;   // Next time step in milliseconds 
uint32_t temp_read_interval = 1000;  // This is in milliseconds
TinyPICO tp = TinyPICO();

#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
SoftwareSerial mySoftwareSerial(26, 25); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
int soundCheck;

#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#define TFT_CS        27
#define TFT_RST        15// Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         14
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
float p = 3.1415926;

#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN  7
#define BUTTON_LEFT  9
#define BUTTON_UP    10
#define BUTTON_SEL   14
int last_x = 0, last_y = 0;
uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) | 
                        (1 << BUTTON_LEFT) | (1 << BUTTON_UP) | (1 << BUTTON_SEL);

void setup(){
  ++bootCount;
  ImageReturnCode stat; // Status from image-reading functions
  Serial.begin(115200);
  while(!Serial) {
    delay(10);
  }  
  if(!SD.begin(SD_CS, SD_SCK_MHZ(10))) { // Breakouts require 10 MHz limit due to longer wires
    Serial.println(F("SD begin() failed"));
    //for(;;); // Fatal error, do not continue
  }
  Serial.println(joyState);
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();
  if(!ss.begin(0x49)){
    Serial.println("ERROR! seesaw not found");
    while(1) {tp.DotStar_CycleColor(5);}
  } else {
    Serial.println("seesaw started");
    Serial.print("version: ");
    Serial.println(ss.getVersion(), HEX);
    ss.pinModeBulk(button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(button_mask, 1);
    mySoftwareSerial.begin(9600);
    delay(100);
    soundCheck = myDFPlayer.begin(mySoftwareSerial);
    if (soundCheck) {
      Serial.println("DFPlayer sound checked!");
      myDFPlayer.setTimeOut(100);
      myDFPlayer.volume(15);  //Set volume value. From 0 to 30
      //myDFPlayer.playMp3Folder(1);  //Play the first sound track
      
      //----Set volume----
      myDFPlayer.volume(10);  //Set volume value (0~30).
      myDFPlayer.volumeUp(); //Volume Up
      myDFPlayer.volumeDown(); //Volume Down
    
      //----Set different EQ----
      myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
      //  myDFPlayer.EQ(DFPLAYER_EQ_POP);
      //  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
      //  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
      //  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
      //  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
    
      //----Set device we use SD as default----
      //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
      myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
      //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
      //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
      //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);
    
      //----Mp3 control----
      //  myDFPlayer.sleep();     //sleep
      //  myDFPlayer.reset();     //Reset the module
      //  myDFPlayer.enableDAC();  //Enable On-chip DAC
      //  myDFPlayer.disableDAC();  //Disable On-chip DAC
      //  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
    
      /*
      myDFPlayer.next();  //Play next mp3
      delay(1000);
      myDFPlayer.previous();  //Play previous mp3
      delay(1000);
      myDFPlayer.play(1);  //Play the first mp3
      delay(1000);
      myDFPlayer.loop(1);  //Loop the first mp3
      delay(1000);
      myDFPlayer.pause();  //pause the mp3
      delay(1000);
      myDFPlayer.start();  //start the mp3 from the pause
      delay(1000);
      myDFPlayer.playFolder(15, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
      delay(1000);
      myDFPlayer.enableLoopAll(); //loop all mp3 files.
      delay(1000);
      myDFPlayer.disableLoopAll(); //stop loop all mp3 files.
      delay(1000);
      myDFPlayer.playMp3Folder(4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
      delay(1000);
      myDFPlayer.advertise(3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
      delay(1000);
      myDFPlayer.stopAdvertise(); //stop advertise
      delay(1000);
      myDFPlayer.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
      delay(1000);
      myDFPlayer.loopFolder(5); //loop all mp3 files in folder SD:/05.
      delay(1000);
      myDFPlayer.randomAll(); //Random play all the mp3.
      delay(1000);
      myDFPlayer.enableLoop(); //enable loop.
      delay(1000);
      myDFPlayer.disableLoop(); //disable loop.
      delay(1000);
      */
      Serial.println("DF read:");
      Serial.println(myDFPlayer.readState()); //read mp3 state
      Serial.println(myDFPlayer.readVolume()); //read current volume
      Serial.println(myDFPlayer.readEQ()); //read EQ setting
      Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
      Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
      Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
    } else {
      Serial.println("SoundCheck Failed");
    }
  }
  //Configure GPIO33 as ext0 wake up source for HIGH logic level
  rtc_gpio_pulldown_en(GPIO_NUM_33);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1);
  pinMode(GPIO_NUM_33, INPUT_PULLDOWN);

   // Setup timer and attach timer to a led pin
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
      
  tft.init(240, 240);           // Init ST7789 240x240
  tft.fillScreen(ST77XX_BLACK);
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  ledcAnalogWrite(LEDC_CHANNEL_0, 255);
  delay(1000);
  tft.fillScreen(ST77XX_BLUE);
  Serial.print(F("Loading adabot240.bmp slowly to screen..."));
  stat = reader.drawBMP("/adabot240.bmp", tft, 0, 0);
  reader.printStatus(stat);   // How'd we do?
  stat = reader.loadBMP("/image24.bmp", img);
  reader.printStatus(stat); // How'd we do?
  
  while (1) {
    if (soundCheck) {
      if (myDFPlayer.available()) {
        printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
      }
    }
    
    int x = ss.analogRead(2);
    int y = ss.analogRead(3);
    if ((abs(x - last_x) > 3) || (abs(y - last_y) > 3)) {
      Serial.print(x); Serial.print(", "); Serial.println(y);
      last_x = x;
      last_y = y;
      /* oooooooooooooooooooo */
      // set the brightness on LEDC channel 0
      ledcAnalogWrite(LEDC_CHANNEL_0, brightness);
      // change the brightness for next time through the loop:
      brightness = brightness + fadeAmount;
      // reverse the direction of the fading at the ends of the fade:
      if (brightness <= 0 || brightness >= 255) {
        fadeAmount = -fadeAmount;
      }
      /* 00000000000000000000000000000 */
    }
    uint32_t buttons = ss.digitalReadBulk(button_mask);
    //Serial.println(buttons, BIN);
    if (! (buttons & (1 << BUTTON_RIGHT))) {
      float sensorValue = analogRead(35);
      Serial.println("Button A pressed");
      joyState = 'E';
      if (soundCheck) {Serial.println("NextMP3");myDFPlayer.next();} else {Serial.println("SoundCheck Failed");}
      Serial.print("Value from pin(Battery): ");Serial.print(sensorValue);  
      tft.println(sensorValue);  
      delay(10);
    }
    if (! (buttons & (1 << BUTTON_DOWN))) {
      testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
      Serial.println("Button B pressed");
      joyState = 'S';
      if (soundCheck) {Serial.println("VolumeDown");myDFPlayer.volume(5
      );} else {Serial.println("SoundCheck Failed");}
      tft.fillScreen(ST77XX_BLACK);
    }
    if (! (buttons & (1 << BUTTON_LEFT))) {
      Serial.println("Button Y pressed");
      joyState = 'W';
    }
    if (! (buttons & (1 << BUTTON_UP))) {
      Serial.println("Button X pressed");
      joyState = 'N';
      if (soundCheck) {Serial.println("VolumeUp");myDFPlayer.volume(17);} else {Serial.println("SoundCheck Failed");}
    }
    if (! (buttons & (1 << BUTTON_SEL))) {
      Serial.println("Button SEL pressed");
      for(int r=0; r<4; r++) { // For each of 4 rotations...
        tft.setRotation(r);    // Set rotation
        tft.fillScreen(0);     // and clear screen
        for(int i=0; i<4; i++) {
          img.draw(tft,
            (tft.width()  * i / 3) - (width  / 2),
            (tft.height() * i / 3) - (height / 2));
        }
        for(int i=0; i<50; i++) {
          img.draw(tft,                                    
            (int16_t)random(-img.width() , tft.width()) ,  
            (int16_t)random(-img.height(), tft.height())); 
        }
      }
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1);
      break;
    }
    tp.DotStar_CycleColor(55);
  }
  Serial.println("Going to sleep");
  //Go to sleep now
  esp_deep_sleep_start();
}

/* no loop when using sleep features */
void loop() {}

//Function that prints the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}
