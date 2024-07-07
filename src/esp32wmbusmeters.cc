/*
 Copyright (C) 2018-2024 Fredrik Öhrström (gpl-3.0-or-later)

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

#include<string.h>
#include<set>

#include"meters.h"
#include"wmbus.h"

#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"freertos/ringbuf.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

using namespace std;

static MeterInfo mi;
extern "C" {

void esp32meter(const char *name, const char *driver, const char *id, const char *key)
{
  if (mi.parse(name, driver, id, key)) {
    ESP_LOGW("WMBUS", "%-20s %s %s %s %s", __FUNCTION__, name, driver, id, key);
  }
  else {
    ESP_LOGE("WMBUS", "%-20s %s %s %s %s", __FUNCTION__, name, driver, id, key);
  }
}
  
void esp32frame(void *ring)
{
  traceEnabled(true);
  logTelegramsEnabled(true);
  
  shared_ptr<Meter> meter = createMeter(&mi);
  Telegram telegram;
  for (;;) {
    size_t   len;
    uint8_t *buf;
    int      rssi;
    //Wait for data
    buf = (uint8_t *)xRingbufferReceive(ring, &len, portMAX_DELAY);
    if (buf == NULL) {
      ESP_LOGD("WMBUS", "%-20s timeout", __FUNCTION__);
      continue;
    }
    printf("%-20s 0x%02x bytes ","CCxxx0_RXFIFO", len);
    for (int i=0;i<len;i++) {
      printf("%02x",buf[i]);
    }
    printf("\n");

    rssi = buf[len-2];
    vector<uchar> frame(buf, buf+len);
    vRingbufferReturnItem(ring, buf);

    vector<Address> id;
    bool match;
    AboutTelegram about("cc1101", rssi, FrameType::WMBUS);
    meter->handleTelegram(about, frame, false, &id, &match, &telegram);
    ESP_LOGI("WMBUS","handle ok");

    string hr, fields, json;
    vector<string> envs, more_json, selected_fields;
    meter->printMeter(&telegram, &hr, &fields, '\t', &json, &envs, &more_json, &selected_fields, true);
    ESP_LOGI("WMBUS","json: %s",json.c_str());    
    for (auto json: more_json) {
      ESP_LOGI("WMBUS","json: %s",json.c_str());
    }
  }

  vTaskDelete(NULL);
}

}
