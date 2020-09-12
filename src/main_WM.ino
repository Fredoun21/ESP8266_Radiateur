/*
  Projet d'apprentissage d'un objet connecté (IoT)  pour réaliser une sonde de température
  ESP8266 + DS12B20 + LED + MQTT + Home-Assistant
  Projets DIY (https://www.projetsdiy.fr) - Mai 2016
  Licence : MIT
*/

// Include the libraries we need
#include <Arduino.h>
#include <stdfred.h>
#include <wifimanagerfred.h>
#include <sensor.h>
#include <ESP8266FtpServer.h>
#include <Ticker.h>

#include "..\include\config.h"
#include "..\include\domoticzConfig.h"

/*
 PIN SETTINGS
 */
#define PIN_FILPILOTE_PLUS 12  // N° de Pin fil pilote
#define PIN_FILPILOTE_MOINS 13 // N° de Pin fil pilote
#define PIN_ACS712 A0          // Mesure de courant
#define PIN_ONE_WIRE_BUS 14    // Mesure de température

long i = 0;

/*
Création des objets
*/
// Server HTTP
ESP8266WebServer server(80);

// client MQTT
WiFiClient espClient;
PubSubClient clientMQTT(espClient);

// bus I2C
OneWire oneWire(PIN_ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

//set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
FtpServer ftpSrv;

// Création tache tempo pour mode confort 1 et 21
Ticker tickerSetHigh;
Ticker tickerSetLow;

/*
 * The setup function. We only start the sensors here
 */
void setup(void)
{
    Serial.begin(115200);

    pinMode(PIN_FILPILOTE_PLUS, OUTPUT);
    pinMode(PIN_FILPILOTE_MOINS, OUTPUT);

    // Positionne en mode Hors Gel à la mise sous tension
    digitalWrite(PIN_FILPILOTE_PLUS, HIGH);
    digitalWrite(PIN_FILPILOTE_MOINS, HIGH);

    // Start LE DS18b20
    DS18B20.begin();

    Serial.printf("\nID MQTT: %s", MQTT_ID);

    // Connexion au réseau wifi
    setupWifiManager(WM_NAME, WM_PASSWORD, LOCAL_IP, LOCAL_GATEWAY, LOCAL_SUBNET, MQTT_SERVER, MQTT_PORT, "");
    // setup_wifi(LOCAL_IP, LOCAL_GATEWAY, LOCAL_SUBNET, LOCAL_SSID, LOCAL_PASSWORD);
    delay(500);

    //Configuration de la connexion au serveur MQTT
    clientMQTT.setServer(MQTT_SERVER, atoi(MQTT_PORT));
    //La fonction de callback qui est executée à chaque réception de message
    clientMQTT.setCallback(callback);

    // On branche la fonction qui gère la premiere page / link to the function that manage launch page
    server.on("/index.html", handleRoot);

    // connexion OK, on demarre le server web // Start the server
    server.begin();
    Serial.println("Server started");

    if (SPIFFS.begin())
    {
        Serial.println("SPIFFS opened!");
        ftpSrv.begin("esp8266", "esp8266"); //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
    }
}

/*
 * Main function, get and show the temperature
 */
void loop(void)
{
    server.handleClient();

    //make sure in loop you call handleFTP()!!
    ftpSrv.handleFTP();

    // Connexion client MQTT
    if (!clientMQTT.connected())
    {
        reconnect(clientMQTT, MQTT_ID, TOPIC_DOMOTICZ_OUT, MQTT_USER, MQTT_PASSWORD);
    }
    clientMQTT.loop();

    unsigned long currentMillis = millis();
    String cmd = "";

    // ajout d'un delais de 60s apres chaque trame envoyés pour éviter d'envoyer
    // en permanence des informations à domoticz et de créer des interférences
    if ((currentMillis - previousMillis > watchdog) || firstLoop == LOW)
    {
        previousMillis = currentMillis;
        if (!firstLoop)
        { // Demande état d'un device
            askMqttToDomoticz(clientMQTT, IDXDomoticz, "getdeviceinfo", TOPIC_DOMOTICZ_IN);
            firstLoop = HIGH;
        }

        // Envoi MQTT température du DS18B20
        sendMqttToDomoticz(clientMQTT, IDXDS18B20, String(valeurDS18B20(DS18B20)), TOPIC_DOMOTICZ_IN);

        // Envoi MQTT mesure de courant du AC712
        sendMqttToDomoticz(clientMQTT, IDXACS712, String(valeurACS712(PIN_ACS712)), TOPIC_DOMOTICZ_IN);
    }
}

/*
callback
Déclenche les actions à la réception d'un message
topic -> nom du topic de réception des message (domoticz/out)
payload -> message reçu
length -> longueur message reçu
 */
void callback(char *topic, byte *payload, unsigned int length)
{
    DynamicJsonDocument jsonBuffer(MQTT_MAX_PACKET_SIZE);
    String messageReceived = "";

    // Affiche le topic entrant - display incoming Topic
    Serial.print("\nMessage arrived [");
    Serial.print(topic);
    Serial.println("] ");

    // decode payload message
    for (uint i = 0; i < length; i++)
    {
        messageReceived += ((char)payload[i]);
    }
    // display incoming message

    Serial.println("Message recu:");

#ifdef DEBUG
    Serial.printf("\nLongueur: %i\n", length);
    Serial.printf("\nTopic: %i\n", topic);
    Serial.print(messageReceived);
#endif

    // if domoticz message
    if (strcmp(topic, TOPIC_DOMOTICZ_OUT) == 0)
    {
        DeserializationError error = deserializeJson(jsonBuffer, messageReceived);
        if (error)
        {
            Serial.print(F("parsing Domoticz/out JSON Received Message failed with code: "));
            Serial.println(error.c_str());
            return;
        }

        int idx = jsonBuffer["idx"];
        int nvalue = jsonBuffer["nvalue"];
        float svalue = jsonBuffer["svalue"];
        float svalue1 = jsonBuffer["svalue1"];
        const char *name = jsonBuffer["name"];

#ifdef DEBUG
        Serial.printf("\nIDX: %i, name: %s, nVALUE: %i, sVALUE: %.2f, sVALUE1: %i\n", idx, name, nvalue, float(svalue), int(svalue1));
#endif
        if (idx == IDXDomoticz)
        {
            // MAJ de l'état du radiateur
            updateFilpilote(PIN_FILPILOTE_PLUS, PIN_FILPILOTE_MOINS, int(svalue1), idx);
        }
    }
}

// MAJ des sorties fil pilote en fonction du message Domoticz
void updateFilpilote(int pinP, int pinM, int svalue, int idx)
{
    String message;

    Serial.println(F("Mise a jour fil Pilote depuis DOMOTICZ: "));

    // Etat de 00 à 10: Radiateur sur Arrêt
    // Etat de 11 à 20: Radiateur sur Hors Gel
    // Etat de 21 à 30: Radiateur sur ECO
    // Etat de 31 à 40: Radiateur sur Confort 2 (T° Confort - 2°C)
    // Etat de 41 à 50: Radiateur sur Confort 1 (T° Confort - 1°C)
    // Etat de 51 à 100: Radiateur sur Confort

    if (0 <= svalue && svalue < 10)
    {
        digitalWrite(pinP, HIGH);
        digitalWrite(pinM, LOW);
        confortStopTask();
        Serial.println(F("Radiateur sur ARRET"));
        message = "Pin ";
        message += String(pinP);
        message += " = HIGH / Pin  ";
        message += String(pinM);
        message += " = LOW";
        Serial.println(message);
    }
    else if (10 <= svalue && svalue < 20)
    {
        digitalWrite(pinP, LOW);
        digitalWrite(pinM, HIGH);
        confortStopTask();
        Serial.println(F("Radiateur sur HORS GEL"));
        message = "Pin ";
        message += String(pinP);
        message += " = LOW / Pin ";
        message += String(pinM);
        message += " = HIGH";
        Serial.println(message);
    }
    else if (20 <= svalue && svalue < 30)
    {
        digitalWrite(pinP, HIGH);
        digitalWrite(pinM, HIGH);
        confortStopTask();
        Serial.println(F("Radiateur sur ECO"));
        message = "Pin ";
        message += String(pinP);
        message += " = HIGH / Pin ";
        message += String(pinM);
        message += " = HIGH";
        Serial.println(message);
    }
    else if (30 <= svalue && svalue < 40)
    {
        confortStopTask();
        confortSetPin(pinP, pinM, 7, 293);
        Serial.println(F("Radiateur sur CONFORT 2"));
        // Absence de courant pendant 293s, puis présence pendant 7s
    }
    else if (40 <= svalue && svalue < 50)
    {
        confortStopTask();
        confortSetPin(pinP, pinM, 3, 297);
        Serial.println(F("Radiateur sur CONFORT 1"));
        // Absence de courant pendant 297s, puis présence pendant 3s
    }
    else if (50 <= svalue && svalue <= 100)
    {
        digitalWrite(pinP, LOW);
        digitalWrite(pinM, LOW);
        confortStopTask();
        Serial.println(F("Radiateur sur CONFORT"));
        message = "Pin ";
        message += String(pinP);
        message += " = LOW / Pin ";
        message += String(pinM);
        message += " = LOW";
        Serial.println(message);
    }
    else
    {
        Serial.println(F("Bad Led Value !"));
    }
}

// Procédure MAJ sortie pour mode confort 1 & 2
void setPinConfort(int state)
{
    digitalWrite(PIN_FILPILOTE_PLUS, state);
    digitalWrite(PIN_FILPILOTE_MOINS, state);
    i++;
    //  Serial.print(F("Compteur: ")); Serial.println(i);
    //  Serial.print(F("STATE: ")); Serial.println(state);

    if (state == 1)
    {
        Serial.println(F("Tempo HIGH"));
    }
    else if (state == 0)
    {
        Serial.println(F("Tempo LOW"));
    }
}

// Lancement tempo pour mode confort 1 & 2
void confortSetPin(int aPinHigh, int aPinLow, float aTempoHigh, float aTempoLow)
{
    tickerSetHigh.attach(aTempoHigh, setPinConfort, 1);
    tickerSetLow.attach(aTempoLow, setPinConfort, 0);
}

// Arrêt tempo pour mode confort 1 & 2
void confortStopTask()
{
    tickerSetHigh.detach();
    tickerSetLow.detach();
}

void handleRoot()
{
    if (server.hasArg("LED"))
    {
    }
    else
    {
        server.send(200, "text/html", "index.html");
    }
}