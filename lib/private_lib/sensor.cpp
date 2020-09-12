#include "sensor.h"

/*
valeurDS18B
Renvoie la température du DS18B20 en float
*/
float valeurDS18B20(DallasTemperature sensor)
{

    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    // Serial.print("Requesting temperatures...");
    sensor.requestTemperatures(); // Send the command to get temperatures
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    float tempC = sensor.getTempCByIndex(0);

    // Check if reading was successful
    if (tempC != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature mesuree: ");
        Serial.println(tempC);
    }
    else
    {
        Serial.println("Error: Pas de donnees de temperature disponible");
    }
    return tempC;
}

/*
 Effectue plusieurs lecture et calcule la moyenne pour pondérer
 la valeur obtenue.
*/
float valeurACS712(int pin)
{
    int valeur;
    float moyenne = 0;

    int nbr_lectures = 50;
    for (int i = 0; i < nbr_lectures; i++)
    {
        valeur = analogRead(pin);
        moyenne = moyenne + float(valeur);
    }
    moyenne = moyenne / float(nbr_lectures);
    Serial.print("Entree ADC: ");
    Serial.println(String(moyenne));

    float amplitude_courant = abs(40 * ((float)moyenne / 1024) - 20);

    Serial.print("\n Courant = ");
    Serial.print(amplitude_courant, 2);
    Serial.println("A");

    return amplitude_courant;
}