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
#include"mqtt_client.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

using namespace std;

static std::vector<MeterInfo> infos;
extern "C" {

void esp32meter(const char *name, const char *driver, const char *id, const char *key)
{
  //Configure meter
  MeterInfo info;
  if (info.parse(name, driver, id, key)) {
    ESP_LOGI("WMBUS", "%s %s %s %s %s", __FUNCTION__, name, driver, id, key);
    infos.push_back(info);
  }
  else {
    ESP_LOGW("WMBUS", "%s %s %s %s %s", __FUNCTION__, name, driver, id, key);
  }
}
static RingbufHandle_t ring;
static esp_mqtt_client_handle_t client;
static TaskHandle_t wmbus = NULL;

void esp32frame(void *ring)
{
  traceEnabled(true);
  debugEnabled(true);
  verboseEnabled(true);
  logTelegramsEnabled(true);
  //Create meters
  std::vector<shared_ptr<Meter>> meters;
  for (auto &info : infos) {
    shared_ptr<Meter> meter = createMeter(&info);
    meters.push_back(meter);
  }
  for (;;) {
    size_t   len;
    uint8_t *buf;
    //Wait for data
    buf = (uint8_t *)xRingbufferReceive(ring, &len, portMAX_DELAY);
    if (buf == NULL) {
      ESP_LOGW("WMBUS", "%s timeout", __FUNCTION__);
      continue;
    }
    if (len < 1) {
      ESP_LOGW("WMBUS", "%s empty", __FUNCTION__);
      continue;
    }
    //Strip crc, rssi, status
    double rssi = (buf[len-2]<0x80) ? double(buf[len-2])/2.0-74 : double(buf[len-2]-256)/2.0-74;
    buf[0] -= 2;
    vector<uchar> frame(buf, buf+1+buf[0]);
    vRingbufferReturnItem(ring, buf);
    //Loop over registered meters
    for (auto &meter : meters) {
      vector<Address> id;
      bool match = false;
      Telegram telegram;
      AboutTelegram about(meter->name(), rssi, FrameType::WMBUS);
      bool handle = meter->handleTelegram(about, frame, false, &id, &match, &telegram);
      telegram.explainParse("("+meter->name()+")", 0);
      if (!handle || !match) continue;
      //Print details
      string hr, fields, json;
      vector<string> envs, more_json, selected_fields;
      meter->printMeter(&telegram, &hr, &fields, '\t', &json, &envs, &more_json, &selected_fields, false);
      char topic[0x100];
      snprintf(topic, sizeof(topic), "wmbusmeters/%s/%s/data", meter->driverName().str().c_str(), meter->name().c_str());
      esp_mqtt_client_publish(client, topic, json.c_str(), 0, 0, 1);
      printf("%s %s\n", topic, json.c_str());
    }
  }
  //Never reached
  vTaskDelete(NULL);
}  
RingbufHandle_t esp32start(esp_mqtt_client_handle_t _client)
{
  client = _client;
  ring = xRingbufferCreate(1028, RINGBUF_TYPE_NOSPLIT);
  xTaskCreate(&esp32frame, "WMBUS", 1024*16, ring, tskIDLE_PRIORITY, &wmbus);
  return ring;
}

}
