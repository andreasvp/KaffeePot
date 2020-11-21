#include <Arduino.h>
#include <SdFat.h>

#include <KaffeeCsv.h>

/*
 * Set DISABLE_CHIP_SELECT to disable a second SPI device.
 * For example, with the Ethernet shield, set DISABLE_CHIP_SELECT
 * to 10 to disable the Ethernet controller.
 */
const int8_t DISABLE_CHIP_SELECT = 10;


// Pin for arduino ethernet SD Card
const int chipSelect = 4;

void initCard(){
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed.");
    while (true);
  }
  Serial.println("initialization done.");
}

void setup()
{
  initCard();

  Serial.println("*********************************************************");
  Serial.println("**    4 Airbas   &    KaffePot Inc.   6y vp            **");
  Serial.println("*********************************************************");

  createKaffeeCsv();
  dumpFile();
}

void loop() // run over and over
{
  float avg = avgAll();
  Serial.print("average: ");Serial.println(avg);

  float avgAbt = avgAbteilung(String("Entwicklung"));
  Serial.print("averageAbteilung(Entwicklung) : ");Serial.println(avgAbt);

  avgAbt = avgAbteilung(String("Qualitätssicherung"));
  Serial.print("averageAbteilung(Qualitätssicherung) : ");Serial.println(avgAbt);
  Serial.println("---------------------------------------------------");

  int newCoffeeAmount = addKaffeeCsvEntry(String("FA695AA9"));
  Serial.print("addCoffee(FA695AA9) : ");Serial.println(newCoffeeAmount);
   Serial.println("---------------------------------------------------");

  newCoffeeAmount = addKaffeeCsvEntry(String("6D51D0F1"));
  Serial.print("addCoffee(6D51D0F1) : ");Serial.println(newCoffeeAmount);

  delay(15000);

  Serial.println("*****************************************************");




}
