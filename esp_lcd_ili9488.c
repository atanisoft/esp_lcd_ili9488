/*
 * SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <driver/gpio.h>
#include <esp_lcd_panel_interface.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_commands.h>
#include <esp_log.h>
#include <esp_rom_gpio.h>
#include <esp_check.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/cdefs.h>

static const char *TAG = "ili9488";

typedef struct
{
    uint8_t cmd;
    uint8_t data[16];
    uint8_t data_bytes;
} lcd_init_cmd_t;

typedef struct
{
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    bool reset_level;
    int x_gap;
    int y_gap;
    uint8_t madctl_val;
    uint8_t colmod_cal;
} ili9488_panel_t;

enum ILI9488_CONSTANTS
{
    ILI9488_INTRFC_MODE_CTL = 0xB0,
    ILI9488_FRAME_RATE_NORMAL_CTL = 0xB1,
    ILI9488_INVERSION_CTL = 0xB4,
    ILI9488_FUNCTION_CTL = 0xB6,
    ILI9488_ENTRY_MODE_CTL = 0xB7,
    ILI9488_POWER_CTL_ONE = 0xC0,
    ILI9488_POWER_CTL_TWO = 0xC1,
    ILI9488_POWER_CTL_THREE = 0xC5,
    ILI9488_POSITIVE_GAMMA_CTL = 0xE0,
    ILI9488_NEGATIVE_GAMMA_CTL = 0xE1,
    ILI9488_ADJUST_CTL_THREE = 0xF7,

    ILI9488_COLOR_MODE_16BIT = 0x55,
    ILI9488_COLOR_MODE_18BIT = 0x66,

    ILI9488_INTERFACE_MODE_USE_SDO = 0x00,
    ILI9488_INTERFACE_MODE_IGNORE_SDO = 0x80,

    ILI9488_IMAGE_FUNCTION_DISABLE_24BIT_DATA = 0x00,

    ILI9488_WRITE_MODE_BCTRL_DD_ON = 0x28,
    ILI9488_FRAME_RATE_60HZ = 0xA0,

    ILI9488_INIT_LENGTH_MASK = 0x1F,
    ILI9488_INIT_DELAY_FLAG = 0x80,
    ILI9488_INIT_DONE_FLAG = 0xFF
};

static const lcd_init_cmd_t ili9488_init[] =
{
    { ILI9488_POSITIVE_GAMMA_CTL,
        { 0x00, 0x03, 0x09, 0x08, 0x16,
          0x0A, 0x3F, 0x78, 0x4C, 0x09,
          0x0A, 0x08, 0x16, 0x1A, 0x0F },
        15
    },
    { ILI9488_NEGATIVE_GAMMA_CTL,
        { 0x00, 0x16, 0x19, 0x03, 0x0F,
          0x05, 0x32, 0x45, 0x46, 0x04,
          0x0E, 0x0D, 0x35, 0x37, 0x0F},
        15
    },
    { ILI9488_POWER_CTL_ONE, { 0x17, 0x15 }, 2 },
    { ILI9488_POWER_CTL_TWO, { 0x41 }, 1 },
    { ILI9488_POWER_CTL_THREE, { 0x00, 0x12, 0x80 }, 3 },
    { LCD_CMD_MADCTL, { LCD_CMD_MX_BIT | LCD_CMD_BGR_BIT }, 1 },
    { LCD_CMD_COLMOD, { ILI9488_COLOR_MODE_18BIT }, 1},
    { ILI9488_INTRFC_MODE_CTL, { ILI9488_INTERFACE_MODE_USE_SDO }, 1 },
    { ILI9488_FRAME_RATE_NORMAL_CTL, { ILI9488_FRAME_RATE_60HZ }, 1 },
	{ ILI9488_INVERSION_CTL, { 0x02 }, 1 },
	{ ILI9488_FUNCTION_CTL, { 0x02, 0x02, 0x3B }, 3},
    { ILI9488_ENTRY_MODE_CTL, { 0xC6 }, 1 },
    { ILI9488_ADJUST_CTL_THREE, { 0xA9, 0x51, 0x2C, 0x02 }, 4 },
    { LCD_CMD_SLPOUT, { 0x00 }, ILI9488_INIT_DELAY_FLAG },
    { LCD_CMD_DISPON, { 0x00 }, ILI9488_INIT_DELAY_FLAG },

    { LCD_CMD_NOP, { 0 }, ILI9488_INIT_DONE_FLAG },
};

static esp_err_t panel_ili9488_del(esp_lcd_panel_t *panel)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);

    if (ili9488->reset_gpio_num >= 0)
    {
        gpio_reset_pin(ili9488->reset_gpio_num);
    }
    ESP_LOGI(TAG, "del ili9488 panel @%p", ili9488);
    free(ili9488);
    return ESP_OK;
}

static esp_err_t panel_ili9488_reset(esp_lcd_panel_t *panel)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9488->io;

    if (ili9488->reset_gpio_num >= 0)
    {
        ESP_LOGI(TAG, "Setting GPIO:%d to %d", ili9488->reset_gpio_num,
                 ili9488->reset_level);
        // perform hardware reset
        gpio_set_level(ili9488->reset_gpio_num, ili9488->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_LOGI(TAG, "Setting GPIO:%d to %d", ili9488->reset_gpio_num,
                 !ili9488->reset_level);
        gpio_set_level(ili9488->reset_gpio_num, !ili9488->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    else
    {
        ESP_LOGI(TAG, "Sending SW_RESET to display");
        esp_lcd_panel_io_tx_param(io, LCD_CMD_SWRESET, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    return ESP_OK;
}

static esp_err_t panel_ili9488_init(esp_lcd_panel_t *panel)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9488->io;

    ESP_LOGI(TAG, "Initializing ILI9488");
    esp_lcd_panel_io_tx_param(io, LCD_CMD_SLPOUT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL, &ili9488->madctl_val, 1);
    esp_lcd_panel_io_tx_param(io, LCD_CMD_COLMOD, &ili9488->colmod_cal, 1);
    int cmd = 0;
    while ( ili9488_init[cmd].data_bytes != ILI9488_INIT_DONE_FLAG )
    {
        ESP_LOGD(TAG, "Sending CMD: %02x, len: %d", ili9488_init[cmd].cmd,
                 ili9488_init[cmd].data_bytes & ILI9488_INIT_LENGTH_MASK);
        esp_lcd_panel_io_tx_param(
            io, ili9488_init[cmd].cmd, ili9488_init[cmd].data,
            ili9488_init[cmd].data_bytes & ILI9488_INIT_LENGTH_MASK);
        if (ili9488_init[cmd].data_bytes & ILI9488_INIT_DELAY_FLAG)
        {
            ESP_LOGI(TAG, "Delaying 100ms");
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        cmd++;
    }

    ESP_LOGI(TAG, "Initialization complete");

    return ESP_OK;
}

static esp_err_t panel_ili9488_draw_bitmap(
    esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end,
    const void *color_data)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) &&
            "starting position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = ili9488->io;

    x_start += ili9488->x_gap;
    x_end += ili9488->x_gap;
    y_start += ili9488->y_gap;
    y_end += ili9488->y_gap;

    size_t len = (x_end - x_start) * (y_end - y_start);

    esp_lcd_panel_io_tx_param(io, LCD_CMD_CASET, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    esp_lcd_panel_io_tx_param(io, LCD_CMD_RASET, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);

    if (ili9488->colmod_cal == ILI9488_COLOR_MODE_18BIT)
    {
        uint32_t *buffer_32bit = (uint32_t *) color_data;
        uint8_t *mybuf = NULL;
        while (mybuf == NULL)
        {
            mybuf = (uint8_t *) heap_caps_malloc(len * 3, MALLOC_CAP_DMA);
            if (mybuf == NULL)
            {
                ESP_LOGW(TAG, "Could not allocate enough DMA memory!");
            }
        }

        uint32_t pixel_color = 0;
        uint32_t pixel_index = 0;

        for (uint32_t i = 0; i < len; i++) {
            pixel_color = buffer_32bit[i];
            mybuf[pixel_index++] = (uint8_t) (((pixel_color & 0xF800) >> 8) |
                                              ((pixel_color & 0x8000) >> 13));
            mybuf[pixel_index++] = (uint8_t) ((pixel_color & 0x07E0) >> 3);
            mybuf[pixel_index++] = (uint8_t) (((pixel_color & 0x001F) << 3) |
                                              ((pixel_color & 0x0010) >> 2));
        }
        esp_lcd_panel_io_tx_color(io, LCD_CMD_RAMWR, mybuf, len * 3);
        heap_caps_free(mybuf);
    }
    else
    {
        esp_lcd_panel_io_tx_color(io, LCD_CMD_RAMWR, color_data, len * 2);
    }

    return ESP_OK;
}

static esp_err_t panel_ili9488_invert_color(
    esp_lcd_panel_t *panel, bool invert_color_data)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9488->io;
    int command = LCD_CMD_INVOFF;
    if (invert_color_data)
    {
        command = LCD_CMD_INVON;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}

static esp_err_t panel_ili9488_mirror(
    esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9488->io;
    if (mirror_x)
    {
        ili9488->madctl_val |= LCD_CMD_MX_BIT;
    }
    else
    {
        ili9488->madctl_val &= ~LCD_CMD_MX_BIT;
    }
    if (mirror_y)
    {
        ili9488->madctl_val |= LCD_CMD_MY_BIT;
    }
    else
    {
        ili9488->madctl_val &= ~LCD_CMD_MY_BIT;
    }
    esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL, &ili9488->madctl_val, 1);
    return ESP_OK;
}

static esp_err_t panel_ili9488_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9488->io;
    if (swap_axes)
    {
        ili9488->madctl_val |= LCD_CMD_MV_BIT;
    }
    else
    {
        ili9488->madctl_val &= ~LCD_CMD_MV_BIT;
    }
    esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL, &ili9488->madctl_val, 1);
    return ESP_OK;
}

static esp_err_t panel_ili9488_set_gap(
    esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    ili9488->x_gap = x_gap;
    ili9488->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_ili9488_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    ili9488_panel_t *ili9488 = __containerof(panel, ili9488_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9488->io;
    int command = 0;

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    on_off = !on_off;
#endif

    if (on_off)
    {
        command = LCD_CMD_DISPON;
    }
    else
    {
        command = LCD_CMD_DISPOFF;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}

esp_err_t esp_lcd_new_panel_ili9488(
    const esp_lcd_panel_io_handle_t io,
    const esp_lcd_panel_dev_config_t *panel_dev_config,
    esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    ili9488_panel_t *ili9488 = NULL;
    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG,
                      err, TAG, "invalid argument");
    ili9488 = (ili9488_panel_t *)(calloc(1, sizeof(ili9488_panel_t)));
    ESP_GOTO_ON_FALSE(ili9488, ESP_ERR_NO_MEM, err, TAG, "no mem for ili9488 panel");

    if (panel_dev_config->reset_gpio_num >= 0)
    {
        gpio_config_t cfg;
        memset(&cfg, 0, sizeof(gpio_config_t));
        esp_rom_gpio_pad_select_gpio(panel_dev_config->reset_gpio_num);
        cfg.pin_bit_mask = BIT64(panel_dev_config->reset_gpio_num);
        cfg.mode = GPIO_MODE_OUTPUT;
        ESP_GOTO_ON_ERROR(gpio_config(&cfg), err, TAG,
                          "configure GPIO for RESET line failed");
    }

    ili9488->madctl_val = LCD_CMD_MX_BIT | LCD_CMD_BGR_BIT;
    if (panel_dev_config->color_space == ESP_LCD_COLOR_SPACE_RGB)
    {
        ESP_LOGI(TAG, "Configuring for RGB color order");
        ili9488->madctl_val &= ~LCD_CMD_BGR_BIT;
    }
    else
    {
        ESP_LOGI(TAG, "Configuring for BGR color order");
    }

    if (panel_dev_config->bits_per_pixel == 16)
    {
        ili9488->colmod_cal = ILI9488_COLOR_MODE_16BIT;
        ESP_LOGI(TAG, "Configuring for 16-bit colors");
    }
    else
    {
        ili9488->colmod_cal = ILI9488_COLOR_MODE_18BIT;
        ESP_LOGI(TAG, "Configuring for 18-bit colors");
    }

    ili9488->io = io;
    ili9488->reset_gpio_num = panel_dev_config->reset_gpio_num;
    ili9488->reset_level = panel_dev_config->flags.reset_active_high;
    ili9488->base.del = panel_ili9488_del;
    ili9488->base.reset = panel_ili9488_reset;
    ili9488->base.init = panel_ili9488_init;
    ili9488->base.draw_bitmap = panel_ili9488_draw_bitmap;
    ili9488->base.invert_color = panel_ili9488_invert_color;
    ili9488->base.set_gap = panel_ili9488_set_gap;
    ili9488->base.mirror = panel_ili9488_mirror;
    ili9488->base.swap_xy = panel_ili9488_swap_xy;
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    ili9488->base.disp_off = panel_ili9488_disp_on_off;
#else
    ili9488->base.disp_on_off = panel_ili9488_disp_on_off;
#endif
    *ret_panel = &(ili9488->base);
    ESP_LOGI(TAG, "new ili9488 panel @%p", ili9488);

    return ESP_OK;

err:
    if (ili9488)
    {
        if (panel_dev_config->reset_gpio_num >= 0)
        {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(ili9488);
    }
    return ret;
}
