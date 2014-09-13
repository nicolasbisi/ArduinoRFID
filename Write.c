#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53    //Arduino Uno
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.

void setup() {
        Serial.begin(9600);        // Initialize serial communications with the PC
        SPI.begin();                // Init SPI bus
        mfrc522.PCD_Init();        // Init MFRC522 card
        Serial.println("Write personal data on a MIFARE PICC ");
}

void loop() {
        
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    key.keyByte[0] = 0xFF;
    key.keyByte[1] = 0xFF;
    key.keyByte[2] = 0xFF;
    key.keyByte[3] = 0xFF;
    key.keyByte[4] = 0xFF;
    key.keyByte[5] = 0xFF;

    
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
            return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())    return;
    
    Serial.print("Card UID:");    //Dump UID
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    } 
    Serial.print(" PICC type: ");   // Dump PICC type
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
     
    byte buffer[34] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x07, 0x80, 0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEE}; // KEY ORIGINAL
    byte block;
    byte status, len;
    
    Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial

    block = 3; 
    //Serial.println("Authenticating using key A...");
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
       Serial.print("PCD_Authenticate() failed: ");
       Serial.println(mfrc522.GetStatusCodeName(status));
       return;
    }   
    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("MIFARE_Write() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
            return;
    }
        else Serial.println("MIFARE_Write() success: ");

    Serial.println(" ");
    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
       
}