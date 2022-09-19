# Autonomous Plant Watering System

- ![60%](https://progress-bar.dev/60) Autonomous plant watering system (ESP-32, C/C++)
- [X] Source all hardware
- [X] Test all hardware
- [X] Try timer/touch deep sleep wake-up
- [X] Add WiFi time and date fetching for weekly watering on chosen day, using WiFiManager library
- [ ] Figure out how to store last watering date and default value (SPIFFS or NVS or RTC RAM)

:heavy_check_mark: SPIFFS: might be the best bet to store day-month-year, updated after each watering so not so frequently + does wear leveling + is the most reliable

:x: NVS: might conflict with how WiFi credentials are stored using WiFi Manager

:x: RTC RAM: is wiped when need to restart with ESP.restart() when WiFi connection attempts times out

- [ ] Solder all electronic components
- [ ] Put up the reservoir/pump/... all together
- [ ] Update the README with the wiring schematics + process documentation

Would be nice to have:
- [ ] Add the possibility to trigger from anywhere through a web page