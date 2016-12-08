#define ON 1
#define OFF 0

#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

#define dataPin D3
#define BLYNK_PRINT Serial

HTU21D myHumidity;

char auth[] = "xxx";
char ssid[] = "xxx";
char pass[] = "xxx";

unsigned long lastButtonPress;
unsigned long lastTransmit;
unsigned long lastUpdate;
unsigned long lastTempCheck;

int enabledState;
int firstTime = 1;
int requiredTemp;
float temp;

WidgetLED powerLED(V2);
WidgetLED heatingLED(V3);
WidgetLED hotWaterLED(V4);
WidgetLED flameLED(V5);

void setup()
{
  EEPROM.begin(3);
  myHumidity.begin();
  //EEPROM.write(1, 0);
  //EEPROM.commit();
  pinMode(dataPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Serial.println("Initialising....");

  lastButtonPress = millis();
  lastTransmit = millis();
  lastUpdate = millis();
  lastTempCheck = millis();
  enabledState = EEPROM.read(1);
  Serial.print("EEPROM: System is  ");
  Serial.println(enabledState ? "ENABLED" : "DISABLED");
  requiredTemp = EEPROM.read(2);
  Serial.print("EEPROM: Target Temperature is ");
  Serial.println(String(requiredTemp) + "°C");
  delay(500);
  Blynk.syncAll();
}

BLYNK_CONNECTED() {
  Serial.println("CONNECTED");  Blynk.run();
  powerLED.on();  Blynk.run();
  Blynk.virtualWrite(V11, requiredTemp);  Blynk.run();
  Blynk.virtualWrite(V10, requiredTemp);  Blynk.run();
}


/***************************************************************************************/
BLYNK_WRITE(V0)
{
  //int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  Serial.print("System  is ");
  Serial.println(enabledState ? "ENABLED" : "DISABLED");

  if ((millis() - lastButtonPress) < 30000)
  {
    Blynk.virtualWrite(V1, "Please Wait");
    Serial.println("Please wait");
    Blynk.virtualWrite(V0, enabledState);
  }
  else
  {
    lastButtonPress = millis();
    enabledState = !enabledState;
    Serial.print("Heating ");
    Serial.println(enabledState ? "ON" : "OFF");
    EEPROM.write(1, enabledState);
    EEPROM.commit();
    Blynk.virtualWrite(V0, enabledState);
    Serial.print("Button state is: ");
    Serial.println(enabledState);
  }
}

BLYNK_WRITE(V10)
{
  requiredTemp = param.asInt();  Blynk.run();
  EEPROM.write(2, requiredTemp);
  EEPROM.commit();
  Serial.print("Target Temperature is ");
  Serial.println(requiredTemp);
}

/****************************************************************/
void heatingControl(boolean onOff)
{
  Serial.print(millis());
  Serial.print("    Transmitting ");
  Serial.print(onOff ? "ON" : "OFF");
  Serial.println(" command. ");
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
    int waitTime = millis();
    if ((millis() - waitTime) < 1000) {
      Blynk.run();
    }
  }
  lastTransmit = millis();
}

void updateBlynk()
{
  Blynk.run();

  Blynk.virtualWrite(V11, requiredTemp);  Blynk.run();
}

void getTemps()
{
  char Buffer1[16]; Blynk.run();
  char Buffer2[16]; Blynk.run();
  float humd = myHumidity.readHumidity(); Blynk.run();
  temp = myHumidity.readTemperature(); Blynk.run();
  dtostrf(humd, 1, 1, Buffer1);    Blynk.run();
  dtostrf(temp, 1, 1, Buffer2); Blynk.run();

  Blynk.virtualWrite(V7, String(Buffer1) + "%"); Blynk.run();
  Blynk.virtualWrite(V6, String(Buffer2) + "℃"); Blynk.run();
  Blynk.virtualWrite(V8, temp);
  Blynk.virtualWrite(V9, humd);
}

void checkTemp()
{
  if ((temp > requiredTemp) && (enabledState == ON))
  {
    heatingControl(OFF);
    Blynk.virtualWrite(V1, "Heating OFF");  Blynk.run();
    Blynk.setProperty(V1, "color", "#FF0000");  Blynk.run();
    Blynk.setProperty(V0, "color", "#FF0000");  Blynk.run();
    Blynk.virtualWrite(V0, 0);  Blynk.run();
    heatingLED.off();       Blynk.run();
  }

  if ((temp < requiredTemp) && (enabledState == ON))
  {
    heatingControl(ON);
    Blynk.virtualWrite(V1, "Heating ON");  Blynk.run();
    Blynk.setProperty(V1, "color", "#00FF00"); Blynk.run();
    Blynk.setProperty(V0, "color", "#00FF00"); Blynk.run();
    Blynk.virtualWrite(V0, 1); Blynk.run();
    heatingLED.on();       Blynk.run();
  }

  lastTempCheck = millis();
}

/***************************************************************************************/
void loop()
{
  Blynk.run();

  if ((millis() -   lastTransmit) > 60000)
  {
    if ((temp < requiredTemp) && (enabledState == ON)) heatingControl(ON);
    else heatingControl(OFF);
  }

  if ((millis() -   lastUpdate) > 1000)
  {
    updateBlynk();
    getTemps();
    lastUpdate = millis();
  }

  if ((millis() -   lastTempCheck) > 60000)
  {
    checkTemp();
  }

}
