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
  DateTime now = rtc.now();
  ssd1306_printFixedN (0, 16, "T1: ", STYLE_NORMAL, FONT_SIZE_2X);
  dtostrf(temp1, 6, 2, result);
  ssd1306_printFixedN (30, 16, result, STYLE_NORMAL, FONT_SIZE_2X);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
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
  dataFile.print(" T1: ");
  dataFile.print(temp1);
  dataFile.println();
  Serial.println();
  dataFile.close();

  delay(1000); // Print time every second
}