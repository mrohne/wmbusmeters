/*
 Copyright (C) 2023-2024 Fredrik Öhrström (gpl-3.0-or-later)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include"meters_common_implementation.h"

#include"driver_dynamic.h"

#include<string.h>

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

bool DriverDynamic::load(DriverInfo *di, const string &file_name, const char *content)
{
    ESP_LOGW("WMBUS","trying to load driver from %s or %p",file_name.c_str(),content);
    return false;
}
