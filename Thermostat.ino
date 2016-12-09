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

unsigned long lastTransmit;
unsigned long lastUpdate;
unsigned long lastTempCheck;
float temp;

int enabledState;
int requiredTemp;

void setup()
{
  EEPROM.begin(3);
  myHumidity.begin();
  //EEPROM.write(2, 0);
  //EEPROM.commit();
  pinMode(dataPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Serial.println("Initialising....");

  lastTransmit = millis();
  lastUpdate = millis();
  lastTempCheck = millis();
  enabledState = EEPROM.read(1);
  Serial.println("EEPROM: System is  " + String(enabledState ? "ENABLED" : "DISABLED"));
  requiredTemp = EEPROM.read(2);
  Serial.println("EEPROM: Target Temperature is " + String(requiredTemp) + "°C");
}

BLYNK_CONNECTED() {
  Serial.println("CONNECTED");  Blynk.run();
  Blynk.virtualWrite(V2, requiredTemp);  Blynk.run();
  Blynk.virtualWrite(V3, requiredTemp);  Blynk.run();
  Blynk.virtualWrite(V0, enabledState);
  Blynk.setProperty(V0, "color", (enabledState ? "#00FF00" : "#FF0000"));  Blynk.run();
  Blynk.virtualWrite(V1, "Ready");  Blynk.run();
  Blynk.setProperty(V1, "color", "#00FF00");  Blynk.run();
  getTemps();
}

BLYNK_WRITE(V0)
{
  enabledState = !enabledState;

  EEPROM.write(1, enabledState);
  EEPROM.commit();

  Serial.println("System is " + String(enabledState ? "ENABLED" : "DISABLED"));

  Blynk.virtualWrite(V0, enabledState);
  Blynk.setProperty(V0, "color", enabledState ? "#00FF00" : "#FF0000");  Blynk.run();
}

BLYNK_WRITE(V2)
{
  requiredTemp = param.asInt();  Blynk.run();
  Blynk.virtualWrite(V3, requiredTemp);  Blynk.run();
  EEPROM.write(2, requiredTemp);
  EEPROM.commit();
  Serial.println("Target Temperature is " + String(requiredTemp));
}

void heatingControl(boolean onOff)
{
  Serial.print(millis());
  Serial.println(" Transmitting " + String(onOff ? "ON signal" : "OFF signal"));
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
  Blynk.virtualWrite(V1, String(onOff ? "Heating ON" : "Heating OFF"));  Blynk.run();
  Blynk.setProperty(V1, "color", String(onOff ? "#00FF00" : "#FF0000"));  Blynk.run();
  lastTransmit = millis();
}

void getTemps()
{
  float humd = myHumidity.readHumidity(); Blynk.run();
  temp = myHumidity.readTemperature(); Blynk.run();

  Blynk.virtualWrite(V4, temp); Blynk.run();
  Blynk.virtualWrite(V5, humd); Blynk.run();

}

void checkTemp()
{
  if ((temp > requiredTemp) && (enabledState == ON)) heatingControl(OFF);  
  if ((temp < requiredTemp) && (enabledState == ON)  heatingControl(ON);
  lastTempCheck = millis();
}

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
    getTemps();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    lastUpdate = millis();
  }

  if ((millis() -   lastTempCheck) > 60000) checkTemp();
}
