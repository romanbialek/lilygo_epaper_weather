**Current Weather (LilyGo T5 epaper 2,66)**

This project uses an ESP32 microcontroller to fetch weather data from the *Open Meteo API* and display it on a *LilyGo T5 E-Paper* Display. The display shows temperature, humidity, wind speed, and last actualization time.

The system is optimized to be powered by solar energy, thanks to the ESP32's low power consumption in the sleep mode. The device wakes up, fetches the latest weather data, updates the display, and then goes back to sleep, conserving power for efficient solar operation.

Based on HelloWorldExample from:
https://github.com/Xinyuan-LilyGO/LilyGo-T5-Epaper-Series/tree/master

- Refreshing every 10 minutes
- Sleep mode during pauses with ultra low power consumption
- Using free https://api.open-meteo.com/ API
- Using Wifi connection

![image](https://github.com/romanbialek/lilygo_epaper_weather/blob/main/lilygo_weather.jpg?raw=true)


