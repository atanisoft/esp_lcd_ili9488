#
# SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
#
# SPDX-License-Identifier: MIT
#

idf_component_register(SRCS main.c
                       REQUIRES driver freertos esp_lcd esp_timer)
