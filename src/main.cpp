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
static const int delayWatering = 10; // [s]
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
  // ONLY USE FOR FIRST SET UP ON A NEW WiFi NETWORK (TO INPUT AND STORE NEW CREDENTIALS)
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
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
  Serial.begin(115200);
  // Local initialization
  WiFiManager wm;
  if (!wm.autoConnect()) {
    Serial.print("Not connected to WiFi");
  }
  else {
    Serial.print("Connected to WiFi");
  }
}

struct tm getDay () {
  // Connect to WiFi network to get date and time info
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  // Local initialization
  WiFiManager wm;
  wm.setConfigPortalTimeout(5); // waits 5 seconds and return 
  if (!wm.autoConnect()) {
    Serial.print("Not connected to WiFi\n");
    delay(3000);
    // If we can't connect to WiFi, restart ESP /!\ MIGHT NEED TO WAIT TIMER TO CALL THIS AGAIN > ISSUES
    ESP.restart();
  }
  // If we get there, ESP-32 is connected to WiFi
  Serial.print("Connected to WiFi\n");
  configTime(gmtOffset, daylightOffset, ntpServer);
  struct tm timeInfo;
  if(!getLocalTime(&timeInfo)){
    Serial.println("Failed to obtain time\n");
    delay(3000);
    ESP.restart();
  }
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S\n");
  wm.disconnect();
  return timeInfo;
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

  // Declare the date/time storing variable
  struct tm dayTime;

  // Wake up actions by wake up source
  Serial.begin(115200); // Open serial monitor to check wake up reason
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
      dayTime = getDay();
      break;
    // Any other wake up source triggers checking the date/time and water if it's ok
    default: 
      Serial.print("DEFAULT\n");
      dayTime = getDay();
      break;
  };

  // Enter deep sleep mode
  esp_deep_sleep_start();
}

void loop() {
  // Will never be called (because of deep sleep mode)
}
////////////////////////////////////////////////////////////////////////////////