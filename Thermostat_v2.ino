#define dataPin D2

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "xxxxxxxx";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "xxxxxxxx";
char pass[] = "xxxxxxxx";
unsigned long lastTime = 0;
int buttonState = 0;
int firstTime = 1;

void setup()
{
  pinMode(dataPin, OUTPUT);
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  delay(1000);
  Serial.println("Ready");
  Blynk.virtualWrite(V1, "Ready");
  Blynk.virtualWrite(V0, buttonState);
  Blynk.virtualWrite(V2, 21);
}

/***************************************************************************************/
BLYNK_WRITE(V0)
{
  //int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  Serial.print("Button state is: ");
  Serial.println(buttonState);

  if ((millis() - lastTime) < 30000)
  {
    Blynk.virtualWrite(V1, "Please Wait");
    Serial.println("Please wait");
    Blynk.virtualWrite(V0, buttonState);
  }
  else
  {
    lastTime = millis();
    buttonState = !buttonState;
    Blynk.virtualWrite(V0, buttonState);
    heatingControl(buttonState);
    Serial.print("Button state is: ");
    Serial.println(buttonState);
    switch (buttonState)
    {
      case 0:
        Blynk.virtualWrite(V1, "Heating OFF");
        Serial.println("Heating OFF");
        break;
      case 1:
        Blynk.virtualWrite(V1, "Heating ON");
        Serial.println("Heating ON");
        break;
    }
  }
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
          digitalWrite(dataPin, LOW);
          delayMicroseconds(500);
          digitalWrite(dataPin, HIGH);
          delayMicroseconds(500);
          break;
        case 1:
          digitalWrite(dataPin, HIGH);
          delayMicroseconds(500);
          digitalWrite(dataPin, LOW);
          delayMicroseconds(500);
          break;
        case 2:
          digitalWrite(dataPin, LOW);
          delayMicroseconds(500);
          break;
      }
    }
    delay(1000);
  }
}

/***************************************************************************************/
void loop()
{
  Blynk.run();

}
