// Arduino Nano-ESP32 Program  
// Railroad Signal Controller
/*
Revised version of the earlier controller that was based on legacy Intel 8749 and 8243 
chips and a Sony IR remote control.

This version uses a Nano-ESP32 and the Blynk iPhone app.  All of the other original hardware used 
to drive the signals has been retained.

The Nano-ESP32 is able to connect to wifi even though it is in the cast aluminum control box.

Detailed hardward confguration is described at the end of this sketch.

Flash duration 0.5 - 0.5 seconds
6 Phases 0.000, 0.166, 0.333, 0.500, 0.666, 0.833 .... 1.000
0, 1, 2, 3, 4, 5

0,3
1,4
2,5
3,0
4,1
5,2

signal locations from garage door closkwise (some signals on same control) 
       0               0     5  3
10 2 9 10 2   4 1   11 1 12  6  3 7 8 

*/
// Template ID, Device Name and Auth Token are provided by Blynk
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "RRS"
#define BLYNK_AUTH_TOKEN ""
// not used in agent, just uploaded app
#define BLYNK_FIRMWARE_VERSION "1.0.1"
char auth[] = BLYNK_AUTH_TOKEN;
char version_str[32] = "V.051025";  // month/day/year

// for eeprom emulation in nano ESP32 flash memory
// esp32 EEPROM retains its value between program uploads
// if the EEPROM is written, then there is a written signature at address 0
// update signature when eeprom data structure is changed
#include <EEPROM.h>
#define EEPROM_SIZE 1000  // This is 1k Byte
uint16_t storedAddress = 0;
int signature;
const int WRITTEN_SIGNATURE = 0xabcdabc2;
char ssid[20] = "";
char pass[20] = ""; // Set password to "" for open networks.
char k_GMT_str[20] = "-7"; //-8 for California ST and -7 for California DST
int k_GMT_int;
char activeTime_str[20] = "2";  // default is two for now - motion should keep alive if needed
int activeTime_int;
unsigned long activeTime_ul;
char enableStartHours_str[20] = "7";  // start motion function at 7 am
char enableEndHours_str[20] = "23";  // end motion function at 11 pm
int enableStartHours_int;
int enableEndHours_int;
struct memory  // create data structure for easier EEPROM reads/writes
{ // note - all strings - max length 19
  int eeprom_signature;
  char eeprom_ssid[20];
  char eeprom_pass[20]; 
  char eeprom_k_GMT[20];
  char eeprom_enableStartHours[20];
  char eeprom_enableEndHours[20];
  char eeprom_activeTime[20];
  } flash;
int eepromFirstWriteFlag = 0;

// for wifi and Blynk
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// for time and clock
#include "time.h"
#include <ESP32Time.h>
ESP32Time rtc(0*3600);
const long  gmtOffset_sec = 3600 * -8; // Pacific Standard Time
const int   daylightOffset_sec = 3600;
const char* ntpServer = "pool.ntp.org";
struct tm timeinfo;

long rssi;
char rssi_str[16];

// for date and time functions
char second_str[16];
int second_int;
char minute_str[16];
int minute_int;
char hour_str[16];
int hour_int;
char days_str[16];
int days_int;
char months_str[16];
int months_int;
char years_str[16];
int years_int;
char date_str[16]; // date string
char time_str[16]; // time string

// for error logging and resets
char error_type_str[32];
char error_type_display_str[32];
// set the reset flag on error and clear the reset flag when restarting
int nano_reset_flag_int = 0;

// for watch dog timer
#include <esp_task_wdt.h>
#define WDT_TIMEOUT 8 // 3, 8 and 16  seconds tested

// for input pushbutton switches
int SWPin = D1; 
int valueSW = 0;  
#include <Bounce2.h>
Bounce debouncerSW = Bounce();  // active low 

// for Blynk
int modeValue = -1; // 0-9
int signalValue = 0; //0-12
int commandValue = 0; //0-5
int enterValue = 0; // 0-1 etc.
int _4BValue = 0; 
int _5MValue = 0;
int _6MValue = 0;
int _11MValue = 0;
int _12MValue = 0;

// Attach virtual serial terminal to Virtual Pin V3
WidgetTerminal terminal(V3);

// for reading a second input line in the Blynk terminal
int terminal_second_line_flag_int = 0;

// for Blynk  turn on or off with ledKeyPress.on(), ledKeyPress.off(); 
WidgetLED ledKeyPress(V10);

// for Blynk commands
char cmd_str[640];
int cmd_length_int = 0;
int cmd_flag_int = 0;
int serial_second_line_flag_int = 0;

// for manual timers
unsigned long currentMillis;
unsigned long previousMillis_Blynk = 0;
unsigned long previousMillis_Phases = 0;
unsigned long previousMillis_blinkOnBoardLED = 0;
unsigned long previousMillis_nanoReset = 0; 
unsigned long previousMillis_updateFan = 0;
unsigned long previousMillis_signalsActive = 0;
unsigned long lastCommandMillis = 0;
unsigned long lastMotionMillis = 0;
unsigned long previousMillis_updateMotion = 0;

// command in last 5 minutes or motion on last 5 minutes
int signalsActiveFlag = 1; 

// for notifications
char notification_str[256]; 

/*
If using Nano ESP32 - in the IDE Tools menu, enter Pin Numbering and choose By GPIO number (legacy).
Then sketch should always use labels to refer to Arduino pins (e.g if you used the 
number 2, replace this with the symbol D2).
This is a more library-compatible scheme and avoids confusion.  
Also see pin table at end of this sketch.
*/

// for leds
// configure leds
// Nano ESP32 on board RGB led
#define LEDR 46  // Note - Boot0  active low
#define LEDB 45  // Note - not an on-board pinout  active low
#define LEDG 0   // Note - B00t1 active low
// led state 0-off, 1-on, 2-blinking(1s)
int LEDRState = 0; // leds off 
int LEDBState = 0; 
int LEDGState = 0;

// green power led always on with power
// yellow on board led - also called LED builtin -
//   is on D13 or GPIO48 - SPI Serial Clock
int onBoardLEDValue = 0; // for blinking on board led

// for 8243 and ESP32 port control
int dataNibble[10] = {0x7, 0x3, 0x3, 0x7, 0xf, 0xf, 0x3, 0x3, 0x0, 0x0}; // 0-4 8243 0, 5-7, 8243 1, ESP32 D2-D5 8, ESP32 D6-D9 9 
/*
to determine initial off states for signal 0-12 outputs on ports 0-9 - use second column
0  1  0x3
1  7  0x3
2  2  0x3
3  6  0x3
4  0  0x7
5  8  0x0 shared with 6
6  8  0x0
7  3  0x7
8  4  0xf 0x7 plus 0x8
9  5  0xf 0x7 plus 0x8
10 split to bit 3 on each of ports 4 and 5
11 9  0x0 shared with 12
12 9  0x0
*/
int nibbleNum = 0; // for selecting correct dataNibble
int bitNum;
int commandNibble = 4; // 8243 write port
int data0Pin = A0; // for 8243 comm
int data1Pin = A1;
int data2Pin = A2;
int data3Pin = A3;
int progPin = A4;
int csPin = A5;
int prog = 0;
int cs = 0;

// for controlling and flashing search lights, grade crossign and color lights 
int stdCmd[13]; // for signals 0-12 - format for bits is flash phase phase phase data data data data
int altCmd[13];
int phase = 0; // starting phase of 0-5
int mode = 0;
int modeLast = -1;
unsigned long modeCurrentMillis;
unsigned long modeStartMillis;
int modeStage = 0;
int modeLastStage = -1;
int reportModeFlag = 0;  // for testing - outputs moda nad stage to Blynk terminal
int bitTest = 0;  // flag for testing indivdual bits without normal operation

// for thermistor, motion, relay and fan controls
// configure thermistor, motion, relay and fan controls
int thermistorPin = A6; 
int thermistorValue; // 0-4096 - 12 bit
int thermistorTemp;
int thermistorReportFlag = 0; // for terminal print out thermistor values 
int highTempFlag = 0;

int motionPin = D10; // active high
int motionState; // state 0-off, 1-on
int prior_motionState;
int motionReportFlag = 1; // default is on
int motionWindowFlag;

int relayPin = D11; // active high (D0)
int relayState; // state 0-off, 1-on
int prior_relayState;
int relayReportFlag = 1; // default is on

int fanPin = D12;
int fanState; // state 0-off, 1-on
int prior_fanState;
int fanReportFlag = 1; // default is on

// for switch
int swPin = D1; 
int swState = -1; // active low input
unsigned long swFallTime;
unsigned long swRiseTime;

void setup()
{
//for data outputs
pinMode(data0Pin, OUTPUT);
pinMode(data1Pin, OUTPUT);
pinMode(data2Pin, OUTPUT);
pinMode(data3Pin, OUTPUT);
pinMode(progPin, OUTPUT);
pinMode(csPin, OUTPUT);
pinMode(D2, OUTPUT);
pinMode(D3, OUTPUT);
pinMode(D4, OUTPUT);
pinMode(D5, OUTPUT);
pinMode(D6, OUTPUT);
pinMode(D7, OUTPUT);
pinMode(D8, OUTPUT);
pinMode(D9, OUTPUT);

// cause exit of 8243 power on mode by cycling PROG and CS pins on each 8243
dataNibbleOut(0, 0x7);
dataNibbleOut(4, 0xf);
// blank all signals
// set all port outputs 0-9 to initial dataNibble[] values
for (int i = 0; i <=9; i++){
  dataNibbleOut(i, dataNibble[i]);
  }
//for Nano LEDS
pinMode(LED_BUILTIN, OUTPUT);
pinMode(LEDR, OUTPUT);
pinMode(LEDG, OUTPUT);
pinMode(LEDB, OUTPUT);
digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
digitalWrite(LEDR, HIGH); // These are active LOW
digitalWrite(LEDG, HIGH); 
digitalWrite(LEDB, HIGH); 

// configure input and outputs
pinMode(motionPin, INPUT_PULLDOWN);  // remains low or off when not connected
motionState = digitalRead(motionPin);
prior_motionState = motionState;
pinMode(relayPin, OUTPUT); // relay
pinMode(fanPin, OUTPUT); // fan
pinMode(swPin, INPUT_PULLUP); // switch
// at start turn off relay
relayOff();
// at start turn fan off
fanOff();

Serial.begin(38400); // for serial monitor
//while (!Serial) {}; // wait for serial port to connect.

// for eeprom
// In the ESP32, a typical Flash page is 64-Bytes and you need to read-modify-write
// an entire page at a time.  The library saves the data to a buffer with the write() 
// or put() function and it is not actually written to Flash memory until 
// EEPROM.commit() is called. 
// Write eeprom data or if reset, obtain eeprom data
// Check signature at address 0
// If the eeprom is written, then there is a correct written signature.
// Note - unlike Arduino MKR1010 flash memory, this flash memory persists after  
// reprogramming.
EEPROM.begin(EEPROM_SIZE);
EEPROM.get(storedAddress, signature);
// If the EEPROM is written, then there is a orrect written signature
if (signature == WRITTEN_SIGNATURE){
  EEPROMRead();  // print confirmation of data written and update variables
  }
else { // eeprom is not written and needs to be written
  EEPROMWrite();
  eepromFirstWriteFlag = 1;  // report new write through Blynk terminal
  }

// for watch dog timer
esp_task_wdt_init(WDT_TIMEOUT, true); // enable wdt
esp_task_wdt_add(NULL); //add current thread to WDT watch
// when time runs out, processor does a hardware reset

// read eeprom first - for time, wifi inputs

// connect to Blynk and WiFi network  // note - this needs to have the eeprom read first to connect to storred ssid
Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);  // this code works well with the nano, not so much with mkr
esp_task_wdt_reset();
// print out the status on the serial port
Serial.print("SSID: ");
Serial.println(WiFi.SSID());
// print out the WiFi IP address:
IPAddress ip = WiFi.localIP();
Serial.print("IP Address: ");
Serial.println(ip);
// print and display the received signal strength
rssi = WiFi.RSSI();
Serial.print("Signal strength (RSSI):");
Serial.print(rssi);
Serial.println(" dBm");
terminal.println();

// The 'ESP32Time' library is just a wrapper interface for the functions available 
// in 'esp_sntp.h'.  There is no real need for ESP32Time.h other than convenience.
//  As long as WiFi is connected, the ESP32's internal RTC will be periodically 
//  synched to NTP.  The synch interval can be reported and can be changed.  

configTime(k_GMT_int *3600, 0, ntpServer);
//configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
// change to dayligh savings time by changing GMT offset to -7 from -8

//struct tm timeinfo; - defined above 
if(!getLocalTime(&timeinfo)){
  //Serial.println("Failed to obtain time");
  }
rtc.setTimeStruct(timeinfo); 
delay(500);

esp_task_wdt_reset();  // refresh watch dog timer
updateDate();
updateTime();
Serial.println();  // for testing
Serial.print(date_str);
Serial.print("  ");
Serial.println(time_str);
Serial.println("RRS Controller is online.");
Serial.print(ssid);
Serial.print("  ");
rssi = WiFi.RSSI();
Serial.print(rssi);
Serial.println(" dBm");
Serial.println(); // add line feed
Serial.println("Type cmd in Blynk App for list of commands.");
Serial.println(); // add line feed

terminal.clear();
terminal.print(date_str);
terminal.print("  ");
terminal.println(time_str);
terminal.println("RRS Controller is online.");
terminal.print(ssid);
terminal.print("  ");
terminal.print(rssi);
terminal.println(" dBm");
terminal.println();  // add line feed
esp_task_wdt_reset();  // refresh watch dog timer

Blynk.logEvent("rrsc_restarted"); // log event to timeline

// for eeprom if first write
if (eepromFirstWriteFlag == 1){  // report new write through Blynk terminal
  terminal.println("EEPROM did not contain data.  Code defaults written to EEPROM.");
  terminal.println();
  EEPROMRead();  // print confirmation of data written and update variables
  }

analogReadResolution(12); // set analog read resolution to 12 bits 0-4096 // may be default in ESP32
thermistorValue = analogRead(thermistorPin);   

// configure input and outputs
pinMode(motionPin, INPUT_PULLDOWN);  // remains low or off when not connected
motionState = digitalRead(motionPin);
prior_motionState = motionState;
pinMode(relayPin, OUTPUT); // relay
pinMode(fanPin, OUTPUT); // fan
pinMode(swPin, INPUT_PULLUP); // switch
// at start turn off relay
relayOff();
// at start turn fan off
fanOff();
// for input pushbutton switches
debouncerSW.attach(swPin); // set up debouncer
debouncerSW.interval(25);  // use a debounce interval of 25 milliseconds

terminal.println("Type cmd for list of commands.");
terminal.println(); // add line feed
terminal.flush();
// end setup
}

void loop()
{
esp_task_wdt_reset();  // refresh watch dog timer
currentMillis = millis();
updateSwitches();
// check inputs and update outputs every 0.166 second - flashes every 1/2 second - phases 0-5
// do not run in bit test mode
if (((currentMillis - previousMillis_Phases) >= 166) && (bitTest == 0)){
  previousMillis_Phases = currentMillis;  // Remember the time
  // output to signals
  updateBlynkSwitches(); // update Blynk switches
  updateMode(); // update mode 
  updateLeds(); // update LEDs for motion and switch
  updateOutputs();  // then update outputs to signals
  // increment phase
  phase++;
  if (phase >= 6) {phase = 0;}
  }
// Check for Blynk terminal command every 0.5 second
if (currentMillis - previousMillis_Blynk >= 500) { 
  previousMillis_Blynk = currentMillis;  // Remember the time
  Blynk.run();
  if (strcmp(cmd_str, "cleared") != 0) {
    menu();
    strcpy(cmd_str, "cleared"); // after menu() runs, clear com_str to avoid repeated commands
    }
  }
// Check for motion every 0.5 second
if (currentMillis - previousMillis_updateMotion >= 500){ 
  previousMillis_updateMotion = currentMillis;  // Remember the time
  // for testing
  motionState = digitalRead(motionPin);  // active high
  if (motionState == HIGH){
    LEDBState = 1; // led on
    lastMotionMillis = currentMillis;  // Remember the time
    }
  else{
    LEDBState = 0; // led off
    }
  if (motionState != prior_motionState){
    prior_motionState = motionState; 
    if (motionReportFlag == 1){
      if (motionState == HIGH){
        terminal.println("motion detected  ");
        }
      else{
        terminal.println("no motion  ");
        }
      terminal.flush();
      Blynk.run();
      }
    }
  }  
// check if signals active every 0.5 seconds
if (currentMillis - previousMillis_signalsActive >= 500) { 
  previousMillis_signalsActive = currentMillis;  // Remember the time
  //// check if motion is in enabled time
  updateTime();
  if (hour_int >= enableStartHours_int && hour_int < enableEndHours_int) {motionWindowFlag = 1; }
  else {motionWindowFlag = 0;}
  // check if motion in enabled time in last 2 minutes or command in last 2 minutes
  int mF = 0;
  int cF = 0;
  // activeTime_ul is in minutes
  if  ((((currentMillis - lastMotionMillis) <= (activeTime_ul * 60000)) // motion input
    && (motionWindowFlag == 1)) && (lastMotionMillis != 0)) {mF = 1;}
  if ((currentMillis - lastCommandMillis) <= (activeTime_ul * 60000)) {cF = 1;}  // command input     
    if ((mF == 1) || (cF == 1)){
      relayOn(); // turn on signals
      if (signalsActiveFlag == 0){ // exit standby and dark mode - this runs only once
        signalsActiveFlag = 1; 
        terminal.println("signals active");
        terminal.println();
        // determine mode
        if (cF != 1) {  // check if the activation was not command as command provides new mode
          changeMode(0); // if so, load mode 0 as no command otherwise entered and  it will remain dark mode
          }
        }
      }
    else {
      relayOff(); // turn off signals
      // enter dark mode for lower power consumption - all leds off
      if (signalsActiveFlag == 1){ // this runs only once
        signalsActiveFlag = 0;
        terminal.println("signals in standby dark mode");
        changeMode(8);  // change mode to dark
      }
    }
  }   
// blink on board led every 1 second
if(currentMillis - previousMillis_blinkOnBoardLED >= 1000) {
  previousMillis_blinkOnBoardLED = currentMillis;  // Remember the time
  blinkOnBoardLED();
  }
// Fan updated every 5 seconds
if (currentMillis - previousMillis_updateFan >= 5000) { 
  previousMillis_updateFan = currentMillis;  // Remember the time
  updateFan();
  }
// Error routine after 5 seconds
if((nano_reset_flag_int == 1) && (currentMillis - previousMillis_nanoReset >= 5000)) {  
  nanoReset();
  }
}  // end of loop

void changeMode(int newMode)
{
mode = newMode;
modeLast = -1;
modeStage = 0;
modeLastStage = -1;
terminal.print("mode changed to ");
terminal.println(newMode);
terminal.flush();
Blynk.run();
return;
}

void updateBlynkSwitches()   
{
if (modeValue != -1){
  lastCommandMillis = currentMillis; // update time out
  swState = -1; // reset switch
  ledKeyPress.on(); // turn keypad led on
  changeMode(modeValue);
  while (modeValue != -1) {delay200ms(); Blynk.run(); } 
  ledKeyPress.off(); // turn keypad led off
  return;
  }
if (enterValue == 1) {// update signal based on signalValue and commandValue on dropdowns
  changeMode(9);  // hold mode
  lastCommandMillis = currentMillis; // update time out
  swState = -1; // reset switch
  ledKeyPress.on(); // turn keypad led on
  sigSet(signalValue, commandValue);
  while (enterValue != 0) {delay200ms(); Blynk.run();} 
  ledKeyPress.off(); // turn keypad led off
  return;
  }
if (_4BValue == 1){
  changeMode(9);  // hold mode
  lastCommandMillis = currentMillis; // update time out
  swState = -1; // reset switch
  ledKeyPress.on(); // turn keypad led on
  // active low
  if (bitRead(stdCmd[4], 2) == 1){gc4_FB(); terminal.println("gc4 bell on");} // check which way to flip
  else {gc4_F(); terminal.println("gc4 bell off");}
  while (_4BValue != 0) {delay200ms(); Blynk.run();} 
  ledKeyPress.off(); // turn keypad led off
  terminal.flush();
  Blynk.run();  
  return;
  }
if (_5MValue == 1){
  changeMode(9);  // hold mode
  lastCommandMillis = currentMillis; // update time out
  swState = -1; // reset switch
  ledKeyPress.on(); // turn keypad led on
  if (bitRead(stdCmd[5], 0) == 0){gy5_MLon(); terminal.println("gy5 motor on");} // check which way to flip
  else{gy5_Lon(); terminal.println("gy5 motor off");}
  while (_5MValue != 0) {delay200ms(); Blynk.run();} 
  ledKeyPress.off(); // turn keypad led off
  terminal.flush();
  Blynk.run();  
  return;
  } 
if (_6MValue != 0){
  changeMode(9);  // hold mode
  lastCommandMillis = currentMillis; // update time out
  swState = -1; // reset switch
  ledKeyPress.on(); // turn keypad led on
  if (bitRead(stdCmd[6], 0) == 0){gy6_MLon(); terminal.println("gy6 motor on"); } // check which way to flip
  else{gy6_Lon();terminal.println("gy6 motor off"); }
  while (_6MValue != 0) {delay200ms(); Blynk.run();} 
  ledKeyPress.off(); // turn keypad led off
  terminal.flush();
  Blynk.run();  
  return;
  } 
if (_11MValue != 0){
  changeMode(9);  // hold mode
  lastCommandMillis = currentMillis; // update time out
  swState = -1; // reset switch
  ledKeyPress.on(); // turn keypad led on
  if (bitRead(stdCmd[11], 0) == 0){ww11_MLon(); terminal.println("ww11 motor on"); } // check which way to flip
  else{ww11_Lon(); terminal.println("ww11 motor off"); }
  while (_11MValue != 0) {delay200ms(); Blynk.run();} 
  ledKeyPress.off(); // turn keypad led off
  terminal.flush();
  Blynk.run();  
  return;
  } 
if (_12MValue == 1){
  changeMode(9);  // hold mode
  lastCommandMillis = currentMillis; // update time outledKeyPress.on(); // turn keypad led on
  swState = -1; // reset switch
  if (bitRead(stdCmd[12], 0) == 0){ww12_MLon(); terminal.println("ww12 motor on"); } // check which way to flip
  else{ww12_Lon(); terminal.println("ww12 motor off");}
  while (_12MValue != 0) {delay200ms(); Blynk.run();} 
  ledKeyPress.off(); // turn keypad led off
  terminal.flush();
  Blynk.run();  
  return;
  }
}

void updateFan()
{
// analog read resolution set earlier to 12 bits 0-4096
thermistorValue = analogRead(thermistorPin); 
// 1400 = 130deg, 1500 = 120deg, 1600 = 110deg, 1700 = 100deg , 
// 1800 = 90deg, 1900 = 80deg, 2000 = 70deg, 2100 = 60deg, approx
thermistorTemp = 80 + ((1900 - thermistorValue) / 10);
if (thermistorReportFlag == 1){
  terminal.print(thermistorValue); // for testing
  terminal.print("  ");
  terminal.print(thermistorTemp);
  terminal.println(" deg");
  terminal.flush();  // output to terminal immediately
  Blynk.run(); 
  }
if (thermistorValue >= 1800){ // 90 deg
  fanOff();
  }
if (thermistorValue >= 1700){ // 100 deg // clear high temp flag is below 100
  // turn high temp flag off (do not change fan on state)
  if (highTempFlag == 1){
    highTempFlag = 0;
    updateTime();
    terminal.print(thermistorValue);
    terminal.print("  ");
    terminal.print(thermistorTemp);
    terminal.println(" deg");
    terminal.print("high temp flag cleared  ");
    terminal.println(time_str); 
    terminal.flush();
    Blynk.run();
    }
  }
if (thermistorValue <= 1700){ // 100 deg
  fanOn();
  }
if ((thermistorValue <= 1500) && ((thermistorValue - 1000) > 0)){ // if high temp, turn off relay 120 deg
  // second test added for thermistor low read value when testing nano stand alone w/o thermistor
  highTempFlag = 1;
  relayOff();
  updateTime();
  strcpy(notification_str, "RRS High Temperature - fan on and relay off.  ");
  terminal.print(time_str);  
  terminal.print("  ");
  terminal.print(notification_str);
  terminal.println(thermistorTemp);
  // format ex Blynk.logEvent("event_code", String("High TemperatureDetected! Tº: ") + temp);
  //Blynk.logEvent("rrs_high_temp", String(notification_str) + thermistorTemp);**** fix this 
  terminal.flush();
  Blynk.run();
  }
}

void relayOn()
{
if (highTempFlag == 1){ // keep relay off if temp over 110 deg (th < 1600)
  // until temp drops to 95 deg (see updateFan())
  // return without turning relay on
  relayOff();
  return;
  }
relayState = 1; // relay on
digitalWrite(relayPin, HIGH); // turn relay on - active high
if (relayState != prior_relayState){
  prior_relayState = relayState; 
  if (relayReportFlag == 1){
    terminal.print("relay on  ");
    updateTime();
    terminal.println(time_str); 
    terminal.flush();
    Blynk.run();
    }
  }
}
  
void relayOff()
{  
relayState = 0; // relay off
digitalWrite(relayPin, LOW);  // active high
if (relayState != prior_relayState){
  prior_relayState = relayState;
  if (relayReportFlag == 1){
    terminal.println("relay off  ");
    updateTime();
    terminal.println(time_str); 
    terminal.flush();
    Blynk.run();
    }
  }
}

void fanOn()
{
fanState = 1;  
digitalWrite(fanPin, HIGH);  // active high
if (fanState != prior_fanState){
  prior_fanState = fanState; 
  if (fanReportFlag == 1){
    terminal.print("fan on  ");
    updateTime();
    terminal.println(time_str); 
    terminal.flush();
    Blynk.run();
    }
  } 
}

void fanOff()
{
fanState = 0;  
digitalWrite(fanPin, LOW);  // active high
if (fanState != prior_fanState){
  prior_fanState = fanState; 
  if (fanReportFlag == 1){
    terminal.println("fan off  ");
    updateTime();
    terminal.println(time_str); 
    terminal.flush();
    Blynk.run();
    }
  } 
}

void printLocalTime() // prints local time to Blynk terminal
{
// struct tm timeinfo; - defined globally above  
//getLocalTime(&timeinfo);
//terminal.print("ntp ");
//terminal.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");  // see strftime for formating options
// rtc loaded above in setup ??
// terminal.print("nano rtc ");
terminal.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format 
terminal.flush();
Blynk.run();
return;
}

void terminal_output()
{
updateTime();
terminal.print(time_str);
terminal.print("  ");
terminal.println(notification_str);
}

void nanoError()
{
  // do not log new errors if one has been reported 
  //   and now in prcess of logging and resetting
  if (nano_reset_flag_int == 1) {return;}
  // general case errors
  strcpy(error_type_display_str, error_type_str);
  strcat(error_type_display_str, " Error");
  // special case error
  if (strcmp(error_type_str, "BT") == 0) {
    strcpy(error_type_display_str, "BT restart");
    }  
  if (strcmp(error_type_str, "ST") == 0) {
    strcpy(error_type_display_str, "ST restart");
    }  
  // set flag for error reporting and restart
  nano_reset_flag_int = 1;
}

void updateSwitches()
{
// Update the debouncer instances
debouncerSW.update();  
if (debouncerSW.fell()){
  lastCommandMillis = currentMillis; // update time out
  swFallTime = currentMillis;
  // wait until rise checking if long press
  while (digitalRead(swPin) == LOW){
    swRiseTime = millis();
    if (swRiseTime - swFallTime > 3000){
      // long press detected
      terminal.println("long press detected");
      terminal.flush();
      Blynk.run();
      changeMode(8); // dark
      swState = -1; 
      return;
      } 
    } 
  // decode switch actions
  swState++; // loop through states
  if (swState > 16) {swState = 0;}
  switch(swState){
    case 0: // red
      changeMode(3);
      break;
    case 1: // red flash
      changeMode(4);
      break;
    case 2: // yellow
      changeMode(5);
      break;
    case 3: // yellow flash
      changeMode(6);
      break;
    case 4: // green
      changeMode(7);
      break;
    case 5:
      changeMode(9);  // hold mode
      gc4_FB();
      terminal.println("gc4 bell on");
      terminal.flush();
      Blynk.run();
      break;
    case 6:
      gc4_F();
      terminal.println("gc4 bell off");
      terminal.flush();
      Blynk.run();
      break;
    case 7:
      gy5_MLon(); 
      terminal.println("gy5 motor on");
      terminal.flush();
      Blynk.run();  
      break;
    case 8:
      gy5_Lon(); 
      terminal.println("gy5 motor off");
      terminal.flush();
      Blynk.run();  
      break;
    case 9:
      gy6_MLon(); 
      terminal.println("gy6 motor on"); // check which way to flip
      terminal.flush();
      Blynk.run();  
      break;
    case 10:
      gy6_Lon(); 
      terminal.println("gy6 motor off");
      terminal.flush();
      Blynk.run();  
      break;
    case 11:
      ww11_MLon(); 
      terminal.println("ww11 motor on");
      terminal.flush();
      Blynk.run();  
      break;
    case 12:
      ww11_Lon(); 
      terminal.println("ww11 motor off");
      terminal.flush();
      Blynk.run();  
      break;
    case 13:
      ww12_MLon(); 
      terminal.println("ww12 motor on");
      terminal.flush();
      Blynk.run();  
      break;
    case 14:
      ww12_Lon(); 
      terminal.println("ww12 motor off");
      terminal.flush();
      Blynk.run();  
      break;
    case 15:
      gc4_FB();
      gy5_MLon(); 
      gy6_MLon(); 
      ww11_MLon(); 
      ww12_MLon(); 
      terminal.println("all motors on!");
      terminal.flush();
      Blynk.run();  
      break;
    case 16: // program 0
      changeMode(0);
      break;
    break; // default
    }
  }  
return;
}

void updateLeds()
{
// LEDR control - switch
switch (LEDRState)
  {
    case 0:
      // turn off led
      digitalWrite(LEDR, HIGH); // active low
      break;
    case 1:
      // turn on led
      digitalWrite(LEDR, LOW);
      break;
    case 2:
      // blink led
      digitalWrite(LEDR, !digitalRead(LEDR));
      break;
    default:
      // default - turn off led
      digitalWrite(LEDR, HIGH);
      break; 
  } 
  // LEDB control - motion state
  switch (LEDBState)
  {
    case 0:
      // turn off led
      digitalWrite(LEDB, HIGH); // active low
      break;
    case 1:
      // turn on led
      digitalWrite(LEDB, LOW);
      break;
    case 2:
      // blink led
      digitalWrite(LEDB, !digitalRead(LEDB));
      break;
    default:
      // default - turn off led
      digitalWrite(LEDB, HIGH);
      break; 
  } 
return;
}

// processor software reset 
void nanoReset()  // runs at end of 5 second nanoReset timer
{
  // send notification
  strcpy(notification_str, error_type_display_str);
  Blynk.logEvent("alarm_restarted", String(notification_str)); 
  // above log restart in timeline nnnnn
  Serial.println(notification_str);
  ESP.restart (); 
}

void updateRTC()
{
configTime(k_GMT_int *3600, 0, ntpServer);
//configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
// change to dayligh savings time by changing GMT offset to -7 from -8
//struct tm timeinfo; - defined above 
if(!getLocalTime(&timeinfo)){
  terminal.println("Failed to obtain time");
  }
rtc.setTimeStruct(timeinfo); 
// printLocalTime();
return;
}

void updateDate()
{
  // get date data
  years_int = rtc.getYear(); // 4 digits 2021
  //sprintf(years_str, "%4d", years_int); 
  sprintf(years_str, "%2d", years_int);  // 2 digits 21 ?
  months_int = rtc.getMonth();  // returns 0-11
  months_int++;
  sprintf(months_str, "%02d", months_int);
  // above converts to 2 character decimal base - pads leading 0s by adding the 0
  days_int = rtc.getDay(); // returns 1-31
  sprintf(days_str, "%02d", days_int);
  strcpy(date_str, years_str);
  strcat(date_str, "-");
  strcat(date_str, months_str);
  strcat(date_str, "-");
  strcat(date_str, days_str);
}  

void updateTime()
{
  //get time data
  hour_int = rtc.getHour(true); // true is 24 hour time 0-23
  sprintf(hour_str, "%02d", hour_int); 
  // above converts to 2 character decimal base - pads leading 0s by adding the 0
  minute_int = rtc.getMinute(); // 0-59
  sprintf(minute_str, "%02d", minute_int); 
  second_int = rtc.getSecond();
  sprintf(second_str, "%02d", second_int); 
  strcpy(time_str, hour_str);
  strcat(time_str, ":");
  strcat(time_str, minute_str);
  strcat(time_str, ":");
  strcat(time_str, second_str);
}  

void EEPROMWrite()
{
// note - function changing these variables must also update
//   the working variables - e.g. k_GMT_int and k_GMT_str
// store signature first
flash.eeprom_signature = WRITTEN_SIGNATURE;
strcpy(flash.eeprom_ssid, ssid);
strcpy(flash.eeprom_pass, pass);
strcpy(flash.eeprom_k_GMT, k_GMT_str);
strcpy(flash.eeprom_enableStartHours, enableStartHours_str);
strcpy(flash.eeprom_enableEndHours, enableEndHours_str);
strcpy(flash.eeprom_activeTime, activeTime_str);
EEPROM.put(storedAddress, flash);
EEPROM.commit();
EEPROMRead();
}

// read eeprom
void EEPROMRead()
{
EEPROM.get(storedAddress, signature);
// If the EEPROM is written, then there is a written signature
if (signature == WRITTEN_SIGNATURE){
  EEPROM.get(storedAddress, flash);
  // Print a confirmation of the EEPROM data
  terminal.println("Data read from EEPROM:  ");
  terminal.print("ssid ");
  terminal.println(flash.eeprom_ssid); 
  terminal.print("pass: ");  
  terminal.println(flash.eeprom_pass);  
  terminal.print("k_GMT: ");
  terminal.println(flash.eeprom_k_GMT); 
  terminal.print("enableStartHours: ");
  terminal.println(flash.eeprom_enableStartHours);
  terminal.print("enableEndHours: ");
  terminal.println(flash.eeprom_enableEndHours);
  terminal.print("activeTime: ");
  terminal.println(flash.eeprom_activeTime);
  terminal.println();
  // convert eeprom data to strings and numbers used in the program
  strcpy(ssid, flash.eeprom_ssid);
  strcpy(pass, flash.eeprom_pass);
  strcpy(k_GMT_str, flash.eeprom_k_GMT);
  strcpy(enableStartHours_str, flash.eeprom_enableStartHours);
  strcpy(enableEndHours_str, flash.eeprom_enableEndHours);
  strcpy(activeTime_str, flash.eeprom_activeTime); 
  k_GMT_int = atoi(flash.eeprom_k_GMT);
  enableStartHours_int = atoi(flash.eeprom_enableStartHours);
  enableEndHours_int = atoi(flash.eeprom_enableEndHours);
  activeTime_int = atoi(flash.eeprom_activeTime); 
  activeTime_ul = (long)activeTime_int; // and then to unsigned long
  }
else { // eeprom is not written and needs to be written
  terminal.println("EEPROM does not contain data.");
  }
terminal.flush();
Blynk.run();
return;
}  

// blinks Nano ESP32 on board yellow LED
void blinkOnBoardLED() 
{
if (onBoardLEDValue == 1) {
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  onBoardLEDValue = 0;
  }
else {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  onBoardLEDValue = 1;
  }
}

void updateMode()
{
switch(mode){
  case 0: mode_0(); break;
  case 1: mode_1(); break;
  case 2: mode_2(); break;
  case 3: mode_3(); break;
  case 4: mode_4(); break;
  case 5: mode_5(); break;
  case 6: mode_6(); break;
  case 7: mode_7(); break;
  case 8: mode_8(); break;
  case 9: mode_9(); break;
  default: return;
  }
}

void readThermistor()
{
// default analog read resolution is 12 bits 0-4096
thermistorValue = analogRead(thermistorPin); 
// 1600 = 110deg, 1700 = 100deg , 1800 = 90deg, 1900 = 80deg  approx
thermistorTemp = 80 + ((1900 - thermistorValue) / 10); // approx
if (thermistorReportFlag == 1){
  terminal.print(thermistorValue); // for testing
  terminal.print("  ");
  terminal.print(thermistorTemp);
  terminal.println(" deg");
  terminal.flush();  // output to terminal immediately
  Blynk.run(); 
  }
}

// delays that maintain on board led blinking  
//   and WDT clearing
void delay25ms(){ 
  currentMillis = millis();
  if(currentMillis - previousMillis_blinkOnBoardLED >= 1000) {
    previousMillis_blinkOnBoardLED = currentMillis;  // Remember the time
    blinkOnBoardLED();
    } 
  delay(25);
  esp_task_wdt_reset();
  }

void delay30ms(){delay(5); delay25ms();}
void delay50ms(){delay(25); delay25ms();}
void delay75ms(){delay(50); delay25ms();}
void delay100ms(){delay(75); delay25ms();}
void delay150ms(){delay(100); delay50ms();}
void delay200ms(){delay(175); delay25ms();}
void delay250ms(){delay(225); delay25ms();}
void delay300ms(){delay(275); delay25ms();}
void delay350ms(){delay(325); delay25ms();}
void delay400ms(){delay(375); delay25ms();}
void delay450ms(){delay(425); delay25ms();}
void delay500ms(){delay250ms(); delay250ms();}
void delay600ms(){delay300ms(); delay300ms();}
void delay700ms(){delay350ms(); delay350ms();}
void delay750ms(){delay500ms(); delay250ms();}
void delay800ms(){delay400ms(); delay400ms();}
void delay900ms(){delay450ms(); delay450ms();}
void delay1s(){delay500ms(); delay500ms();}
void delay2s(){delay1s(); delay1s();}
void delay3s(){delay2s(); delay1s();}
void delay4s(){delay2s(); delay2s();}
void delay5s(){delay3s(); delay2s();}
void delay6s(){delay4s(); delay2s();}
void delay7s(){delay4s(); delay3s();}
void delay8s(){delay4s(); delay4s();}
void delay9s(){delay5s(); delay4s();}
void delay10s(){delay5s(); delay5s();}

/*
In the IDE Tools menu, enter Pin Numbering and choose By GPIO number (legacy);
Make sure the sketch always uses labels to refer to pins. If you used the number 2, 
replace this with the symbol D2 everywhere.  This will switch to a more library-compatible 
scheme and avoid the above confusion.  Do not include GPIO in the number. 
See pin table below.

Nano	ESP32
D0	GPIO44  reported that this has messages from the bootloader on boot - casued problems on RRS
D1	GPIO43  reported that this has messages from the bootloader on boot - caused problems on RRS
D2	GPIO5  reported that this esp32 pin 5 may conflict with use of eeprom - have not seen this to date.
D3	GPIO6
D4	GPIO7
D5	GPIO8
D6	GPIO9  reported do not want to use gpi0 6-11 as also used for integrated flash?? eeprom?? - have not seen this to date. 
D7	GPIO10
D8	GPIO17
D9	GPIO18
D10	GPIO21
D11	GPIO38
D12	GPIO47
D13	GPIO48  also built in led and SPI clock and used with Blynk for link indication
A0	GPIO1
A1	GPIO2
A2	GPIO3
A3	GPIO4
A4	GPIO11
A5	GPIO12
A6	GPIO13
A7	GPIO14
BOOT0	GPIO46  also Red on rgb led 
BOOT1	GPIO0   also Green on rgb led

GPIO 45 is not on board pinout but is Blue on rgb 
Note - some early boards (not mine) have different rgb colors

w5500 Ethernet module uses 5, 16-19 and 23, so could be a problem with this board as 16 and 19 and 23 have no outputs.
use uno and ethernet board instead?  could these be reassigned?

*/

