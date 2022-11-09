# LVGL Example using ILI9488 display

This is a very basic example using LVGL and an ILI9488 SPI display.

## Default pin assignments

For the ESP32 pins are as follows:
| pin | usage |
| --- | ----- |
| SPI MISO | 2 |
| SPI MOSI | 15 |
| SPI CLK | 14 |
| TFT CS | 16 |
| TFT DC | 17 |
| TFT Backlight | 18 |

On the ESP32 only the TFT pins can be reconfiguring by using `idf.py menuconfig`.

For the ESP32-S3 pins are as follows:
| pin | usage |
| --- | ----- |
| SPI MISO | 13 |
| SPI MOSI | 15 |
| SPI CLK | 14 |
| TFT CS | 3 |
| TFT DC | 9 |
| TFT Backlight | 12 |

On the ESP32-S3 all pins can be reconfigured by using `idf.py menuconfig`.

