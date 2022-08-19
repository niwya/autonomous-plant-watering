#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

// Declare the pins used
static const uint8_t loadPin = 32;
static const uint8_t touchPin = 27;

// Desired time between waterings
static const int delayWatering = 30; // [s]
// Desired watering time
static const int durationWatering = 1; // [s]

// Constants
static const int sToMilisec = 1000;
static const int sToMicrosec = 1000000;

void callback(){
  // Placeholder callback function
  // Should be void if touch is used as wakeup from deep sleep
  // Will be called if touch is used when ESP-32 is on
}

void waterPlant(const int duration, const int pin){
  // Water during the chosen duration
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
};

void setupWiFi() {
  // Set mode
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
  // Local initialization
  WiFiManager wm;
  // Wipe stored credentials for testing, comment for production
  wm.resetSettings();
  // Connect to access point using password
  bool res;
  res = wm.autoConnect("AutoConnectAP", "password");
  if (!res) {
    Serial.print("Not connected to WiFi");
  }
  else {
    Serial.print("Connected to WiFi");
  }
};

void setup() {
  // Initiate PIN on which load is 
  pinMode(loadPin, OUTPUT);

  // Configure wake up sources
  // BY TIMER
  esp_sleep_enable_timer_wakeup(delayWatering*sToMicrosec);
  // BY TOUCH
  touchAttachInterrupt(touchPin, callback, 30);
  esp_sleep_enable_touchpad_wakeup();
  // BY CLOCK

  // Print why we were woken up
  Serial.begin(9600);
  delay(1000); // 1 second delay to allow the Serial Monitor to open
  esp_sleep_wakeup_cause_t wakeupSource = esp_sleep_get_wakeup_cause();
  switch(wakeupSource) {
    case ESP_SLEEP_WAKEUP_TOUCHPAD: 
      Serial.print("TOUCHPAD\n"); 
      waterPlant(durationWatering*sToMilisec, loadPin);
      break;
    case ESP_SLEEP_WAKEUP_TIMER: 
      // waterPlant(durationWatering*sToMilisec, loadPin);
      setupWiFi();
      Serial.print("TIMER\n"); break;
    default: Serial.print("DEFAULT\n"); break;
  };

  // Deep sleep mode
  esp_deep_sleep_start();
}

void loop() {
  // Will never be called (because of deep sleep mode)
}
