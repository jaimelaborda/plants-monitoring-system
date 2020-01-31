#include <M5Stack.h>

#define MyRED 0xe8e4
#define MyGREEN 0x2589
#define MyBLACK 0x0000
#define MyWHITE 0xffff
#define MyGRAY 0x7bef

uint8_t currentMenu = 0;
uint8_t numMenu = 2;
uint8_t currentSubMenu = 0;
uint8_t numSubMenu = 4;

int datosTemp[10] = {0,5,10,15,20,25,30,35,40,45};
int datosHum[10] = {0,1,1,1,1,1,1,1,1,1};
int datosLux[10] = {5,5,5,5,5,5,5,5,50,5};
int datosSuelo[10] = {10,10,10,10,10,10,10,10,10,10};

void setup() {
  M5.begin();
  M5.Lcd.setBrightness(100);
  M5.Lcd.setTextSize(3);
}

void loop() {
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

  else if (M5.BtnB.wasPressed()) {
    if (currentSubMenu == numSubMenu) currentSubMenu = 0;
    else currentSubMenu = currentSubMenu + 1;
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
  M5.Lcd.clear(WHITE);
  switch (currentSubMenu) {
    case 0: Graph(10,0,50,datosTemp, "TEMPERATURA"); break;
    case 1: Graph(10,0,50,datosHum, "HUMEDAD"); break;
    case 2: Graph(10,0,50,datosLux, "LUZ"); break;
    case 3: Graph(10,0,50,datosSuelo, "SUELO"); break;
    default: Graph(10,0,50,datosTemp, "DEFAULT"); break;
  }
}

void Graph (int xPoints, int yMin, int yMax, int graphData[], String title) {
  // xPoints: numero de datos a representar en el eje x
  // yMin: Valor minimo del eje y
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
  M5.Lcd.setCursor(ox-10,y);
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
    M5.Lcd.print(valor);
  }

  // Dibujar los puntos
  for (int i = 1; i<=xPoints; i++) {
    int xCoord = ox+(x-ox)/(xPoints-1)*(xPoints-i);
    int yCoord = map(graphData[xPoints-i],yMin,yMax,oy,y);
    M5.Lcd.fillCircle(xCoord,yCoord,2,RED);
  }

  // Dibujar los conectores
  for (int i = 2; i<=xPoints; i++) {
    // Punto inicial
    int xCoord1 = ox+(x-ox)/(xPoints-1)*(xPoints-(i-1));
    int yCoord1 = map(graphData[xPoints-(i-1)],yMin,yMax,oy,y);
    // Punto final
    int xCoord2 = ox+(x-ox)/(xPoints-1)*(xPoints-i);
    int yCoord2 = map(graphData[xPoints-i],yMin,yMax,oy,y);
    
    M5.Lcd.drawLine(xCoord1,yCoord1,xCoord2,yCoord2,RED);
  }
  
}
