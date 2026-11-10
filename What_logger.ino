
/*
A Data logger for environmental brightness - What could possibly go wrong?
hnns 26

*/
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <BH1750.h>



// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  10000 // 10 seconds in milliseconds between entries (reduce to take more/faster data)

// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to 
// the last 10 reads if power is lost but it uses less power and is much faster!
#define SYNC_INTERVAL LOG_INTERVAL // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

// the digital pins that connect to the LEDs
#define redLEDpin 2
#define greenLEDpin 3

BH1750 lightMeter(0x23);
RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);

  while(1);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println();
  
  // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  
#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  // connect to RTC and BH1750
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }
   
   if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  //uncomment this line if you need to re-set dateTime on a previously configured device
  // the following line sets the RTC to the date & time this sketch was compiled
  //upload the code once, then comment out and upload again. 
  //*************************************************
  //RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //**************************************************
  
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    logfile.println("BH1750 failed");
#if ECHO_TO_SERIAL
    Serial.println("BH1750 failed");
#endif
  }
  

  logfile.println("millis,stamp,date,time,light");    
#if ECHO_TO_SERIAL
  Serial.println("millis,stamp,date,time,light");
#endif //ECHO_TO_SERIAL
}

void loop(void)
{
  DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  digitalWrite(greenLEDpin, HIGH);
  
  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m);           // milliseconds since start
  logfile.print(", ");    
#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");  
#endif

  // fetch the time
  now = RTC.now();
  // log time
  logfile.print(now.unixtime()); // seconds since 1/1/1970
  logfile.print(", ");
  //logfile.print('"');
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print("/");
  logfile.print(now.year(), DEC);
  logfile.print(", ");
  if (now.hour()<10) logfile.print("0");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  if (now.minute()<10) logfile.print("0");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  if (now.second()<10) logfile.print("0");
  logfile.print(now.second(), DEC);
  //logfile.print('"');
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 1/1/1970
  Serial.print(", ");
  //Serial.print('"');
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print("/");
  Serial.print(now.year(), DEC);
  Serial.print(", ");
  if (now.hour()<10) Serial.print("0");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  if (now.minute()<10) Serial.print("0");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  if (now.second()<10) Serial.print("0");
  Serial.print(now.second(), DEC);
  //Serial.print('"');
#endif //ECHO_TO_SERIAL

  float lightReading = -1;
  if (lightMeter.measurementReady()) {
    lightReading = lightMeter.readLightLevel()*10;
  }
  
  logfile.print(", ");    
  logfile.print(lightReading);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(lightReading);
#endif //ECHO_TO_SERIAL

  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif //ECHO_TO_SERIAL

  digitalWrite(greenLEDpin, LOW);

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  logfile.flush();
  digitalWrite(redLEDpin, LOW);
}