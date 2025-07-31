#include <max6675.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include "ssd1306.h"
#include "ssd1306_console.h"


float temp1 = 0;
char temp1Converted[3] = ""; 
MAX6675 thermocouple1(8, 9, 10); 

RTC_DS3231 rtc; // Create an RTC object
Ssd1306Console  console;
char result[8];
unsigned long previousMillis = 0;
unsigned long interval = 30000;

// Uncomment the following line and set the desired date and time for initial setup.
// This should be commented out after the first successful upload to avoid resetting the time again.
// DateTime now(2024, 9, 22, 15, 30, 0); // YYYY, MM, DD, HH, MM, SS
const int chipSelect = 9;
void setup() {
  Serial.begin(9600);
  ssd1306_128x64_i2c_init();
  ssd1306_fillScreen(0x00);
  ssd1306_setFixedFont(ssd1306xled_font6x8);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }
  millis();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(10));

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    // Set the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC is running with correct time.");
  }
}

void loop() {
  temp1 = thermocouple1.readCelsius();
  Serial.println(temp1);

  printTemperatureToDisplay("T1: ", temp1, 0, 16, 30, 16);
  printTemperatureToDisplay("T2: ", 25.5, 0, 32, 30, 32);

  unsigned long currentMillis = millis();

  if (currentMillis >= previousMillis + interval) {
      previousMillis = currentMillis;
      saveToFile();
  }


  delay(500); // Print time every second
}

void saveToFile() {
  DateTime now = rtc.now();
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  printDate(now, dataFile);
  printTemperatureToFile(dataFile, " T1: ", temp1);
  dataFile.println();
  Serial.println();
  dataFile.close();
}

void printDate(DateTime now, File dataFile) {
  Serial.print(now.year(), DEC);
  dataFile.print(now.year(), DEC);
  Serial.print('/');
  dataFile.print('/');
  Serial.print(now.month(), DEC);
  dataFile.print(now.month(), DEC);
  dataFile.print('/');
  Serial.print('/');
  Serial.print(now.day(), DEC);
  dataFile.print(now.day(), DEC);
  Serial.print(" ");
  dataFile.print(" ");
  Serial.print(now.hour(), DEC);
  dataFile.print(now.hour(), DEC);
  Serial.print(':');
  dataFile.print(':');
  Serial.print(now.minute(), DEC);
  dataFile.print(now.minute(), DEC);
  Serial.print(':');
  dataFile.print(':');
  Serial.print(now.second(), DEC);
  dataFile.print(now.second(), DEC);
}

void printTemperatureToFile(File dataFile, char* name, float temp) {
    dataFile.print(name);
    dataFile.print(temp);
}

void printTemperatureToDisplay(char* name, float temperature, uint8_t nameX, uint8_t nameY, uint8_t tempX, uint8_t tempY) {
  char temp1Result[8];
  ssd1306_printFixedN (nameX, nameY, name, STYLE_NORMAL, FONT_SIZE_2X);
  dtostrf(temperature, 6, 2, temp1Result);
  ssd1306_printFixedN (tempX, tempY, temp1Result, STYLE_NORMAL, FONT_SIZE_2X);
}