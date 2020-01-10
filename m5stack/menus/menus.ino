#include <M5Stack.h>

#define MyRED 0xe8e4
#define MyGREEN 0x2589
#define MyBLUE 0x51d

uint8_t currentMenu = 0;
uint8_t numMenu = 2;

void setup() {
  M5.begin();
  M5.Lcd.setBrightness(100);
  M5.Lcd.setTextSize(3);
}

void loop() {
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
