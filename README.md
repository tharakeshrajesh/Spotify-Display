# Spotify Display

A sleek desktop display that shows your currently playing Spotify track in real time. Perfect for desks, workspaces, streaming setups, and smart home environments.

![Project Banner](https://stasis.hackclub-assets.com/images/1782182841924-dkybw8.png)

## Features

* Displays current Spotify song information
*  Shows artist and album details
*  Album artwork display
*  Real-time playback updates
*  Wi-Fi connected
*  Auto-refreshes when tracks change

## BOM

| Name            | Purpose                             | Quantity | Total Cost (USD) | Link                                                 | Distributor |
|-----------------|-------------------------------------|----------|------------------|------------------------------------------------------|-------------|
| TFT LCD Display | Showing songs and info              | 1        | 1.00             | https://www.aliexpress.us/item/3256812128321582.html | AliExpress  |
| ESP32 C3 Mini   | The MCU to control screen and logic | 1        | 4.04             | https://www.aliexpress.us/item/3256810385497528.html | AliExpress  |


## Requirements

* 1.8 inch TFT LCD Display
* ESP32 C3 Mini
* DuPont Cables
* USB power supply (make sure it's compatible with your ESP32)
* 3D-printed or custom enclosure (files available for printing)
* Wi-Fi connection

## Getting Started

### 1. Download the step/obj files and slice it for your printer then print

### 2. Install Dependencies


I used PlatformIO so here is my platformio.ini file:
```
[env:esp32-c3-devkitc-02]
platform = espressif32
board = esp32-c3-devkitc-02
framework = arduino
monitor_speed = 115200
lib_deps =
    finianlandes/SpotifyEsp32
    mbed-cognoscan/Adafruit_ST7735
    sakurakoi/Adafruit-GFX-SH1122
```
If you are using ArduinoIDE then install these:
```
finianlandes/SpotifyEsp32
mbed-cognoscan/Adafruit_ST7735
sakurakoi/Adafruit-GFX-SH1122
```

### 3. Create a Spotify Application

1. Visit the Spotify Developer Dashboard
2. Create a new application
3. Copy your:

   * Client ID
   * Client Secret
4. Add your redirect URI

### 4. Configure main.cpp

Replace the variables for what they need

```cpp
const char* SSID          = "WiFi SSID/Name here";
const char* PASSWORD      = "Wifi Password here";
const char* CLIENT_ID     = "Client ID Here";
const char* CLIENT_SECRET = "Client Secret here";
```

### 5. Build the Project

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## How is it unique
In the firmware, I added button combos so that if you are pressing more than one button then that acts as another separate button.<br>
You can customize it to do whatever you want. By default, it plays a certain song which you can change by changing the track ID.

## Why I made it
I recently started using Spotify a few months ago and it got annoying to go to do actions because I had to switch windows each time. This would solve that issue. <br>
Also it looks cool.