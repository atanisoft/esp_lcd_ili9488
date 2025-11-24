# Change log for esp_lcd_ili9488

## v1.1.0 – Add ESP-IDF v6.0 Compatibility

* Added conditional inclusion of `esp_driver_ledc` in the main component **REQUIRES** list for ESP-IDF >= v6.0.
* Updated `esp_lcd_new_panel_ili9488` to handle the ESP-IDF v6.0 color-order API change (`color_space` -> `rgb_ele_order`).

## v1.0.11 - Updates to LVGL examples

* Removed Kconfig.projbuild from the LVGL example as it introduced complexity
that wasn't needed.
* Restrict LVGL example to LVGL 8.x as it will require rework for LVGL 9.x.
* Updated LVGL example for ESP-IDF v5.4 

## v1.0.10 - Bug fixes and 16-bit mode for i8080 example

* Ensure the color_buffer is freed upon delete (@nebkat)
* Enable 16-bit mode support for i8080 example and updates for idf v5.0 APIs

## v1.0.9 - Bug fixes and new example

* New i8080 example has been added (@meibao-real)
* A bug with rotation has been fixed (@jacobvc)

## v1.0.8 - Adds support for portrait / landscape orientation.

Thanks to a contribution from @jacobvc it is possible to configure the
display as portrait (default) or landscape.

## v1.0.5 - Add 16-bit support for parallel IO interface support

Parallel IO mode (Intel 8080 interface) should use 16-bit color mode instead
of being forced into 18-bit color mode that is required for use when the SPI
interface is used.

## v1.0.4 - License tagging fixes

This release only updates the SPDX license tagging of source files.

## v1.0.3 - Bug Fixes and LVGL example update

This release removes (and desupports) a dependency on 32-bit color depth when
using LVGL, the color conversion code has only ever supported 16-bit color data
but was incorrectly casting the source color data to `uint32_t` instead of
`uint16_t`.

As a result of the above bug fix the `esp_lcd_panel_dev_config_t` field
`bits_per_pixel` will be ignored by `esp_lcd_new_panel_ili9488` and the ILI9488
will always use the 18-bit color mode.

The LVGL example has been updated as below:

* Remove unused `lvgl_port_update_callback` since the example does not include
touch support or any screen rotation API calls.
* Add option for enabling / disabling double buffering with LVGL, default is to
use a single buffer.
* Added missing Kconfig.projbuild entry for `TFT_RESET_PIN`, added new entry
`DISPLAY_COLOR_MODE` which can be used to toggle between BGR and RGB color mode
on the ILI9488 display.
* Enhanced example to switch from only using `lv_spinner_create` to instead use
`lv_meter_create` and `lv_anim_t` to display an animated gauge.
* Reduced default backlight brightness to 75%.
* Switched from using `esp_register_freertos_tick_hook` to `esp_timer_create`
for the tick update handler.
* Add code to set the background color of the display to black.

## v1.0.2 - Bug Fix Release

This release contains a bug fix related to a compilation failure with ESP-IDF
v5.0.1 replacing `esp_lcd_color_space_t` with `lcd_color_rgb_endian_t` but
retaining the original name in `esp_lcd_panel_dev_config_t` for the value. At
this time the library supports both ESP-IDF v4.4.x and v5.x but the new values
in `lcd_color_rgb_endian_t` are not being used as they are compatible with the
existing values defined in `esp_lcd_color_space_t`, once ESP-IDF v5.1.x has
become stable and legacy enums are removed this library will be updated accordingly.

Fixed: https://github.com/atanisoft/esp_lcd_ili9488/issues/1

## v1.0.1 - No changes

## v1.0.0 - Initial release

This release is the first revision of this component and includes basic support
for ILI9488 displays including the 18-bit color depth conversion which is
required for SPI interface displays.
