#include "WiFi.h"
#include "AsyncUDP.h"
#include <PubSubClient.h>
#include <M5Stack.h>
#include "config.h"

#define dataLength 10 //Longitud de los arrays que almacenan los datos

// Colores definidos para los gráficos
#define MyRED 0xe8e4
#define MyGREEN 0x2589
#define MyBLUE 0x51d
#define MyGRAY 0x7bef

// Variables para seleccionar menú
uint8_t currentMenu = 0; // Menú actual
uint8_t numMenu = 2; // Número total de menús
uint8_t currentSubMenu = 0;
uint8_t numSubMenu = 3;
boolean irrigation = false;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Variables para recibir y almacenar datos del servidor MQTT
int newData;
int temperatureData[dataLength];
int humidityData[dataLength];
int luxData[dataLength];
int soilData[dataLength];

void initArray(int arrayLength, int arrayToInit[]) {
  for (int i = 0; i < arrayLength; i++) {
    arrayToInit[i] = -255;
  }
}

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
    if (currentMenu == 0 || (currentMenu == 1 && currentSubMenu == 0)) drawMenu(currentMenu);
  }
  else if (aTopic == topicHumidity) {
    Serial.print("HUMEDAD: ");
    Serial.println(newData);
    addValue(humidityData, newData, dataLength);
    if (currentMenu == 0 || (currentMenu == 1 && currentSubMenu == 1)) drawMenu(currentMenu);
  }
  else if (aTopic == topicLight) {
    Serial.print("LUZ: ");
    Serial.println(newData);
    addValue(luxData, newData, dataLength);
    if (currentMenu == 0 || (currentMenu == 1 && currentSubMenu == 2)) drawMenu(currentMenu);
  }
  else if (aTopic == topicSoil) {
    Serial.print("SUELO: ");
    Serial.println(newData);
    addValue(soilData, newData, dataLength);
    if (currentMenu == 0 || (currentMenu == 1 && currentSubMenu == 3)) drawMenu(currentMenu);
  }
  else if (aTopic == topicRelayStatus) {
    Serial.print("ACTIVACION: ");
    Serial.println(newData);
    if (newData == 0) irrigation = false;
    else if (newData == 1) irrigation = true;
    drawMenu(currentMenu);
  }
}


void setup() {
    initArray(dataLength, temperatureData);
    initArray(dataLength, humidityData);
    initArray(dataLength, luxData);
    initArray(dataLength, soilData);

    M5.begin();
    M5.Lcd.setTextSize(3); //Tamaño del texto
    M5.Lcd.setBrightness(100);
    
    Serial.begin(115200);
    
    M5.Lcd.clear(WHITE);
    M5.Lcd.setTextColor(BLACK);
    
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10,20);
    M5.Lcd.print("Connecting to ");
    M5.Lcd.print(ssid);
    M5.Lcd.print("...");
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }else{
      M5.Lcd.setCursor(10,60);
      M5.Lcd.print("Connected!");
      delay(1000);
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
        client.subscribe(topicRelay);
        client.subscribe(topicRelayStatus);
      } else {
        delay(5000);
      }
  }
  // Cliente a la escucha
  client.loop();

  if (M5.BtnA.wasPressed()) {
    currentSubMenu = 0;
    if (currentMenu == 0) currentMenu = numMenu;
    else currentMenu = currentMenu - 1;
    drawMenu(currentMenu);
  }

  else if (M5.BtnC.wasPressed()) {
    currentSubMenu = 0;
    if (currentMenu == numMenu) currentMenu = 0;
    else currentMenu = currentMenu + 1;
    drawMenu(currentMenu);
  }

  else if (M5.BtnB.wasPressed() && currentMenu == 1) {
    if (currentSubMenu == numSubMenu) currentSubMenu = 0;
    else currentSubMenu = currentSubMenu + 1;
    drawMenu(currentMenu);
  }

  else if (M5.BtnB.wasPressed() && currentMenu == 2) {
    if (irrigation == false) startIrrigation(true);
    else startIrrigation(false);
    drawMenu(currentMenu);
  }
  
  M5.update();
}

void startIrrigation (bool toDo) {
  if (toDo == true) {
    irrigation = true;
    Serial.println(1);
    client.publish(topicRelay,"1");
  }
  else {
    irrigation = false;
    Serial.println(0);
    client.publish(topicRelay,"0");
  }
}

void drawMenu (uint8_t menu) {
  switch (menu) {
    case 0: drawMenu0(); break;
    case 1: drawMenu1(); break;
    case 2: drawMenu2(); break;
    default: drawMenu0(); break;
  }
}

// Menu para los valores actuales
void drawMenu0 (void) {
  M5.Lcd.clear(WHITE);
  M5.Lcd.setTextColor(BLACK);
  
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(25,20);
  M5.Lcd.print("ESTADO SENSORES");
  
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(20,100);
  M5.Lcd.print("TEMPERATURA:");
  M5.Lcd.print(temperatureData[0]);

  M5.Lcd.setCursor(20,160);
  M5.Lcd.print("HUMEDAD:");
  M5.Lcd.print(humidityData[0]);

  M5.Lcd.setCursor(200,100);
  M5.Lcd.print("LUZ:");
  M5.Lcd.print(luxData[0]);

  M5.Lcd.setCursor(200,160);
  M5.Lcd.print("SUELO:");
  M5.Lcd.print(soilData[0]);
}

// Menu para los graficos
void drawMenu1 (void) {
  M5.Lcd.clear(WHITE);
  switch (currentSubMenu) {
    case 0: Graph(dataLength,10,40,temperatureData, "TEMPERATURA"); break;
    case 1: Graph(dataLength,0,100,humidityData, "HUMEDAD"); break;
    case 2: Graph(dataLength,0,50000,luxData, "LUZ"); break;
    case 3: Graph(dataLength,0,100,soilData, "SUELO"); break;
    default: Graph(dataLength,0,50,temperatureData, "TEMPERATURA"); break;
  }
}

// Menu para activar riego
void drawMenu2 (void) {
  M5.Lcd.clear(WHITE);
  M5.Lcd.setTextColor(BLACK);
  
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(50,20);
  M5.Lcd.print("CONTROL RIEGO");
  M5.Lcd.setCursor(145,70);

  if (irrigation == true) {
    M5.Lcd.print("ON");
    M5.Lcd.fillCircle(160,190,30,BLUE);
    M5.Lcd.fillTriangle(160,100,132,180,188,180,BLUE);
  }
  else {
    M5.Lcd.print("OFF");
    M5.Lcd.fillCircle(160,190,30,BLACK);
    M5.Lcd.fillTriangle(160,100,132,180,188,180,BLACK);
  }
  
}

// Función para generar gráficas
void Graph (int xPoints, int yMin, int yMax, int graphData[], String title) {
  // xPoints: numero de datos a representar en el eje x
  // yMin: valor minimo del eje y
  // yMax: valor maximo del eje y
  // graphData: vector con los valores a representar (de longitud xPoints)
  // title: titulo del grafico

  int ox, oy, x, y;
  ox = 20;
  oy = 220;
  x = 300;
  y = 50;
  
  // Dibujar los ejes
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.drawLine(ox,oy,x,oy,BLACK); // Eje x
  M5.Lcd.setCursor(x,oy+5);
  M5.Lcd.printf("t");
  M5.Lcd.drawLine(ox,oy,ox,y,BLACK); // Eje y
  M5.Lcd.setCursor(ox-10,y-5);
  M5.Lcd.print("C");
  
  // Titulo
  M5.Lcd.setCursor(60,20);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.print(title);

  // Dibujar lineas eje X
  /*for (int i = 1; i<10; i++) {
    M5.Lcd.drawLine(ox+((x-ox)/10)*i,oy,ox+((x-ox)/10)*i,y,MyGRAY);
  }*/

  // Dibujar lineas eje Y
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setCursor(ox-15,oy);
  M5.Lcd.print(yMin);
  for (int i = 1; i<10; i++) {
    M5.Lcd.drawLine(ox,oy+((y-oy)/10)*i,x,oy+((y-oy)/10)*i,MyGRAY);
    M5.Lcd.setCursor(ox-15,oy+((y-oy)/10)*i-5);
    int valor = ((yMax-yMin)/10)*(i);
    if (title == "LUZ") {
      M5.Lcd.setCursor(ox-20,oy+((y-oy)/10)*i-5);
      valor = valor/1000;
      M5.Lcd.print(valor);
      M5.Lcd.print("k");
    }
    else {
      M5.Lcd.print(valor);
    }
  }

  // Dibujar los puntos
  for (int i = 1; i<=xPoints; i++) {
    if (graphData[xPoints-i] != -255) {
      int xCoord = map(xPoints-i,0,xPoints-1,x,ox);
      int yCoord = map(graphData[xPoints-i],yMin,yMax,oy,y);
      M5.Lcd.fillCircle(xCoord,yCoord,2,RED);
    }
  }

  // Dibujar los conectores
  for (int i = 0; i<xPoints-1; i++) {
    if (graphData[i+1]!= -255) {
      // Punto inicial
      int xCoord1 = map(i,0,xPoints-1,x,ox);
      int yCoord1 = map(graphData[i],yMin,yMax,oy,y);
      // Punto final
      int xCoord2 = map(i+1,0,xPoints-1,x,ox);
      int yCoord2 = map(graphData[i+1],yMin,yMax,oy,y);
      // Dibujar linea
      M5.Lcd.drawLine(xCoord1,yCoord1,xCoord2,yCoord2,RED);
    }
  }
}
