# esp_lcd driver for ILI9488 displays

This component provides an implementation of the ILI9488 LCD controller using the esp_lcd component APIs.

| LCD controller | Communication interface | Component name | Link to datasheet |
| :------------: | :---------------------: | :------------: | :---------------: |
| ILI9488        | SPI                     | esp_lcd_ili9488 | [Specification](https://focuslcds.com/content/ILI9488.pdf) |

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