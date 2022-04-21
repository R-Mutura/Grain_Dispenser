#include <Arduino.h>
#include <MFRC522.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>


#include "weight.h"

#define TFT_CS        2
#define TFT_RST       -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        15
#define TFT_BACKLIGHT -1 // Display backlight pin

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
 
 //inititialize display with software spi
 Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, mosi, sclk, TFT_RST);
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

  tft.init(240, 240);                // Init ST7789 240x240
  tft.setRotation(2);

  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print("WELCOME!");
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  // put your main code here, to run repeatedly:
   readrfid(); //must be called before checking tag data
   Serial.print(tagdata[0]);
   Serial.print(tagdata[1]);
   Serial.print(tagdata[2]);
   Serial.print(tagdata[3]);
   Serial.print(tagdata[4]);

  
  tft.setCursor(6, 6);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
    tft.print("Tag UID: ");
    tft.print(tagdata[0] + " ");
    tft.print(tagdata[1] + " ");
    tft.print(tagdata[2] + " ");
    tft.print(tagdata[3] + " ");
    tft.println(tagdata[4]);
  
  //total weight is equal to weight loadcell 1 +loadcell 2...this should be calibrated separately before rollout
  float weight = getweight();
    tft.print("Weight ");
    tft.print(weight);
    tft.println("g");
    
    tft.print("Level ");
    tft.print(getlevel());
    tft.println("cm");


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