#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <NTPClient.h>
#include "time.h"
#include <string>
#include <ostream>

////////////////////////////// PROGRAM CONSTANTS ///////////////////////////////
// Declare the pins used
static const uint8_t loadPin = 32;
static const uint8_t touchPin = 27;

// Desired time between waterings
static const int delayWatering = 30; // [s]
// Desired watering time
static const int durationWatering = 1; // [s]

// NTP server to fetch date/time
static const char* ntpServer = "pool.ntp.org";
static const long UTC = -5;
static const long gmtOffset = UTC*3600; // [s]
static const int daylightOffset = 3600; // [s]

// Constants
static const int sToMilisec = 1000;
static const int sToMicrosec = 1000000;
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////// HELPER FUNCTIONS ///////////////////////////////
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

void resetWiFiCredentials() {
  // ONLY USE FOR FIRST SET UP ON A NEW WiFi NETWORK
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
  // Local initialization
  WiFiManager wm;
  // Wipe stored credentials
  wm.resetSettings();
  // Connect to access point using password
  bool res;
  res = wm.autoConnect("AutoConnectAP", "password");
  // From there it's manual setup (connect to AutoConnectAP WiFi network from ESP-32
  // and enter desired WiFi network credentials)
  if (!res) {
    Serial.print("Not connected to WiFi");
  }
  else {
    Serial.print("Connected to WiFi");
  }
};

void checkWiFi() {
  // ONLY USE TO TEST IF CURRENT CREDENTIALS CONNECTS THE ESP-32 TO DESIRED WiFi NETWORK
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
  // Local initialization
  WiFiManager wm;
  if (!wm.autoConnect()) {
    Serial.print("Not connected to WiFi");
  }
  else {
    Serial.print("Connected to WiFi");
  }
}

void getDay () {
  // Connect to WiFi network to get date and time info
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
  // Local initialization
  WiFiManager wm;
  if (!wm.autoConnect()) { // add failsafe to delay/retry if connecting is a bit slow, right now if fails it opens configs and stays
                           // there until ESP32 is turned off and on again
    Serial.print("Not connected to WiFi\n");
    return;
  }
  else {
    Serial.print("Connected to WiFi\n");
    configTime(gmtOffset, daylightOffset, ntpServer);
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time\n");
      return;
    }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");
  // instead of printing, return the tm (or just the day) to the setup() function
  wm.disconnect();
  }
  return;
};
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////// SETUP AND LOOP ////////////////////////////////
void setup() {
  // Initiate PIN on which load is 
  pinMode(loadPin, OUTPUT);

  // Configure wake up sources
  // BY TIMER
  esp_sleep_enable_timer_wakeup(delayWatering*sToMicrosec);
  // BY TOUCH
  touchAttachInterrupt(touchPin, callback, 30);
  esp_sleep_enable_touchpad_wakeup();

  // Wake up actions by wake up source
  Serial.begin(9600); // Open serial monitor to check wake up reason
  delay(1000); // 1 second delay to allow the Serial Monitor to open
  esp_sleep_wakeup_cause_t wakeupSource = esp_sleep_get_wakeup_cause();
  switch(wakeupSource) {
    case ESP_SLEEP_WAKEUP_TOUCHPAD: 
      // If requested to water plant by touch, do so
      Serial.print("TOUCHPAD\n"); 
      waterPlant(durationWatering*sToMilisec, loadPin);
      break;
    case ESP_SLEEP_WAKEUP_TIMER: 
      // Connect to WiFi every 24hrs to check the date and water plant if is goal date
      Serial.print("TIMER\n");
      getDay();
      break;
    // Any other wake up source triggers nothing - so that the plant is not watered
    // each time the ESP-32 is powered up after power loss
    default: Serial.print("DEFAULT\n"); break;
  };

  // Enter deep sleep mode
  esp_deep_sleep_start();
}

void loop() {
  // Will never be called (because of deep sleep mode)
}
////////////////////////////////////////////////////////////////////////////////