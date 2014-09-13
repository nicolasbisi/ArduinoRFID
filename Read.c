#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.

void setup() {
        Serial.begin(9600);        // Initialize serial communications with the PC
        SPI.begin();                // Init SPI bus
        mfrc522.PCD_Init();        // Init MFRC522 card
        //Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");
}

void loop() {
        
        // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
        MFRC522::MIFARE_Key key;
        key.keyByte[0] = 0xFF;
        key.keyByte[1] = 0xFF;
        key.keyByte[2] = 0xFF;
        key.keyByte[3] = 0xFF;
        key.keyByte[4] = 0xFF;
        key.keyByte[5] = 0xEE;
        // Look for new cards
        if ( ! mfrc522.PICC_IsNewCardPresent()) {
                return;
        }

        // Select one of the cards
        if ( ! mfrc522.PICC_ReadCardSerial()) {
                return;
        }
        // Now a card is selected. The UID and SAK is in mfrc522.uid.
        
        // Dump UID
        Serial.print("Card UID:");
        for (byte i = 0; i < mfrc522.uid.size; i++) {
                Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                Serial.print(mfrc522.uid.uidByte[i], HEX);
        } 
        Serial.println();

        // Dump PICC type
        byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.print("PICC type: ");
        Serial.println(mfrc522.PICC_GetTypeName(piccType));
        if (        piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
                &&        piccType != MFRC522::PICC_TYPE_MIFARE_1K
                &&        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
                return;
        }
               
        byte trailerBlock   = 3;
        byte status;
        // Authenticate using key A.
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print("PCD_Authenticate() failed: ");
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }
        
        byte buffer[18];
        byte size = sizeof(buffer);        
        status = mfrc522.MIFARE_Read(1, buffer, &size);
        Serial.print((char)buffer[0]);
        Serial.print((char)buffer[1]);
        Serial.print((char)buffer[2]);
        Serial.print((char)buffer[3]);
        Serial.print((char)buffer[4]);
        Serial.print((char)buffer[5]);
        Serial.print((char)buffer[6]);
            
        Serial.print (" ");
        status = mfrc522.MIFARE_Read(2, buffer, &size);
        Serial.print((char)buffer[0]);
        Serial.print((char)buffer[1]);
        Serial.print((char)buffer[2]);
        Serial.println((char)buffer[3]);
        
        // Halt PICC
        mfrc522.PICC_HaltA();

        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();
        
        
}