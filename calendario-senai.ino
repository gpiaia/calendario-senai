/*
EULA licence.
Copyright (c) 2021 -  SENAI
Eurides <eurides.terres@senairs.org>
Guilherme Piaia <guilherme.piaia@senairs.org>
All rights reserved.
Baseado no sketch https://create.arduino.cc/projecthub/user0035382/adjusting-clock-on-128x64-monochrome-display-st7920-b9569a
*/

#include <stdlib.h>
#include <Wire.h>
#include "RTClib.h"
#include "U8glib.h"

// Modulo RTC DS1307 ligado as portas A4 e A5 do Arduino ------------------------
RTC_DS1307 rtc;

U8GLIB_ST7920_128X64_1X u8g(6, 5, 4, 7); //Enable, RW, RS, RESET
int display = 1;

// Defines e variaveis  ------------------------------------------------ --------
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};
int loop_cnt = 0;
String senai = "SENAI";
String print_senai = "";
String str = "";
int X = 0;
int Y = 0;
int I = 0;
int X2 = 0;
int Y2 = 0;
int X3 = 0;
int Y3 = 0;
int Timer = 0;
int Minutt = 0;
int Sekund = 0; 
int lastSekund = 0;
float Vinkel = 0;
int an = 0;
int luna = 0;
int zi = 0;
int zis = 0;

int ScreenWith = 128;
int ScreenWithC = 64;
int ScreenHeight = 64;
int ScreenHeightC = 32;
int yArray[128];
int deics = ScreenWith/8;
int deigrec = 14;
int deics2 = 0;
int de = 10;

//the variables provide the holding values for the set clock routine
int setyeartemp; 
int setmonthtemp;
int setdaytemp;
int setoretemp;
int setzitemp;
int setminstemp;
int setsecs = 0;
int maxday; // maximum number of days in the given month
int TimeMins; // number of seconds since midnight
int TimerMode = 2; //mode 0=Off 1=On 2=Auto
int TimeOut = 10;
int TimeOutCounter;

// These variables are for the push button routine
int buttonstate = 0; //flag to see if the button has been pressed, used internal on the subroutine only
int pushlengthset = 1500; // value for a long push in mS
int pushlength = pushlengthset; // set default pushlength
int pushstart = 0;// sets default push value for the button going low
int pushstop = 0;// sets the default value for when the button goes back high

int knobval; // value for the rotation of the knob
int knobvalminus;
int knobvalplus;

boolean buttonflag = false; // default value for the button flag
#define buton 12
char tmp_string[8];

const byte ledPin = 8;
const byte interruptPin = 2;
volatile byte state = HIGH;


// Tela do Senai ----------------------------------------------------------------
void draw(void) {
    DateTime now = rtc.now();
    u8g.setFont(u8g_font_chikita);
    u8g.drawLine(104, 0, 106, 0);
    u8g.drawStr( 1, 7, "       CENTRO DE FORMACAO");
    u8g.drawLine(98, 8, 99, 7);
    u8g.drawStr( 0, 13, "               PROFISSIONAL");
    u8g.setFont(u8g_font_helvB12r);
  
    if (loop_cnt >= 15){
      str += " ";  
      loop_cnt = 0;
    } else{
      loop_cnt += 1;
    }
    
    print_senai = str + senai;
    if(str.length() == 16){
        print_senai = "I" + str + senai;
      }
    if(str.length() == 17){
        print_senai = "AI"+ str + senai;
    }
    if(str.length() == 18){
      print_senai = "NAI"+ str + senai;
    }
    if(str.length() == 19){
      print_senai = "ENAI"+ str + senai;
    }
    if(str.length() >= 20){
      print_senai = senai;
      str = "";
    }
    
    u8g.drawStr( 2, 27, print_senai.c_str());
    u8g.setFontRefHeightText();
    u8g.setFont(u8g_font_chikita);
    u8g.drawStr( 0, 34, "  NEY DAMASCENO FERREIRA");
    u8g.setFont(u8g_font_chikita);
    u8g.drawLine(77, 34, 78, 33);
    u8g.drawStr( 0, 40, "                GRAVATAI-RS");
    
    u8g.setFont(u8g_font_6x10);   
    u8g.setPrintPos(2*deics + deics2, 64); 
    if (zi < 10) u8g.print("0");
    u8g.print(zi);
    u8g.print("/");
    if (luna < 10) u8g.print("0");
    u8g.print(luna);
    u8g.print("/");
    u8g.print(an);
    u8g.setPrintPos(2*deics + deics2+de, 54); 
    u8g.setFont(u8g_font_helvB12r);
    if (Timer < 10) u8g.print(" ");
    u8g.print(Timer);
    if (Sekund%2 != 1)
    u8g.print(":");
    
    u8g.setPrintPos(ScreenWith/2 + 3, 54); 
    if (Minutt < 10) u8g.print("0");
    u8g.print(Minutt);
    u8g.setFont(u8g_font_6x10);   
}

// Funcao de inicializacao -------------------------------------------------------
void setup(void) {
    pinMode(buton,INPUT_PULLUP);//push button on encoder connected to A0 (and GND)
    pinMode(11, INPUT_PULLUP);           // set pin to input with pullup
    pinMode(10, INPUT_PULLUP);           // set pin to input with pullup
    pinMode(ledPin, OUTPUT);
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);

    digitalWrite(ledPin, LOW);
    u8g.setFont(u8g_font_6x10);
  
    Serial.begin(9600);
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
  
    if (! rtc.isrunning()) {
      Serial.println("RTC is NOT running!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

// Loop principal -----------------------------------------------------------------
void loop(void) {
    u8g.setFont(u8g_font_6x10);
    DateTime now = rtc.now();
    Timer = now.hour(), DEC;
    Minutt = now.minute(), DEC;
    Sekund = now.second(), DEC;
    an = now.year(), DEC;
    luna = now.month(), DEC;
    zi = now.day(), DEC;
    zis = now.dayOfTheWeek();

    if( Sekund != lastSekund){
      Serial.print(Timer);
      Serial.print(':');
      Serial.print(Minutt);
      Serial.print(':');
      Serial.print(Sekund);
      Serial.print(" - ");
      Serial.print(an);
      Serial.print('/');
      Serial.print(luna);
      Serial.print('/');
      Serial.print(zi);
      Serial.print(" (");
      Serial.print(daysOfTheWeek[zis]);
      Serial.println(") ");
      lastSekund = Sekund;
    }
    
    // picture loop
    u8g.firstPage(); 
    do {
      draw();
    } while( u8g.nextPage() );
    // rebuild the picture after some delay
    delay(100);
  
     pushlength = pushlengthset;
     pushlength = getpushlength();
     delay (10);
     
     if (pushlength < pushlengthset) 
     {
      //ShortPush ();   
       Serial.println("short push");
        // picture loop
      u8g.firstPage(); 
      do {
        draw1();
      } while( u8g.nextPage() );
      // rebuild the picture after some delay
      delay(1000);
    
     }
      //This runs the setclock routine if the knob is pushed for a long time
      if (pushlength > pushlengthset) {
           Serial.println("long push");
           DateTime now = rtc.now();
           setyeartemp=now.year(),DEC;
           setmonthtemp=now.month(),DEC;
           setdaytemp=now.day(),DEC;
           setoretemp=now.hour(),DEC;
           setminstemp=now.minute(),DEC;
           setclock();
           pushlength = pushlengthset;
       }
}

// subroutine to return the length of the button push. -----------------------------------
int getpushlength() {
    buttonstate = digitalRead(buton);  
    if(buttonstate == LOW && buttonflag==false) {     
          pushstart = millis();
          buttonflag = true;
      }   
    if (buttonstate == HIGH && buttonflag==true) {
     pushstop = millis ();
     pushlength = pushstop - pushstart;
     buttonflag = false;
    }
    return pushlength;
}

// Tela de seguranca ----------------------------------------------------------------
void draw1(void) {
  u8g.drawStr( 10, 32, "Segure para Editar!");
}

// Configura o relogio ----------------------------------------------------------------------
void setclock (){
   setyear ();
   setmonth ();
   setday ();
   //setzi();
   setore ();
   setmins (); 
   rtc.adjust(DateTime(setyeartemp,setmonthtemp,setdaytemp,setoretemp,setminstemp,setsecs));
   delay (1000); 
}

// Subrotina para editar o ano -----------------------------------------------------------
int setyear () {
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setyeartemp;
    }

    knobvalminus=digitalRead(11);
    knobvalplus=digitalRead(10);
    knobval = 0;
    if (knobvalminus == LOW) {
      knobval = -1;
      delay (80);
    }
    
    if (knobvalplus == LOW) {
      knobval = 1;
      delay (80);
    }
    
    setyeartemp=setyeartemp + knobval;
    if (setyeartemp < 2021) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2021;
    }

    itoa(setyeartemp, tmp_string, 10); // https://www.avrfreaks.net/forum/how-do-i-print-variable-u8glib
    u8g.firstPage(); 
    do {
        u8g.drawStr( 0, 20, "Configurar ano:");
        u8g.drawStr(25, 40, tmp_string);
    } while( u8g.nextPage() );  
    setyear();
}

// Subrotina para editar o mes -----------------------------------------------------------
int setmonth () {
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setmonthtemp;
    }

    knobvalminus=digitalRead(11);
    knobvalplus=digitalRead(10);
    knobval = 0;
    if (knobvalminus == LOW) {
      knobval = -1;
      delay (80);
    }
    
    if (knobvalplus == LOW) {
      knobval = 1;
      delay (80);
    }
    setmonthtemp=setmonthtemp + knobval;
    if (setmonthtemp < 1) {// month must be between 1 and 12
      setmonthtemp = 1;
    }
    if (setmonthtemp > 12) {
      setmonthtemp=12;
    }
    itoa(setmonthtemp, tmp_string, 10);
    u8g.firstPage(); 
    do {
        u8g.drawStr( 0, 20, "Configurar mes:");
        u8g.drawStr(25, 40, tmp_string);
    } while( u8g.nextPage() ); 
    setmonth();
}

// Subrotina para editar o dia -----------------------------------------------------------
int setday () {
    if (setmonthtemp == 4 || setmonthtemp == 5 || setmonthtemp == 9 || setmonthtemp == 11) { //30 days hath September, April June and November
        maxday = 30;
    }
    else {
        maxday = 31; //... all the others have 31
    }
    if (setmonthtemp ==2 && setyeartemp % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
        maxday = 29;
    }
    if (setmonthtemp ==2 && setyeartemp % 4 !=0) {
        maxday = 28;
    }
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
        return setdaytemp;
    }
    knobvalminus=digitalRead(11);
    knobvalplus=digitalRead(10);
    knobval = 0;
    if (knobvalminus == LOW) {
        knobval = -1;
        delay (80);
    }
    
    if (knobvalplus == LOW) {
        knobval = 1;
        delay (80);
    }
    setdaytemp = setdaytemp+ knobval;
    if (setdaytemp < 1) {
        setdaytemp = 1;
    }
    if (setdaytemp > maxday) {
        setdaytemp = maxday;
    }
    itoa(setdaytemp, tmp_string, 10);  
    u8g.firstPage(); 
    do {
        u8g.drawStr( 0, 20, "Configurar dia:");
        u8g.drawStr(25, 40, tmp_string);
    } while( u8g.nextPage() ); 
    setday();
}

// Subrotina para editar o dia da semana -----------------------------------------------------------
int setzi () {
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setzitemp;
    }
    knobvalminus=digitalRead(11);
    knobvalplus=digitalRead(10);
    knobval = 0;
    if (knobvalminus == LOW) {
      knobval = -1;
      delay (80);
    }
    
    if (knobvalplus == LOW) {
      knobval = 1;
      delay (80);
    }
    setzitemp=setzitemp + knobval;
    if (setzitemp < 0) {// month must be between 0 and 6
      setzitemp = 0;
    }
    if (setzitemp > 6) {
      setzitemp=6;
    }
    itoa(setzitemp, tmp_string, 10);
    u8g.firstPage(); 
    do {
        u8g.drawStr( 0, 20, "Configurar dia da semana:");
        u8g.drawStr(25, 40, tmp_string);
        u8g.drawStr(0,60, daysOfTheWeek[setzitemp]);
    } while( u8g.nextPage() ); 
    setzi();
}


// Subrotina para editar a hora -----------------------------------------------------------
int setore () {
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
        return setoretemp;
    }
    knobvalminus=digitalRead(11);
    knobvalplus=digitalRead(10);
    knobval = 0;
    if (knobvalminus == LOW) {
        knobval = -1;
        delay (80);
    }
    if (knobvalplus == LOW) {
        knobval = 1;
        delay (80);
    }
    setoretemp=setoretemp + knobval;
    if (setoretemp < 0) {
        setoretemp = 0;
    }
    if (setoretemp > 23) {
       setoretemp=23;
    }
    itoa(setoretemp, tmp_string, 10);  
    u8g.firstPage(); 
    do {
        u8g.drawStr( 0, 20, "Configurar hora:");
        u8g.drawStr(25, 40, tmp_string);
      } while( u8g.nextPage() ); 
    setore();
}


// Subrotina para editar os minutos -----------------------------------------------------------
int setmins () {
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
        return setminstemp;
    }
    knobvalminus=digitalRead(11);
    knobvalplus=digitalRead(10);
    knobval = 0;
    if (knobvalminus == LOW) {
        knobval = -1;
        delay (80);
    }
    
    if (knobvalplus == LOW) {
        knobval = 1;
        delay (80);
    }
    setminstemp=setminstemp + knobval;
    if (setminstemp < 0) {
       setminstemp = 0;
    }
    if (setminstemp > 59) {
        setminstemp=59;
    }
    itoa(setminstemp, tmp_string, 10);  
    u8g.firstPage(); 
    do {
        u8g.drawStr( 0, 20, "Configurar minuto:");
        u8g.drawStr(25, 40, tmp_string);
    } while( u8g.nextPage() ); 
    setmins();
}

// Funcao para inverter o estado do LED quando houver uma interrupcao e acionar o LED
void blink() {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    /// Debounce da interrupcao
    if (interrupt_time - last_interrupt_time > 200){
        Serial.print("LED interruption \n");
        digitalWrite(ledPin, state);
        state = !state;
    }
    last_interrupt_time = interrupt_time;
}
