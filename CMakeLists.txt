#
# SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.20)

idf_component_register(SRCS "esp_lcd_ili9488.c"
                       INCLUDE_DIRS "include"
                       REQUIRES "driver esp_lcd")