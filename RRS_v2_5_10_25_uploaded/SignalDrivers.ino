// Signal Drivers

// during each phase:
//   blynk and swith inputs are read
//   mode is set or maintained 
//   mode timer is updated 
//   modes 0-3 have several delay stages - others run through one time.
//   mode 9 is entered into automatically when signals are manully set through the blynk app
//   mode 8 is the dark mode to reduce current load and heat

// configuraiton bit values unique to each signal are set in stdCmd and altCmd
// the updateOutputs function uses the stdCmd and altCmd configurations to generate output for each signal based on phase
// these outputs are then updated by dataNibbleOut to the Nano-ESP32 data ports D5-2 and 9-6 and the legacy 8243 output expanders
// 3 output bits (possibly 4 if OB_Y_LED is not used, or 6 if 2 spare 8243 outputs are also used, or many many more
//   if a MCP23017 i2c port expander is used after logic connector H3) are available for controlling additional signals

// one of mode_0() throgh mode_9() is called every phase and takes action based on elapsed time
void mode_0()  
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  // reset mode timer
  modeStartMillis = millis(); 
  modeStage = 0; 
  }
modeCurrentMillis = millis(); // check time in mode loop
// starts at modeStage 0
if (modeCurrentMillis - modeStartMillis >= 10000) {modeStage = 1;}
if (modeCurrentMillis - modeStartMillis >= 12000) {modeStage = 2;}
if (modeCurrentMillis - modeStartMillis >= 22000) {modeStage = 3;}
if (modeCurrentMillis - modeStartMillis >= 24000) {modeStage = 4;}
if (modeCurrentMillis - modeStartMillis >= 34000) {modeStage = 5;}
if (modeCurrentMillis - modeStartMillis >= 36000) {modeStage = 6;}
if (modeCurrentMillis - modeStartMillis >= 46000) {modeStage = 7;}
if (modeCurrentMillis - modeStartMillis >= 48000) {modeStage = 8;}
if (modeCurrentMillis - modeStartMillis >= 50000) {modeStage = 9;}
if (modeCurrentMillis - modeStartMillis >= 60000) {modeStage = 10;}
if (modeCurrentMillis - modeStartMillis >= 70000) {modeStage = 11;}
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
switch(modeStage) { // select stage based om time elapsed in loop
  case 0:   // load first stage
    //
    gc4_F();
    gy5_Lon();  // must address all 13 signal types
    gy6_Lon();
    ww11_Lon();
    ww12_Lon();
    sl0_R();
    cl7_R();
    sl1_R();
    cl8_R();
    sl2_R();
    cl9_R();
    sl3_R();
    cl10_R();
    break;
    // d10s
  case 1: 
    sl0_RF();
    cl7_RF();
    break;
    // d2s
  case 2:
    sl1_RF();
    cl8_RF();
    sl2_RF();
    cl9_RF();
    sl3_RF();
    cl10_RF();
    gc4_F();
    break;
    // d10s 
  case 3: 
    sl0_Y();
    cl7_Y();
    break;
    //d2s
  case 4:
    sl1_Y();
    cl8_Y();
    sl2_Y();
    cl9_Y();
    sl3_Y();
    cl10_D();
    gc4_F();
    break;
    // d10s 
  case 5:
    sl0_YF();
    cl7_YF();
    break;
    // d2s
  case 6:
    sl1_YF();
    cl8_YF();
    sl2_YF();
    cl9_YF();
    sl3_YF();
    cl10_D();
    gc4_D();
    break;
    // d10s 
  case 7:
    sl0_G();
    cl7_G();
    break;
    // d2s
  case 8:
    sl1_G();
    cl8_G();
    break;
    // d2s
  case 9:
    sl2_G();
    cl9_G();
    break;
    //d2s
  case 10:
    sl3_G();
    cl10_G();
    gc4_D();
    break;
    //d10s
  case 11:
    // last stage is control loop
    modeStartMillis = millis(); 
    modeStage = 0;
    break;
  }
return;
}

void mode_1()
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStartMillis = millis(); 
  modeStage = 0; 
  }
modeCurrentMillis = millis(); // check time in mode loop
// modeStage = 0 is default
if (modeCurrentMillis - modeStartMillis >= 10000) {modeStage = 1;}
if (modeCurrentMillis - modeStartMillis >= 12000) {modeStage = 2;}
if (modeCurrentMillis - modeStartMillis >= 22000) {modeStage = 3;}
if (modeCurrentMillis - modeStartMillis >= 24000) {modeStage = 4;}
if (modeCurrentMillis - modeStartMillis >= 34000) {modeStage = 5;}
if (modeCurrentMillis - modeStartMillis >= 36000) {modeStage = 6;}
if (modeCurrentMillis - modeStartMillis >= 46000) {modeStage = 7;}
if (modeCurrentMillis - modeStartMillis >= 51000) {modeStage = 8;}
if (modeCurrentMillis - modeStartMillis >= 56000) {modeStage = 9;}
if (modeCurrentMillis - modeStartMillis >= 61000) {modeStage = 10;}
if (modeCurrentMillis - modeStartMillis >= 71000) {modeStage = 11;} 
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
switch(modeStage) { // select stage based om time elapsed in loop
  case 0:   // load first stage
    //
    gc4_F();
    gy5_Lon();
    gy6_Lon();
    ww11_Lon();
    ww12_Lon();
    sl0_R();
    cl7_R();
    sl1_Y();
    cl8_Y();
    sl2_G();
    cl9_G();
    sl3_R();
    cl10_R();
    break;
    // d10s
  case 1:
    sl0_RF();
    cl7_RF();
    break;
    // d2s
  case 2:
    sl1_YF();
    cl8_YF();
    sl2_R();
    cl9_R();
    sl3_RF();
    cl10_R();
    gc4_F();
    break;
    // d10s 
  case 3:
    sl0_YF();
    cl7_YF();
    break;
    // d2s
  case 4:
    sl1_R();
    cl8_R();
    sl2_Y();
    cl9_Y();
    sl3_YF();
    cl10_D();
    gc4_F();
    break;
    // d10s 
  case 5:
    sl0_G();
    cl7_G();
    break;
    // d2s
  case 6:
    sl1_RF();
    cl8_RF();
    sl2_YF();
    cl9_YF();
    sl3_G();
    cl10_G();
    gc4_D();
    break;
    // d10s 
  case 7:
    sl0_R();
    cl7_R();
    break;
    // d2s
  case 8:
    sl1_Y();
    cl8_Y();
    break;
    // d5s
  case 9:
    sl2_G();
    cl9_G();
    break;
    //d5s
  case 10:
    sl3_R();
    cl10_R();
    gc4_D();
    break;
    // d10s
  case 11:
    // last stage is control loop
    modeStartMillis = millis(); 
    modeStage = 0;
    break;
  }
return;
}

void mode_2() // update as almost the same as mode 1 now
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStartMillis = millis(); 
  modeStage = 0; 
  }
modeCurrentMillis = millis(); // check time in mode loop
// modeStage = 0 is default
if (modeCurrentMillis - modeStartMillis >= 10000) {modeStage = 1;}
if (modeCurrentMillis - modeStartMillis >= 12000) {modeStage = 2;}
if (modeCurrentMillis - modeStartMillis >= 22000) {modeStage = 3;}
if (modeCurrentMillis - modeStartMillis >= 24000) {modeStage = 4;}
if (modeCurrentMillis - modeStartMillis >= 34000) {modeStage = 5;}
if (modeCurrentMillis - modeStartMillis >= 36000) {modeStage = 6;}
if (modeCurrentMillis - modeStartMillis >= 46000) {modeStage = 7;}
if (modeCurrentMillis - modeStartMillis >= 51000) {modeStage = 8;}
if (modeCurrentMillis - modeStartMillis >= 56000) {modeStage = 9;}
if (modeCurrentMillis - modeStartMillis >= 61000) {modeStage = 10;}
if (modeCurrentMillis - modeStartMillis >= 71000) {modeStage = 11;} 
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
switch(modeStage) { // select stage based om time elapsed in loop
  case 0:   // load first stage
    //
    gc4_F();
    gy5_Lon();
    gy6_Lon();
    ww11_Lon();
    ww12_Lon();
    sl0_G();
    cl7_G();
    sl1_G();
    cl8_G();
    sl2_G();
    cl9_G();
    sl3_G();
    cl10_G();
    break;
    // d10s
  case 1:
    sl0_RF();
    cl7_RF();
    break;
    // d2s
  case 2:
    sl1_YF();
    cl8_YF();
    sl2_R();
    cl9_R();
    sl3_RF();
    cl10_R();
    gc4_F();
    break;
    // d10s 
  case 3:
    sl0_YF();
    cl7_YF();
    break;
    // d2s
  case 4:
    sl1_R();
    cl8_R();
    sl2_Y();
    cl9_Y();
    sl3_YF();
    cl10_D();
    gc4_F();
    break;
    // d10s 
  case 5:
    sl0_G();
    cl7_G();
    break;
    // d2s
  case 6:
    sl1_RF();
    cl8_RF();
    sl2_YF();
    cl9_YF();
    sl3_G();
    cl10_G();
    gc4_D();
    break;
    // d10s 
  case 7:
    sl0_R();
    cl7_R();
    break;
    // d2s
  case 8:
    sl1_Y();
    cl8_Y();
    break;
    // d5s
  case 9:
    sl2_G();
    cl9_G();
    break;
    //d5s
  case 10:
    sl3_R();
    cl10_R();
    gc4_D();
    break;
    // d10s
  case 11:
    // last stage is control loop
    modeStartMillis = millis(); 
    modeStage = 0;
    break;
  }
return;
}

void mode_3()
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStage = 0; // when changing stages, modeLastStage is set to -1
  }
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
gc4_F();  // this will run one time
gy5_Lon();
gy6_Lon();
ww11_Lon();
ww12_Lon();
sl0_R();
cl7_R();
sl1_R();
cl8_R();
sl2_R();
cl9_R();
sl3_R();
cl10_R();
return;
}

void mode_4()
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStage = 0; // when changing stages, modeLastStage is set to -1
  }
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
gc4_F();  // this will run one time
gy5_Lon();
gy6_Lon();
ww11_Lon();
ww12_Lon();
sl0_RF();
cl7_RF();
sl1_RF();
cl8_RF();
sl2_RF();
cl9_RF();
sl3_RF();
cl10_RF();
return;
}

void mode_5()
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStage = 0; // when changing stages, modeLastStage is set to -1
  }
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
gc4_F();  // this will run one time
gy5_Lon();
gy6_Lon();
ww11_Lon();
ww12_Lon();
sl0_Y();
cl7_Y();
sl1_Y();
cl8_Y();
sl2_Y();
cl9_Y();
sl3_Y();
cl10_D();
return;
}

void mode_6()
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStage = 0; // when changing stages, modeLastStage is set to -1
  }
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
gc4_F();  // this will reun one time
gy5_Lon();
gy6_Lon();
ww11_Lon();
ww12_Lon();
sl0_YF();
cl7_YF();
sl1_YF();
cl8_YF();
sl2_YF();
cl9_YF();
sl3_YF();
cl10_D();
return;
}

void mode_7()
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStage = 0; // when changing stages, modeLastStage is set to -1
  }
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
gc4_F();  // this will run one time
gy5_Lon();
gy6_Lon();
ww11_Lon();
ww12_Lon();
sl0_G();
cl7_G();
sl1_G();
cl8_G();
sl2_G();
cl9_G();
sl3_G();
cl10_G();
return;
}

void mode_8() // dark
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStage = 0; // when changing stages, modeLastStage is set to -1
  }
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
gc4_D();  // this will run one time
gy5_MLoff();
gy6_MLoff();
ww11_MLoff();
ww12_MLoff();
sl0_RD();
cl7_D();
sl1_RD();
cl8_D();
sl2_RD();
cl9_D();
sl3_RD();
cl10_D();
return;
}

void mode_9() // hold
{
if (mode != modeLast){ // check if new mode
  modeLast = mode; // save mode
  modeStage = 0; // when changing stages, modeLastStage is set to -1
  }
if (modeStage == modeLastStage) {return;} // no need to repeat stages
else {
  modeLastStage = modeStage;
  reportModeStage();
  }
// no signal changes, just hold
return;
}

void  reportModeStage()
{  
if (reportModeFlag == 1){
  terminal.print("mode  ");
  terminal.print(mode);
  terminal.print("-");
  terminal.println(modeStage);
  terminal.flush();
  Blynk.run();  
  }
}

void sigSet(int sigNum, int sigCmd)
  {
  changeMode(9); // change mode to 9 hold to freeze any program signal changes
  switch(sigNum){
      case 0:
        switch(sigCmd){
          case 0:
            sl0_R(); 
            break;  
          case 1:
            sl0_RF(); 
            break;  
          case 2:
            sl0_Y(); 
            break;  
          case 3:
            sl0_YF(); 
            break;  
          case 4:
            sl0_G(); 
            break;  
          case 5:
            sl0_RD(); 
            break;  
          }
        break;  
      case 1:     
        switch(sigCmd){
          case 0:
            sl1_R(); 
            break;  
          case 1:
            sl1_RF(); 
            break;  
          case 2:
            sl1_Y(); 
            break;  
          case 3:
            sl1_YF(); 
            break;  
          case 4:
            sl1_G(); 
            break;  
          case 5:
            sl1_RD(); 
            break;  
          }
        break;
      case 2:     
        switch(sigCmd){
          case 0:
            sl2_R(); 
            break;  
          case 1:
            sl2_RF(); 
            break;  
          case 2:
            sl2_Y(); 
            break;  
          case 3:
            sl2_YF(); 
            break;  
          case 4:
            sl2_G(); 
            break;  
          case 5:
            sl2_RD(); 
            break;  
          }
          break;
      case 3:     
        switch(sigCmd){
          case 0:
            sl3_R(); 
            break;  
          case 1:
            sl3_RF(); 
            break;  
          case 2:
            sl3_Y(); 
            break;  
          case 3:
            sl3_YF(); 
            break;  
          case 4:
            sl3_G(); 
            break;  
          case 5:
            sl3_RD(); 
            break;  
          }
        break;
      case 4:     
        switch(sigCmd){
          case 0:
            gc4_F(); 
            break;  
          case 1:
            gc4_FB(); 
            break;  
          case 2:
            gc4_DB(); 
            break;  
          case 3:
            gc4_D(); 
            break;  
          //case 4:
          //  gc4_L(); 
          //  break;  
          //case 5:
          //  gc4_R();
          //  break; 
          //case 6:
          //  gc4_LR();
          //  break;  
          }
        break;
      case 5:     
        switch(sigCmd){
          case 0:
            gy5_Lon(); 
            break;  
          case 1:
            gy5_MLon(); 
            break;  
          case 2:
            gy5_Mon(); 
            break;  
          case 3:
            gy5_MLoff(); 
            break;  
          }
        break;         
      case 6:     
        switch(sigCmd){
          case 0:
            gy6_Lon(); 
            break;  
          case 1:
            gy6_MLon(); 
            break;  
          case 2:
            gy6_Mon(); 
            break;  
          case 3:
            gy6_MLoff(); 
            break;  
          }
        break; 
      case 7:
        switch(sigCmd){
          case 0:
            cl7_R(); 
            break;  
          case 1:
            cl7_RF(); 
            break;  
          case 2:
            cl7_Y(); 
            break;  
          case 3:
            cl7_YF(); 
            break;  
          case 4:
            cl7_G(); 
            break;  
          case 5:
            cl7_D(); 
            break;  
          }
        break;            
      case 8:
        switch(sigCmd){
          case 0:
            cl8_R(); 
            break;  
          case 1:
            cl8_RF(); 
            break;  
          case 2:
            cl8_Y(); 
            break;  
          case 3:
            cl8_YF(); 
            break;  
          case 4:
            cl8_G(); 
            break;  
          case 5:
            cl8_D(); 
            break;  
          }
        break;  
       case 9:
        switch(sigCmd){
          case 0:
            cl9_R(); 
            break;  
          case 1:
            cl9_RF(); 
            break;  
          case 2:
            cl9_Y(); 
            break;  
          case 3:
            cl9_YF(); 
            break;  
          case 4:
            cl9_G(); 
            break;  
          case 5:
            cl9_D(); 
            break;  
          }
        break;   
      case 10:
        switch(sigCmd){  // no yellow so no case 2, 3
          case 0:
            cl10_R(); 
            break;  
          case 1:
            cl10_RF(); 
            break;  
          case 4:
            cl10_G(); 
            break;  
          case 5:
            cl10_D(); 
            break;  
          }
        break;     
      case 11:     
        switch(sigCmd){
          case 0:
            ww11_Lon(); 
            break;  
          case 1:
            ww11_MLon(); 
            break;  
          case 2:
            ww11_Mon(); 
            break;  
          case 3:
            ww11_MLoff(); 
            break;  
          }
        break; 
      case 12:     
        switch(sigCmd){
          case 0:
            ww12_Lon(); 
            break;  
          case 1:
            ww12_MLon(); 
            break;  
          case 2:
            ww12_Mon(); 
            break;  
          case 3:
            ww12_MLoff(); 
            break;  
          }
        break; 
        }
  terminal.print("Signal ");
  terminal.print(sigNum);
  terminal.print("   ");
  terminal.print("Command ");
  terminal.print(sigCmd);
  terminal.println();
  terminal.flush();
  Blynk.run();
  return;
}

// Signal configuration for ouput driver
// sl0 port 1 - phase 1, 4
//   format - fl ph ph ph G Y GL! RL! (active low !)
void sl0_R() {stdCmd[0] = 0b00010010;} 
void sl0_RD(){stdCmd[0] = 0b00010011;}
void sl0_Y() {stdCmd[0] = 0b00010100;} 
void sl0_G() {stdCmd[0] = 0b00011001;}
void sl0_RF(){stdCmd[0] = 0b11000011; altCmd[0] = 0b00010010;} // flashes start with dark light (looks better)
void sl0_YF(){stdCmd[0] = 0b11000111; altCmd[0] = 0b00010100;} 
void sl0_GF(){stdCmd[0] = 0b11001011; altCmd[0] = 0b00011001;} 

// sl1 port 7 - phase 2, 5
//   format - fl ph ph ph G Y GL! RL! (active low !)
void sl1_R() {stdCmd[1] = 0b00100010;} 
void sl1_RD(){stdCmd[1] = 0b00100011;}
void sl1_Y() {stdCmd[1] = 0b00100100;} 
void sl1_G() {stdCmd[1] = 0b00101001;}
void sl1_RF(){stdCmd[1] = 0b11010011; altCmd[1] = 0b000100010;} // flashes start with dark light (looks better)
void sl1_YF(){stdCmd[1] = 0b11010111; altCmd[1] = 0b000100100;} 
void sl1_GF(){stdCmd[1] = 0b11011011; altCmd[1] = 0b000101001;} 

// sl2 port 2 - phase 4, 1
//   format - fl ph ph ph G Y GL! RL! (active low !)
void sl2_R() {stdCmd[2] = 0b01000010;} 
void sl2_RD(){stdCmd[2] = 0b01000011;}
void sl2_Y() {stdCmd[2] = 0b01000100;} 
void sl2_G() {stdCmd[2] = 0b01001001;}
void sl2_RF(){stdCmd[2] = 0b10010011; altCmd[2] = 0b01000010;} // flashes start with dark light (looks better)
void sl2_YF(){stdCmd[2] = 0b10010111; altCmd[2] = 0b01000100;} 
void sl2_GF(){stdCmd[2] = 0b10011011; altCmd[2] = 0b01001001;} 

// sl3 port 6 - phase 0, 3
//   format - fl ph ph ph G Y GL! RL! (active low !)
void sl3_R() {stdCmd[3] = 0b00000010;} 
void sl3_RD(){stdCmd[3] = 0b00000011;}
void sl3_Y() {stdCmd[3] = 0b00000100;} 
void sl3_G() {stdCmd[3] = 0b00001001;}
void sl3_RF(){stdCmd[3] = 0b10110011; altCmd[3] = 0b00000010;} // flashes start with dark light (looks better)
void sl3_YF(){stdCmd[3] = 0b10110111; altCmd[3] = 0b00000100;} 
void sl3_GF(){stdCmd[3] = 0b10111011; altCmd[3] = 0b00001001;} 

// gc4 port 0 - phase 0, 3
//   format - fl ph ph ph NA BELL! RIGHT! LEFT!
void gc4_F() {stdCmd[4] = 0b10000110; altCmd[4] = 0b00110101;}
void gc4_FB(){stdCmd[4] = 0b10000010; altCmd[4] = 0b00110001;} 
void gc4_D() {stdCmd[4] = 0b00000111;} 
void gc4_DB(){stdCmd[4] = 0b00000011;} 
void gc4_L() {stdCmd[4] = 0b00000110;}
void gc4_R() {stdCmd[4] = 0b00000101;}
void gc4_LR(){stdCmd[4] = 0b00000100;}

// gy5 - port 8 bits 0, 1 - phase 0
//   format - NA ph ph ph NA NA 5L 5M
void gy5_Mon()  {stdCmd[5] = 0b00000001;} 
void gy5_Lon()  {stdCmd[5] = 0b00000010;}
void gy5_MLon() {stdCmd[5] = 0b00000011;} 
void gy5_MLoff(){stdCmd[5] = 0b00000000;}

// gy6 - port 8 bits 2, 3 - phase 0
//   format - NA ph ph ph NA NA 6L 6M
void gy6_Mon()  {stdCmd[6] = 0b00000001;} 
void gy6_Lon()  {stdCmd[6] = 0b00000010;}
void gy6_MLon() {stdCmd[6] = 0b00000011;} 
void gy6_MLoff(){stdCmd[6] = 0b00000000;}

// cl7 port 3 - phase 0, 3
//  format - fl ph ph ph NA G! Y! R!
void cl7_R() {stdCmd[7] = 0b00000110;} 
void cl7_D() {stdCmd[7] = 0b00000111;}
void cl7_Y() {stdCmd[7] = 0b00000101;} 
void cl7_G() {stdCmd[7] = 0b00000011;}
void cl7_RF(){stdCmd[7] = 0b10000111; altCmd[7] = 0b00110110;} // flashes start with dark light (looks better)
void cl7_YF(){stdCmd[7] = 0b10000111; altCmd[7] = 0b00110101;} 
void cl7_GF(){stdCmd[7] = 0b10000111; altCmd[7] = 0b00110011;} 

// cl8 - port 4 - phase 1, 4
//  format - fl ph ph ph NA G! Y! R!
void cl8_R() {stdCmd[8] = 0b00010110;} 
void cl8_D() {stdCmd[8] = 0b00010111;}
void cl8_Y() {stdCmd[8] = 0b00010101;} 
void cl8_G() {stdCmd[8] = 0b00010011;}
void cl8_RF(){stdCmd[8] = 0b10010111; altCmd[8] = 0b01000110;} // flashes start with dark light (looks better)
void cl8_YF(){stdCmd[8] = 0b10010111; altCmd[8] = 0b01000101;} 
void cl8_GF(){stdCmd[8] = 0b10010111; altCmd[8] = 0b01000011;} 

// cl9 - port 5 - phase 2, 5
//  format - fl ph ph ph NA G! Y! R!
void cl9_R() {stdCmd[9] = 0b00100110;} 
void cl9_D() {stdCmd[9] = 0b00100111;}
void cl9_Y() {stdCmd[9] = 0b00100101;} 
void cl9_G() {stdCmd[9] = 0b00100011;}
void cl9_RF(){stdCmd[9] = 0b10100111; altCmd[9] = 0b01010110;} // flashes start with dark light (looks better)
void cl9_YF(){stdCmd[9] = 0b10100111; altCmd[9] = 0b01010101;} 
void cl9_GF(){stdCmd[9] = 0b10100111; altCmd[9] = 0b01010011;} 

// cl10 ports 4.3, 5.3 phase 1, 4
//   format - fl ph ph ph NA NA G! R!
void cl10_R() {stdCmd[10] = 0b00010010;} 
void cl10_D() {stdCmd[10] = 0b00010011;}
void cl10_G() {stdCmd[10] = 0b00010001;}
void cl10_RF(){stdCmd[10] = 0b10010011; altCmd[10] = 0b01000010;} // flashes start with dark light (looks better)
void cl10_GF(){stdCmd[10] = 0b10010011; altCmd[10] = 0b01000001;}  

// ww11 - port 9 bits 0, 1 - phase 0
//   format - NA ph ph ph NA NA 11L 11M
void ww11_Mon()   {stdCmd[11] = 0b00000001;} 
void ww11_Lon()   {stdCmd[11] = 0b00000010;}
void ww11_MLon()  {stdCmd[11] = 0b00000011;} 
void ww11_MLoff() {stdCmd[11] = 0b00000000;}

// ww12 - port 9 bits 2, 3 - phase 0
//   format - NA ph ph ph NA NA 12L 12M
void ww12_Mon()   {stdCmd[12] = 0b00000001;} 
void ww12_Lon()   {stdCmd[12] = 0b00000010;}
void ww12_MLon()  {stdCmd[12] = 0b00000011;} 
void ww12_MLoff() {stdCmd[12] = 0b00000000;}

void updateOutputs()
{
// globals for controlling signals
// loop and array were not considred practical since signals share some nibble ports 
// for signals 0-12
// int stdCmd[12]; 
//   format for bits is -- flash phase phase phase data data data data
// int altCmd[12];
//   for flashing command if flash bit set in stdCmd
//   format for bits is -- na phase phase phase data data data data
// int phase; phases 0-5
int flashBit;
int sigPhase;
int dOut; 
// sl0 - port 1
sigPhase = (stdCmd[0] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = stdCmd[0] & 0xf;
  dataNibbleOut(1, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[0] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[0] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = altCmd[0] & 0xf;
    dataNibbleOut(1, dOut); // output data if correct phase
    }
  }
// sl1 - port 7
sigPhase = (stdCmd[1] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = stdCmd[1] & 0xf;
  dataNibbleOut(7, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[1] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[1] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = altCmd[1] & 0xf;
    dataNibbleOut(7, dOut); // output data if correct phase
    }
  }
// sl2 - port 2
sigPhase = (stdCmd[2] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = stdCmd[2] & 0xf;
  dataNibbleOut(2, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[2] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[2] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = altCmd[2] & 0xf;
    dataNibbleOut(2, dOut); // output data if correct phase
    }
  }
// sl3 - port 6
sigPhase = (stdCmd[3] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = stdCmd[3] & 0xf;
  dataNibbleOut(6, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[3] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[3] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = altCmd[3] & 0xf;
    dataNibbleOut(6, dOut); // output data if correct phase
    }
  }
// gc4 - port 0
sigPhase = (stdCmd[4] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = stdCmd[4] & 0xf;
  dataNibbleOut(0, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[4] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[4] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = altCmd[4] & 0xf;
    dataNibbleOut(0, dOut); // output data if correct phase
    }
  }
// gy5 - port 8 bits 0, 1 - flashBit and altCmd NA
sigPhase = (stdCmd[5] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = dataNibble[8]; // get current output and then update for gy5
  bitWrite(dOut, 0, bitRead(stdCmd[5], 0)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 1, bitRead(stdCmd[5], 1)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(8, dOut);  // output data if correct phase  
  }
// gy6 - port 1 bits 2, 3 - flashBitand altCmd NA
sigPhase = (stdCmd[6] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = dataNibble[8]; // get current output and then update for gy6
  bitWrite(dOut, 2, bitRead(stdCmd[6], 0)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 3, bitRead(stdCmd[6], 1)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(8, dOut);  // output data if correct phase  
  }
// cl7 - port 3
sigPhase = (stdCmd[7] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = stdCmd[7] & 0xf;
  dataNibbleOut(3, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[7] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[7] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = altCmd[7] & 0xf;
    dataNibbleOut(3, dOut); // output data if correct phase
    }
  }
// cl8 - port 4 bits 1, 2, 3
sigPhase = (stdCmd[8] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = dataNibble[4];
  bitWrite(dOut, 0, bitRead(stdCmd[8], 0)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 1, bitRead(stdCmd[8], 1)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 2, bitRead(stdCmd[8], 2)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(4, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[8] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[8] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = dataNibble[4];
    bitWrite(dOut, 0, bitRead(altCmd[8], 0)); // bitRead(x, n) bitWrite(x, n, v)
    bitWrite(dOut, 1, bitRead(altCmd[8], 1)); // bitRead(x, n) bitWrite(x, n, v)
    bitWrite(dOut, 2, bitRead(altCmd[8], 2)); // bitRead(x, n) bitWrite(x, n, v)
    dataNibbleOut(4, dOut);  // output data if correct phase  
    }
  }
// cl9 - port 5 bits 1, 2, 3
sigPhase = (stdCmd[9] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = dataNibble[5];
  bitWrite(dOut, 0, bitRead(stdCmd[9], 0)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 1, bitRead(stdCmd[9], 1)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 2, bitRead(stdCmd[9], 2)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(5, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[9] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[9] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = dataNibble[5];
    bitWrite(dOut, 0, bitRead(altCmd[9], 0)); // bitRead(x, n) bitWrite(x, n, v)
    bitWrite(dOut, 1, bitRead(altCmd[9], 1)); // bitRead(x, n) bitWrite(x, n, v)
    bitWrite(dOut, 2, bitRead(altCmd[9], 2)); // bitRead(x, n) bitWrite(x, n, v)
    dataNibbleOut(5, dOut);  // output data if correct phase  
    }
  }
// cl10 - port 4 bit 3 R! and port 5 bit 3 G!
sigPhase = (stdCmd[10] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = dataNibble[4];
  bitWrite(dOut, 3, bitRead(stdCmd[10], 0)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(4, dOut);  // output data if correct phase  
  dOut = dataNibble[5];
  bitWrite(dOut, 3, bitRead(stdCmd[10], 1)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(5, dOut);  // output data if correct phase  
  }
flashBit = (stdCmd[10] >> 7) & 0x1;  // check if stdCmd has flash bit set
if (flashBit == 1){
  sigPhase = (altCmd[10] >> 4) & 0x7; // if so, check if altCmd phase
  if (sigPhase == phase){ 
    dOut = dataNibble[4];
    bitWrite(dOut, 3, bitRead(altCmd[10], 0)); // bitRead(x, n) bitWrite(x, n, v)
    dataNibbleOut(4, dOut);  // output data if correct phase  
    dOut = dataNibble[5];
    bitWrite(dOut, 3, bitRead(altCmd[10], 1)); // bitRead(x, n) bitWrite(x, n, v)
    dataNibbleOut(5, dOut);  // output data if correct phase  
    }
  }
// ww11 - port 9 bits 0, 1 - flashBit and altCmd NA
sigPhase = (stdCmd[11] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = dataNibble[9];  // get current output and then update for ww11
  bitWrite(dOut, 0, bitRead(stdCmd[11], 0)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 1, bitRead(stdCmd[11], 1)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(9, dOut);  // output data if correct phase  
  }
// ww12 - port 9 bits 2, 3 - flashBit and altCmd NA
sigPhase = (stdCmd[12] >> 4) & 0x7;
if (sigPhase == phase){ // check if stdCmd phase
  dOut = dataNibble[9]; // get current output and then update for ww12
  bitWrite(dOut, 2, bitRead(stdCmd[12], 0)); // bitRead(x, n) bitWrite(x, n, v)
  bitWrite(dOut, 3, bitRead(stdCmd[12], 1)); // bitRead(x, n) bitWrite(x, n, v)
  dataNibbleOut(9, dOut);  // output data if correct phase  
  }
}

void dataNibbleOut(int num, int dataOut)
{
// globals
// int dataNibble[10] // ports 0-4 8243 0, 5-7, 8243 1, ESP32 D5-D2 8, ESP32 D9-D6 9 
if ((num >= 0) && (num <= 3)) { // select 8243-0
  digitalWrite(csPin, LOW); // select 8243-0
  delay(5);
  // write command first
  commandNibble = 4 + num; // 0b0100 + portNum - 01 is write command an 00 is port address
  digitalWrite(data3Pin, bitRead(commandNibble, 3));
  digitalWrite(data2Pin, bitRead(commandNibble, 2));
  digitalWrite(data1Pin, bitRead(commandNibble, 1));
  digitalWrite(data0Pin, bitRead(commandNibble, 0));
  delay(5); // set up command data
  digitalWrite(progPin, LOW); // latch command
  delay(5);
  digitalWrite(data3Pin, bitRead(dataOut, 3));
  digitalWrite(data2Pin, bitRead(dataOut, 2));
  digitalWrite(data1Pin, bitRead(dataOut, 1));
  digitalWrite(data0Pin, bitRead(dataOut, 0));
  delay(5); // set up data
  digitalWrite(progPin, HIGH); // latch data
  delay(5);
  }
if ((num >= 4) && (num <= 7)) { // select 8243-1
  digitalWrite(csPin, HIGH); // select 8243-1
  delay(5);
  // write command first
  commandNibble = 4 + num - 4; // 0b0100 + portNum - 4 as 01 is write command an 00 is port address
  digitalWrite(data3Pin, bitRead(commandNibble, 3));
  digitalWrite(data2Pin, bitRead(commandNibble, 2));
  digitalWrite(data1Pin, bitRead(commandNibble, 1));
  digitalWrite(data0Pin, bitRead(commandNibble, 0));
  delay(5); // set up command data
  digitalWrite(progPin, LOW); // latch command
  delay(5);
  digitalWrite(data3Pin, bitRead(dataOut, 3));
  digitalWrite(data2Pin, bitRead(dataOut, 2));
  digitalWrite(data1Pin, bitRead(dataOut, 1));
  digitalWrite(data0Pin, bitRead(dataOut, 0));
  delay(5); // set up data
  digitalWrite(progPin, HIGH); // latch data
  delay(5);
  }
if (num == 8) { // select Nano D2-D5
  digitalWrite(D5, bitRead(dataOut, 3));
  digitalWrite(D4, bitRead(dataOut, 2));
  digitalWrite(D3, bitRead(dataOut, 1));
  digitalWrite(D2, bitRead(dataOut, 0));
  }
if (num == 9) { // select Nano D6-D9
  //terminal.println(dataOut); // for testing
  //terminal.flush();
  //Blynk.run();
  digitalWrite(D9, bitRead(dataOut, 3));
  digitalWrite(D8, bitRead(dataOut, 2));
  digitalWrite(D7, bitRead(dataOut, 1));
  digitalWrite(D6, bitRead(dataOut, 0));
  }
dataNibble[num] = dataOut;  // update array
}
