
// Grundenhed for DB 
struct Entry {
  char     id[ENTRY_ID_LEN];
  int16_t  data;
};

// Til RAM kopien 
static int16_t  ramEntryCount = 0;
static Entry    ramEntries[MAX_ENTRIES];

// Hjælpefunktioner til EEPROM 
int16_t getEntryCount() {
  int16_t cnt;
  EEPROM.get(ADDR_ENTRY_COUNT, cnt);
  if (cnt < 0 || cnt > MAX_ENTRIES) cnt = 0;
  return cnt;
}

void setEntryCount(int16_t cnt) {
  EEPROM.put(ADDR_ENTRY_COUNT, cnt);
}

// Skrev fra DB til ram og omvendt

void loadDBtoRAM() {
  ramEntryCount = getEntryCount();
  for (int i = 0; i < ramEntryCount; i++) {
    int addr = ADDR_ENTRIES_BASE + i * ENTRY_SIZE;
    // read ID
    for (int j = 0; j < ENTRY_ID_LEN; j++) {
      ramEntries[i].id[j] = EEPROM.read(addr + j);
    }
    // read data (little-endian)
    uint8_t lo = EEPROM.read(addr + ENTRY_ID_LEN + 0);
    uint8_t hi = EEPROM.read(addr + ENTRY_ID_LEN + 1);
    ramEntries[i].data = (hi << 8) | lo;
  }
}

void commitDB() {
  setEntryCount(ramEntryCount);
  for (int i = 0; i < ramEntryCount; i++) {
    int addr = ADDR_ENTRIES_BASE + i * ENTRY_SIZE;

    for (int j = 0; j < ENTRY_ID_LEN; j++) {
      EEPROM.update(addr + j, ramEntries[i].id[j]);
    }
    EEPROM.update(addr + ENTRY_ID_LEN + 0, lowByte(ramEntries[i].data));
    EEPROM.update(addr + ENTRY_ID_LEN + 1, highByte(ramEntries[i].data));
  }
}



// Push ny RFID-enhed med assosieret data til RAM
bool pushRAM(const char newID[ENTRY_ID_LEN], int16_t newData) {
  if (ramEntryCount >= MAX_ENTRIES) return false;

  for (int i = ramEntryCount; i > 0; i--) {
    ramEntries[i] = ramEntries[i - 1];
  }
  memcpy(ramEntries[0].id, newID, ENTRY_ID_LEN);
  ramEntries[0].data = newData;
  ramEntryCount++;
  return true;
}

bool editRAM(int index, int16_t newData) {
  if (index < 0 || index >= ramEntryCount) return false;
  ramEntries[index].data = newData;
  return true;
}


bool removeRAM(int index) {
  if (index < 0 || index >= ramEntryCount) return false;
  for (int i = index; i < ramEntryCount - 1; i++) {
    ramEntries[i] = ramEntries[i + 1];
  }
  ramEntryCount--;
  return true;
}

void resetRAM() {
  ramEntryCount = 0;
}