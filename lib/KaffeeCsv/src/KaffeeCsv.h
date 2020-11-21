
extern SdFat SD;

extern const bool DEBUG;

void createKaffeeCsv();
void listSdFiles();

int addKaffeeCsvEntry(String cardId);

float avgAbteilung(String abteilung);
float avgAll();

void dumpFile();
