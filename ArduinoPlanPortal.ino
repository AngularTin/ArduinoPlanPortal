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
static const int16_t ADDR_ENTRY_COUNT  = 0;                  // 2-byte int16 count
static const int   ENTRY_ID_LEN       = 8;                  // fixed-længde ID bytes
static const int   ENTRY_DATA_LEN     = sizeof(int16_t);    // 2 bytes
static const int   ENTRY_SIZE         = ENTRY_ID_LEN + ENTRY_DATA_LEN;
static const int   ADDR_ENTRIES_BASE  = ADDR_ENTRY_COUNT + sizeof(int16_t);
static const int   MAX_ENTRIES        = 50;                 // Ændrer så EEPROM ikke overfyldes



// Login til hotspot
char ssid[] = "Den Magiske Portal";
char pass[] = "odderuwu"; // tihi

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
    t();
    // Konstruerer request, relevante del er mellem første '/' or ' '
    String path = "";
    bool meet_slash = false;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (meet_slash) {
          if (c==' ') break;
          path += c;
        } else if (c=='/') {
          meet_slash=true;
        }
      }
    }
    t();

    // Hånter request
    Serial.print("Path: ");
    Serial.println(path);

    if (path == "doplaysound") {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println(0);
    } // Den beder om et browsericon, no way in hell at sådan et bliver lavet
      else if (path == "favicon.ico") {
      client.println("HTTP/1.1 204 No Content");
      client.println("Connection: close");
      client.println();
    } else if (path == "") {
      construct_site(client);
    } else {
      handlerestrequest(path);
    }

    delay(1);
    client.stop();
    return;
  }
  
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    handle_rfid();
  }

}



