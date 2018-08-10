// IMPORTANT: ELEGOO_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Elegoo_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Elegoo_TFTLCD tft;

#define BOXSIZE 40
#define PENRDIUS 3
int oldcolor, currentcolor;
int screen = 0;
int graphcount = 0;

void setup(void) {
  
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.4\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.4\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

   uint16_t identifier = tft.readID();
   if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }
  else if(identifier==0x1111)
  {     
      identifier=0x9328;
       Serial.println(F("Found 0x9328 LCD driver"));
  }
  else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9328;
  
  }
  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  tft.fillRect(280, 0,BOXSIZE, BOXSIZE, RED);
  tft.fillRect(280, BOXSIZE, BOXSIZE, BOXSIZE, MAGENTA);
  tft.fillRect(280, BOXSIZE*2, BOXSIZE, BOXSIZE, GREEN);
  tft.drawRect(280, 0, BOXSIZE, BOXSIZE, WHITE);
  currentcolor = RED;

  pinMode(13, OUTPUT);
  
  //Graph();
  //Sinewave();
  
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000


void loop() 
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  
  // if sharing pins, you'll need to fix the directions of the touchscreen pins

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {}
    
    if (p.y < (TS_MINY-5)) {
      Serial.println("erase");
      // press the bottom of the screen to erase 
      tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, BLACK);
    }
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    //p.x = tft.width()-map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     //p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

    
    tft.fillRect(10,220,90,240, BLACK);
    tft.setCursor(10,220);
    tft.setTextColor(WHITE);  tft.setTextSize(2);
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");
    tft.print(p.x);
    tft.print(",");
    tft.print(p.y);
    
    if (p.y > 210) {
       oldcolor = currentcolor;

       if (p.x > 280) {
         tft.fillScreen(BLACK);  
         Screen1();
         screen = 1;
       } else if (p.x > 210) {
         tft.fillScreen(BLACK); 
         Screen2();
         screen = 2;
       } else if (p.x > 140) {
         tft.fillScreen(BLACK); 
         Screen3();
         screen = 3;
       }
       
       if (oldcolor != currentcolor) {
          if (oldcolor == RED) tft.fillRect(280, 0, BOXSIZE, BOXSIZE, RED);
          if (oldcolor == MAGENTA) tft.fillRect(280, 40, BOXSIZE, BOXSIZE, MAGENTA);
          if (oldcolor == GREEN) tft.fillRect(280, 80, BOXSIZE, BOXSIZE, GREEN);
    }
    }
     
     int mapSen = analogRead(A5);
     int atmpsi = 0.00;
     float boost = ((((float)mapSen/(float)1023+0.04)/.004)*.145)-atmpsi;
     tft.setTextColor(GREEN);tft.setTextSize(2);
     tft.fillRect(100,0,60,20,BLACK);
     tft.setCursor(100,0);tft.print(boost);tft.print(" PSI");
     delay(5);

     if (currentcolor == GREEN){
     Boostgraph();
     tft.setTextColor(GREEN);tft.setTextSize(2);
     tft.fillRect(210,0,40,20,BLACK);
     tft.print(" ON");
     }
} 

//END OF VOID LOOP


  unsigned long Jabberwocky() {
  tft.setCursor(0, 0);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.println("Twas brillig");
  tft.setTextSize(2);
  tft.println("and the slithy toves");
  tft.setTextSize(1);
  tft.println("Did gyre and gimble in the wabe:");
  tft.println("All mimsy were the borogoves,");
  tft.println("And the mome raths outgrabe.");
  tft.println("'Beware the Jabberwock, my son!'");
}

  unsigned long Elephant() {
  tft.setCursor(0, 0);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.println("He thought he saw");
  tft.setTextSize(3);
  tft.println("an Elephant");
  tft.setTextSize(1);
  tft.println("That practised on a fife");
  tft.println("He looked again, and found it was");
  tft.println("A letter from his wife.");
  tft.println("'At length I realize,' he said,");
  tft.println("'The bitterness of life! '");
}

  unsigned long Screen1() {
    currentcolor = RED;
         Jabberwocky();
         tft.drawRect(280, 0, BOXSIZE, BOXSIZE, WHITE);
         tft.fillRect(280, 40, BOXSIZE, BOXSIZE, MAGENTA);
         tft.fillRect(280, 80, BOXSIZE, BOXSIZE, GREEN);
         tft.setCursor(290,12);tft.print("1");
  }


  unsigned long Screen2() {
    currentcolor = MAGENTA;
         Elephant();
         tft.fillRect(280, 0, BOXSIZE, BOXSIZE, RED);
         tft.drawRect(280, 40, BOXSIZE, BOXSIZE, WHITE);
         tft.fillRect(280, 80, BOXSIZE, BOXSIZE, GREEN);
         tft.setCursor(290,52);tft.print("2");
  }

    unsigned long Screen3() {
      currentcolor = GREEN;
         tft.fillRect(280, 0, BOXSIZE, BOXSIZE, RED);
         tft.fillRect(280, 40, BOXSIZE, BOXSIZE, MAGENTA);
         tft.drawRect(280, 80, BOXSIZE, BOXSIZE, WHITE);
         tft.setTextColor(GREEN);tft.setTextSize(1);
         tft.setCursor(290,102);tft.print("3");
         Graph();
         Boostgraph();
}

unsigned long Graph() {
  tft.drawFastVLine(20, 40, 160, WHITE); //Origin is at (20,200) Address at 21, 201
  tft.drawFastHLine(20, 200, 240, WHITE); //bottom
  //tft.drawFastHLine(20, 120, 240, WHITE); //middle
  tft.setTextColor(GREEN);tft.setTextSize(2);
  tft.setCursor(0,0);tft.print("BOOST");
}

unsigned long Sinewave() {
  int count;
  float i = 0.0;
  for(i = 0.0; i < 24.0; i = i + (0.1)) {
    count = count + 1;
    delay(1);
    //tft.setCursor(20,0);tft.fillRect(30, 0, 150, 40, RED);
    //tft.print(" ");tft.print(i);tft.print(" ");tft.print(i2);
    tft.drawFastHLine(20 + count,120 + (80*sin(i)), 1, BLUE); //x position then y offset
    tft.drawFastHLine(20 + count - 1,120, 1, WHITE); 
    tft.fillRect(21 + count, 40, 15,160, BLACK);
}
}

unsigned long Boostgraph() {
  int mapSen = analogRead(A5);
  int atmpsi = 14.23;          //should be around 14.23 PSI at home
  float boost = ((((float)mapSen/(float)1023+0.04)/.004)*.145)-atmpsi;
    graphcount = graphcount + 1;
    if (graphcount == 240){
      graphcount = 0;
    }
    //tft.setCursor(20,0);tft.fillRect(30, 0, 150, 40, RED);
    //tft.print(" ");tft.print(i);tft.print(" ");tft.print(i2);
    tft.drawFastHLine(20 + graphcount,120 + (boost * 40), 1, BLUE); //x position then y offset
    tft.drawFastHLine(20, 200, 240, WHITE); 
    tft.fillRect(25 + graphcount, 40, 20,160, BLACK);
}


