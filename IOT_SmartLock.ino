#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <Time.h>

#define RST_PIN         5          
#define SS_PIN          10         

#define GREEN_LED_PIN   7          // Green LED pin
#define RED_LED_PIN     8          // Red LED pin

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

char ssid[50] = "CHANGE";
char pass[50] = "CHANGE";

const char kHostname[] = "CHANGE TO YOUR AWS IP";
const char kPath[] = "CHANGE TO YOUR PATH";
const int kNetworkTimeout = 30 * 1000;    // Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

const char* ntpServer = "pool.ntp.org";   // NTP server
const long gmtOffset_sec = -28800;        // UTC-8 for California
const int daylightOffset_sec = 0;         // No daylight saving in December

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
// Function to print the current time when called
String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "FAILED"; // Default time if synchronization fails
  }
  char timeString[25];
  strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S", &timeinfo); 
  return String(timeString);
}

void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);    // Attempts to connect to WiFi
    while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }
    Serial.println("");
    Serial.println("WiFi connected");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
   // mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD

      // Set LED pins as output
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
   
}

void loop() {
    int err = 0;
    WiFiClient c;
    HttpClient http(c);
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

    bool valid = validUID(mfrc522.uid.uidByte, authorizedUIDs, numRows); // Checks if card is valid
    String currentTime = getFormattedTime();       // Gets the current local time
    // String to store on AWS web page
    String val = String ("?authorized=") + String(valid) + String("&time=") + String(currentTime);
    err = http.POST("CHANGE TO IP ADDRESS AWS", 5000, val.c_str());    // Sent to AWS
     // Check if the scanned UID is valid
    if (valid)
    {
        Serial.println("Authorized");
        digitalWrite(GREEN_LED_PIN, HIGH);  // Turn on green LED
        delay(2000);  // Keep LED on for 2 seconds 
        digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
    } 
    else {
        Serial.println("Unauthorized");
        digitalWrite(RED_LED_PIN, HIGH);   // Turn on red LED
         delay(2000);  // Keep LED on for 2 seconds 
         digitalWrite(RED_LED_PIN, LOW);    // Turn off red LED
    }
    // Checks if the data sent
    if (err == 0) {
        Serial.println("Data sent successfully.");
    } else {
        Serial.print("Error sending data: ");
        Serial.println(err);
    }

    // Halt the card
    mfrc522.PICC_HaltA();
}


