# Simple GPS Tracker
## GPS Tracker with ESP32 and Adafruit 128x64 OLED Module
### Using dual core, where core 1 works on GPS process and OLED Display, and core 2 works on WiFi connection and POST requests with caching (fail to send compensation)

## Steps to use
- Get yourself an ESP32 Devkit/Development board, UART GPS Module (Ublox Neos), optionally an OLED Module, and a (preferably) stable WiFi Internet Connection
- Clone
- Modify the SSID, Password and Endpoint
- Setup Endpoint to accept tracker id, lat, long, and timestamp (Example JSON POST Object : {"tracker_id": 2, "lat": "0.836855", "lon": "106.519936", "timestamp": "2024-02-25 08:14:55"}
- Turn on the device and your WiFi Connection
- Wait for the GPS Module to establish a fix on the location, as indicated by the display and the blinking of the GPS Module LED
- Once connected it will try to continously send a request per second
- ???
- Enjoy
