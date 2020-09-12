#ifndef WIFIMANAGERFRED_H
#define WIFIMANAGERFRED_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

#include <LittleFS.h>
#include <ArduinoJson.h>

void setupWifiManager(char apName[40], char apPassword[40], char static_ip[16], char static_gw[16], char static_sn[16], char mqtt_server[40], char mqtt_port[6], char blynk_token[33]);
void saveConfigCallback();

#endif //WIFIMANAGERFRED_H