// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// LIBRARY
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------- RFID Library ------- //
#include <SPI.h>
#include <MFRC522.h>

// ------- LCD Display (16x02) ------- //
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------- Melody & Tone ------- //
#include "pitches.h" // https://gist.github.com/mikeputnam/2820675#file-pitches-h

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLE
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// variables for GPIO of components
// To know which pin used (reference) : https://content.arduino.cc/assets/Pinout-NANO_latest.pdf
#define SS_PIN 10
#define RST_PIN 9
const int doorPin = 6; // relay pin
const int greenLedPin = 4;
const int redLedPin = 3;
const int buzzerPin = 2;

// variables for RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);
String cardUIDs[] = {
    "74 F0 BA 71 4F", // 1st user RFID
    "D1 02 BF 50 3C", // 2nd user RFID
    "43 40 89 54 DE"  // 3rd user RFID
};
const int numberOfCards = sizeof(cardUIDs) / sizeof(cardUIDs[0]);

// variables for melodies and note durations for buzzer
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
int welcomeMelody[] = {NOTE_C4, NOTE_D4, NOTE_E4};
int welcomeDurations[] = {4, 4, 4};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// RFID FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void initializeRFID()
{
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID initialized.");
}

String getCardUID()
{
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  return content.substring(1); // remove the leading space
}

void checkRFID()
{
  // check the UID of the card
  String content = getCardUID();
  Serial.print("Card detected       : ");
  Serial.println(content);

  // handle next action based on whether the RFID card is registered or not inside cardUIDs array
  bool found = false;
  for (int i = 0; i < numberOfCards; i++)
  {
    if (content == cardUIDs[i])
    {
      found = true;
      Serial.print("Card are registered : ");
      Serial.println(content);
      break; // exit the loop once the card is found
    }
  }
  found ? grantAccess() : denyAccess();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// LCD FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void initializeLcd()
{
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  // WRITE THE PROJECT TITLE BELOW
  lcd.print("                "); // 16 column
  lcd.setCursor(0, 1);
  lcd.print("                "); // 16 column
  delay(5000);
  lcd.clear();
}

void clearRow(int row)
{
  lcd.setCursor(0, row);
  lcd.print("                "); // 16 column
}

void printRow(int coloumn, int row, String messages)
{
  clearRow(row);
  lcd.setCursor(coloumn, row);
  lcd.print(messages);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// UTIL FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void initializeOuputPin(int pin, int state)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state);
}

void playWelcomeMelody()
{
  Serial.println("Play welcome melody.");
  for (int i = 0; i < 3; i++)
  {
    int noteDuration = 1000 / welcomeDurations[i];
    tone(buzzerPin, welcomeMelody[i], noteDuration);
    delay(noteDuration * 1.30);
    noTone(buzzerPin);
  }
}

void playGrantAccessMelody()
{
  Serial.println("Play access granted melody.");
  for (int i = 0; i < 2; i++)
  {
    for (int thisNote = 0; thisNote < 12; thisNote++)
    {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(buzzerPin, melody[thisNote], noteDuration);
      delay(noteDuration * 1.30);
      noTone(buzzerPin);
    }
    delay(500);
  }
}

void openDoor()
{
  Serial.println("Opening the door.");
  digitalWrite(doorPin, LOW); // Unlock the door
  delay(200);
  lcd.clear();
  for (int i = 3; i > 0; i--)
  {
    lcd.print("DOOR WILL CLOSE");
    lcd.setCursor(0, 1);
    lcd.print("IN ");
    lcd.print(i);
    lcd.print(" SEC");
    delay(500);
    lcd.clear();
  }
  digitalWrite(doorPin, HIGH);    // Lock the door again
  digitalWrite(greenLedPin, LOW); // Turn off green LED
  delay(200);
  lcd.clear();
  lcd.print("DOOR IS CLOSED");
  lcd.setCursor(0, 1);
  lcd.print("NOW");
  delay(2000);
}

void grantAccess()
{
  Serial.println("WELCOME TO YOUR ROOM");
  printRow(0, 0, "WELCOME TO");
  printRow(0, 1, "YOUR ROOM");
  delay(1000);
  digitalWrite(greenLedPin, HIGH);
  playGrantAccessMelody();
  openDoor();
}

void denyAccess()
{
  Serial.println("UNKNOWN ACCESS");
  printRow(0, 0, "CARD NOT FOUND");
  printRow(0, 1, "TRY AGAIN");
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(redLedPin, HIGH); // Turn on red LED
    delay(500);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(redLedPin, LOW); // Turn off red LED
    delay(500);
  }
  delay(1000);
}

void mainFunction()
{
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    Serial.println(" PUT YOUR CARD. ");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" PUT YOUR CARD. ");
  }
  else
  {
    playWelcomeMelody();
    checkRFID();
    mfrc522.PICC_HaltA();
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// SETUP FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup()
{
  Serial.begin(9600);

  // Initialize the I/O
  initializeRFID();
  initializeLcd();

  initializeOuputPin(redLedPin, LOW);
  initializeOuputPin(greenLedPin, LOW);
  initializeOuputPin(buzzerPin, LOW);
  initializeOuputPin(doorPin, HIGH);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// LOOP FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop()
{
  mainFunction();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------


//Code By Wan Muhammad Danial B Mohamad Fariq//