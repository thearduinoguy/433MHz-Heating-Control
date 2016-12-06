void setup() {
  pinMode(2, OUTPUT);
  delay(2000);
}

/****************************************************************/
void heatingControl(boolean onOff)
{
  int on[]   = {0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1};
  int off[]  = {0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1};

  for (int repeat = 1; repeat <= 3; repeat++)
  {
    for (int i = 0; i < sizeof( onOff ? on : off  ) / sizeof( int ); i++) {
      switch (onOff ? on[i] : off[i])
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
void loop()
{
  heatingControl(true);
  delay(20000);

  heatingControl(false);
  delay(20000);
}
