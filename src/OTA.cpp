// #define OTA_ENABLED

#ifdef OTA_ENABLED

#include "credentials.h"

#include "LCD_defs.h"
#include "defines.h"

#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

void handleOTA() { ArduinoOTA.handle(); }

void initOTA() {

  displayOTASetup();

  // Start wifi for OTA
  Serial.println("Starting Wifi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    displayOTAFail();
    ;
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // ArduinoOTA.setPort(3232);              // Port defaults to 3232
  ArduinoOTA.setHostname("POAPSU_OTA"); // Hostname defaults to esp3232-[MAC]

  // ArduinoOTA.setPassword("admin");       // No authentication by default

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
        // using SPIFFS.end()
        Serial.println("Start updating " + type);

        displayOTAStarting(type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
        //    lcd.setTextColor(ILI9341_GREEN);
        //    lcd.fillRect(100, 180, 65, 22, ILI9341_BLACK);
        //    lcd.setCursor(75, 198);
        //    lcd.print("Done!");
        displayOTADone();
        delay(2000);
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        esp_task_wdt_reset();

        // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        Serial.print("Progress: ");

        uint8_t actual_progress = progress / (total / 100);
        Serial.println(actual_progress);

        // show progress on lcd
        displayOTAProgress(actual_progress);
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();

  displayOTASuccess(WiFi.localIP().toString());

  Serial.print("OTA Ready at IP address: ");
  Serial.println(WiFi.localIP());

  delay(5000);

  //  while(1){}
}
#else

void initOTA() { return; }
void handleOTA() { return; }

#endif
