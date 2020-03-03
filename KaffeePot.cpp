#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

#include <CSVFile.h>

const bool debugFlag = true;

/*
 * Serial Properties
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
 * SD Card Properties
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
    while(1);
  }
  if (debugFlag) Serial.println("initSD fertig");
}

/*
 * Einfache Datei Informationen die wiederverwendet werden können
 * */

char kaffeeFileFn[30] = "Kaffee.csv";
char origKaffeeFileFn[30] = "KaffeeOr.csv";

CSVFile kaffeeFile;
CSVFile origKaffeeFile;

void setup() {
  initSerial();
  initSD(); 
}

/*
 * Diese Funktion wird aufgerufen wenn ein Kaffe zum Chip hinzugefügt werden soll
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

  char neuerZaehler[]="     ";
  
  // Initialisieren und Öffnen zum Schreiben
  if (SD.exists(tempFileFn) && !SD.remove(tempFileFn)){
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
      while (kaffeeFile.readField(buffer, BUFFER_SIZE)){
        if (strcmp(chipNummer,buffer) == 0 ){
          if (debugFlag) Serial.print("Gefunden in Zeile: ");
          if (debugFlag) Serial.println(kaffeeFile.getNumberOfLine()); 
          inZeileGefunden = true;  
        }
        // Merke den Wert des letzten Feldes
        strcpy(wertGefunden, buffer);
        
        if (inZeileGefunden){
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
      
    }while(kaffeeFile.nextField());
    
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
 * Ausgabe der übergebenen Datei auf dem Serial
 * */

void dumpFile(char dumpFn[30]) {
  Serial.print("START DATEI DUMP         **");
  Serial.println(dumpFn);

  const byte BUFFER_SIZE = 5;
  char buffer[BUFFER_SIZE + 1];
  buffer[BUFFER_SIZE] = '\0';

  CSVFile dumpFile;
  if (!dumpFile.open(dumpFn, O_RDWR)){
    Serial.println("Fehler beim Öffnen der dumpDatei");
    return;
  }
  dumpFile.gotoBeginOfFile();

  do
  {
    Serial.print(dumpFile.getNumberOfLine());
    Serial.print("->");
    do {
      while (dumpFile.readField(buffer, BUFFER_SIZE)){
        Serial.print(buffer);
      }
      Serial.print(";");
    }while(dumpFile.nextField());
    Serial.println();
  }
  while (dumpFile.nextLine());
    
  dumpFile.close();

  Serial.print("END DATEI DUMP                **");
  Serial.println(dumpFn);
}

// Hier nun die eigentliche Verarbeitung

void loop() {

  if (debugFlag) dumpFile(kaffeeFileFn);

  Serial.println("*******************************************START");

  // Hinzufügen eines Kaffees für Cleo
  addKaffee("fd6a0ed1cfc7f5db6a4af0ce485dbb54");
  if (debugFlag) dumpFile(kaffeeFileFn);
  delay(5000);

  // Hinzufügen eines Kaffees für Cleo
  addKaffee("fd6a0ed1cfc7f5db6a4af0ce485dbb54");
  if (debugFlag) dumpFile(kaffeeFileFn);
  delay(5000);

  // Hinzufügen eines Kaffees für Andre
  addKaffee("7e3128ca4b3a6f384f8f13da91b5c04d");
  if (debugFlag) dumpFile(kaffeeFileFn);
  delay(5000);

  Serial.println("************************************END");
  
  delay(50000);
}
