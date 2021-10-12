int16_t plot_readings[128];
#define Y_OFFSET 14
unsigned long f, f1, f2, stepSize;

int freq2screen(unsigned long freq){
unsigned long f1, f2, hz_per_pixel;
hz_per_pixel = spanFreq / 100;
f1 = centerFreq - spanFreq/2;
return (int)((freq - f1)/hz_per_pixel) + X_OFFSET;
}
int pwr2screen(int y){
  return  -y/2 + Y_OFFSET;
}
int vswr2screen(int y){
  if (y >= 100)
  y = 99;
  return y/2 + Y_OFFSET-1;
}
/***************** Draw Power Grid on Display ***********************
void setupPowerGrid(){
  int x, y;
  char p[20];

  GLCD.ClearScreen();  

  while(ENTER())      //asteapta relaxarea butonului
  delay(100);
  
 
 // updateHeading();

 // sprintf(p, "%ldK, %ldK/div", centerFreq/1000, spanFreq/10000); 
 // GLCD.DrawString(p, 0, 57);

  //draw the horizontal grid
  for (y = -10; y >= -90; y-= 20){
    for (x = X_OFFSET; x <= 100+ X_OFFSET; x += 2)
    GLCD.SetDot(x,pwr2screen(y),BLACK);
  }

  //draw the vertical grid
  f1 = centerFreq - (spanFreq/2);
  f2 = centerFreq + (spanFreq/2);
  for (f = f1; f <= f2; f += spanFreq/10){
    for (y =0; y <= 50; y += 2)
    GLCD.SetDot(freq2screen(f),y+Y_OFFSET-2,BLACK);
  }

  //for (y = -10; y >= -90; y-= 20){
   // itoa(y, p, 10);
   // GLCD.DrawString(p, 0, pwr2screen(y)-4);
  //}  
}*/
/******************* Draw SWR Grid *********************************/
void plotVSWR(){
Label_1:  
  int x, y;
  char p[20];
  GLCD.ClearScreen();  
 
  GLCD.FillRect(0, 0, 128, 10, WHITE);
  updateHeading();

  //draw the horizontal grid
  for (y = 0; y <= 100; y += 20){
 //   Serial.print("d");
 //   Serial.println(vswr2screen(y));
  for (x = X_OFFSET; x <= 100+ X_OFFSET; x += 2)
  GLCD.SetDot(x,vswr2screen(y),BLACK);
  }

  //draw the vertical grid
  f1 = centerFreq - (spanFreq/2);
  if (f1 < 0)
      f1 = 0;
  f2 = f1 + spanFreq;
  for (f = f1; f <= f2; f += spanFreq/10){
 //   Serial.print(f);
 //   Serial.print(",");
  //  Serial.println(freq2screen(f));
  for (y =0; y <= 50; y += 2)
  GLCD.SetDot(freq2screen(f),y+Y_OFFSET,BLACK);
  }

  for (y = Y_OFFSET; y < 100 + Y_OFFSET; y += 20){
    itoa(y/10, p, 10);
    strcat(p, ".");
    GLCD.DrawString(p, 0, vswr2screen(y)-8);
  }  

  f1 = centerFreq - (spanFreq/2);
  f2 = f1 + spanFreq;
  stepSize = (f2 - f1)/100;
  int i = 0, vswr_reading;

  for (f = f1; f < f2; f += stepSize){
    takeReading(f);
    int esc = analogRead(ESC_BUTON);   //citeste butonul ESC
    if(esc<20) {
      GLCD.ClearScreen();
      updateScreen();
      return;
      }
    delay(20);
    //now take the readings
    return_loss = openReading(f) - analogRead(DBM_READING)/5;
    if (return_loss > 30) return_loss = 30;
    if (return_loss < 0) return_loss = 0;
    
    vswr_reading = pgm_read_word_near(vswr + return_loss);
    plot_readings[i] = vswr_reading;

    if (i == 0)
      GLCD.SetDot(freq2screen(f),vswr2screen(vswr_reading),BLACK);
    else
      GLCD.DrawLine(i + X_OFFSET-1, vswr2screen(plot_readings[i-1]), i + X_OFFSET, vswr2screen(plot_readings[i])); 
    i++;
  }

  int current_pos = 50;                         //cursorul DFAULT la mijloc
  powerHeading(current_pos);

  while (!ENTER()){                           //cata vreme nu este apasat butonul...
    i = Up_Down();delay(100);                  //executa miscarea cursorului stg/dr
    
    if ((i < 0 && current_pos + i >= 0) || 
      (i > 0 && current_pos + i <= 100)){
      current_pos += i;
      powerHeading(current_pos);               //update valoarea curenta a frecventei in antet
    }
    int esc = analogRead(ESC_BUTON);   //citeste butonul ESC
    if(esc<20) {
      GLCD.ClearScreen();
      updateScreen();
      return;
      }
  }
  //daca butonul este apasat...
    while(ENTER()){ //asteapta relaxarea butonului
    delay(100);
    goto Label_1;}   
}
/***************** Deseneaza cursorul *************************************/
void updateCursor(int pos, char*text){
  GLCD.FillRect(0,0,127,10, WHITE);
  GLCD.DrawString(text, 0, 0);
  GLCD.DrawLine(pos+ X_OFFSET, 8, pos + X_OFFSET,9);
}
/***************** Afiseaza valoarea dB a cursorului in antet ************/
void powerHeading(int current_pos){
  
  GLCD.FillRect(0,0,127,12, WHITE);
  freqtoa(f1 + (stepSize * current_pos), b);
  centerFreq=(f1 + (stepSize * current_pos));     //update frecventa la pozitia cursorului
  GLCD.DrawString(b, 0, 0);
  
  if (mode == MODE_ANTENNA_ANALYZER)
    sprintf (b, " %d.%01d", plot_readings[current_pos]/10,plot_readings[current_pos] % 10);
  else
    sprintf(b, "%ddbm", plot_readings[current_pos]);

  GLCD.DrawString(b, 80, 0);
  GLCD.DrawLine(current_pos+ X_OFFSET, 8, current_pos + X_OFFSET,11);
}
/*********************Grafic PLOT PWR **********************************/
void plotPower(){
  int x, y, pwr;
  char p[20], scan=0;

    while(ENTER())              //asteapta relaxarea tastei <ENTER> daca a fost apasata
    delay(100);

    GLCD.ClearScreen(); 
    GLCD.FillRect(0, 0, 128, 10, WHITE);
     
    while (!ENTER()){           //daca nu se mai apasa tasta <ENTER>...
    int current_pos = 50;       //seteaza cursorul la mujlocul scalei  
    updateHeading(); 

//draw the horizontal grid
  for (y = -10; y >= -90; y-= 20){
    for (x = X_OFFSET; x <= 100+ X_OFFSET; x += 2)
    GLCD.SetDot(x,pwr2screen(y),BLACK);
  }
//draw the vertical grid
  f1 = centerFreq - (spanFreq/2);
  f2 = centerFreq + (spanFreq/2);
  for (f = f1; f <= f2; f += spanFreq/10){
    for (y =0; y <= 50; y += 2)
    GLCD.SetDot(freq2screen(f),y+Y_OFFSET-2,BLACK);
  }
  
   for (y = -10; y >= -90; y-= 20){
   itoa(y, p, 10);
   GLCD.DrawString(p, 0, pwr2screen(y)-4);
   }
/*
    
  //draw the horizontal grid
  for (y = 0; y <= 100; y += 20){   
    Serial.print("d");
    Serial.println(pwr2screen(y));
    for (x = X_OFFSET; x <= 100+ X_OFFSET; x += 2)
      GLCD.SetDot(x,vswr2screen(y),BLACK);
  }
  //draw the vertical grid
  f1 = centerFreq - (spanFreq/2);
  if (f1 < 0)
      f1 = 0;
  f2 = f1 + spanFreq;
  for (f = f1; f <= f2; f += spanFreq/10){
    for (y =0; y <= 50; y += 2)
      GLCD.SetDot(freq2screen(f),y+Y_OFFSET,BLACK);
  }
  for (y = -80; y <= -20; y += 20){
  itoa(y, p, 10);
  GLCD.DrawString(p, 0, pwr2screen(y)-4);
  }
*/
  f1 = centerFreq - (spanFreq/2);
  f2 = f1 + spanFreq;
  stepSize = (f2 - f1)/100;
  int i = 0, vswr_reading;

  for (f = f1; f < f2; f += stepSize){
    takeReading(f);
    int esc = analogRead(ESC_BUTON);   //citeste butonul ESC
    if(esc<20) {
      GLCD.ClearScreen();
      updateScreen();
      return;
    }
    delay(20);
    //now take the readings
    analogRead(DBM_READING);
    analogRead(DBM_READING);
    analogRead(DBM_READING);
    
    int r = analogRead(DBM_READING)/5 + dbmOffset;
    plot_readings[i] = r;
   // Serial.print(plot_readings[i]);
   // Serial.print('-');
 
    if (i == 0)
      GLCD.SetDot(X_OFFSET, pwr2screen(plot_readings[i]),BLACK);
    else
      GLCD.DrawLine(i + X_OFFSET-1, pwr2screen(plot_readings[i-1]), i + X_OFFSET, pwr2screen(plot_readings[i])); 
    i++;
  }
  /********************* Noua rutina de scanare automata ****************************************/
again:
    i = Up_Down();delay(100);                  //verifica tastele <UP><DOWN>
    if ((i < 0 && current_pos + i >= 0) ||      //daca oricare tasta este apasata...
      (i > 0 && current_pos + i <= 100)){
      current_pos += i;                         //modifica pozitia cursorului
      powerHeading(current_pos);                //si update valoare frecventa
      scan=1;                                   //activeaza modul scan manual
      }
    if (ENTER()) scan=0;                      //daca se re-apasa tasta <ENTER> activeaza modul automat  
    while (scan==1) goto again;                 //repeta scanarea manuala daca este activa
    while (ENTER()) delay(100);
    GLCD.ClearScreen(); 
  }
}
