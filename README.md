# esp_lcd driver for ILI9488 displays

This component provides an implementation of the ILI9488 LCD controller using the esp_lcd component APIs.

| LCD controller | Communication interface | Component name | Link to datasheet |
| :------------: | :---------------------: | :------------: | :---------------: |
| ILI9488        | SPI or Intel 8080       | esp_lcd_ili9488 | [Specification](https://focuslcds.com/content/ILI9488.pdf) |

## Note on supported communication interfaces

When using the SPI interface it is required to use 18-bit color depth mode as below:

```
    const esp_lcd_panel_dev_config_t lcd_config = 
    {
    ...
        .bits_per_pixel = 18,
    ...
    };
```

When using the Intel 8080 (Parallel) interface the 16-bit color depth mode should be used.

## Display Reset pin

If the display requires the usage of a RESET pin during the initialization process
be sure to configure it as below:

```
    const esp_lcd_panel_dev_config_t lcd_config = 
    {
        .reset_gpio_num = CONFIG_TFT_RESET_PIN,
        ....
```

If the display does not require this pin set this value to GPIO_NUM_NC (-1).

## Using this component in your project

This package can be added to your project in two ways:

1. Using [Espressif's component service](https://components.espressif.com/) as:
```
dependencies:
  atanisoft/esp_lcd_ili9488: "~1.0.0"
```

2. Using the git repository directly:

```
dependencies:
  esp_lcd_ili9488:
    git: https://github.com/atanisoft/esp_lcd_ili9488.git
```

For more information on the usage of the `idf_component.yml` file please refer to [Espressif's documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html).

## Supported platforms

At this time testing is limited to ESP32 and ESP32-S3, other ESP32 variants should work but are not tested.

## Required sdkconfig entries

This driver converts the color data from 16-bit to 18-bit as part of the `draw_bitmap` callback.
Therefore it is required to set `CONFIG_LV_COLOR_DEPTH_16=y` in your sdkconfig. In the future other
color depths may be supported.
