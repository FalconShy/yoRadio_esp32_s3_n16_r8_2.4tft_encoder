# 📻 yoRadio ESP32-S3 N16R8 | 2.4" ST7789 TFT

**Custom yoRadio firmware | Version 2026.31.03 - English Version**

This repository contains my modified version of the open-source yoRadio project, adapted for the **ESP32-S3 N16R8** with a **2.4-inch ST7789 TFT display**, rotary encoder and dedicated sleep button.

## 🔧 What's improved?

- Fixed automatic daylight saving time (DST) switching between summer and winter time.
- Fixed sleep button functionality for reliable operation.
- Customized the user interface and color scheme.
- Adapted the configuration for a 2.4" ST7789 TFT display.
- Integrated rotary encoder controls.

## ⚙️ Hardware

- ESP32-S3 N16R8 (16 MB Flash / 8 MB PSRAM)
- 2.4" TFT display (ST7789)
- Rotary encoder
- Dedicated sleep button
- 2.4 GHz Wi-Fi connectivity

## 📸 Project Gallery

**[Insert photo of the completed radio]**

**[Insert screenshot of the TFT interface]**

## 🔌 Wiring & Pinout

2.4 TFT
	-KO - GPIO 8
	-PUSH - GPIO 17
	-B - GPIO 16
	-A - GPIO 15
	-BLK - GPIO 14
	-CS - GPIO 10
	-DC - GPIO 9
	-RES - GPIO 38
	-SDA -  GPIO 11
	-SCL - GPIO 12
	-VCC - 3V3
	-GND - GND
	
	
PCM5102
	VIN - 3V3
	GND - GND
	LRCK - GPIO 6
	DIN - GPIO 5
	BCK - GPIO 4
	

Pin assignments and connection details will be documented here.

## 🚀 Installation

Download the repository, configure the firmware for your hardware, and upload it to your ESP32-S3 board.
you require extra libraries
Requirements

Arduino IDE 1.8.19 (Arduino IDE 2.x is not officially supported)

ESP32 Boards package version 2.0.0 or newer

ESP32 Filesystem Uploader

Required Arduino libraries (depending on the hardware configuration)

Required Libraries

Install the required libraries using the Arduino IDE Library Manager:

Adafruit GFX Library

Adafruit ST7735 and ST7789 Library

OneButton

Additional libraries may be required depending on the enabled features.

Setup Instructions

Install Arduino IDE 1.8.19.

Install the ESP32 Boards package.

Install the required libraries.

Install the ESP32 Filesystem Uploader.

Open yoRadio.ino in Arduino IDE.

Configure myoptions.h for your hardware.

Upload the filesystem data using Tools > ESP32 Sketch Data Upload.

Compile and upload the firmware to your ESP32-S3.

For complete installation instructions and additional dependencies, refer to the original project:

Original yoRadio repository

Official installation guide

Original dependencies

## 📜 Credits & License

Based on the original [yoRadio project by e2002](https://github.com/e2002/yoradio).

This is an independent, modified version shared with the open-source community.

The original project is licensed under GPL-3.0.
