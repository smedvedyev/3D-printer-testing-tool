#include <Arduino.h>

#define LED 2
#define FAN 3
#define NTC A0
#define MAXFANSPEED 255
#define MEDIUMFANSPEED 178
#define LOWFANSPEED 102

float tempSampleRead = 0;
float tempLastSample = 0;
float tempSampleSum = 0;
float tempSampleCount = 0;
float tempMean;

int temp;
int value = 0;
bool firstStartup = true;

void startupFan()
{
  analogWrite(FAN, MAXFANSPEED);
  delay(200);
  firstStartup = false;
}

int readTemp()
{
  // Vo = Vs * (R1 / ( R2 + R0 ))
  /* 1- Temperature Measurement */

  if (millis() >= tempLastSample + 1) /* every 1 milli second taking 1 reading */
  {
    tempSampleRead = analogRead(NTC);               /* read analog value from sensor */
    tempSampleSum = tempSampleSum + tempSampleRead; /* add all analog value for averaging later*/
    tempSampleCount = tempSampleCount + 1;          /* keep counting the sample quantity*/
    tempLastSample = millis();                      /* reset the time in order to repeat the loop again*/
  }

  if (tempSampleCount == 1000) /* after 1000 sample readings taken*/
  {
    unsigned int anaIn = analogRead(NTC);
    float T = log(10000.0 * ((1024.0 / anaIn - 1)));

    T = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * T * T)) * T) - 217.15;

    Serial.print("Value: ");
    Serial.print(anaIn);
    Serial.print(" (");
    Serial.print(T);
    Serial.println(" C)");

    tempSampleSum = 0;   /* reset all the total analog value back to 0 for the next count */
    tempSampleCount = 0; /* reset the total number of samples taken back to 0 for the next count*/
    return T;
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(NTC, INPUT);
}

void loop()
{
  temp = readTemp();

  if (temp <= 16)
  {
    digitalWrite(LED, HIGH);
  }
  else if (temp >= 22)
  {
    value = MAXFANSPEED;
  }
  else if (temp >= 20)
  {
    if (firstStartup)
    {
      startupFan();
    }
    value = MEDIUMFANSPEED;
  }
  else if (temp >= 18)
  {
    if (firstStartup)
    {
      startupFan();
    }
    value = LOWFANSPEED;
  }
  analogWrite(FAN, value);
}