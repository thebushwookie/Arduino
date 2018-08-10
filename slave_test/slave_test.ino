float atmpsi = 14.35;
int mapSen = analogRead(A0);


void setup()
{
  Serial.begin(9600);
  int mapSen = 0;
}

void loop()
{
  mapSen = analogRead(A0);
  float atmpsi = ((((float)mapSen / (float)1023 + 0.04) / .004) * .145);
  if (atmpsi == 0) {
    exit;
  }
  
  Serial.println(mapSen);
}
