// Helper functions for web protocl logic 

// Logic for ID lit 
void appendID(const String& newID) {
  for (int i = MAX_IDS - 1; i > 0; i--) {
    prevIDs[i] = prevIDs[i - 1];
  }
  prevIDs[0] = newID;
}


// REST Backend
void handlerestrequest(const char* path) {
  // EDIT endpoint:  /edit?i=<index>&v=<value>
  
  if (strncmp(path, "ping", 4)==0) {
    Serial.println("pong");
  } else if (strncmp(path, "edit", 4)==0) {
      int selected_id = path[7] - '0';
      int new_value   = path[11] - '0';
      if (editRAM(selected_id, new_value)) {
        Serial.print("Edited entry ");
        Serial.print(selected_id);
        Serial.print(" → ");
        Serial.println(new_value);
      } else {
        Serial.println("Edit failed: index out of range");
      }
  // DELETE endpoint: /delete?i=<index>
  } else if (strncmp(path, "delete", 6)==0) {
      int selected_id = path[9] - '0';
      Serial.print(selected_id);
      if (removeRAM(selected_id)) {
        Serial.print("Removed entry ");
        Serial.println(selected_id);
      } else {
        Serial.println("Remove failed: index out of range");
      } 
  // Add cart endpoint: /addcard?i=<index>
  } else if (strncmp(path, "addcard", 7)==0) {
      int selected_index = path[10] - '0';
      String selected_card_id = prevIDs[selected_index];
      Serial.print(selected_card_id);

      // Sørger for at duplikator ikke kan tilføjes
      bool duplicate = false;
      for (int j = 0; j < ramEntryCount; j++) {
        char idBuf[ENTRY_ID_LEN + 1];
        memcpy(idBuf, ramEntries[j].id, ENTRY_ID_LEN);
        idBuf[ENTRY_ID_LEN] = '\0';
        if (selected_card_id.equals(idBuf)) {
          duplicate = true;
          break;
        }
      }

      if (duplicate) {
        Serial.println(" Add failed: card already exists");
      } else {
        if (pushRAM(selected_card_id.c_str(), 0)) {
          Serial.print("Added entry ");
          Serial.println(selected_index);
        } else {
          Serial.println(" failed: index out of range");
        }
      }
  } // Endpoint til at sætte farve /setcolor?r=123&g=45&b=67&w=89
  else if (strncmp(path, "setcolor?", 9) == 0) {
    int r = 0, g = 0, b = 0, w = 0;
    sscanf(path, "setcolor?r=%d&g=%d&b=%d&w=%d", &r, &g, &b, &w);
    maincolor.red = r;
    maincolor.green = g;
    maincolor.blue = b;
    maincolor.white = w;

    //client.println("HTTP/1.1 200 OK");
    //client.println("Content-Type: text/plain");
    //client.println("Connection: close");
    //client.println();
    //client.println("OK");

  // Endpoint til alt andet
  } else {
    Serial.println("Unknown path");
  }
}