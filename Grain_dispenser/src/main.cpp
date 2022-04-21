#include <Arduino.h>
#include <MFRC522.h>

#include "weight.h"

#define ECHO 16
#define TRIG 17

#define DT1  32
#define SCK1 33

#define DT2   25
#define SCK2 26

#define SS_PIN   4  // ESP32 pin GIOP5 
#define RST_PIN -1 // ESP32 pin GIOP27 

#define mosi 12
#define sclk 14
#define miso 13
 
 //uid storage variable
  byte tagdata[5] = {0};
MFRC522 rfid(SS_PIN);


void  readrfid    ();
int   getlevel    ();
float getweight   ();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin(sclk , miso, mosi); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
   readrfid(); //must be called before checking tag data
   Serial.print(tagdata[0]);
   Serial.print(tagdata[1]);
   Serial.print(tagdata[2]);
   Serial.print(tagdata[3]);
   Serial.print(tagdata[4]);
  
  //total weight is equal to weight loadcell 1 +loadcell 2...this should be calibrated separately before rollout
  float weight = getweight();
}

void readrfid(){

  
    if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        tagdata[i] = rfid.uid.uidByte[i];
        Serial.print(rfid.uid.uidByte[i], HEX);

      }
      Serial.println();

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }

  int level =getlevel();

}

int getlevel(){
  int distance = 0;
  int duration = 0;
  digitalWrite(TRIG,HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG,LOW);
  delayMicroseconds(20);
  duration = pulseIn(ECHO, HIGH); //To receive the reflected signal.
  distance= duration*0.034/2; //IN CENTIMETERS
  if (distance>80)
  {
    return 1; //storage level is very low and refil is needed
  }
  if (distance>30 && distance <=80)
  {
    return 5; //storage level is okay
  }
    if (distance<=30)
  {
    return 10; //storage level is full
  }

}
 float getweight(){
     float weight1 = getweightdata(DT1,SCK1);
    float weight2 = getweightdata(DT2,SCK2);
    float weight = weight1 + weight2;
   return (weight); //the actual weight of the object

 }