#include "wifimanagerfred.h"

// flag for saving data
bool shouldSaveConfig = false;

// callback notifying us of the need to save config
void saveConfigCallback()
{
    Serial.println("Should save config");
    shouldSaveConfig = true;
}

void setupWifiManager(char *apName[40], char *apPassword[40], char static_ip[16], char static_gw[16], char static_sn[16], char mqtt_server[40], char mqtt_port[6], char blynk_token[33])
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println();

    // clean FS, for testing
    LittleFS.format();

    // read configuration from FS json
    Serial.println("mounting FS...");

    if (LittleFS.begin())
    {
        Serial.println("mounted file system");
        if (LittleFS.exists("/config.json"))
        {
            // file exists, reading and loading
            Serial.println("reading config file");
            File configFile = LittleFS.open("/config.json", "r");
            if (configFile)
            {
                Serial.println("opened config file");
                size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument jsonBuffer(1024);
                DeserializationError error = deserializeJson(jsonBuffer, configFile);
                if (error)
                {
                    Serial.print(F("parsing Domoticz/out JSON Received Message failed with code: "));
                    Serial.println(error.c_str());
                    return;
                }
                else
                {
                    Serial.println("\nparsed json");

                    strcpy(mqtt_server, jsonBuffer["mqtt_server"]);
                    strcpy(mqtt_port, jsonBuffer["mqtt_port"]);
                    strcpy(blynk_token, jsonBuffer["blynk_token"]);

                    if (jsonBuffer["ip"])
                    {
                        Serial.println("setting custom ip from config");
                        strcpy(static_ip, jsonBuffer["ip"]);
                        strcpy(static_gw, jsonBuffer["gateway"]);
                        strcpy(static_sn, jsonBuffer["subnet"]);
                        Serial.println(static_ip);
                    }
                    else
                    {
                        Serial.println("no custom ip in config");
                    }
                }
            }
        }
    }
    else
    {
        Serial.println("failed to mount FS");
    }
    // end read
    Serial.println(static_ip);
    Serial.println(blynk_token);
    Serial.println(mqtt_server);

    // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
    WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 34);

    // WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    // set static ip
    IPAddress _ip, _gw, _sn;
    _ip.fromString(static_ip);
    _gw.fromString(static_gw);
    _sn.fromString(static_sn);

    wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);

    // add all your parameters here
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_blynk_token);

    // reset settings - for testing
    //  wifiManager.resetSettings();

    // set minimu quality of signal so it ignores AP's under that quality
    // defaults to 8%
    wifiManager.setMinimumSignalQuality();

    // sets timeout until configuration portal gets turned off
    // useful to make it all retry or go to sleep
    // in seconds
    wifiManager.setTimeout(120);

    // fetches ssid and pass and tries to connect
    // if it does not connect it starts an access point with the specified name
    // here  "AutoConnectAP"
    // and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect())
    // if (!wifiManager.autoConnect(apName, apPassword))
    {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        // reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
    }

    // if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    // read updated parameters
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    strcpy(blynk_token, custom_blynk_token.getValue());

    // save the custom parameters to FS
    if (shouldSaveConfig)
    {
        Serial.println("saving config");
        DynamicJsonDocument jsonBuffer(1024);

        jsonBuffer["mqtt_server"] = mqtt_server;
        jsonBuffer["mqtt_port"] = mqtt_port;
        jsonBuffer["blynk_token"] = blynk_token;

        jsonBuffer["ip"] = WiFi.localIP().toString();
        jsonBuffer["gateway"] = WiFi.gatewayIP().toString();
        jsonBuffer["subnet"] = WiFi.subnetMask().toString();

        File configFile = LittleFS.open("/config.json", "w");
        if (!configFile)
        {
            Serial.println("failed to open config file for writing");
        }

        serializeJson(jsonBuffer, Serial);
        if (serializeJson(jsonBuffer, configFile) == 0)
        {
            Serial.println(F("Failed to write to file"));
        }
        configFile.close();
        // end save
    }

    Serial.println("local ip");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.subnetMask());
}