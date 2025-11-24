#
# SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.20)

set(requires "driver freertos esp_lcd esp_timer")

if(IDF_VERSION_MAJOR GREATER_EQUAL 6)
    list(APPEND requires "esp_driver_ledc")
endif()

idf_component_register(SRCS "esp_lcd_ili9488.c"
                       INCLUDE_DIRS "include"
                       REQUIRES ${requires})
