#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti WiFiMulti;

void setupWifi (void) {
  WiFiMulti.addAP("VIRGIN187", "DEEED699");

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void vTaskWifi( void *pvParameters ) {
  char *pcTaskName;
  pcTaskName = ( char * ) pvParameters;
  for( ;; ) {
    Serial.print(pcTaskName);
    const uint16_t port = 8080;
    const char * host = "192.168.2.13"; // ip or dns
    Serial.print("Connecting to ");
    Serial.println(host);
    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        vTaskDelete(NULL);
    } else {
      client.print("GET /index.html HTTP/1.1\n\n");
  
      int maxloops = 0;
    
      //wait for the server's reply to become available
      while (!client.available() && maxloops < 1000) {
        maxloops++;
        vTaskDelay(1);
      }
      if (client.available() > 0) {
        Serial.println("begin");
        while(client.available()) {
          //read back one line from the server
          char c = client.read();
          Serial.write(c);
        }
        Serial.println("end");
      } else {
        Serial.println("client.available() timed out ");
      }
      Serial.println("Closing connection.");
      client.stop();
    }
    Serial.println("Waiting 10 seconds before restarting...");
    vTaskDelay(10000);
  }
}
static const char *pcTextForWifi = "WiFiWiFiWiFiWiFi\r\n";
