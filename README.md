# 📻 yoRadio ESP32-S3 N16R8 | 2.4" ST7789 TFT

**Custom yoRadio firmware | Version 2026.31.03**

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
	KO - GPIO 8
	PUSH - GPIO 17
	B - GPIO 16
	A - GPIO 15
	BLK - GPIO 14
	CS - GPIO 10
	DC - GPIO 9
	RES - GPIO 38
	SDA -  GPIO 11
	SCL - GPIO 12
	VCC - 3V3
	GND - GND
	
	
PCM5102
	VIN - 3V3
	GND - GND
	LRCK - GPIO 6
	DIN - GPIO 5
	BCK - GPIO 4
	

Pin assignments and connection details will be documented here.

## 🚀 Installation

Download the repository, configure the firmware for your hardware, and upload it to your ESP32-S3 board.

## 📜 Credits & License

Based on the original [yoRadio project by e2002](https://github.com/e2002/yoradio).

This is an independent, modified version shared with the open-source community.

The original project is licensed under GPL-3.0.
