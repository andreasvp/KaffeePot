//https://raw.githubusercontent.com/adafruit/SdFat/master/examples/ReadCsvStream/ReadCsvStream.ino
#include <SPI.h>
#include <SdFat.h>

//------------------------------------------------------------------------------
#define errorHalt(msg) {Serial.println(F(msg)); SysCall::halt();}
//------------------------------------------------------------------------------
#define SFPRINT(x) {if (DEBUG) Serial.print(F(x));}
#define SFPRINTLN(x) {if (DEBUG) Serial.println(F(x));}


const bool DEBUG = false;
const char fileName[30] = "Report.CSV";

SdFat SD;

void createKaffeeCsv() 
{
	SFPRINTLN("<Start createKaffeeCsv()>");

	// create or open and truncate output file
	ofstream sdout(fileName);

	// write file from string stored in flash
	sdout << F(
			"3AB59C07,Mitarbeiter1,Qualitätssicherung,10,01:08:14,23.06.16\n"
			"87594C39,Mitarbeiter2,Entwicklung,10,01:08:14,23.06.16\n"
			"CAB49116,Mitarbeiter3,Qualitätssicherung,10,01:08:14,23.06.16\n"
			"296E2887,Mitarbeiter4,Entwicklung,10,01:08:14,23.06.16\n"
			"FA695AA9,Mitarbeiter5,Entwicklung,10,01:08:14,23.06.16\n"
			"C25846D2,Mitarbeiter6,Qualitätssicherung,10,01:08:14,23.06.16\n"
			"6D51D0F1,Mitarbeiter7,Qualitätssicherung,10,01:08:14,23.06.16\n"
			"F7EBF7E5,Mitarbeiter8,Entwicklung,90,01:08:14,23.06.16\n") << flush;

	// check for any errors
	if (!sdout) {
		errorHalt("writeFile");
	}

	sdout.close();

	SFPRINTLN("<End createKaffeeCsv()>");
}

File root;
char name[260] = {};

void printDirectory(File dir, int numTabs) {

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }

    memset(name, 0, sizeof(name));
    if (!entry.getName(name, sizeof(name))) {
      errorHalt("getName");
    }

    Serial.print(name);
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      Serial.println("");
    }
    entry.close();
  }
}

void listSdFiles(){
	SFPRINTLN("<Start listSdFiles()>");
	root = SD.open("/");
	printDirectory(root, 0);
	SFPRINTLN("<End listSdFiles()>");
}

//CardEntry readCsvEntry(char* cardId){
 // TODO
// }

int addKaffeeCsvEntry(String cardId){
 // TODO
 return 1;
}

float avgAbteilung(String abteilung){

// Length (with one extra character for the null terminator)
int str_len = abteilung.length() + 1;

// Prepare the character array (the buffer)
char char_abteilung[str_len];

// Copy it over
abteilung.toCharArray(char_abteilung, str_len);

	SFPRINTLN("<Start avgAbteilung()>");

	long countCoffeeuser = 0;
	long totalCoffee = 0;

	// // UID[15] , Name [30], Abteilung[20], KaffeeAnzahl , Zeit[9] , Datum[9]
	char uidField[15] = {};
	char nameField[30] = {};
	char abteilungField[30] = {};
	long kaffeeAnzahl = 0;
	char zeitField[9] = {};
	char datumField[9] = {};
	char c1 = ' ', c2 = ' ', c3 = ' ', c4 = ' ', c5 = ' ';  // space for commas.

	// open input file
	ifstream sdin(fileName);

	// check for open error
	if (!sdin.is_open()) {
		errorHalt("open");
	}

	// read until input fails
	while (1) {

		sdin.get(uidField, sizeof(uidField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak uidField>"); break; } // Assume EOF if fail.

	 	sdin >> c1;

		sdin.get(nameField, sizeof(nameField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak nameField>"); break; } // Assume EOF if fail.

		sdin >> c2;

		sdin.get(abteilungField, sizeof(abteilungField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak abteilungField>"); break; } // Assume EOF if fail.

		sdin >> c3 >> kaffeeAnzahl >> c4;

		// relevante Abteilung
		if (strcmp(char_abteilung,abteilungField)==0){
			totalCoffee = totalCoffee + kaffeeAnzahl;
			countCoffeeuser++;
		}

		sdin.get(zeitField, sizeof(zeitField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak zeitField>"); break; } // Assume EOF if fail.

		sdin >> c5;

		sdin.get(datumField, sizeof(datumField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak datumField>"); break; } // Assume EOF if fail.

		//Skip CR/LF.
		sdin.skipWhite();

		if (sdin.fail()) {
			errorHalt("bad input");
		}

		// // error in line if not commas
		if (c1 != ',' || c2 != ',' || c3 != ',' || c4 != ',' || c5 != ',') {
			errorHalt("comma");
		}

	}

	// Error in an input line if file is not at EOF.
	if (!sdin.eof()) {
		errorHalt("readFile");
	}

	SFPRINTLN("<End avgAbteilung()>");

	if (countCoffeeuser>0) {
		return totalCoffee/countCoffeeuser;
	} else return 0;

}

float avgAll(){

	long countCoffeeuser = 0;
	long totalCoffee = 0;

	SFPRINTLN("<Start avgAll()>");

	// UID[15] , Name [30], Abteilung[20] , KaffeeAnzahl , Zeit[9] , Datum[9]
	char uidField[15] = {};
	char nameField[30] = {};
	char abteilungField[30] = {};
	long kaffeeAnzahl = 0;
	char zeitField[9] = {};
	char datumField[9] = {};
	char c1 = ' ', c2 = ' ', c3 = ' ', c4 = ' ', c5 = ' ';  // space for commas.

	// open input file
	ifstream sdin(fileName);

	// check for open error
	if (!sdin.is_open()) {
		errorHalt("open");
	}

	// read until input fails
	while (1) {

		sdin.get(uidField, sizeof(uidField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak uidField>"); break; } // Assume EOF if fail.

		sdin >> c1;

		sdin.get(nameField, sizeof(nameField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak nameField>"); break; } // Assume EOF if fail.

		sdin >> c2;

		sdin.get(abteilungField, sizeof(abteilungField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak abteilungField>"); break; } // Assume EOF if fail.

		sdin >> c3 >> kaffeeAnzahl >> c4;

		totalCoffee = totalCoffee + kaffeeAnzahl;
		countCoffeeuser++;

		sdin.get(zeitField, sizeof(zeitField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak zeitField>"); break; } // Assume EOF if fail.

		sdin >> c5;

		sdin.get(datumField, sizeof(datumField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak datumField>"); break; } // Assume EOF if fail.

		//Skip CR/LF.
		sdin.skipWhite();

		if (sdin.fail()) {
			errorHalt("bad input");
		}

		// // error in line if not commas
		if (c1 != ',' || c2 != ',' || c3 != ',' || c4 != ',' || c5 != ',') {
			errorHalt("comma");
		}

	}

	// Error in an input line if file is not at EOF.
	if (!sdin.eof()) {
		errorHalt("readFile");
	}

	SFPRINTLN("<End avgAll()>");

	if (countCoffeeuser>0) {
		return totalCoffee/countCoffeeuser;
	} else return 0;
}



void dumpFile() {

	SFPRINTLN("<Start dumpFile()>");
	// UID[15] , Name [30], Abteilung[20] , KaffeeAnzahl , Zeit[9] , Datum[9]
	char uidField[15];
	char nameField[30];
	char abteilungField[30];
	long kaffeeAnzahl;
	char zeitField[9];
	char datumField[9];
	char c1, c2, c3, c4, c5;  // space for commas.

	// open input file
	ifstream sdin(fileName);

	// check for open error
	if (!sdin.is_open()) {
		errorHalt("open");
	}

	// read until input fails
	while (1) {

		sdin.get(uidField, sizeof(uidField), ',');
		if (sdin.fail()) {SFPRINTLN("<dumpBreak uidField>"); break; } // Assume EOF if fail.

		sdin >> c1;

		sdin.get(nameField, sizeof(nameField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak nameField>"); break; } // Assume EOF if fail.

		sdin >> c2;

		sdin.get(abteilungField, sizeof(abteilungField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak abteilungField>"); break; } // Assume EOF if fail.

		sdin >> c3 >> kaffeeAnzahl >> c4;

		sdin.get(zeitField, sizeof(zeitField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak zeitField>"); break; } // Assume EOF if fail.

		sdin >> c5;

		sdin.get(datumField, sizeof(datumField), ',');
		if (sdin.fail()) { SFPRINTLN("<dumpBreak datumField>"); break; } // Assume EOF if fail.

		//Skip CR/LF.
		sdin.skipWhite();

		if (sdin.fail()) {
			errorHalt("bad input");
		}

		// // error in line if not commas
		if (c1 != ',' || c2 != ',' || c3 != ',' || c4 != ',' || c5 != ',') {
			errorHalt("comma");
		}

		// print strings
		Serial.print("uidField : ");Serial.println(uidField);
		Serial.print("nameField : ");Serial.println(nameField);
		Serial.print("abteilungField : ");Serial.println(abteilungField);
		Serial.print("kaffeeAnzahl : ");Serial.println(kaffeeAnzahl);
		Serial.print("zeitField : ");Serial.println(zeitField);
		Serial.print("datumField : ");Serial.println(datumField);
	}

	// Error in an input line if file is not at EOF.
	if (!sdin.eof()) {
		errorHalt("readFile");
	}
	sdin.close();

	SFPRINTLN("<End dumpFile()>");
}
