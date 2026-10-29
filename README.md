# 📻 yoRadio ESP32-S3 N16R8 | 2.4" ST7789 TFT

**Custom yoRadio Firmware | Version 2026.31.03 | English Edition**

A customized version of the open-source [yoRadio project by e2002](https://github.com/e2002/yoradio), designed for the **ESP32-S3 N16R8** microcontroller with a **2.4-inch ST7789 TFT display**, rotary encoder, dedicated sleep button, and PCM5102 I2S DAC.

This version includes improvements to the user interface, hardware configuration, daylight saving time handling, and button functionality.

---

## ✨ Features & Improvements

- **Automatic Daylight Saving Time (DST)** - Fixed switching between summer and winter time.
- **Sleep Button** - Improved sleep button functionality for reliable operation.
- **Customized UI** - Modified interface layout and color scheme.
- **TFT Display Support** - Optimized configuration for the 2.4" ST7789 display.
- **Rotary Encoder** - Integrated controls for navigation and volume adjustment.
- **Wi-Fi Connectivity** - Supports 2.4 GHz wireless networks.
- **Web Interface** - Configure radio stations and control the device through a web browser.

## ⚙️ Hardware Requirements

| Component | Specification |
|---|---|
| Microcontroller | ESP32-S3 N16R8 |
| Flash Memory | 16 MB |
| PSRAM | 8 MB |
| Display | 2.4" ST7789 TFT (SPI) |
| Audio DAC | PCM5102 (I2S) |
| Controls | Rotary encoder with push button |
| Sleep Control | Dedicated push button |
| Connectivity | Wi-Fi 2.4 GHz |
| Power Supply | USB-C (ESP32 board) |

## 🔌 Wiring & Pinout

The following pin configuration is used in this custom firmware.

### TFT Display - ST7789 (2.4")

| TFT Pin | ESP32-S3 Pin | Function |
|---|---|---|
| VCC | 3V3 | Power Supply |
| GND | GND | Ground |
| SCL | GPIO 12 | SPI Clock (SCK) |
| SDA | GPIO 11 | SPI Data (MOSI) |
| RES | GPIO 38 | Display Reset |
| DC | GPIO 9 | Data / Command |
| CS | GPIO 10 | Chip Select |
| BLK | GPIO 14 | Backlight Control |

### Rotary Encoder

| Encoder Pin | ESP32-S3 Pin | Function |
|---|---|---|
| A | GPIO 15 | Encoder Channel A |
| B | GPIO 16 | Encoder Channel B |
| PUSH | GPIO 17 | Encoder Push Button |

### Sleep Button

| Button Pin | ESP32-S3 Pin | Function |
|---|---|---|
| KO | GPIO 8 | Sleep Control |

### Audio DAC - PCM5102 (I2S)

| PCM5102 Pin | ESP32-S3 Pin | Function |
|---|---|---|
| VIN | 3V3 | Power Supply |
| GND | GND | Ground |
| BCK | GPIO 4 | I2S Bit Clock |
| DIN | GPIO 5 | I2S Audio Data |
| LRCK | GPIO 6 | I2S Word Select |

**Note:** The wiring tables describe this specific hardware configuration. Other ESP32 boards or display modules may require different pin assignments.

---

## 🚀 Installation

### 1. Software Requirements

Before compiling the firmware, install the following software:

| Software | Version / Requirement |
|---|---|
| Arduino IDE | **1.8.19 (Legacy)** |
| ESP32 Boards Package | 2.0.0 or newer, compatible with the firmware |
| ESP32 Filesystem Uploader | Required for the Arduino IDE filesystem upload method |
| USB Driver | Depending on your ESP32-S3 board |

**Important:** Arduino IDE 1.8.19 is recommended according to the original yoRadio documentation. Arduino IDE 2.x is not officially supported by the original installation instructions.

### 2. Required Arduino Libraries

Install the following libraries using **Arduino IDE → Sketch → Include Library → Manage Libraries**.

| Library | Purpose |
|---|---|
| [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) | Graphics and text rendering |
| [Adafruit ST7735 and ST7789 Library](https://github.com/adafruit/Adafruit-ST7735-Library) | TFT display driver |
| [OneButton](https://github.com/mathertel/OneButton) | Button handling, if not already bundled |

Some dependencies are already included in newer versions of yoRadio. Additional libraries may be required depending on the enabled hardware and firmware features.

See the [original yoRadio dependencies](https://github.com/e2002/yoradio#dependencies) for more information.

### 3. Download the Firmware

Download this repository using **Code → Download ZIP**, or clone it using Git.

Extract the downloaded files to a local directory.

### 4. Configure the Firmware

1. Open `yoRadio.ino` in Arduino IDE.
2. Select your ESP32-S3 board in **Tools → Board**.
3. Open `myoptions.h`.
4. Verify the display, encoder, DAC, and button pin configuration.
5. Configure the required hardware settings.

**Important:** Modify `myoptions.h` rather than editing the original `options.h` file.

If the display requires the custom yoRadio font symbols, follow the original instructions for replacing `Adafruit_GFX_Library/glcdfont.c` with `yoRadio/fonts/glcdfont.c`.

### 5. Upload Filesystem Data

Install the [ESP32 Filesystem Uploader](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) if you are using the legacy Arduino IDE upload method.

In Arduino IDE:

1. Connect the ESP32-S3 to your computer.
2. Select the correct COM port.
3. Select **Tools → ESP32 Sketch Data Upload**.
4. Upload the filesystem data to the board.

**Note:** Newer upstream yoRadio versions also provide a Web Board Uploader, which can be used instead of the legacy filesystem upload tool.

### 6. Upload the Firmware

1. Verify the ESP32-S3 board settings.
2. Compile the project.
3. Select **Sketch → Upload**.
4. After uploading, restart the ESP32-S3.

### 7. First Startup & Wi-Fi Configuration

By default, the original yoRadio firmware creates a wireless access point if no Wi-Fi network has been configured.

- **Wi-Fi SSID:** `yoRadioAP`
- **Default Password:** `12345987`
- **Configuration Address:** `http://192.168.4.1`

Connect to the access point and configure your home Wi-Fi network.

After connecting to your network, open the radio's local IP address in a web browser to access the yoRadio interface and configure radio stations.

For additional information, refer to the [official yoRadio installation guide](https://github.com/e2002/yoradio/wiki/How-to-flash).

---

## 📸 Project Gallery

Photos of this custom ESP32-S3 build and its TFT interface will be added here.

For examples of the original project, visit the [official yoRadio image gallery](https://github.com/e2002/yoradio/blob/main/Images.md).

## 🔗 Useful Links

- [Original yoRadio Repository](https://github.com/e2002/yoradio)
- [Official yoRadio Wiki](https://github.com/e2002/yoradio/wiki)
- [yoRadio Installation Guide](https://github.com/e2002/yoradio/wiki/How-to-flash)
- [yoRadio Hardware Configuration Generator](https://e2002.github.io/docs/myoptions-generator.html)
- [Arduino IDE 1.8.19](https://www.arduino.cc/en/software)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)

---

## 📜 Credits & License

This project is based on the original **[yoRadio by e2002](https://github.com/e2002/yoradio)**.

Special thanks to **e2002** and all contributors to the original yoRadio project for developing and maintaining the open-source firmware.

This repository contains an unofficial, modified version adapted for a specific ESP32-S3 hardware configuration.

The original yoRadio project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.

This modified firmware is distributed under the same GPL-3.0 license. Copyright notices and license conditions from the original project must be preserved.

For full license details, see the [original yoRadio license](https://github.com/e2002/yoradio/blob/main/LICENSE).

---

**Based on yoRadio • Customized for ESP32-S3 N16R8 • Version 2026.31.03**
