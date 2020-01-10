#include "WiFi.h"
#include "AsyncUDP.h"
#include <PubSubClient.h>
#include <M5Stack.h>
#include "config.h"

#define dataLength 5 //Longitud de los arrays que almacenan los datos

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int newData;
int temperatureData[dataLength];
int humidityData[dataLength];
int luxData[dataLength];
int soilData[dataLength];

// Desplaza el array a la derecha e inserta el nuevo valor
void addValue(int myArray[], int newValue, int arrayLength) {
  for(int i = arrayLength-1; i > 0; i--){
      myArray[i] = myArray[i-1];
    }
  myArray[0] = newValue;
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Guarda el nuevo dato en newData
  payload[length] = '\0';
  newData = atoi((char *)payload);
  // Guarda el topic en aTopic
  String aTopic = (char*)topic;
  
  if (aTopic == topicTemperature) {
    Serial.print("TEMPERATURA: ");
    Serial.println(newData);
    addValue(temperatureData, newData, dataLength);
  }
  else if (aTopic == topicHumidity) {
    Serial.print("HUMEDAD: ");
    Serial.println(newData);
    addValue(humidityData, newData, dataLength);
  }
  else if (aTopic == topicLight) {
    Serial.print("LUZ: ");
    Serial.println(newData);
    addValue(luxData, newData, dataLength);
  }
  else if (aTopic == topicSoil) {
    Serial.print("SUELO: ");
    Serial.println(newData);
    addValue(soilData, newData, dataLength);
  }
}


void setup()
{
    M5.begin();
    M5.Lcd.setTextSize(2); //Tamaño del texto
    
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }
    client.setServer(mqtt_server,mqtt_port);
    client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
      Serial.print("Connecting ...");
      if (client.connect("m5stack",mqtt_user,mqtt_password)) {
        Serial.println("connected");
        client.subscribe(topicTemperature);
        client.subscribe(topicHumidity);
        client.subscribe(topicLight);
        client.subscribe(topicSoil);
      } else {
        delay(5000);
      }
  }
  // Cliente a la escucha
  client.loop();
}
