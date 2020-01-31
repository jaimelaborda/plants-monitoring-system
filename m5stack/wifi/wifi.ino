#include "WiFi.h"
#include "AsyncUDP.h"
#include <PubSubClient.h>
#include <M5Stack.h>
#include "config.h"

#define dataLength 5 //Longitud de los arrays que almacenan los datos

// Colores definidos para los gráficos
#define MyRED 0xe8e4
#define MyGREEN 0x2589
#define MyBLUE 0x51d

// Variables para seleccionar menú
uint8_t currentMenu = 0; // Menú actual
uint8_t numMenu = 2; // Número total de menús

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Variables para recibir y almacenar datos del servidor MQTT
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

// Se ejecuta cuando se recibe un nuevo valor desde el servidor
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
    M5.Lcd.setTextSize(3); //Tamaño del texto
    M5.Lcd.setBrightness(100);
    
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

    drawMenu(currentMenu);
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

  if (M5.BtnA.wasPressed()) {
    if (currentMenu == 0) currentMenu = numMenu;
    else currentMenu = currentMenu - 1;
    drawMenu(currentMenu);
  }

  else if (M5.BtnC.wasPressed()) {
    if (currentMenu == numMenu) currentMenu = 0;
    else currentMenu = currentMenu + 1;
    drawMenu(currentMenu);
  }  
  M5.update();
}

void drawMenu (uint8_t menu) {
  switch (menu) {
    case 0: drawMenu0(); break;
    case 1: drawMenu1(); break;
    case 2: drawMenu2(); break;
    default: drawMenu0(); break;
  }
}

void drawMenu0 (void) {
  M5.Lcd.clear(BLACK);
  M5.Lcd.fillCircle(160,120,80,MyRED);
  M5.Lcd.setCursor(116,215);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("MENU 0");
}

void drawMenu1 (void) {
  M5.Lcd.clear(BLACK);
  M5.Lcd.fillCircle(160,120,80,MyGREEN);
  M5.Lcd.setCursor(116,215);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("MENU 1");
}

void drawMenu2 (void) {
  M5.Lcd.clear(BLACK);
  M5.Lcd.fillCircle(160,120,80,MyBLUE);
  M5.Lcd.setCursor(116,215);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("MENU 2");
}

void Graph (int xPoints, yMin, yMax, graphData) {
  // xPoints: numero de datos a representar en el eje x
  // yMin: Valor minimo del eje y
  // yMax: valor maximo del eje y
  // graphData: vector con los valores a representar (de longitud xPoints)
  // title: titulo del grafico
  
}
