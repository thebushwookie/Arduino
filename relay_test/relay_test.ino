int relaypin = 3;
float atmpsi = 14.35;
int mapSen = analogRead(A0);
int datapin = 9;
float mapSenCalc;

void setup()
{
  Serial.begin(9600);
  pinMode(relaypin, OUTPUT);
  int mapSen = 0;

}

void loop()
{
  int mapSen = analogRead(A0);
  float atmpsi = ((((float)mapSen / (float)1023 + 0.04) / .004) * .145);
  //mapSenCalc = map(mapSen, 0, 1023, 0, 255);
  int mapSenCalc = int(mapSen/4);
  Serial.print(mapSen);
  Serial.print(",");
  Serial.println(mapSenCalc);
  analogWrite(datapin, mapSenCalc);
  if (atmpsi < 14) {
    digitalWrite(relaypin, HIGH);
    delay(100);
  }
  else {
    digitalWrite(relaypin, LOW);
    delay(100);
  }
}
