/******** Graphic User Interface Routine************************************************/
int x1, y1, w, h, x2, y2;

#define X_OFFSET 18
#define MENU_CHANGE_MHZ 0
#define MENU_CHANGE_KHZ 1
#define MENU_CHANGE_HZ  2
#define MENU_MODE_SWR 3
#define MENU_MODE_PWR 4
#define MENU_MODE_SNA 5
#define MENU_SPAN 6
#define MENU_PLOT 7
#define ACTION_SELECT 1
#define ACTION_DESELECT 2
#define ACTION_UP 3
#define ACTION_DOWN 4

int uiFocus = MENU_PLOT, knob=35, uiSelected=-1;
/********************returns true if the button is pressed*********************/
int ENTER(){
  if (digitalRead(UPBUTON)== LOW && digitalRead(DWNBUTON)==LOW)  //apasarea ambelor butoane=ENTER
  return 1;
  else
  return 0;
}
/*** NOUA RUTINA care inlocuieste encoderul cu doua taste UP/DOWN *************/
int Up_Down(void) {
  int result = 0;                               //reseteaza valoarea initiala 
  if (digitalRead(UPBUTON) == LOW) result++;    //Buton UP apasat= +1
  if (digitalRead(DWNBUTON) == LOW) result--;   //Buton DOWN apasat= -1
  return result;
}
/********************* Rutina de afisare a frecventei **************************/
void freqtoa(unsigned long f, char *s){
  char p[16];  
  memset(p, 0, sizeof(p));
  ultoa(f, p, DEC);
  s[0] = 0;

   if (f >= 100000000l){
    strncat(s, p, 3);
    strcat(s, ".");
    strncat(s, &p[3], 3);
    strcat(s, ".");
    strncat(s, &p[6], 3);
  }
  else if (f >= 10000000l){
    strcat(s, " ");
    strncat(s, p, 2);
    strcat(s, ".");
    strncat(s, &p[2], 3);
    strcat(s, ".");
    strncat(s, &p[5], 3);
  }
  else if (f >= 1000000l){
    strcat(s, "  ");
    strncat(s, p, 1);
    strcat(s, ".");
    strncat(s, &p[1], 3);    
    strcat(s, ".");
    strncat(s, &p[4], 3);
  }
  else {
    strcat(s, "    ");
    strncat(s, p, 3);
    strcat(s, ".");
    strncat(s, &p[3], 3);    
  }
}
/********************Rutina actualizare bargraf******************************/
void updateMeter(){
  int percentage = 0;
  int vswr_reading, r;

  GLCD.FillRect(0, 15, 128, 8, WHITE);

  if (mode == MODE_ANTENNA_ANALYZER){
    r = analogRead(DBM_READING)/5;
    return_loss = openReading(frequency) - r;
 //   Serial.print("db:");
 //   Serial.println(r);
    if (return_loss > 30)
       return_loss = 30;
    if (return_loss < 0)
       return_loss = 0;
  //  Serial.println(return_loss);
    vswr_reading = pgm_read_word_near(vswr + return_loss);
        if(vswr_reading>=99){ 
        sprintf (c, "SWR:>>>");
        }
        else{
        sprintf (c, "SWR:%d.%01d", vswr_reading/10, vswr_reading%10);
        }
    percentage = vswr_reading - 10;
  }
  else {
    sprintf(c, "%ddbm", analogRead(DBM_READING)/5 + dbmOffset);
    percentage = 110 + analogRead(DBM_READING)/5 + dbmOffset;
  }
  
  GLCD.DrawString(c, 0, 15);  
  //leave the offset to 37 pixels
  GLCD.DrawRoundRect(45, 15, 82, 6, 2);                     //deseneaza contur bargraf
  GLCD.FillRect(47, 17, (percentage * 8)/10, 2, BLACK);     //traseaza bargraf
}
/****************************************************************************************/
// this builds up the top line of the display with frequency and mode
void updateHeading() {
  int vswr_reading;
  // tks Jack Purdum W8TEE
  // replaced fsprint commmands by str commands for code size reduction

  memset(c, 0, sizeof(c));
  memset(b, 0, sizeof(b));

  ultoa(centerFreq, b, DEC);

  if (mode == MODE_ANTENNA_ANALYZER)
    strcpy(c, "SWR ");
  else if (mode == MODE_MEASUREMENT_RX)
    strcpy(c, "PWR ");
  else if (mode == MODE_NETWORK_ANALYZER)
    strcpy(c, "SNA ");
  
  //one mhz digit if less than 10 M, two digits if more

   if (centerFreq >= 100000000l){
    strncat(c, b, 3);
    strcat(c, ".");
    strncat(c, &b[3], 3);
    strcat(c, ".");
    strncat(c, &b[6], 3);
  }
  else if (centerFreq >= 10000000l){
    strncat(c, b, 2);
    strcat(c, ".");
    strncat(c, &b[2], 3);
    strcat(c, ".");
    strncat(c, &b[5], 3);
  }
  else if (centerFreq >=1000000l){
    strncat(c,b,1);
    strcat(c, ".");
    strncat(c, &b[1], 3);    
    strcat(c, ".");
    strncat(c, &b[4], 3);
  }
  else {
    strcat(c, "    ");
    strncat(c, b, 3);
    strcat(c, ".");
    strncat(c, &b[3], 3); 
  }

  GLCD.DrawString(c, 0, 0);

  itoa(spanFreq/10000, c, 10);
  strcat(c, "K/d");
  GLCD.DrawString(c, 128-(strlen(c)*6), 0);
}
/********************* MENIUL DE CALIBRARE*******************************/
void drawCalibrationMenu(int selection){

  GLCD.ClearScreen();
  GLCD.FillRect(0, 0, 128, 64, WHITE);
  GLCD.DrawString("* CALIBRATION MENU  *", 0,0);
  GLCD.DrawString(" FREQUENCY GENERATOR ", 0, 20);
  GLCD.DrawString(" RETURN LOSS LEVEL   ", 0, 40);

  if (selection == 0)
    GLCD.DrawRect(0,15,127,17);
  if (selection == 1)
    GLCD.DrawRect(0,35,127,17);  
}

void calibration_mode(){
  int i, select_item = 0;
drawCalibrationMenu(select_item);
 //wait for the button to be lifted
  while(ENTER())
    delay(100);
  delay(100);

  while (1){
    drawCalibrationMenu(select_item);
  
    while(!ENTER()){
      i = Up_Down();
      
      if(i > 0 && select_item == 0){
        select_item = 1;
        drawCalibrationMenu(select_item);
      }
      else if (i < 0 && select_item == 1){
        select_item = 0; 
        drawCalibrationMenu(select_item);
      }
      delay(50);
    }  
  
    while(ENTER())
      delay(100);
    delay(100);
    
    if (!select_item)
      calibrateClock();
    else
      calibrateMeter();
  }
}
/**************************************************************************************/
void uiFreq(int action){

  GLCD.FillRect(0, 25, 128, 11, WHITE);  
  GLCD.DrawString("FREQ=", 5,27);
  GLCD.DrawString(" Hz", 109,27);
    freqtoa(centerFreq, b);
  GLCD.DrawString(b, 39, 27);
  if (uiFocus == MENU_CHANGE_MHZ)
    GLCD.DrawRect(38,25,18,11);
  else if (uiFocus == MENU_CHANGE_KHZ)
    GLCD.DrawRect(62,25,18,11);
  else if (uiFocus == MENU_CHANGE_HZ)
    GLCD.DrawRect(86,25,18,11);

 // Serial.print("uiFreq action:");
 // Serial.println(action);
  if (!action)
    return; 

  if (action == ACTION_SELECT) {
   
    if (uiFocus == MENU_CHANGE_MHZ)
      GLCD.InvertRect(38,25,18,11);
    else if (uiFocus == MENU_CHANGE_KHZ)
      GLCD.InvertRect(62,25,18,11);
    else if (uiFocus == MENU_CHANGE_HZ)
      GLCD.InvertRect(86,25,18,11);

    //wait for the button to be released    
    while(ENTER())
    delay(500);
     
    while(!ENTER()){
      int r = analogRead(DBM_READING);
      if (r != prev){
        takeReading(centerFreq);
        updateMeter();
        prev = r;
      }
      int i = Up_Down();
      if (i < 0 && centerFreq > 1000000l){
        if (uiFocus == MENU_CHANGE_MHZ)
          centerFreq += 1000000l * i;
        else if (uiFocus == MENU_CHANGE_KHZ)
          centerFreq += 10000l * i;
        else if (uiFocus == MENU_CHANGE_HZ)
          centerFreq +=100l * i;
        if (centerFreq < 4000000000l && centerFreq > 150000000l)
          centerFreq = 150000000l;
        delay(200);
      }
      else if (i > 0 && centerFreq < 499000000l){
        if (uiFocus == MENU_CHANGE_MHZ)
          centerFreq += 1000000l * i;
        else if (uiFocus == MENU_CHANGE_KHZ)
          centerFreq += 10000l * i;
        else if (uiFocus == MENU_CHANGE_HZ)
          centerFreq += 100l * i;
        delay(200);
      }
      else 
        continue;
    
    GLCD.FillRect(0, 25, 128, 11, WHITE); 
    GLCD.DrawString("FREQ=", 5,27);
    GLCD.DrawString(" Hz",109,27);
    freqtoa(centerFreq, b);
    GLCD.DrawString(b, 39, 27);

      if (uiFocus == MENU_CHANGE_MHZ)
        GLCD.InvertRect(38,25,18,11);
      else if (uiFocus == MENU_CHANGE_KHZ)
        GLCD.InvertRect(62,25,18,11);
      else if (uiFocus == MENU_CHANGE_HZ)
        GLCD.InvertRect(86,25,18,11);
    }
    delay(200); //wait for the button to debounce

    GLCD.FillRect(0, 25, 128, 11, WHITE);  
    GLCD.DrawString("FREQ=", 5,27);
    GLCD.DrawString(" Hz",109,27);
    freqtoa(centerFreq, b);
    GLCD.DrawString(b, 39, 27);
    
    if (uiFocus == MENU_CHANGE_MHZ)
      GLCD.DrawRect(38,25,18,11);
    else if (uiFocus == MENU_CHANGE_KHZ)
      GLCD.DrawRect(62,25,18,11);
    else if (uiFocus == MENU_CHANGE_HZ)
      GLCD.DrawRect(86,25,18,11);   
  }  
}
/*************************** Selectie MODE SWR ****************************/
void uiSWR(int action){
  GLCD.FillRect(6,37,22,12, WHITE);
  GLCD.DrawString("SWR", 9, 40);

  if (action == ACTION_SELECT){
    mode = MODE_ANTENNA_ANALYZER;
    uiPWR(0);
    uiSNA(0);
        
    delay(200);
    EEPROM.put(LAST_MODE, mode);
  }
  
  if (uiFocus == MENU_MODE_SWR)
    GLCD.DrawRect(6,37,22,12);

  if (mode == MODE_ANTENNA_ANALYZER)
    GLCD.InvertRect(8,39,18,8);    

  takeReading(centerFreq);
  updateMeter();
}
/*******************Selectie MODE PWR ************************************/
void uiPWR(int action){
  GLCD.FillRect(30,37,22,12, WHITE);    
  GLCD.DrawString("PWR", 33, 40);

  if (action == ACTION_SELECT){
    mode = MODE_MEASUREMENT_RX;
    uiSWR(0);
    uiSNA(0);
    //updateScreen();
    delay(200);
    EEPROM.put(LAST_MODE, mode);
  }
  if (uiFocus == MENU_MODE_PWR)
    GLCD.DrawRect(30,37,22,12);
  if (mode == MODE_MEASUREMENT_RX)
    GLCD.InvertRect(32,39,18,8);
  takeReading(centerFreq);
  updateMeter();
}
/************************* Selectie MODE SNA *******************************/
void uiSNA(int action){
  GLCD.FillRect(54,37,22,12, WHITE);
  GLCD.DrawString("SNA", 57, 40);

  if (action == ACTION_SELECT){
    mode = MODE_NETWORK_ANALYZER;
    uiSWR(0);
    uiPWR(0);
    delay(200);
//    updateScreen();
    EEPROM.put(LAST_MODE, mode);
  }
  if (uiFocus == MENU_MODE_SNA)
    GLCD.DrawRect(54,37,22,12);
  if (mode == MODE_NETWORK_ANALYZER)
    GLCD.InvertRect(56,39,18,8);
  takeReading(centerFreq);
  updateMeter();
}
/*************** Rutina Modificare INTERVAL *****************/
void uiSpan(int action){
  GLCD.FillRect(56, 49, 29, 13, WHITE);
  if (spanFreq >= 1000000l)
    sprintf(b, "Interval= %3ldM", spanFreq/1000000l);
  else
    sprintf(b, "Interval= %3ldK", spanFreq/1000l);
  GLCD.DrawString(b, 0,52);

  if (uiFocus == MENU_SPAN)
    GLCD.DrawRect(56, 49, 29, 13);

  if (action == ACTION_SELECT) {
      //invert the selection
      GLCD.InvertRect(58, 51, 25, 9);

    //wait for the button to be released    
    while(ENTER())      
    delay(200);
     
    while(!ENTER()){
      int i = Up_Down(); delay(100);
      
      if (selectedSpan > 0 && i > 0){
        selectedSpan--;
        spanFreq = spans[selectedSpan];
        EEPROM.put(LAST_SPAN, selectedSpan);
        delay(200);
      }
      else if (selectedSpan < MAX_SPANS && i < 0){
        selectedSpan++;
        spanFreq = spans[selectedSpan];
        EEPROM.put(LAST_SPAN, selectedSpan);
        delay(200);
      }
      else 
        continue;
         
      GLCD.FillRect(56, 49, 29, 13, WHITE);
      
      if (spanFreq >= 1000000l)
        sprintf(b, "Interval= %3ldM", spanFreq/1000000l);
      else
        sprintf(b, "Interval= %3ldK", spanFreq/1000l);
      GLCD.DrawString(b, 0,52);
      GLCD.InvertRect(58, 51, 26, 10);
    }
   
    delay(200);
       
       GLCD.FillRect(56, 49, 29, 13, WHITE);
      if (spanFreq >= 1000000l)
        sprintf(b, "Interval= %3ldM", spanFreq/1000000l);
      else
        sprintf(b, "Interval= %3ldK", spanFreq/1000l);
      GLCD.DrawString(b, 0,52);
      GLCD.DrawRect(56, 49, 29, 13);
  }
}
/************************** MODE PLOT GRID **********************/
void uiPlot(int action){
  GLCD.FillRect(90, 42, 37,20, WHITE);    //mark fond chenar alb
  GLCD.DrawRect(90, 42, 37,20);     //deseneaza chenar
  GLCD.DrawString("PLOT", 98, 49);  //scrie "PLOT" in chenar

  if (uiFocus == MENU_PLOT)         //deseneaza un chenar mai mic 
    GLCD.DrawRect(92, 44, 33, 16);

  if (action == ACTION_SELECT){
    if (mode == MODE_ANTENNA_ANALYZER)
      plotVSWR();
    else 
      plotPower();
    updateScreen();
  }
}
/*******************************************************************/
void uiMessage(int id, int action){
  switch(id){
    case MENU_CHANGE_MHZ:
    case MENU_CHANGE_KHZ:
    case MENU_CHANGE_HZ:
      uiFreq(action);
      break;
    case MENU_MODE_SWR:
      uiSWR(action);
      break;
    case MENU_MODE_PWR:
      uiPWR(action);
      break;
    case MENU_MODE_SNA:
      uiSNA(action);
      break;
    case MENU_SPAN:
      uiSpan(action);
      break;
    case MENU_PLOT:
      uiPlot(action);
      break;
    default:
    return;
  }
}
/********************************************************************/
void updateScreen(){
  int esc = analogRead(ESC_BUTON);   //citeste butonul ESC
  strcpy(b, " RF-Sniper   Bat=");    // draw the title bar
/********************** Masoara tensiunea bateriei *************/
  double Vbat = (esc *4.8875)/1000;
  char strBat[16];
  dtostrf(Vbat, 1, 1, strBat);            // Format string Vbat 
  strcat(b,strBat);strcat(b,"V");
 // GLCD.ClearScreen();
  GLCD.FillRect(0, 0, 128, 10, WHITE);
  GLCD.DrawString(b, 1, 2);  
  GLCD.InvertRect(0,0, 128,10);
  /*******************************************************************/
  //update all the elements in the display
  updateMeter();
  uiFreq(0);
  uiSWR(0);
  uiPWR(0);
  uiSNA(0);
  uiSpan(0);
  uiPlot(0);
}
/*******************************************************************/
void doMenu(){
  unsigned long last_freq;
  int i = Up_Down();
  
  if (ENTER()){
    if (uiSelected == -1)
      uiMessage(uiFocus, ACTION_SELECT);
    if (uiSelected != -1){
      uiMessage(uiFocus, ACTION_DESELECT);
    }
  }

  if (i == 0) return;  
  if (i > 0 && knob < 35) knob += i;
  if (i < 0 && knob >= 0) knob += i;      
  
  if (uiFocus != knob/5){
    int prev = uiFocus;
    uiFocus = knob/5;
    uiMessage(prev, 0);
    uiMessage(uiFocus, 0);
  }
}
