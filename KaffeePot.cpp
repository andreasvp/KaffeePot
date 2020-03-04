#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

#include <CSVFile.h>

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#include "TouchScreen.h"
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


const bool debugFlag = true;

/*
   Serial Properties
 * */
const long baudRate = 9600;

void initSerial() {
  // Open serial communications and wait for port to open:
  Serial.begin(baudRate);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  if (debugFlag) Serial.println("initSerial fertig");
}

/*
   SD Card Properties
 * */
SdFat SD;       // file system
#define SD_CS_PIN SS  // SD chip select pin

void initSD() {
  if (debugFlag) Serial.println("initSD start");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  if (!SD.begin(SS)) {
    if (debugFlag) Serial.println("initSD Fehler");
    while (1);
  }
  if (debugFlag) Serial.println("initSD fertig");
}

void initTFT() {
  tft.reset();
  tft.begin(0x9341);
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
}

/*
   Einfache Datei Informationen die wiederverwendet werden können
 * */

char kaffeeFileFn[30] = "Kaffee.csv";
char origKaffeeFileFn[30] = "KaffeeOr.csv";

CSVFile kaffeeFile;
CSVFile origKaffeeFile;

void setup() {
  initSerial();
  initSD();
  initTFT();
}

/*
   Diese Funktion wird aufgerufen wenn ein Kaffe zum Chip hinzugefügt werden soll
 * */
void addKaffee(char chipNummer[]) {

  Serial.print("Hinzufügen eines Kaffee auf ");
  Serial.println(chipNummer);

  char tempFileFn[] = "nKaffee.csv";
  CSVFile tempKaffeeFile;

  const byte BUFFER_SIZE = 41;
  char buffer[BUFFER_SIZE + 1];

  bool inZeileGefunden = false;
  char wertGefunden[BUFFER_SIZE + 1];

  char neuerZaehler[] = "     ";

  // Initialisieren und Öffnen zum Schreiben
  if (SD.exists(tempFileFn) && !SD.remove(tempFileFn)) {
    Serial.println("Fehler beim Löschen der tempDatei");
  }

  if (!tempKaffeeFile.open(tempFileFn, O_RDWR | O_CREAT)) {
    Serial.println("Fehler beim Öffnen der tempDatei");
  }

  // Loop durch die Datei und Suche die chipNummer
  kaffeeFile.open(kaffeeFileFn, O_READ);
  kaffeeFile.gotoBeginOfFile();

  do
  {

    buffer[BUFFER_SIZE] = '\0';
    wertGefunden[BUFFER_SIZE] = '\0';

    do {
      while (kaffeeFile.readField(buffer, BUFFER_SIZE)) {
        if (strcmp(chipNummer, buffer) == 0 ) {
          if (debugFlag) Serial.print("Gefunden in Zeile: ");
          if (debugFlag) Serial.println(kaffeeFile.getNumberOfLine());
          inZeileGefunden = true;
        }
        // Merke den Wert des letzten Feldes
        strcpy(wertGefunden, buffer);

        if (inZeileGefunden) {
          if (debugFlag) Serial.println(wertGefunden);
        }

      }

      if ( kaffeeFile.isEndOfLine() && inZeileGefunden) {

        sprintf(neuerZaehler, "%d", String(wertGefunden).toInt() + 1);
        tempKaffeeFile.addField(neuerZaehler);
        if (debugFlag) Serial.print("Neuer Wert ");
        if (debugFlag) Serial.println(neuerZaehler);

      } else {
        tempKaffeeFile.addField(wertGefunden);
      }

    } while (kaffeeFile.nextField());

    // Setze den Merker zurück
    inZeileGefunden = false;
    // Füge neue Zeile hinzu, wenn noch welche kommen
    if (!kaffeeFile.isEndOfFile()) tempKaffeeFile.addLine();

  }
  while (kaffeeFile.nextLine());

  if (!kaffeeFile.close() && debugFlag ) Serial.println("Fehler orig file.close");

  if (!tempKaffeeFile.close() && debugFlag ) Serial.println("Fehler temp file.close");

  // remove the file:
  if (!SD.remove(kaffeeFileFn) && debugFlag ) Serial.println("Fehler orig file.remove");

  if (!SD.rename(tempFileFn, kaffeeFileFn) && debugFlag ) Serial.println("Fehler temp file.rename");

}

/*
   Ausgabe der übergebenen Datei auf dem Serial
 * */

void dumpFile(char dumpFn[30]) {
  Serial.print("START DATEI DUMP         **");
  Serial.println(dumpFn);

  const byte BUFFER_SIZE = 5;
  char buffer[BUFFER_SIZE + 1];
  buffer[BUFFER_SIZE] = '\0';

  CSVFile dumpFile;
  if (!dumpFile.open(dumpFn, O_RDWR)) {
    Serial.println("Fehler beim Öffnen der dumpDatei");
    return;
  }
  dumpFile.gotoBeginOfFile();

  do
  {
    Serial.print(dumpFile.getNumberOfLine());
    Serial.print("->");
    do {
      while (dumpFile.readField(buffer, BUFFER_SIZE)) {
        Serial.print(buffer);
      }
      Serial.print(";");
    } while (dumpFile.nextField());
    Serial.println();
  }
  while (dumpFile.nextLine());

  dumpFile.close();

  Serial.print("END DATEI DUMP                **");
  Serial.println(dumpFn);
}

void displayWillkommen() {
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.println("KaffeePot");
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("How can I serve?");
}

void displayKnoepfe(){
  tft.setRotation(0);
  tft.fillRect(1, 1, 50, 50, WHITE);
  tft.fillRect(1, 60, 50, 50, BLUE);
  tft.fillRect(1, 120, 50, 50, RED);
  tft.fillRect(1, 180, 50, 50, CYAN);
  tft.fillRect(1, 240, 50, 50, YELLOW);
}

// Hier nun die eigentliche Verarbeitung

void loop() {


  displayWillkommen();

  displayKnoepfe();


do {
  TSPoint p = ts.getPoint();

  if (p.z > ts.pressureThreshhold){

    Serial.print("---> X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.println(p.y);

    p.x = map(p.x, 948, 204, 0, 320);
    p.y = map(p.y, 910, 195, 0, 240);

    Serial.print("===> X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.println(p.y);


    if (p.x>1 && p.x<51 && p.y>170 && p.y<230){ Serial.println("WHITE");}
    if (p.x>61 && p.x<111 && p.y>170 && p.y<230){ Serial.println("BLUE");}
    if (p.x>121 && p.x<171 && p.y>170 && p.y<230){ Serial.println("RED");}
    if (p.x>181 && p.x<231 && p.y>170 && p.y<230){ Serial.println("CYAN");}
    if (p.x>241 && p.x<291 && p.y>170 && p.y<230){ Serial.println("YELLOW");}

  }
}while(true);


  // Hinzufügen eines Kaffees für Cleo
  addKaffee("fd6a0ed1cfc7f5db6a4af0ce485dbb54");
  delay(5000);

  // Hinzufügen eines Kaffees für Cleo
  addKaffee("fd6a0ed1cfc7f5db6a4af0ce485dbb54");
  delay(5000);

  // Hinzufügen eines Kaffees für Andre
  addKaffee("7e3128ca4b3a6f384f8f13da91b5c04d");
  delay(5000);

}
