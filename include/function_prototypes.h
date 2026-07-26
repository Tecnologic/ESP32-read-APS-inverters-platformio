#ifndef FUNCTION_PROTOTYPES_H
#define FUNCTION_PROTOTYPES_H

// Web/pages
void handleAbout(AsyncWebServerRequest *request);
void start_server();
void zendPageBasis(AsyncWebServerRequest *request);
void handleBasisconfig(AsyncWebServerRequest *request);
void zendPageGEOconfig(AsyncWebServerRequest *request);
void zendPageIPconfig();
void handleIPconfig(AsyncWebServerRequest *request);
void initWebSocket();
void handleNotFound(AsyncWebServerRequest *request);
void zendPageMQTTconfig(AsyncWebServerRequest *request);
void loginAdmin(AsyncWebServerRequest *request);
void start_portal();
void handlePortalRoot();
void handleForm();
void handlePortalNotFound(AsyncWebServerRequest *request);
int connectWifi();
void handlePortalClose(AsyncWebServerRequest *request);
void scanWifi();
String makeList(int aantal);
void start_wifi();
void loginBoth(AsyncWebServerRequest *request, String who);

// Helpers
void ledblink(int i, int wacht);
void empty_serial();
void empty_serial2();
void delayedReset();
void flashErase(AsyncWebServerRequest *request);
void eventSend(byte what);
int readInverterfiles();
void test_actionFlag();
void poll_all();
String getChipId(bool sec);
void showDir();
void consoleOut(String toLog);
bool checkRemote(String url);

// Config/data storage
void SPIFFS_read();
void writeStruct(String whichfile, int nummer);
bool leesStruct(String whichfile);
void wifiConfigsave();
void basisConfigsave();
void mqttConfigsave();
void printStruct(String bestand);
bool file_open_for_read(const char* bestand);

// Inverters/pages
void handleInverterconfig(AsyncWebServerRequest *request);
void handleInverterdel(AsyncWebServerRequest *request);
void printInverters();
void remove_gaps();
String processor(const String& var);
void inverterForm();
void structCopy(int a, int b);

// Runtime
IRAM_ATTR void isr();
void buttonPressed();
void Update_Log(int what, const char* message);
String putList(const String& var);
bool mqttConnect();
void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length);
void force_values();
void handleDataRequests(AsyncWebServerRequest *request);
void handleForms(AsyncWebServerRequest *request);
void handle_Serial();
void scroll(int aantal);

// Zigbee / decoding
int decodePollAnswer(int which);
float extractValue(uint8_t startPosition, uint8_t valueLength, float valueSlope, float valueOffset, char toDecode[CC2530_MAX_SERIAL_BUFFER_SIZE]);
void mqttPoll(int which);
char * readZB(char inMess[]);
void sendZB(char printString[]);
String checkSumString(char cmd[]);
char *sLen(const char Command[]);
int StrToHex(char str[]);
String ECU_REVERSE();
void inverterReboot(int which);
void resetValues(bool energy, bool mustSend);
bool waitSerial2Available();
char *split(char *str, const char *delim);

void coordinator_init();
bool coordinator(bool normal);
void sendNO();
void healthCheck();
int checkCoordinator();
void ZBhardReset();
void pairOnActionflag();
void handlePair(AsyncWebServerRequest *request);
bool pairing(int which);
bool decodePairMessage(int which);
void polling(int which);
void querying(int which);
int decodeQueryAnswer(int welke);
bool setMaxPower(int which);

// Misc pages + utility
void confirm();
double round2(double value);
double round1(double value);
void testMessage(bool console);
void rawMessage(bool console);
void sendRaw(char printString[]);
int decodeGeneralAnswer(bool throttle);
void getTijd();
void sun_setrise();
bool zomertijd();
int dow(int y, int m, int d);
void tijd_convert();
int tijd_cvrt(char TIJD[6], bool wat);

// Legacy compatibility hooks
void write_eeprom();

#endif