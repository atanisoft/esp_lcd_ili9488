# ESP LCD ILI9488

Implementation of the ILI9488 LCD controller with esp_lcd component. 

| LCD controller | Communication interface | Component name | Link to datasheet |
| :------------: | :---------------------: | :------------: | :---------------: |
| ILI9488        | SPI                     | esp_lcd_ili9488     | [Specification](https://focuslcds.com/content/ILI9488.pdf) |

## Add to project

This package is not part of the [Espressif's component service](https://components.espressif.com/)
at this time. Adding this component to your project can be done by using adding the following
to your `idf_component.yml` file:
```
esp_lcd_ili9488:
    git: https://github.com/atanisoft/esp_lcd_ili9488.git
```

Under the `dependencies` section such as:
```
dependencies:
  idf: ">=4.4"
  esp_lcd_ili9488:
    git: https://github.com/atanisoft/esp_lcd_ili9488.git
```

For more information on the usage of the `idf_component.yml` file please refer to [Espressif's documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html).

## Example usage

Coming soon.