#include "WiFi.h"
#include "AsyncUDP.h"
#include <PubSubClient.h>
#include <M5Stack.h>
#include "config.h"

#define dataLength 5

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int newData;

int temperatureData[dataLength];
int humidityData[dataLength];
int luxData[dataLength];

void addValue(int myArray[], int newValue, int arrayLength) {
  //Desplaza el array hacia la derecha y añade el nuevo valor
  for(int i = arrayLength-1; i > 0; i--){
      myArray[i] = myArray[i-1];
    }
  myArray[0] = newValue;
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  newData = atoi((char *)payload);
  /*Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" Dato: ");
  Serial.println(newData);*/
  String aTopic = (char*)topic;
  if (aTopic == topicTemperature) {
    Serial.println("TEMPERATURA");
    addValue(temperatureData, newData, dataLength);
    for(int i=0; i<dataLength; i++) {
      Serial.print(temperatureData[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  else if (aTopic == topicHumidity) {
    Serial.println("HUMEDAD");
  }
  else if (aTopic == topicLight) {
    Serial.println("LUZ");
  }
  else Serial.println("OTRO");
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
