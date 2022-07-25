#include <Arduino.h>

// Pin from which the signal to send to the relay originates
static const int signalPin = 32;

// Desired time between waterings
static const int delayWatering = 30; // [s]
// Desired watering time
static const int durationWatering = 1; // [s]

// 
static const int sToMilisec = 1000;
static const int sToMicrosec = 1000000;

void callback(){
  //placeholder callback function
}

void setup() {
  // Initiate PIN
  pinMode(signalPin, OUTPUT);

  // Configure wake up sources
  // BY TIMER
  esp_sleep_enable_timer_wakeup(delayWatering*sToMicrosec);
  // BY TOUCH
  touchAttachInterrupt(T7, callback, 40);
  esp_sleep_enable_touchpad_wakeup();

  // Water during the chosen duration
  digitalWrite(signalPin, HIGH);
  delay(durationWatering*sToMilisec);
  digitalWrite(signalPin, LOW);

  // Deep sleep mode
  esp_deep_sleep_start();
}

void loop() {

}
