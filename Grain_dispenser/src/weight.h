#include <Arduino.h>


long sample=0;
float val=0;
long count=0;

unsigned long readCount(int DT, int SCK)
{
unsigned long Count;
unsigned char i;
pinMode(DT, OUTPUT);
digitalWrite(DT,HIGH);
digitalWrite(SCK,LOW);
Count=0;
pinMode(DT, INPUT);
while(digitalRead(DT));
for (i=0;i<24;i++)
{
digitalWrite(SCK,HIGH);
Count=Count<<1;
digitalWrite(SCK,LOW);
if(digitalRead(DT))
Count++;
}
digitalWrite(SCK,HIGH);
Count=Count^0x800000;
digitalWrite(SCK,LOW);
return(Count);
}

void calibrate(int DTc, int SCKc)
{

Serial.println("Calibrating...");
Serial.println("Please Wait...");
for(int i=0;i<100;i++)
{
count=readCount(DTc,SCKc);
sample+=count;
}
sample/=100;
Serial.println("Put 100g & wait");
count=0;
while(count<1000)
{
count=readCount(DTc,SCKc);
count=sample-count;
}
Serial.println();
Serial.println("Please Wait....");
delay(2000);
for(int i=0;i<100;i++)
{
count=readCount(DTc,SCKc);
val+=sample-count;
}
val=val/100.0;
val=val/100.0; // put here your calibrating weight
Serial.println();
}

float getweightdata(int dtx, int sckx)
{   
    count = readCount(dtx, sckx);
    float w=(((count-sample)/val)-2*((count-sample)/val));
    return(w);
}