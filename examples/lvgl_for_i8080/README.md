| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |
# LVGL porting example

LVGL is an open-source graphics library for creating modern GUIs. It has plenty of built-in graphical elements with low memory footprint, which is friendly for embedded GUI applications.

This example can be taken as a skeleton of porting the LVGL library onto the `esp_lcd` driver layer. **Note** that, this example only focuses on the display interface, regardless of the input device driver.

The whole porting code is located in [this main file](main/lvgl_example_main.c), and the UI demo code is located in [another single file](main/lvgl_demo_ui.c).

The UI will display two images (one Espressif logo and another Espressif text), which have been converted into C arrays by the [online converting tool](https://lvgl.io/tools/imageconverter), and will be compiled directly into application binary.

This example is constructed by [IDF component manager](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/tools/idf-component-manager.html), all the external dependency will be handled by the CMake build system automatically. In this case, it will help download the lvgl from [lvgl](https://components.espressif.com/component/lvgl/lvgl) and esp_lcd_ili9488 from [esp_lcd_ili9488](https://components.espressif.com/components/atanisoft/esp_lcd_ili9488), with the version specified in the [manifest file](main/idf_component.yml).

This example uses the [esp_timer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html) to generate the ticks needed by LVGL. For more porting guides, please refer to [LVGL porting doc](https://docs.lvgl.io/master/porting/index.html).

## How to use the example

### Hardware Required

* An ESP development board
* An Intel 8080 interfaced (so called MCU interface or parallel interface) LCD
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```
   ESP3233 Board                  LCD Screen
┌─────────────┐              ┌────────────────┐
│             │              │                │
│         3V3 ├─────────────►│ VCC            │
│             │              │                │
│         GND ├──────────────┤ GND            │
│             │              │                │
│  DATA[0..7] │◄────────────►│ DATA[0..7]     │
│             │              │                │
│        IO12 ├─────────────►│ PCLK           │
│             │              │                │
│        IO10 ├─────────────►│ CS             │
│             │              │                │
│        IO14 ├─────────────►│ D/C            │
│             │              │                │
│         IO9 ├─────────────►│ RST            │
│             │              │                │
│        IO46 ├─────────────►│ BCKL           │
│             │              │                │
└─────────────┘              └────────────────┘
```


The GPIO number used by this example can be changed in [lvgl_example_main.c](main/lvgl_example_main.c).
Especially, please pay attention to the level used to turn on the LCD backlight, some LCD module needs a low level to turn it on, while others take a high level. You can change the backlight level macro `EXAMPLE_LCD_BK_LIGHT_ON_LEVEL` in [lvgl_example_main.c](main/lvgl_example_main.c).

### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A fancy animation will show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Example Output

```bash
I (321) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (341) example: Turn off LCD backlight
I (341) gpio: GPIO[46]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (351) example: Initialize Intel 8080 bus
I (351) example: Install LCD driver of ili9488
I (361) gpio: GPIO[9]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (371) ili9488: Configuring for RGB color order
I (371) ili9488: new ili9488 panel @0x3fce9e38
I (381) ili9488: Setting GPIO:9 to 0
I (391) ili9488: Setting GPIO:9 to 1
I (401) ili9488: Initializing ILI9488
I (601) ili9488: Initialization complete
I (601) example: Turn on LCD backlight
I (601) example: Initialize LVGL library
I (601) example: Register display driver to LVGL
I (611) example: Install LVGL tick timer
I (611) example: Display LVGL animation
```
