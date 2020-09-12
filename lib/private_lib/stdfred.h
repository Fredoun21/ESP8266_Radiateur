#ifndef STDFRED_H
#define STDFRED_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


void setup_wifi(const char *local_ip, const char *gateway, const char *subnet, const char *ssid, const char *password);
void setserverMQTT(PubSubClient &clientMQTT, const char *ip, const char *port);
void reconnect(PubSubClient &clientMQTT, const char *id, const char *topic, const char *user, const char *password);
void sendMqttToDomoticz(PubSubClient &clientMQTT, int idx, String svalue, const char *topic);
void askMqttToDomoticz(PubSubClient &clientMQTT, int idx, String svalue, const char *topic);

#endif //STDFRED_H