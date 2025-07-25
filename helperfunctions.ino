// Logic for ID lit 
void appendID(const String& newID) {
  for (int i = MAX_IDS - 1; i > 0; i--) {
    prevIDs[i] = prevIDs[i - 1];
  }
  prevIDs[0] = newID;
}

// Isolerer stien i request'en
String extractPath(const String& request) {
  int start = request.indexOf(' ');
  if (start == -1) return "";

  int end = request.indexOf(' ', start + 1);
  if (end == -1) return "";

  return request.substring(start + 1, end);
}


// REST Backend
void handlerequest(const String& path) {
  // EDIT endpoint:  /edit?i=<index>&v=<value>
  
  if (path.startsWith("/ping")) {
    Serial.println("pong");
  } else if (path.startsWith("/edit")) {
    int i_pos = path.indexOf("i=");
    int amp_pos = path.indexOf('&', i_pos);
    int v_pos = path.indexOf("v=", amp_pos);
    if (i_pos >= 0 && amp_pos > i_pos && v_pos > amp_pos) {
      int selected_id = path.substring(i_pos + 2, amp_pos).toInt();
      int new_value   = path.substring(v_pos + 2).toInt();
      if (editRAM(selected_id, new_value)) {
        Serial.print("Edited entry ");
        Serial.print(selected_id);
        Serial.print(" → ");
        Serial.println(new_value);
      } else {
        Serial.println("Edit failed: index out of range");
      }
    } else {
      Serial.println("Edit failed: bad query format");
    }

  // DELETE endpoint: /delete?i=<index>
  } else if (path.startsWith("/delete")) {
    int i_pos = path.indexOf("i=");
    if (i_pos >= 0) {
      int selected_id = path.substring(i_pos + 2).toInt();
      if (removeRAM(selected_id)) {
        Serial.print("Removed entry ");
        Serial.println(selected_id);
      } else {
        Serial.println("Remove failed: index out of range");
      }
    } else {
      Serial.println("Remove failed: bad query format");
    }
  // Add cart endpoint: /addcard?i=<index>
  } else if (path.startsWith("/addcard")) {
    int i_pos = path.indexOf("i=");
    if (i_pos >= 0) {
      int selected_index = path.substring(i_pos + 2).toInt();
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
  } else {
    Serial.println("Add failed: bad query format");
  }
  
  // Endpoint til at sætte tilstand
} else {
    Serial.print("Unknown path: ");
    Serial.println(path);
  }
}