#include "stdfred.h"
// #include "..\include\config.h"

/*
setup_wifi
Connexion du module ESP au wifi
local_ip -> adressi IP du module
gateway -> passerelle réseau
subnet -> masque de sous réseau
ssid -> nom du SSID pour la connexion Wifi
password -> mot de passe pour la connexion Wifi 
 */
void setup_wifi(const char *local_ip, const char *gateway, const char *subnet, const char *ssid, const char *password)
{
    //set static ip
    IPAddress _ip, _gw, _sn;
    _ip.fromString(local_ip);
    _gw.fromString(gateway);
    _sn.fromString(subnet);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println(F("Connection Wifi..."));
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.config(_ip, _gw, _sn);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println(F("\nWiFi connected"));
    Serial.print(F("SSID: "));
    Serial.println(WiFi.SSID());
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("Puissance du signal (RSSI):"));
    Serial.print(WiFi.RSSI());
    Serial.println(F(" dBm"));
}

/*
setserverMQTT
Setup du server MQTT 
clientMQTT -> instance PubSubClient
ip -> adresse du server MQTT 
port -> numéro du port MQTT (1883)
*/
void setserverMQTT(PubSubClient &clientMQTT, const char *ip, const char *port)
{
    IPAddress _ip;
    _ip.fromString(ip);

    clientMQTT.setServer(_ip, atoi(port));
}

/*
reconnect
Connexion server MQTT
id -> nom du client 
topic -> nom du topic pour envoyer les messages (domoticz/in)
*/
void reconnect(PubSubClient &clientMQTT, const char *id, const char *topic, const char *user, const char *password)
{
    //Boucle jusqu'à obtenir une reconnexion
    while (!clientMQTT.connected())
    {
        Serial.print("Connexion au serveur MQTT... Status= ");
        if (clientMQTT.connect(id, user, password))
        {
            Serial.println("OK");
            // suscribe to MQTT topics
            Serial.print("Subscribe to domoticz/out topic. Status= ");
            if (clientMQTT.subscribe(topic, 0))
                Serial.println("OK");
            else
            {
                Serial.print("KO, erreur: ");
                Serial.println(clientMQTT.state());
            };
        }
        else
        {
            Serial.print("KO, erreur : ");
            Serial.println(clientMQTT.state());
            Serial.println(" On attend 5 secondes avant de recommencer");
            delay(5000);
        }
    }
}

/*
sendMqttToDomoticz
Mise à jour valeur domoticz par messages MQTT 
idx -> adresse IDX domoticz du materiel
svalue -> donnée converti en String à envoyer 
topic -> topic pour envoyer les message(domoticz/in)
*/
void sendMqttToDomoticz(PubSubClient &clientMQTT, int idx, String svalue, const char *topic)
{
    char msgToPublish[MQTT_MAX_PACKET_SIZE];

    StaticJsonDocument<1024> doc;
    doc["idx"] = idx;
    doc["nvalue"] = 0;
    doc["svalue"] = svalue;
    serializeJson(doc, msgToPublish);
    Serial.print(msgToPublish);
    Serial.print(" Published to ");
    Serial.print(topic);
    Serial.print(". Status=");
    if (clientMQTT.publish(topic, msgToPublish))
        Serial.println("OK");
    else
        Serial.println("KO");
}

/*
askMqttToDomoticz
Mise à jour valeur domoticz par messages MQTT 
idx -> adresse IDX domoticz du materiel
svalue -> donnée converti en String à envoyer 
topic -> topic pour envoyer les message(domoticz/in)
*/
void askMqttToDomoticz(PubSubClient &clientMQTT, int idx, String svalue, const char *topic)
{
    char msgToPublish[MQTT_MAX_PACKET_SIZE];

    StaticJsonDocument<1024> doc;
    doc["idx"] = idx;
    doc["command"] = svalue;
    serializeJson(doc, msgToPublish);
    Serial.print(msgToPublish);
    Serial.print(" Published to ");
    Serial.print(topic);
    Serial.print(". Status=");
    if (clientMQTT.publish(topic, msgToPublish))
        Serial.println("OK");
    else
        Serial.println("KO");
}
