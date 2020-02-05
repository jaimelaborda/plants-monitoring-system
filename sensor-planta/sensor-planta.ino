/***************************************************
  Plant monitoring system v1
  by Jaime Laborda & Alberto Diaz
  Sistemas Embebidos - MUISE
 ****************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include "DHTesp.h"
#include "config.h" //Credentials are store here

#define DHTPIN 14 //D5
#define DHTTYPE DHTesp::DHT11

//Calibration soil sensor
#define SOIL_HUM_100 100
#define SOIL_HUM_0 430

// Update these with values suitable for your network.

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

DHTesp dht;
BH1750 lightMeter;

float humidity;
float temperature;
float lux;
int soil_humidity;

void readSensors(){

  Serial.println("Reading sensors:");
  Serial.println("Temp  RH   Lux   Soil");
  temperature = dht.getTemperature(); //in Celsius
  humidity = dht.getHumidity(); //getHumidity
  lux = lightMeter.readLightLevel();
  soil_humidity = map(analogRead(0), SOIL_HUM_0, SOIL_HUM_100 , 0, 100);

  Serial.print(temperature);
  Serial.print(" | ");
  Serial.print(humidity);
  Serial.print(" | ");
  Serial.print(lux);
  Serial.print(" | ");
  Serial.println(soil_humidity);
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, LOW);

  Serial.begin(115200);
  Wire.begin();
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);


  // T & RH Sensor
  dht.setup(DHTPIN, DHTTYPE); // Connect DHT sensor to GPIO 17
  // Light sensor
  lightMeter.begin();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    toggleLed();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp8266_planta", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void toggleLed(){
  digitalWrite(BUILTIN_LED, LOW);
  delay(10);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(10);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    //Leo los sensores
    readSensors();

    //Publico
    char temperature_str[10]; 
    char humidity_str[10]; 
    char lux_str[10];
    char soil_humidity_str[10];
    
    sprintf(temperature_str, "%.2f", temperature); //Convierto a string temperatura
    sprintf(humidity_str, "%.2f", humidity); //Convierto a string humedad
    sprintf(lux_str, "%.2f", lux); //Convierto a string lux
    sprintf(soil_humidity_str, "%d", soil_humidity);
    
    client.publish(mqtt_topic_temperature, temperature_str);
    client.publish(mqtt_topic_humidity, humidity_str);
    client.publish(mqtt_topic_lux, lux_str);
    client.publish(mqtt_topic_soil_humidity, soil_humidity_str);

    Serial.println("Message published!\n");

    toggleLed();
  }
}
