#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <SPIFFS_ImageReader.h>

#define BL_PIN 32    //backlight
#define TFT_CS  27
#define TFT_RST -1 
#define TFT_DC  14


void setupGFX(void) {
  /*
	reader.loadBMP(picture, img);
  //reader.loadBMP(picture, imgTwo);
  img.draw(tft,0,0);
	*/ //delay(5000);
}

void vTaskGFX(void *pvParameters) {
	Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
  pinMode(BL_PIN, OUTPUT);  
  digitalWrite(BL_PIN, HIGH);
  tft.init(240, 240); // Init ST7789 240x240
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLUE);
	SPIFFS_ImageReader reader;
	SPIFFS_Image img1;
	SPIFFS_Image img2;
	char picture[] = "/eyeL.bmp";
	char pictureTwo[] = "/eyeR.bmp";
	char pictureThree[] = "/bot.bmp";
	reader.loadBMP(picture, img1);
	reader.loadBMP(pictureTwo, img2);
	reader.drawBMP(pictureThree, tft, 0, 0);
  (void) pvParameters;
  for( ;; ) {
		//Serial.println("wooooooooooo");
		img1.draw(tft,60,80);
		img2.draw(tft,130,80);
		vTaskDelay(100);
		img2.draw(tft,60,80);
		img1.draw(tft,130,80);
		vTaskDelay(100);
	}
}
