#include <WiFiS3.h>
#include "SPI.h"
#include "MFRC522.h"
#include <Arduino.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9

int LEDPin = 3;   // Status LED
int mode = 0;


// Setup til rfidlæser
MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

const int MAX_IDS = 10;
String prevIDs[MAX_IDS];

// Konfiguration til DB via EEPROM 
const int16_t ADDR_ENTRY_COUNT  = 0;                  // 2-byte int16 count
const int   ENTRY_ID_LEN       = 8;                  // fixed-længde ID bytes
const int   ENTRY_DATA_LEN     = sizeof(int16_t);    // 2 bytes
const int   ENTRY_SIZE         = ENTRY_ID_LEN + ENTRY_DATA_LEN;
const int   ADDR_ENTRIES_BASE  = ADDR_ENTRY_COUNT + sizeof(int16_t);
const int   MAX_ENTRIES        = 50;                 // Ændrer så EEPROM ikke overfyldes

// TIl at sætte farven
struct Color {
  int red;
  int green;
  int blue;
  int white;
};

Color maincolor;

// Login til hotspot
const char ssid[] = "Den Magiske Portal";
const char pass[] = "odderuwu"; // tihi

// Hvor meget kode efter første / som faktisk processeres, hjælper med performance
const int MAX_REQUEST_LEN = 32;
const int SKIP_AMOUNT = 4;


WiFiServer server(80);

void setup() {
  // Setup serial monitoering for debugging 
  Serial.begin(115200);
  while (!Serial);

  // Setup server
  WiFi.beginAP(ssid, pass);
  while (WiFi.status() != WL_AP_LISTENING) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nAccess Point started!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  // Setup rfidlæser
  SPI.begin();
  rfid.PCD_Init();
  pinMode(LEDPin, OUTPUT);

  for (int i = 0; i < MAX_IDS; i++) {
    prevIDs[i] = "00000000";   
  }
  
  // Setup DB
  loadDBtoRAM();

  pushRAM("Test1234", 0);
  pushRAM("Oddere69", 1);

  // Starter uden farve
  maincolor = {255,0,0,0};

}

int prev_time = 0;

void t() {
  Serial.println(millis() - prev_time);
}

// Main loop
void loop() {
  int new_time = millis();
  int delta_t = new_time - prev_time; 
  prev_time = new_time;
  if (delta_t > 35) {
    Serial.println(delta_t);
  }

  WiFiClient client = server.available();
  if (client) {
    Serial.println("Begin!");
    
    // Konstruerer request
    char path[MAX_REQUEST_LEN] = "";
    int i = 0;
    while (i < MAX_REQUEST_LEN + SKIP_AMOUNT) {
      if (client.available()) {
        char c = client.read();
        if (i > SKIP_AMOUNT) {
          if (c == ' ') {
            path[i - SKIP_AMOUNT] = '\0';
            break;
          }
          path[i - SKIP_AMOUNT - 1] = c;
          }
        i++;
        } 
      }

    // Håndter request
    Serial.print("Path: ");
    Serial.println(path);

    if (strcmp(path, "doplaysound")==0) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println(0);
    } // Den beder om et browsericon, no way in hell at sådan et bliver lavet
      else if (strcmp(path, "favicon.ico")==0) {
      client.println("HTTP/1.1 204 No Content");
      client.println("Connection: close");
      client.println();
    } else if (strcmp(path, "")==0) {
      construct_site(client);
    } else {
      handlerestrequest(path);
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("OK");
    }
    delay(1);
    client.stop();
    return;
  }
  
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    handle_rfid();
  }

}



