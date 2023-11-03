#include "defines.h"
#include "display.h"

BluetoothSerial BT_Serial;

bool BT_Connected = false;
bool waitForBT = false; // Overrides encoder switch read if set to true here

#define BT_DEV_NAME "POAPSU"

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {

  if (event == ESP_SPP_SRV_OPEN_EVT) {

    Serial.println();
    Serial.print("Bluetooth Client Connected - (Address: ");

    // Print client address
    for (int i = 0; i < 6; i++) {
      Serial.printf("% 02X", param->srv_open.rem_bda[i]);

      if (i < 5) {
        Serial.print(":");
      }
    }

    Serial.println(")");
    Serial.println();

    digitalWrite(BT_LED, HIGH);
    BT_Connected =
        true; // Needs to be after the prints, code will start right away

  } else if (event == ESP_SPP_CLOSE_EVT) {

    Serial.println("Bluetooth Client disconnected");

    digitalWrite(BT_LED, LOW);
    BT_Connected = false;
  }
}

void initBT() {

  pinMode(BT_LED, OUTPUT);
  digitalWrite(BT_LED, LOW);

  BT_Serial.register_callback(callback);

  /// DEBUG inversion
  if (!waitForBT) { // Encoder 1 key down at boot

    startBT();

    Serial.println("Waiting for BT...");
    displayWaitForBT();

    while (!BT_Connected) {
      esp_task_wdt_reset();
      handleOTA();
      yield();
    }
    displayConnectedBT();
    delay(2000);
  }
}

void startBT() {

  BT_Serial.begin(BT_DEV_NAME);
  BT_Serial.setTimeout(100); // Timeout value for parseInt and stuff
}
