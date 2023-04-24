/*
  Titre      : TTN
  Auteur     : Duvalier Tsagmo
  Date       : 24/04/2023
  Description: envoyer les donnees du button et du rotention sensor
  Version    : 0.0.1
*/

#include <Arduino.h>
#include <MKRWAN.h>

// Initialisation du modem LoRa
LoRaModem modem;

// Identifiants de l'application et clé d'authentification
String appEui = "0000000000000000";
String appKey = "7CA3FC586104C28961D1899743304B36";

// Broches utilisées pour le bouton et le capteur de rotation
int buttonPin = 2;
int rotationPin = A0;

void setup()
{
    // Initialisation de la communication série et attente de la connexion
    Serial.begin(9600);
    while (!Serial)
        ;

    // Configuration de la broche du bouton en entrée avec pull-up
    pinMode(buttonPin, INPUT_PULLUP);

    // Initialisation du modem LoRa et vérification de la connexion
    if (!modem.begin(US915))
    {
        Serial.println("Failed to start module");
        while (1)
            ;
    };

    // Connexion au réseau LoRaWAN avec la méthode d'authentification OTAA
    int connected = modem.joinOTAA(appEui, appKey);
    while (!connected)
    {
        Serial.println("Retry...");
        connected = modem.joinOTAA(appEui, appKey);
        if (!modem.joinOTAA(appEui, appKey))
        {
            Serial.println("Fail");
        }
        else
        {
            break;
        }
    };
}

void loop()
{
    // Lecture de l'état du bouton et de la valeur du capteur de rotation
    int buttonState = digitalRead(buttonPin);
    int rotationValue = analogRead(rotationPin);

    // Construction de la charge utile avec les données lues
    String payload = "button=" + String(buttonState) + "&rotation=" + String(rotationValue);

    // Envoi de la charge utile via le réseau LoRaWAN
    Serial.println();
    int err;
    modem.setPort(1);
    modem.beginPacket();
    modem.print("button:");
    modem.print(String(buttonState));

    modem.print(",Rotation:");
    modem.print(String(rotationValue));
    err = modem.endPacket(true);
    Serial.print("Message state: ");
    Serial.println(err);

    // Gestion des erreurs d'envoi
    switch (err)
    {
    case -1:
        Serial.println("Timeout");
        break;

    case 1:
        Serial.println("Message sent !");
        break;
    default:
        Serial.print("Error while sending message with code: ");
        Serial.println(err);
        break;
    }

    // Attente avant la prochaine itération de la boucle
    Serial.println();
    delay(5000);
}
