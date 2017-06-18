#include <CapacitiveSensor.h>
#define TRUE 1
#define FALSE 0

CapacitiveSensor cs_11_2 = CapacitiveSensor(11,2);        // 10-20M resistor between pins 11 & 2, pin 2 is sensor pin, add a wire and or foil
CapacitiveSensor cs_11_4 = CapacitiveSensor(11,4);        // 1M resistor between pins 11 & 4, pin 4 is touchsensor pin, add a wire and or foil

//runtime
long thisTime;
long lastTime;
//fake trigger
long triggertime;
long triggerSchwelle=300;
long trigger;
int fake=FALSE;
//fake radiation scan
long scantime;
long radiation;
long mittelwert=0;
int t1=100;
long output=0;
int t2=1;


long minRadiation=1000;
long maxRadiation=0;
uint8_t radiationRatio=0;

char led_pin=9; //led pin

void setup()
{
    cs_11_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
    cs_11_4.set_CS_AutocaL_Millis(0xFFFFFFFF);
    Serial.begin(115200);
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, fake);
}

void loop()
{   
#ifdef DEBUG
    lastTime = thisTime;
    thisTime = millis();
    Serial.println(thisTime-lastTime);
#endif

    if (isTime(&scantime, 50)){
        //read actual "radiation"
        radiation=scanForRadiation();
        //calc min and max
        maxRadiation=max(maxRadiation, radiation);
        if (radiation>=0){
          minRadiation=min(minRadiation, radiation);
        }
        //calculate the output
        radiationRatio=constrain((double)255/(maxRadiation)*(radiation-mittelwert),0,255);
        mittelwert=(((double)mittelwert*t1)+ radiation)/(t1+1);
        //lower end cutoff
        /*if(radiationRatio<=5){
          radiationRatio=0;
        }*/

        //smooth the output
        output=(((double)output*t2)+radiationRatio)/(t2+1);
        //lower end cutoff
        if(output<=5){
          output=0;
        }
        analogWrite(led_pin, output);
    }


    
    if (isTime(&triggertime, 500)){
      //faketrigger activation
      trigger=TriggerCheck();
      if (trigger > triggerSchwelle){
          fake=TRUE;
      }
      else {
          fake=FALSE;
      }
    }

#ifdef DEBUG
        Serial.print(mittelwert);
        Serial.print("\t");
        Serial.print(radiationRatio);
        Serial.print("\t");
        Serial.print(maxRadiation);
        Serial.print("\t");
        Serial.print(minRadiation);
        Serial.print("\t");
        Serial.print(radiation);
        Serial.print("\t");
        Serial.print(trigger);
        Serial.print("\t");
        Serial.println(fake);
#endif
    
}

long scanForRadiation()
{   
    long distance =  cs_11_2.capacitiveSensor(1);
    return distance;
}

long TriggerCheck()
{
    long distance =  cs_11_4.capacitiveSensor(50);
    return distance;
}

long isTime(long * time, long diff)
{
    long retval = 0;
    long now = millis();
    if(*time > now)
    {
        if( (now + (0x3fffffffL)) - (*time - (0x3fffffffL)) >= diff )
        {
            *time = now;
            return 1;
        }
    }
    if((now - *time) > diff)
    {
        *time = now;
        retval = 1;
    }
    return retval;
}

