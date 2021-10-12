
#include <glcd.h>
#include <fonts/allFonts.h>
#include <Wire.h>
#include <EEPROM.h>

int nextChar = 0;
unsigned long centerFreq=14000000l; //initially set to 25 MHz
unsigned long spanFreq=25000000l;   //intiially set to 50 MHz
long spans[] = {
  25000000l,
  10000000l,
   5000000l,
   1000000l,
   
    500000l,
    100000l,
     50000l,
     10000l,
      5000l
};

int selectedSpan = 0;
#define MAX_SPANS 8

int enc_prev_state = 3;

uint32_t xtal_freq_calibrated = 27000000l;

/* I/O ports to read the tuning mechanism */
#define DWNBUTON (A2)
#define UPBUTON (A1)
#define Backlight 17

/* offsets into the EEPROM storage for calibration */
#define MASTER_CAL 0
#define LAST_FREQ 4
#define OPEN_HF 8
#define OPEN_VHF 12
#define OPEN_UHF 16
#define LAST_SPAN 20
#define LAST_MODE 24

//to switch on/off various clocks
#define SI_CLK0_CONTROL  16      // Register definitions
#define SI_CLK1_CONTROL 17
#define SI_CLK2_CONTROL 18

#define IF_FREQ  (24993000l)
#define MODE_ANTENNA_ANALYZER 0
#define MODE_MEASUREMENT_RX 1
#define MODE_NETWORK_ANALYZER 2
unsigned long mode = MODE_ANTENNA_ANALYZER;

char b[32], c[32], a[32];
int return_loss;
unsigned long frequency = 10000000l;
int openHF = 96;
int openVHF = 96;
int openUHF = 68;

#define DBM_READING (A6)
#define ESC_BUTON (A7)
int dbmOffset = -114;

int menuOn = 0;
unsigned long timeOut = 0;

const int PROGMEM vswr[] = {
999,  // 0 db
174,  // 1 db
87,   // 2 db
58,   // 3 db
40,   // 4 db
35,   // 5 db
30,   // 6 db
26,   // 7 db
20,   // 8 db
19,   // 9 db
18,   // 10 db
17,   // 11 db
16,   // 12 db
15,   // 13 db
14,   // 14 db
14,   // 15 db
14,   // 16 db
13,   // 17 db
13,   // 18 db
12,   // 19 db
12,   // 20 db
12,   // 21 db
12,   // 22 db
11,   // 23 db
11,   // 24 db
11,   // 25 db
11,   // 26 db
10,   // 27 db
10,   // 28 db 
10,   // 29 db
10    //30 db
};
/*
 * The return loss in db to vswr look up.
 * The VSWR is stored as multiplied by 10, i.e. VSWR of 1.5 is stored as 15
 */
                               //4,2 4,15 4,1 4,0 3,95 3,9 3,85 3,85 3,8 3,7 
//const char Procent[] PROGMEM = {"100, 90, 80, 70, 60, 50, 40, 30, 20, 10"};

//const int PROGMEM db_distortion[] = {
//};
/***********************************/
void active_delay(int delay_by){
  unsigned long timeStart = millis();

  while (millis() - timeStart <= delay_by) {
      //Background Work      
  }
}
/**********************************/
//int tuningClicks = 0;
//int tuningSpeed = 0;

void updateDisplay(){
  sprintf(b, "%ldK, %ldK/div", frequency/1000, spanFreq/10000); 
  GLCD.DrawString(b, 20, 57);
}
/********************Rutina de calibrare a Generatorului *************************/
int calibrateClock(){
  int knob = 0;
  int32_t prev_calibration;
  char  *p;

  GLCD.ClearScreen();
  GLCD.DrawString("Check RF-OUT Freqv.", 0, 0);
  GLCD.DrawString("Freqvency = 10,00 MHz", 0, 10);
  GLCD.DrawString("Tune UP/DOWN Buttons", 0, 20);
  GLCD.DrawString("Click Enter to Save", 0, 30);

  GLCD.DrawString("Save", 64, 45);
  GLCD.DrawRect(60,40,35,20);


  //keep clear of any previous button press
  while (ENTER())
    active_delay(100);
  active_delay(100);

  prev_calibration = xtal_freq_calibrated;
  xtal_freq_calibrated = 27002000l;

  si5351aSetFrequency_clk0(10000000l);  
  ltoa(xtal_freq_calibrated - 27000000l, c, 10);
  GLCD.FillRect(0,40,50,15, WHITE);
  GLCD.DrawString(c, 4, 45);     

  while (!ENTER())
  {
    knob = Up_Down();

    if (knob > 0)
      xtal_freq_calibrated += 10;
    else if (knob < 0)
      xtal_freq_calibrated -= 10;
    else 
      continue; //don't update the frequency or the display

    si5351aSetFrequency_clk0(10000000l);  
      
    ltoa(xtal_freq_calibrated - 27000000l, c, 10);
    GLCD.FillRect(0,40,50,15, WHITE);
    GLCD.DrawString(c, 4, 45);     
  }
  while(ENTER())
  delay(100);
  delay(100);
  GLCD.ClearScreen();
  GLCD.DrawString("Calibration Saved", 0, 25);

  EEPROM.put(MASTER_CAL, xtal_freq_calibrated);
  delay(2000);
}
/********************************************************************************/
int readOpen(unsigned long f){
  int i, r;

  takeReading(f);
  delay(100);
  r = 0;
  for (i = 0; i < 10; i++){
    r += analogRead(DBM_READING)/5;
    delay(50);
  }
  //debug the open reading
  sprintf(c, "Open read= %04d", r);
  GLCD.DrawString(c, 0, 42);
  delay(1000);  
  return (r+5)/10;
}
/********** Rutina Calibrare SWR **************************/
int calibrateMeter(){
  
  GLCD.ClearScreen();
  GLCD.DrawString("Disconnect Antenna", 0, 0);
  GLCD.DrawString("port and press Button", 0, 10);
  GLCD.DrawString("to calibrate SWR", 0, 20);
  GLCD.DrawString("OK", 10, 42);
  GLCD.DrawRect(5,35,20,20);
    
  //wait for a button down
  while(!ENTER())
    active_delay(50);

  GLCD.ClearScreen();
  GLCD.DrawString("Calibrating.....", 10, 25);
  delay(1000);
  
  int i, r;
  mode = MODE_ANTENNA_ANALYZER;
  delay(100);
  r = readOpen(20000000l);
//  Serial.print("open reading of 20MHz ");Serial.println(r);
  EEPROM.put(OPEN_HF, r);

  r = readOpen(140000000l);
  //Serial.print("open reading of 140MHz ");Serial.println(r);
  EEPROM.put(OPEN_VHF, r);

  r = readOpen(440000000l);
  //Serial.print("open reading of 440MHz ");Serial.println(r);
  EEPROM.put(OPEN_UHF, r);
  
  menuOn = 0;
 
  GLCD.ClearScreen();
  GLCD.DrawString("Done!",10,25);
  delay(1000);

  //switch off just the tracking source
  si5351aOutputOff(SI_CLK0_CONTROL);
  takeReading(centerFreq);
  updateDisplay();
}
/***************************************/
int openReading(unsigned long f){
  if (f < 60000000l)
    return openHF;
  else if (f < 150000000l)
    return openVHF;
  else
    return openUHF;
}

long prev_freq = 0;   //this is used only inside takeReading, it should have been static local
int prevMode = 0;
void takeReading(long newfreq){
  long local_osc;

  if (newfreq < 20000l)
      newfreq = 20000l;
  if (newfreq < 150000000l)
  {
    if (newfreq < 50000000l)
      local_osc = newfreq + IF_FREQ;
    else
      local_osc = newfreq - IF_FREQ;
  } else {
    newfreq = newfreq / 3;
    local_osc = newfreq - IF_FREQ/3;
  }

  if (prev_freq != newfreq || prevMode != mode){
    switch(mode){
    case MODE_MEASUREMENT_RX:
      si5351aSetFrequency_clk2(local_osc);
      si5351aOutputOff(SI_CLK1_CONTROL);
      si5351aOutputOff(SI_CLK0_CONTROL);
    break;
    case MODE_NETWORK_ANALYZER:
      si5351aSetFrequency_clk2(local_osc);
      si5351aOutputOff(SI_CLK1_CONTROL);        
      si5351aSetFrequency_clk0(newfreq);
    break;
    default:
      si5351aSetFrequency_clk2(local_osc);  
      si5351aSetFrequency_clk1(newfreq);
      si5351aOutputOff(SI_CLK0_CONTROL);        
    }      
    prev_freq = newfreq;
    prevMode = mode;
  }     
}
/********** Rutina de Initializare **************/
void setup() {
  GLCD.Init();            
  GLCD.SelectFont(System5x7);
  
  Serial.begin(9600);
  b[0]= 0;

  Wire.begin();
 // Serial.begin(9600);
//  Serial.flush();  
//  Serial.println(F("RF-Sniper YO6PIR"));
  analogReference(DEFAULT);

  unsigned long last_freq = 14000000;
  EEPROM.get(MASTER_CAL, xtal_freq_calibrated);
  //EEPROM.get(LAST_FREQ, last_freq);
  EEPROM.get(OPEN_HF, openHF);
  EEPROM.get(OPEN_VHF, openVHF);
  EEPROM.get(OPEN_UHF, openUHF);
 // EEPROM.get(LAST_SPAN, selectedSpan);
  EEPROM.get(LAST_MODE, mode);

 // Serial.print("*hf_open:");
 // Serial.println(openHF);
  //the openHF reading is actually -19.5 dbm
  dbmOffset = -23 - openHF;

 // Serial.println(last_freq);
  if (0 < last_freq && last_freq < 500000000l)
      centerFreq = last_freq;

  if (xtal_freq_calibrated < 26900000l || xtal_freq_calibrated > 27100000l)
    xtal_freq_calibrated = 27000000l;
  if (mode < 0 || mode > 2) mode = 0;

  spanFreq = spans[selectedSpan];
  
  pinMode(UPBUTON, INPUT_PULLUP);
  pinMode(DWNBUTON, INPUT_PULLUP);
  pinMode(17, OUTPUT);  //Iesire de comanda Backligt
  digitalWrite(17, LOW); //Defaul Backlight=OFF
  
  if (ENTER()){   //daca se apasa butonul la pornire
    calibration_mode();   //intra in mod calibrare
  } else 
    updateScreen();
  
  si5351aOutputOff(SI_CLK0_CONTROL);
  takeReading(frequency);   //citeste frecventa
  updateMeter();            //update bargraf
}
int prev = 0;
/********************** Bucla principala de program ****************/
void loop(){
  doMenu();       //afisare meniu principal
    
  int r = analogRead(DBM_READING);    //citeste dBm 
 /******************Citeste tasta ESC *********************/
  int esc = analogRead(ESC_BUTON);   //citeste butonul ESC
  if(esc<20){
      digitalWrite(17, !digitalRead(17));  
      delay(500);
      updateScreen();
 }
  /**********************************************************/
  if (r != prev){
    takeReading(centerFreq);
    updateMeter();
    prev = r;
  //  Serial.println(r);
  }  
  delay(50);   
}
