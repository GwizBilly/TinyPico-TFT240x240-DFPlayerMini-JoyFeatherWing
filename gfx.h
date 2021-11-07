
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <SPIFFS_ImageReader.h>
SPIFFS_ImageReader reader;

#define BL_PIN 32    //backlight
#define TFT_CS  27
#define TFT_RST -1 
#define TFT_DC  14
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void setupGFX(void) {
  pinMode(BL_PIN, OUTPUT);  
  digitalWrite(BL_PIN, HIGH);
  tft.init(240, 240); // Init ST7789 240x240
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLUE);
  SPIFFS_Image img;
  char picture[] = "/adabot.bmp";
  reader.loadBMP(picture, img);
  img.draw(tft,0,0);
  //testfastlines(ST77XX_RED, ST77XX_BLUE);
}

void setupMP3(void) {
  SPIFFS_Image img;
  char picture[] = "/MP3Player.bmp";
  reader.loadBMP(picture, img);
  img.draw(tft,0,0);
}
