// By Mike McRoberts 2016
// Control of a Worcester 24CDi Boiler via 433MHz radio transmitter

int on[]   = {0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1};
int off[]  = {0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1};

/****************************************************************/
void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  Serial.println("Ready");
  delay(2000);
}

/****************************************************************/
void heatingOn()
{
  Serial.println("Boiler On");
  for (int repeat = 1; repeat <= 3; repeat++)
  {
    for (int i = 0; i < sizeof( on ) / sizeof( int ); i++) {
      switch (on[i])
      {
        case 0:
          digitalWrite(2, LOW);
          delayMicroseconds(500);
          digitalWrite(2, HIGH);
          delayMicroseconds(500);
          break;
        case 1:
          digitalWrite(2, HIGH);
          delayMicroseconds(500);
          digitalWrite(2, LOW);
          delayMicroseconds(500);
          break;
        case 2:
          digitalWrite(2, LOW);
          delayMicroseconds(500);
          break;
      }
    }
    delay(1000);
  }
}

/****************************************************************/
void heatingOff()
{
  Serial.println("Boiler Off");
  for (int repeat = 1; repeat <= 3; repeat++)
  {
    for (int i = 0; i < sizeof( off ) / sizeof( int ); i++) {
      switch (off[i])
      {
        case 0:
          digitalWrite(2, LOW);
          delayMicroseconds(500);
          digitalWrite(2, HIGH);
          delayMicroseconds(500);
          break;
        case 1:
          digitalWrite(2, HIGH);
          delayMicroseconds(500);
          digitalWrite(2, LOW);
          delayMicroseconds(500);
          break;
        case 2:
          digitalWrite(2, LOW       );
          delayMicroseconds(500);
          break;
      }
    }
    delay(1000);
  }
}

/****************************************************************/
void loop()
{
  heatingOn();
  delay(20000);

  heatingOff();
  delay(20000);
}
