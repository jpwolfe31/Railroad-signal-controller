void menu() // main waterfall command menu
{ 
  // return if command already executed
  if (strcmp(cmd_str, "cleared") == 0) return;
  // get command length used for decoding below
  cmd_length_int = strlen(cmd_str);  // note - does not include '\0'.
  
  // page two commands start first
  // for second line Wifi ssid
    if ((serial_second_line_flag_int == 2) && (strcmp(cmd_str, "") != 0)) { // returns 0 if equal
     if (strlen(cmd_str) > 15) { // note- null character ‘\0’ not counted
       terminal.println ("Invalid entry");
       terminal.println();  // add line feed
       terminal.flush();
       Blynk.run();
       serial_second_line_flag_int = 0; // reset file read flag 
       return;
       }
     strcpy(ssid, cmd_str);
     terminal.print("Wifi SSID changed to: ");
     terminal.println(ssid); 
     terminal.println();  // add line feed
     terminal.flush();
     Blynk.run();
     EEPROMWrite(); 
     serial_second_line_flag_int = 0; // reset file read flag 
     return;
     }
  // for second line Wifi password
    if ((serial_second_line_flag_int == 3) && (strcmp(cmd_str, "") != 0)) { // returns 0 if equal
     if (strlen(cmd_str) > 15) { // note- null character ‘\0’ not counted
       terminal.println ("Invalid entry");
       terminal.println();  // add line feed
       terminal.flush();
       Blynk.run();
       serial_second_line_flag_int = 0; // reset file read flag 
       return;
       }
     strcpy(pass, cmd_str);
     terminal.print("Wifi password changed to: ");
     terminal.println(pass); 
     terminal.println();  // add line feed
     terminal.flush();
     Blynk.run();
     EEPROMWrite();
     serial_second_line_flag_int = 0; // reset file read flag 
     return;
     }
  // for second line GMT offset
    if ((serial_second_line_flag_int == 4) && (strcmp(cmd_str, "") != 0)) { // returns 0 if equal
      int l = atoi(cmd_str);
      if ((l > 12) || (l < -12)) {
        terminal.println ("Invalid entry");
        terminal.println();  // add line feed
        terminal.flush();
        Blynk.run();
        serial_second_line_flag_int = 0; // reset file read flag 
        return;
        }
      strcpy(k_GMT_str, cmd_str);
      k_GMT_int = atoi(k_GMT_str);
      EEPROMWrite();
      serial_second_line_flag_int = 0; // reset file read flag
      updateRTC();
      // read new time
      updateTime();
      //terminal.print("GMT offset changed to: ");
      //terminal.println(k_GMT_str); 
      //terminal.println("Clock updated"); 
      terminal.println(); // add line feed
      terminal.flush();
      Blynk.run();
      return;
      }
    // for second line enable start hours
    if ((serial_second_line_flag_int == 5) && (strcmp(cmd_str, "") != 0)) { // returns 0 if equal
      int l = atoi(cmd_str);
      if ((l < 0) || (l > 24)) {
        terminal.println ("Invalid entry");
        terminal.println();  // add line feed
        terminal.flush();
        Blynk.run();
        serial_second_line_flag_int = 0; // reset file read flag 
        return;
        }
      strcpy(enableStartHours_str, cmd_str);
      terminal.print("Enable start hour changed to: ");
      terminal.println(enableStartHours_str); 
      enableStartHours_int = atoi(enableStartHours_str);
      terminal.print("Integer is: ");
      terminal.println(enableStartHours_int);  
      terminal.println();  // add line feed
      terminal.flush();
      Blynk.run();
      EEPROMWrite();
      serial_second_line_flag_int = 0; // reset file read flag
      return;
      }
  // for second line enable end hours
    if ((serial_second_line_flag_int == 6) && (strcmp(cmd_str, "") != 0)) { // returns 0 if equal
      int l = atoi(cmd_str);
      if ((l < 0) || (l > 24)) {
        terminal.println ("Invalid entry");
        terminal.println();  // add line feed
        serial_second_line_flag_int = 0; // reset file read flag 
        return;
        }
      strcpy(enableEndHours_str, cmd_str);
      terminal.print("Enable end hour changed to: ");
      terminal.println(enableEndHours_str); 
      enableEndHours_int = atoi(enableEndHours_str);
      terminal.print("Integer is: ");
      terminal.println(enableEndHours_int);  
      terminal.println();  // add line feed
      terminal.flush();
      Blynk.run();
      EEPROMWrite();
      serial_second_line_flag_int = 0; // reset file read flag
      return;
      }  
  // for second line active time
    if ((serial_second_line_flag_int == 7) && (strcmp(cmd_str, "") != 0)) { // returns 0 if equal
      int l = atoi(cmd_str);
      if ((l < 1) || (l > 1440)) {  // time is in minutes
        terminal.println ("Invalid entry");
        terminal.println();  // add line feed
        serial_second_line_flag_int = 0; // reset file read flag 
        return;
        }
      strcpy(activeTime_str, cmd_str);
      terminal.print("Active time (minutes) changed to: ");
      terminal.println(activeTime_str); 
      activeTime_int = atoi(activeTime_str); // convert string to integer 
      activeTime_ul = (long)activeTime_int; // and then to unsigned long
      terminal.print("Integer is: ");
      terminal.println(activeTime_int);  
      terminal.println();  // add line feed
      terminal.flush();
      Blynk.run();
      EEPROMWrite();
      serial_second_line_flag_int = 0; // reset file read flag
      return;
      }
  esp_task_wdt_reset();  // refresh watch dog timer
  // end page two commands

  if (strcmp(cmd_str, "cmd") == 0) { // list commands
    terminal.println("/v    - set mode to v 0-9");
    terminal.println("     0 1 2 3r 4rf 5y 6yf 7g 8d 9hold"); 
    terminal.println("-0x   - sl0  r0 rf1 y2 yf3 g4 dark5");
    terminal.println("-1x   - sl1  r0 rf1 y2 yf3 g4 dark5");
    terminal.println("-2x   - sl2  r0 rf1 y2 yf3 g4 dark5");
    terminal.println("-3x   - sl3  r0 rf1 y2 yf3 g4 dark5");
    terminal.println("-4x   - gc4  f0 fb1 b2 dark3");
    terminal.println("-5x   - gy5  l0 ml1 m2 dark3");
    terminal.println("-6x   - gy6  l0 ml1 m2 dark3");
    terminal.println("-7x   - cl0  r0 rf1 y2 yf3 g4 dark5");
    terminal.println("-8x   - cl8  r0 rf1 y2 yf3 g4 dark5");
    terminal.println("-9x   - cl9  r0 rf1 y2 yf3 g4 dark5");
    terminal.println("-10x  - cl10 r0 rf1 g4 dark5");
    terminal.println("-11x  - ww11 l0 lm1 m2 dark3");
    terminal.println("-12x  - wwl2 l0 lm1 m2 dark3");
    terminal.println("clr   - local terminal clear");
    terminal.println("cmd   - list available commands");
    terminal.println("c     - Blynk terminal clear");
    terminal.println("clr   - local terminal clear");
    terminal.println("cmd   - list available commands");
    terminal.println("cmdm  - list more commands");
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    } // note 50 max terminal width
 if (strcmp(cmd_str, "cmdm") == 0) { // list more commands
    terminal.println("rst    - reset controller");
    terminal.println("sig    - report WiFi signal strength"); 
    terminal.println("v      - report version of code");
    terminal.println(".pbv   - set port 0-9 and bit 0-3 to v");
    terminal.println("tr     - report time once");
    terminal.println("ts     - report/syncs rtc/WiFi times");
    terminal.println("rth    - report thermistor values (toggle)");
    terminal.println("rms    - report motion states (15s)");
    terminal.println("rfan   - report fan status (toggle)"); 
    terminal.println("rmot   - report motion status (toggle)");
    terminal.println("rrel   - report relay status (toggle)");
    terminal.println("rmod   - report mode and stage (toggle)");
    terminal.println("cat    - cancel active time");
    terminal.println("fon    - fan on");
    terminal.println("foff   - fan off");
    terminal.println("ron    - relay on");
    terminal.println("roff   - relay off");
    terminal.println("st     - report op status"); 
    terminal.println("tled   - test nano leds");
    terminal.println("wdt    - test watchdog timer");
    terminal.println("cssid  - change Wifi SSID (eeprom)");  // second page flag 2
    terminal.println("cpass  - change Wifi password (eeprom)");   // second page flag 3
    terminal.println("cgmto  - change GMT offset (eeprom)");  // second page flag 4
    terminal.println("csthr  - change enable start hour (eeprom)");  // second page flag 5
    terminal.println("cenhr  - change enable end hour (eeprom)");  // second page flag 6
    terminal.println("cactm  - change active time minutes (eeprom)");  // second page flag 7
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if ((cmd_str[0] != '.') && (bitTest == 1)){ // if exiting bitTest mode, set mode to 0
    bitTest = 0; // set mode to 0 to unfreeze normal phased operation
    changeMode(0);
    }
  if (cmd_str[0] == '.'){ // in bitTest mode
    // .pbv - set port and bit to v
    bitTest = 1; // set bitTest to freeze normal phased operation
    // see loop function for functions skipped in normal op freeze
    nibbleNum = cmd_str[1] - 0x30; // 0-9
    int bitNum = cmd_str[2] - 0x30; // 0-3
    int bitVal = cmd_str[3] - 0x30; // 0-1
    if ((nibbleNum > 9) || (bitNum > 3) || (bitVal > 1)) {
      terminal.println("out of range");
      return;
      }
    terminal.println();
    terminal.print(nibbleNum);
    terminal.print(bitNum);
    terminal.println(bitVal);
    terminal.flush();
    Blynk.run();
    // write data
    int dataO = bitWrite(dataNibble[nibbleNum], bitNum, bitVal); // update output value
    dataNibbleOut(nibbleNum, dataO); // output updated port data
    return;
    }
  if (cmd_str[0] == '/'){
    // /v - set mode to v  
    lastCommandMillis = currentMillis; // update time out
    swState = -1; // reset switch
    int modeTerm = cmd_str[1] - 0x30; // modes 0-8
    if (modeTerm > 9) {
      terminal.println("out of range");
      return;
      }
    changeMode(modeTerm); // set modes 0-8 
    return;
    }
  if (cmd_str[0] == '-'){
    int sNum = 0;
    int sCmd = 0;
    lastCommandMillis = currentMillis; // update time out
    swState = -1; // reset switch
    if (strlen(cmd_str) == 3) { // signals 0-9
      sNum = cmd_str[1] - 0x30;
      sCmd = cmd_str[2] - 0x30;
      }
    if (strlen(cmd_str) == 4) {
      sNum = 10 + (cmd_str[2] - 0x30);
      sCmd = cmd_str[3] - 0x30;
      }
    if ((sNum > 12) || (sCmd > 6)) {
      terminal.println("out of range");
      return;
      }
    sigSet(sNum, sCmd);
    return;
    }
  if (strcmp(cmd_str, "rth") == 0) {  //  report thermistor values
    if (thermistorReportFlag == 0){thermistorReportFlag = 1;}
    else thermistorReportFlag = 0;
    return;
    }
  if (strcmp(cmd_str, "rms") == 0) {  //  report motion state (15s)
    terminal.println("Motion State for 15 seconds:");
    for (int j = 0; j < 15; j++){
      motionState = digitalRead(motionPin);  // active high
      terminal.print("  ");
      terminal.println(motionState);
      terminal.flush();  // output to terminal immediately
      //Blynk.run();      
      delay1s();
      //esp_task_wdt_reset();  // refresh watch dog timer
      }
    return;
    }
  if (strcmp(cmd_str, "rfan") == 0) {  //  report fan status
    if (fanReportFlag == 0){fanReportFlag = 1;}
    else fanReportFlag = 0;
    return;
    }
  if (strcmp(cmd_str, "rmot") == 0) {  //  report motion status
    if (motionReportFlag == 0){motionReportFlag = 1;}
    else motionReportFlag = 0;
    return;
    }
  if (strcmp(cmd_str, "rmod") == 0) {  //  report mode stage
    if (reportModeFlag == 0){reportModeFlag = 1;}
    else reportModeFlag = 0;
    return;
    }  
  if (strcmp(cmd_str, "cat") == 0) {  //  cancel active time in 10 seconds
    lastMotionMillis = currentMillis - (activeTime_ul * 60000) + 10000;
    lastCommandMillis = currentMillis - (activeTime_ul * 60000) + 10000;
    return;
    }
  if (strcmp(cmd_str, "ron") == 0) {  //  relay on
    relayOn(); // turn on relay
    return;
    } 
  if (strcmp(cmd_str, "roff") == 0) {  //  relay off
    relayOff(); // turn off relay
    return;
    } 
  if (strcmp(cmd_str, "fon") == 0) {  //  fan on
    fanOn(); // turn on fan
    return;
    }
  if (strcmp(cmd_str, "foff") == 0) {  //  fan off
    fanOff(); // turn off fan
    return;
    } 
  if (strcmp(cmd_str, "tled") == 0){
    // tled - test leds
    terminal.println();
    terminal.println("Testing Nano ESP32 built-in leds for 12 seconds");
    terminal.println();
    terminal.flush();
    Blynk.run();
    for (int iled = 0; iled <4; iled++){
      digitalWrite(LED_BUILTIN, HIGH);  // Note also used by Blynk to show link status active high
      digitalWrite(LEDR, LOW);  // active low red 
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
      delay1s();
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW); // active low green
      digitalWrite(LEDB, HIGH);
      delay1s();
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, LOW); // active low blue
      delay1s();
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH); // off
      digitalWrite(LED_BUILTIN, LOW);
      delay1s();
      }
    delay(50);
    return;
    }
  if (strcmp(cmd_str, "rst") == 0) {  // reset nano
    terminal.println(); // add line feed
    terminal.println("device reset through Blynk terminal");
    // report type of error
    strcpy(error_type_str, "BT");
    nanoError();
    return;
    }
  if (strcmp(cmd_str, "tr") == 0) {  // report time
    // report time data
    terminal.println(); // add line feed
    // terminal.print("nano rtc ");
    // (String) returns time with specified format 
    //terminal.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // for testing
    updateRTC();
    updateDate();
    updateTime();
    terminal.print(date_str);
    terminal.print("  ");
    terminal.println(time_str);
    terminal.println();
    }  
  if (strcmp(cmd_str, "ts") == 0) {  // report and sync rtc and current wifi times
    // get time data
    terminal.println(); // add line feed
    updateTime();
    terminal.print("rtc time was ");
    terminal.println(time_str);
    // update rtc
    updateRTC();
    // read new time
    terminal.println("WiFi and rtc time synced");
    updateTime();
    terminal.print("rtc time is now ");
    terminal.println(time_str);
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "c") == 0) { // Clear the terminal content    // note - returns 0 if equal
    terminal.clear();  // this is the remote clear.  type clr for a local clear.
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "cssid") == 0) { // change Wifi SSID
    terminal.println("Enter new Wifi SSID: ");
    serial_second_line_flag_int = 2;  // set flag for next WifI SSID line read
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "cpass") == 0) { // change Wifi password
    terminal.println("Enter new Wifi password: ");
    serial_second_line_flag_int = 3;  // set flag for next WifI password line read
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "cgmto") == 0) { // change GMT offset
    terminal.println("Enter new GMT offset: ");
    serial_second_line_flag_int = 4;  // set flag for next GMT offset line read
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "csthr") == 0) { // change enable start hour
    terminal.println("Enter new enable start hour: ");
    serial_second_line_flag_int = 5;  // set flag for next enable start hour line read
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "cenhr") == 0) { // change enable end hour
    terminal.println("Enter new enable end hour: ");
    serial_second_line_flag_int = 6;  // set flag for next enable end hour line read
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
 if (strcmp(cmd_str, "cactm") == 0) { // change active time
    terminal.println("Enter new active time (minutes): ");
    serial_second_line_flag_int = 7;  // set flag for next active time line read
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "v") == 0) {  // report version
    terminal.println(); // add line feed
    terminal.print("Version of Controller Code is: ");
    terminal.println(version_str);
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "sig") == 0) {  // report wifi signal strength
    rssi = WiFi.RSSI();
    terminal.println(); // add line feed
    terminal.print("Signal strength (RSSI) is ");
    terminal.print(rssi);
    terminal.println(" dBm");
    terminal.println(); // add line feed
    terminal.flush();
    Blynk.run();
    return;
    }
  if (strcmp(cmd_str, "wdt") == 0) {  // check wdt function
    unsigned int t;
    terminal.println("\nWatchdog Test - run 18 seconds with a WDTimer.clear()\n");
    //Serial.println("\nWatchdog Test - run 18 seconds with a WDT.clear()\n");
    for (t = 1; t <= 18; ++t) {
      esp_task_wdt_reset();  // refresh wdt - before it loops
      delay(950);
      terminal.print(t);
      terminal.print(".");
      terminal.flush();
      Blynk.run(); 
      }
    terminal.println("\n\nWatchdog Test - free run wait for reset at 8 seconds\n");
    for (t = 1; t >= 1; ++t) {
      delay(950);
      terminal.print(t);
      terminal.print(".");
      terminal.flush();
      Blynk.run();
      }
    return;
    }   
  if (strcmp(cmd_str, "st") == 0) {
    terminal.print("thermistorValue = ");
    terminal.print(thermistorValue); 
    terminal.print("   ");
    terminal.print(thermistorTemp);
    terminal.println(" deg F");
    terminal.print("mode = ");
    terminal.println(mode);    
    terminal.print("modeStage = ");
    terminal.println(modeStage);    
    terminal.print("swState = ");
    terminal.println(swState);
    terminal.print("LEDBState (motion) = ");
    terminal.println(LEDBState);
    terminal.print("fanState = ");
    terminal.println(fanState); 
    terminal.print("relayState = ");
    terminal.println(relayState); 
    terminal.print("uptime = ");
    terminal.print(millis() / 60000);
    terminal.println(" minutes");
    // activeTime is in minutes
    terminal.print("activeTime = ");
    terminal.print(activeTime_ul);
    terminal.println(" minutes");
    currentMillis = millis();
    terminal.print("remaining time motion(sec) = ");
    // activeTime is in minutes
    if (lastMotionMillis != 0){ // note - if no motion, then lastMotionMillis will be 0
      // Note - min used to avoid negative number
      terminal.println(((activeTime_ul * 60000)- min((currentMillis - lastMotionMillis), (activeTime_ul * 60000)))/1000);
      }
    else {terminal.println(0);}
    terminal.print("remaining time command(sec) = "); // above calcuation returns seconds due to /1000
    terminal.println(((activeTime_ul * 60000)- min((currentMillis - lastCommandMillis), (activeTime_ul * 60000)))/1000);
    terminal.print("ssid = ");
    terminal.println(ssid);
    terminal.print("pass = ");
    terminal.println(pass);
    terminal.print("GMT offset = ");
    terminal.println(k_GMT_int);
    terminal.print("enableStartHours = ");
    terminal.println(enableStartHours_int);
    terminal.print("enableEndHours = ");
    terminal.println(enableEndHours_int);
    terminal.println(); // add line
    terminal.flush();
    Blynk.run();
    return;
    }
  
// end of command waterfall
}

// for push buttons on Blynk app
BLYNK_WRITE(V0)  // multiple keys for mode
{
modeValue = param.asInt();
}

BLYNK_WRITE(V1)  // pull down for signal
{
signalValue = param.asInt(); 
}

BLYNK_WRITE(V2)  
{
commandValue = param.asInt(); // pull down for signal
}

BLYNK_WRITE(V4)  
{
enterValue = param.asInt(); // enter switch for signal pull down menus
}

BLYNK_WRITE(V5)  
{
_4BValue = param.asInt(); 
}

BLYNK_WRITE(V6)  
{
_5MValue = param.asInt(); 
}

BLYNK_WRITE(V7)  
{
_6MValue = param.asInt(); 
}

BLYNK_WRITE(V8)  
{
_11MValue = param.asInt(); 
}

BLYNK_WRITE(V9)  
{
_12MValue = param.asInt(); 
}

// for Blynk terminal commands
BLYNK_WRITE(V3) // for reading terminal commands written from Blynk
{
strcpy(cmd_str, param.asStr());  // copy Blynk app terminal input to cmd_str  
//terminal.println(cmd_str); // for testing
//terminal.flush();
//Blynk.run();
}

