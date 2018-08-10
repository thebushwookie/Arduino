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

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define	BLACK   0x0000
#define	BLUE    0x002F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRDIUS 3
int oldcolor, currentcolor = RED;
int screen = 0;
int graphcount = 0;
int barcount = 0;
int newBar = 0;
int lastBar = 0;
int lastPSI = 0;
int PSI = 0;
int currentbar = 7;
int difference = 0;
float atmpsi = 14.4;
boolean inited = false;

void setup(void) {

  int mapSen = analogRead(A5);
  float atmpsi = ((((float)mapSen / (float)1023 + 0.04) / .004) * .145);

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
  identifier = 0x9341;
  Serial.println(F("Found 0x9341 LCD driver"));

  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  tft.fillRect(280, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(280, BOXSIZE, BOXSIZE, BOXSIZE, MAGENTA);
  tft.fillRect(280, BOXSIZE * 2, BOXSIZE, BOXSIZE, GREEN);
  tft.drawRect(280, 0, BOXSIZE, BOXSIZE, WHITE);
  currentcolor = RED;

  pinMode(13, OUTPUT);

}

#define MINPRESSURE 10
#define MAXPRESSURE 1000


void loop()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {}

  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));

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
  float boost = ((((float)mapSen / (float)1023 + 0.04) / .004) * .145) - atmpsi;
  tft.setTextColor(GREEN);
  tft.setTextSize(2);

  if (currentcolor == GREEN) {

    tft.setTextColor(GREEN);
    tft.setTextSize(2);
    tft.fillRect(210, 0, 40, 20, BLACK);
    tft.print(" ON");
    tft.setCursor(290, 92); tft.print("3");
    Screen3();
  }

  if (currentcolor == RED) {
    tft.setTextColor(GREEN);
    tft.setTextSize(2);
    tft.fillRect(210, 0, 40, 20, BLACK);
    tft.setCursor(290, 12); tft.print("1");
    Screen1();
  }

  if (currentcolor == MAGENTA) {
    tft.setTextColor(GREEN);
    tft.setTextSize(2);
    tft.fillRect(210, 0, 40, 20, BLACK);
    tft.setCursor(290, 52); tft.print("2");
    Screen2();
  }
}

//END OF VOID LOOP

void Screen1() {
  currentcolor = RED;
  tft.drawRect(280, 0, BOXSIZE, BOXSIZE, WHITE);
  tft.fillRect(280, 40, BOXSIZE, BOXSIZE, MAGENTA);
  tft.fillRect(280, 80, BOXSIZE, BOXSIZE, GREEN);
  BarGraph();
  BoostBar();
}

void Screen2() {
  currentcolor = MAGENTA;
  tft.fillRect(280, 0, BOXSIZE, BOXSIZE, RED);
  tft.drawRect(280, 40, BOXSIZE, BOXSIZE, WHITE);
  tft.fillRect(280, 80, BOXSIZE, BOXSIZE, GREEN);
  tft.drawRect(20, 139, 280, 32, WHITE);
  BoostGFX();
}

void Screen3() {
  currentcolor = GREEN;
  tft.fillRect(280, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(280, 40, BOXSIZE, BOXSIZE, MAGENTA);
  tft.drawRect(280, 80, BOXSIZE, BOXSIZE, WHITE);
  Graph();
  Boostgraph();
}

void Graph() {
  tft.drawFastVLine(20, 40, 160, WHITE); //Origin is at (20,200) Address at 21, 201
  tft.drawFastHLine(20, 200, 240, WHITE); //bottom
  //tft.drawFastHLine(20, 120, 240, WHITE); //middle
  tft.setTextColor(GREEN); tft.setTextSize(2);
  tft.setCursor(0, 0); tft.print("BOOST");
}

void BarGraph() {
  tft.drawFastVLine(89, 40, 160, WHITE);
  //tft.drawFastHLine(20, 120, 240, WHITE); //middle
  tft.setTextColor(GREEN); tft.setTextSize(2);
  tft.setCursor(0, 0); tft.print("BAR");
}

void Sinewave() {
  int count;
  float i = 0.0;
  for (i = 0.0; i < 24.0; i = i + (0.1)) {
    count = count + 1;
    delay(1);
    tft.drawFastHLine(20 + count, 120 + (80 * sin(i)), 1, BLUE);
    tft.drawFastHLine(20 + count - 1, 120, 1, WHITE);
    tft.fillRect(21 + count, 40, 15, 160, BLACK);
  }
}

void Boostgraph() {  //option 3

  long mapSen = analogRead(A5);
  for (byte b = 0; b < 150; b++) {
    mapSen += analogRead(A5);
  }
  long mapAvg = int(mapSen / 150);
  float boost = ((((float)mapAvg / (float)1024 + 0.04) / .004) * .145) - atmpsi;
  graphcount = graphcount + 1;
  if (graphcount == 240) {
    graphcount = 0;
  }
  tft.fillRect(21 + graphcount, 40, 12, 160, BLACK);
  tft.drawFastVLine(21 + graphcount, 150 + (boost * -3), 10, BLUE);
  tft.drawFastHLine(21 + graphcount, 150 + (boost * -3), 1, WHITE);
}

void BoostBar() {  //option 1

  long mapSen = 0;
  static int oldPSI;
  for (byte b = 0; b < 250; b++) {
    mapSen += analogRead(A5);
  }
  long mapAvg = mapSen / 250;
  int PSI = map(mapAvg, 0, 1024, 0, 31);
  int difference = PSI - currentbar;

  tft.fillRect(0, 20, 100, 20, BLACK);
  tft.setCursor(0, 20);
  tft.print(difference);
  tft.setCursor(70, 20);
  tft.print(PSI);

  if (PSI != currentbar) {
    int diff = abs(oldPSI - PSI);
    if (diff > 1) {
      if (PSI > oldPSI)
        PSI = oldPSI + 1;
      else
        PSI = oldPSI - 1;
    }
    drawBar(PSI);
    oldPSI = PSI;
  }
}

void drawBar (unsigned int PSI) {
  if (PSI <= 4) {
    PSI = 4;
  }
  if (PSI >= 11) { //Boost Draw
    tft.fillRect(90 + ((PSI - 11) * 10), 40, 9, 160, RED);
    tft.fillRect(80, 40, 9, 160, BLACK);
  }
  if ((PSI >= 11) && (difference <= -1)) { //Boost erase
    tft.fillRect(90 + ((PSI - 10) * 10), 40, 9, 160, BLACK);
  }
  if (PSI < 11) { //Vac draw
    tft.fillRect(90 - ((11 - PSI) * 10), 40, 9, 160, BLUE);
    tft.fillRect(90, 40, 9, 160, BLACK);
  }
  if ((PSI < 11) && (difference >= 1)) { //Vac erase
    tft.fillRect(80 - ((11 - PSI) * 10), 40, 9, 160, BLACK);
  }
  currentbar = PSI;
}

void BoostGFX() {  //option 2
  long mapSen = 0;
  static int oldPSI;
  for (byte b = 0; b < 250; b++) {
    mapSen += analogRead(A5);
  }
  long mapAvg = mapSen / 250;
  int PSI = map(mapAvg, 0, 1024, 0, 31);
  static float peakPSI;
  int difference = PSI - currentbar;

  if (PSI != currentbar) {
    int diff = abs(oldPSI - PSI);
    if (diff > 1) {
      if (PSI > oldPSI)
        PSI = oldPSI + 1;
      else
        PSI = oldPSI - 1;
    }
    drawBarGFX(PSI);
    oldPSI = PSI;
    tft.fillRect(40, 60, 232, 56, BLACK);
    tft.setTextColor(WHITE); tft.setTextSize(8);
    if (PSI > 17) {
      //tft.setTextColor(RED);
    }
    float boost = ((((float)mapAvg / (float)1023 + 0.04) / .004) * .145) - atmpsi;
    tft.setCursor(40, 60);
    if (boost < 0) {
      tft.print(" VAC");
    }
    if (boost >= 0) {
      tft.print(boost);
    }
    if (boost > peakPSI){
      peakPSI = boost;
      tft.setCursor(50,0);
      tft.fillRect(50,0,120,20,BLACK);
      tft.setTextSize(2);
      tft.print(peakPSI,3);tft.print(" PSI");
    }
  }
}

void drawBarGFX (int PSI) { // white bar graph
  if (PSI <= 8) {
    PSI = 8;
  }
  if (PSI >= 11) {
    tft.fillRect(50 + ((PSI - 11) * 10), 140, 9, 30, WHITE);
    tft.fillRect(40, 140, 9, 30, BLACK);
  }
  if ((PSI >= 11) && (difference <= -1)) {
    tft.fillRect(50 + ((PSI - 10) * 10), 140, 9, 30, BLACK);
  }
  if (PSI < 11) {
    tft.fillRect(50 - ((11 - PSI) * 10), 140, 9, 30, WHITE);
    tft.fillRect(50, 140, 9, 30, BLACK);
  }
  if ((PSI < 11) && (difference >= 1)) {
    tft.fillRect(40 - ((11 - PSI) * 10), 140, 9, 30, BLACK);
  }
  currentbar = PSI;
}


