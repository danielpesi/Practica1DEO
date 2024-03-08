#include <SPI.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <DHT_U.h>
#include <UbiConstants.h>
#include <UbiTypes.h>
#include <UbidotsEsp32Mqtt.h>
#define DHTPIN 27
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-4A0XexFStypzV1WtTm4rfQydg0Y330";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "esp32";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "t"; // Put here your Variable label to which data  will be published
const char *HUMEDAD_VARIABLE_LABEL = "h"; 
const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;
uint8_t analogPin = 27; // Pin used to read data from GPIO34 ADC_CH6.

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}



void setup() {
  Serial.begin(115200); //init del dht
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
    Serial.println(F("DHTxx test!"));

    dht.begin();
  // Titulos
    tft.init();
    tft.fillScreen(TFT_NAVY);
    tft.drawString("Sensor DHT-11", 10, 15, 2);
    tft.drawString("Humedad: ", 10, 33, 2);
    tft.drawString("Temperatura: ", 10, 125, 2);
    
}



void loop() {
  // imprime en pantalla valores de temperatura y humedad
  delay(2000);
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if ((millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    float value = analogRead(analogPin);
    Serial.print(value);
    Serial.println();
    
  }
  ubidots.loop();
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    Serial.print(F("Humedad: "));
    Serial.print(h);
    Serial.print(F("% Temperatura: "));
    Serial.print(t);
    Serial.print(F(" C "));

    tft.drawString(String(h), 30, 60, 7);
    tft.drawString(String(t), 30, 150, 7);
    ubidots.add(VARIABLE_LABEL, t); // Insert your variable Labels and the value to be sent
    ubidots.add(HUMEDAD_VARIABLE_LABEL, h);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
    

}

