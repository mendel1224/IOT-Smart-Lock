#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         5          
#define SS_PIN          10         

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

byte authorizedUIDs[][4] = {
    {0x33, 0x37, 0x1D, 0x16}, // UID 1
};

bool validUID(byte * scannedTag, byte authorizedTags[][4] , int numRows)
{
  // loop through valid uid array
  // if match, return true
  // else, return false


  
  for (int row = 0; row < numRows; row++ )
  {
      for (int columns = 0; columns < 4; columns++)
      {
        // compare each element
        if (scannedTag[columns] == authorizedTags[row][columns] )
        {
            if (columns == 3) // elements 0 -> 3 all mathch, tag is authorized
            {
              return true;
            }
        }
        else 
        {
          break;
        }
      }
  }
  // If all tags were checked and non matched the scanned in tag, the tag is not authorized
  return false;
}




void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    delay(4);           // Optional delay
   // mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD
   
}

void loop() {
    // Check if a new card is present
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

     // Calculate the number of rows in the authorizedTags array
    int numRows = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

     
      if (validUID(mfrc522.uid.uidByte, authorizedUIDs, numRows)) 
        {

        Serial.println("Authorized");
   } 
   else {
        Serial.println("Unauthorized");
    }

  
    // Halt the card
    mfrc522.PICC_HaltA();
}


