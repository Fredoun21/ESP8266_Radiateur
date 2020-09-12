#ifndef DOMOTICZCONFIG_H
#define DOMOTICZCONFIG_H

/*
CONFIGURATION DOMOTICZ
*/
#undef ESP50  // ESP test
#define ESP53 // ESP SDB
#undef ESP54  // ESP Cuisine
#undef ESP55  // ESP MaxAux
#undef ESP56  // ESP Romeo
#undef ESP57  // ESP Parent

#ifdef ESP50
// Parametres du dispositif dans Domoticz
#define NAMEID "ESP50" // Nom du dispositif
#define LOCAL_IP "192.168.1.130" //Adresse IP du radiateur dans congif Domoticz
#define IDXDomoticz 6            //Numéro IDX du radiateur dans congif Domoticz
#define IDXDS18B20 11            //Numéro IDX de la sonde de T° dans congif Domoticz
#define IDXACS712 19             //Numéro IDX de la mesure de courant dans congif Domoticz
#define MQTT_ID "ESP50Client"    //Nom du client sur MQTT
#define WM_NAME "ConnectESP50" //Configuration WifiManager
#endif

#ifdef ESP53
// Parametres du dispositif dans Domoticz
#define NAMEID "SdB_Radiateur"        // Nom du dispositif
#define LOCAL_IP "192.168.1.53"       //Adresse IP du radiateur dans congif Domoticz
#define IDXDomoticz 7                 //Numéro IDX du radiateur dans congif Domoticz
#define IDXDS18B20 17                 //Numéro IDX de la sonde de T° dans congif Domoticz
#define IDXACS712 16                  //Numéro IDX de la mesure de courant dans congif Domoticz
#define MQTT_ID "ESP53_SDB_RADIATEUR" //Nom du client sur MQTT
#define WM_NAME "ConnectRadiateurSDB" //Configuration WifiManager
#endif

#ifdef ESP54
// Parametres du dispositif dans Domoticz
#define NAMEID "Cuisine_Radiateur"        // Nom du dispositif
#define LOCAL_IP "192.168.1.54"           //Adresse IP du radiateur dans congif Domoticz
#define IDXDomoticz 8                     //Numéro IDX du radiateur dans congif Domoticz
#define IDXDS18B20 14                     //Numéro IDX de la sonde de T° dans congif Domoticz
#define IDXACS712 15                      //Numéro IDX de la mesure de courant dans congif Domoticz
#define MQTT_ID "ESP54_CUISINE_RADIATEUR" //Nom du client sur MQTT
#define WM_NAME "ConnectRadiateurCuisine" //Configuration WifiManager
#endif

#ifdef ESP55
// Parametres du dispositif dans Domoticz
#define NAMEID "MaxAux_Radiateur"        // Nom du dispositif
#define LOCAL_IP "192.168.1.55"          //Adresse IP du radiateur dans congif Domoticz
#define IDXDomoticz 20                   //Numéro IDX du radiateur dans congif Domoticz
#define IDXDS18B20 21                    //Numéro IDX de la sonde de T° dans congif Domoticz
#define IDXACS712 22                     //Numéro IDX de la mesure de courant dans congif Domoticz
#define MQTT_ID "ESP55_MAXAUX_RADIATEUR" //Nom du client sur MQTT
#define WM_NAME "ConnectRadiateurMaxAux" //Configuration WifiManager
#endif

#ifdef ESP56
// Parametres du dispositif dans Domoticz
#define NAMEID "Romeo_Radiateur"        // Nom du dispositif
#define LOCAL_IP "192.168.1.56"         //Adresse IP du radiateur dans congif Domoticz
#define IDXDomoticz 23                  //Numéro IDX du radiateur dans congif Domoticz
#define IDXDS18B20 24                   //Numéro IDX de la sonde de T° dans congif Domoticz
#define IDXACS712 25                    //Numéro IDX de la mesure de courant dans congif Domoticz
#define MQTT_ID "ESP56_ROMEO_RADIATEUR" //Nom du client sur MQTT
#define WM_NAME "ConnectRadiateurRomeo" //Configuration WifiManager
#endif

#ifdef ESP57
// Parametres du dispositif dans Domoticz
#define NAMEID "Parent_Radiateur"        // Nom du dispositif
#define LOCAL_IP "192.168.1.57"          //Adresse IP du radiateur dans congif Domoticz
#define IDXDomoticz 26                   //Numéro IDX du radiateur dans congif Domoticz
#define IDXDS18B20 27                    //Numéro IDX de la sonde de T° dans congif Domoticz
#define IDXACS712 28                     //Numéro IDX de la mesure de courant dans congif Domoticz
#define MQTT_ID "ESP57_PARENT_RADIATEUR" //Nom du client sur MQTT
#define WM_NAME "ConnectRadiateurParent" //Configuration WifiManager
#endif

#endif //DOMOTICZCONFIG_H