void handle_rfid() {
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX);
  }

  strID.toUpperCase();
  Serial.print("Tap card key: ");
  Serial.println(strID);
  appendID(strID);
  delay(500);

  if (strID.indexOf("5A206BEA") >= 0) {
    Serial.println("**Adgang Tilladt**");
    digitalWrite(LEDPin, HIGH);
    delay (100);
    digitalWrite(LEDPin, LOW);
    delay (50);
    digitalWrite(LEDPin, HIGH);
    delay (100);
    digitalWrite(LEDPin, LOW);

    return;
  }
  else {
    Serial.println("**Adgang Nægtet**");
    digitalWrite(LEDPin, HIGH);
    delay (1000);
    digitalWrite(LEDPin, LOW);
    return;
  }
}