// Modified for SPFD5408 Library by Barricade
//touch_keypad
// Version 0.1.0 for UNO r3
#include <touch_keypad_GFX.h>    // Core graphics library
#include <touch_keypad_TFTLCD.h> // Hardware-specific library
#include <touch_keypad_TouchScreen.h>
#include<EEPROM.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Calibration
#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 900
#define TS_MAXY 905
#define BOXSIZE 20

#define MAX_DIGITS 8

const char NO_KEY = '\0';
int color;
int width;
int halfWidth;
int height;
int halfHeight;
int x;
int y;
int z;
int memory[5]; // pamięc wciśniętych klawiszy - 5 znaków
volatile int code=0; // code=0 -> brak code=1 jest aktywny
volatile int l=0; // zmienna pomocnicza, licznik znaków
volatile int result=0; // rezultat porównania ciągu 5-ciu znaków
int mykey[5]; // taklica wciśnietych znaków


/*-----------------------------------------------------------------------*
 *------------------Writer function--------------------------------------*/
void Writer(String text,int osX = 20, int beginning = 10,  int lSize = 5, int color = BLACK){
tft.setTextColor(color);
tft.setTextSize(lSize);
tft.setCursor(osX,beginning);
tft.print(text);
}

/*-----------------------------------------------------------------------*
 *-----------------------Check EEPROM for PIN----------------------------*/
void read_EEPROM() // funkcja sprawdza obecnośc koduu w pamięci EEPROM
 {
  for (int i=0;i<5;i++) {memory[i]=EEPROM.read(i);} 
  if(memory[0]!=0 && memory[1]!=0 && memory[2]!=0 && memory[3]!=0
  && memory[4]!=0) code=1; else code=0; 
 }
 
/*-----------------------------------------------------------------------*
 *-----------------------Write new code to EEPROM------------------------*/
void new_code() // funkcja ustawia nową kombinację dla codeu
 {
  for(int i=0;i<5;i++)EEPROM.write(i,255); 
  Writer("New password:",10,10,3,BLUE);
  while(l<5)
    {
    char keypressed = getKeyCode();
    if (keypressed != NO_KEY)
      {
      EEPROM.write(l,keypressed);l++; 
      upperScreenFrame();
      } 
    }
  upperScreenClean();
  Writer("Saved!",10,10,5,BLUE);
  delay(1000);
  l=0; 
 }

 /*-----------------------------------------------------------------------*
 *-----------------------Check if key is pressed--------------------------*/
bool keyPressed(int zet){
while(zet > 0){
  delay(200);
  TSPoint q = ts.getPoint();
  int iks = q.x;
  int igrek = q.y;
   zet = q.z;
  pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(YM, OUTPUT);
  }
return true;
}

/*-----------------------------------------------------------------------*
 *-----------------------Get char code of input key----------------------*/
char getKeyCode(){
bool pressed = false;
while(pressed==false){
  TSPoint q = ts.getPoint();
  x = q.x;
  y = q.y;
  z = q.z;
   pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(YM, OUTPUT);
  if( z> 0 && z<1000){
    x = map(x, TS_MINX, TS_MAXX, tft.width(), 0);
    y = map(y, TS_MINY, TS_MAXY, tft.height()+BOXSIZE, 0); 
    pressed = keyPressed(z);
  }
 }
return ButtonPressed(x,y);
}

/*-----------------------------------------------------------------------*
 *-----------------------Check if right PIN entered----------------------*/
void checkAcces()
 {
  upperScreenClean();
  Writer("Enter PIN:",10,10,3,BLUE);
  while(l<5)
    {
  char keypressed = getKeyCode();
    if (keypressed != NO_KEY)
      {
      mykey[l]=keypressed;l++;
      upperScreenFrame();
      } 
    }
  if( mykey[0]==memory[0] && mykey[1]==memory[1] && mykey[2]==memory[2] 
  && mykey[3]==memory[3] && mykey[4]==memory[4] )
    {
    granted();
    } 
  else
    {
      result++;
      if(mykey[0]=='1' && mykey[1]=='2' && mykey[2]=='3' && mykey[3]=='4' 
      && mykey[4]=='5' )
        { // 12345
        l=0;
        upperScreenClean();
        new_code();
        } 
      else result++;
    }
  if(result>1){ 
    //warning_0();
    denied();
    result=0;
  }
  l=0; 
  upperScreenClean();
 }

/*-----------------------------------------------------------------------*
 *---------------------- call after wrong pin entered--------------------*/
void denied(void){
  upperScreenClean();
  Writer("Denied",20,10,5,RED);
  delay(2000);
}

/*-----------------------------------------------------------------------*
 *-----------------------call after right pin entered--------------------*/
void granted(void){
  upperScreenClean();
  Writer("Accept",20,10,5,GREEN);
  delay(2000);
}

/*-----------------------------------------------------------------------*
 *-----------------------Clean input screen------------------------------*/
void upperScreenClean(void){
  tft.fillRect(0,0,width*3,height,WHITE);
}

 /*-----------------------------------------------------------------------*
 *-----------------------Draw '*'*n on input screen-------------------------*/
void upperScreenFrame(void){
  int digit =(tft.width()/5);
  for(int i=0;i<=l;i++){
    Writer("*",(digit*l+1)-25,35,4);
  }
}

/*-----------------------------------------------------------------------*
 *----------------------draw lines---------------------------------------*/
void Lines(void){
color = RED;
  tft.drawFastVLine(width,height,tft.height(),color);
  tft.drawFastVLine(width*2,height,tft.height(),color);
  tft.drawFastHLine(0,height,tft.height(),color);
  tft.drawFastHLine(0,height*2,tft.height(),color);
  tft.drawFastHLine(0,height*3,tft.height(),color);
  tft.drawFastHLine(0,height*4,tft.height(),color);
}

/*-----------------------------------------------------------------------*
 *---------------------draw number on table------------------------------*/
//
void Numbers(void){
  color = RED;
  tft.setTextColor(color);
  tft.setTextSize(7);
  //1 row
   for ( int iks = 0; iks < 3; iks++){
    tft.setCursor((halfWidth-20)+(width*iks),((halfHeight-25)+height));
    tft.print(iks+1);
    }
    //2 row
    for ( int iks = 3; iks < 6; iks++){
    tft.setCursor((halfWidth-20)+(width*(iks-3)),((halfHeight-25)+height*2));
    tft.print(iks+1);
    }
     //3 row
    for ( int iks = 6; iks < 9; iks++){
    tft.setCursor((halfWidth-20)+(width*(iks-6)),((halfHeight-25)+height*3));
    tft.print(iks+1);
    }
    //4 row
    tft.setCursor((halfWidth-20),((halfHeight-25)+height*4));
    tft.print("*");
    tft.setCursor((halfWidth-20+width),((halfHeight-25)+height*4));
    tft.print("0");
    tft.setCursor((halfWidth-20+(width*2)),((halfHeight-25)+height*4));
    tft.print("#");
}

/*-----------------------------------------------------------------------*
 *----------------------------Matrix-------------------------------------*/
int ButtonPressed(int corX, int corY){
  int matrixX;
  int matrixY;
if(corX <= width){
    matrixX = 1;
  }
  else if(corX <= width*2){
    matrixX = 2;
  }
  else if(corX <= width*3){
    matrixX = 3;
  }
if(corY > height && corY <= height*2){
  matrixY = 1;
  }
  else if(corY > height && corY <= height*3){
    matrixY = 2;
  }
  else if(corY > height && corY <= height*4){
    matrixY = 3;
  }
  else if(corY > height && corY <= height*5){
    matrixY = 4;
  }
  return Matrix(matrixX,matrixY);
}

/*-----------------------------------------------------------------------*
 *-------------------character on matrix---------------------------------*/
char Matrix(int matX, int matY){
  if(matX == 1 && matY == 1){
    //1
    return 49;
  }
  else if(matX == 2 && matY == 1){
    //2
    return 50;
  }
  else if(matX == 3 && matY == 1){
    //3
    return 51;
  }
  else if(matX == 1 && matY == 2){
    //4
    return 52;
  }
  else if(matX == 2 && matY == 2){
    //5
    return 53;
  }
  else if(matX == 3 && matY == 2){
    //6
    return 54;
  }
  else if(matX == 1 && matY == 3){
    //7
    return 55;
  }
  else if(matX == 2 && matY == 3){
    //8
    return 56;
  }
  else if(matX == 3 && matY == 3){
    //9
    return 57;
  }
  else if(matX == 1 && matY == 4){
    //gwiazdka
  return 42;
  }
  else if(matX == 2 && matY == 4){
    //0
    return 48;
  }
  else if(matX == 3 && matY == 4){
    // kratka
    return 35;
  }
  //error
  else return 69;
}
 
/*------------------------------Setup------------------------------------*
 *-----------------------------------------------------------------------*/
void setup(void) {
  tft.reset();
  tft.begin(0x9341); // SDFP5408
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  width = tft.width() /3;
  height = tft.height() /5;
  halfWidth = width /2;
  halfHeight = height /2;
  upperScreenClean();
  Lines();
  Numbers();
}

void loop(void) {
  read_EEPROM();delay(30);
  if(code==0)new_code();
  else checkAcces();
}




