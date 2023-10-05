#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Le con#include "Ball.hpp"

unsigned long currentTime = 0;


#define SCREEN_WIDTH 128 // Largeur de l'écran OLED, en pixels
#define SCREEN_HEIGHT 64 // Hauteur de l'écran OLED, en pixels

#define OLED_RESET     -1 // Numéro de la broche de réinitialisation (ou -1 si partageant la broche de réinitialisation de l'Arduino)

// Adresse pour le modèle du cours : 0x3C;
// Exécuter l'exemple i2c_scanner si cela ne fonctionne pas
#define SCREEN_ADDRESS 0x3C ///< Voir la fiche technique pour l'adresse;

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Nombre de flocons de neige dans l'exemple d'animation

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 
  0b00000111,0b11100000,
  0b00011000,0b01111000,
  0b00111000,0b01111100,
  0b01110000,0b00111110,
  0b01100000,0b00011110,
  0b10000111,0b10000001,
  0b10001111,0b11001101,
  0b11000111,0b10011111,
  0b11100000,0b00011111,
  0b11100000,0b00001101,
  0b11001111,0b11110001,
  0b01110010,0b01001110,
  0b00100010,0b01000100,
  0b00100000,0b00000100,
  0b00010000,0b00001000,
  0b00001111,0b11110000,
};

#define MARGIN 10

// Voir le pinout du Auriga
// Port x >> S2/S1
// Port 1 >> D4/D5
// Port 2 >> D2/D3
// Port 3 >> D6/D7
// Port 4 >> D8/D9
// Port 5 >> D16/D17
// Port 6 >> A15/A10
// Port 7 >> A14/A9
// Port 8 >> A13/A8
// Port 9 >> A12/A7
// Port 10 >> A11/A5

short s1Pin = A10;
short s2Pin = A15;

void setup() {
  Serial.begin(9600);

  pinMode (s1Pin, OUTPUT);
  pinMode (s2Pin, OUTPUT);

  // SSD1306_SWITCHCAPVCC = génère la tension d'affichage à partir de 3,3V en interne
  if(!screen.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;) {
      Serial.println(F("Échec d'allocation SSD1306"));
      delay(1000);
    } // Ne pas continuer, boucler indéfiniment
  }

  // Afficher le contenu initial du tampon d'affichage à l'écran --
  // la bibliothèque l'initialise avec un écran d'accueil Adafruit.
  screen.display();
  delay(2000); // Pause de 2 secondes

}

void loop() {
  currentTime = millis();
  blinkTask(currentTime);
  displayTask(currentTime);
  serialTask(currentTime);
}

void serialTask(unsigned long time){
  static unsigned long lastTime = 0;
  static int rate = 1000;
  static short ledState = LOW;

  if (time - lastTime < rate) return;

  lastTime = time;

  Serial.println (time);

}

void blinkTask(unsigned long time) {
  static unsigned long lastTime = 0;
  static int rate = 100;
  static short ledState = LOW;

  if (time - lastTime < rate) return;

  lastTime = time;

  digitalWrite (s1Pin, ledState);
  digitalWrite (s2Pin, !ledState);

  digitalWrite (LED_BUILTIN, ledState);

  ledState = !ledState;
}

int posX = SCREEN_WIDTH;
short velX = -2;
short passCount = 0;

void displayTask(unsigned long time) {
  static unsigned long lastTime = 0;
  static int animInt = 50;

  if (time - lastTime < animInt) return;

  lastTime = time;

  posX += velX;
  if (posX < -5) {
    posX = SCREEN_WIDTH;
    passCount++;
  }

  // Effacer le tampon
  screen.clearDisplay();
  
  if (passCount != 4) {
    drawStickMan(posX, SCREEN_HEIGHT / 2);
    animInt = 50;
  } else {
    screen.drawSplash();
    passCount = 0;
    animInt = 2000;
  }

  screen.display();
}

void drawStickMan(int x, int y) {
  const short radius = 5;
  const short length = 10;

  screen.drawCircle(x, y, radius, WHITE);
  screen.drawLine(x, y + radius, x, y + radius + length, WHITE);
  screen.drawLine(x - radius, y + length, x + radius, y + length, WHITE);
  screen.drawLine(x - radius, y + 2 * length, x,  y + radius + length, WHITE);
  screen.drawLine(x + radius, y + 2 * length, x,  y + radius + length, WHITE);
}
