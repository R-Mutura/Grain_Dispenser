int measurement_done_flag = 0;

#include <Arduino.h>
#include "weight.h"
#include "PCF8574.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define mosi 12
#define sclk 14
#define miso 13
 
#define TFT_CS        2
#define TFT_RST       -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        15
#define TFT_BACKLIGHT -1 // Display backlight pin

 //inititialize display with software spi
 Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, mosi, sclk, TFT_RST);
 
//loadcell amp1
#define DT1  32
#define SCK1 33

//loadcell amp2
#define DT2   25
#define SCK2 26

#define pcf_addr  0x20
PCF8574 pcf(pcf_addr);

//PCF8574 pins
//pcf
#define open_pos   0
#define closed_pos 1
#define greenled   2
#define redled     3
#define solenoid   4
#define rfid_rst   5
//on esp32s
#define buzzer     27

float getweight();


float basic()
{
 ///////////////all basic operations will be handled here ////////////////////////
 //1. GET INITIAL CONTAINER WEIGHT
   float container = getweight();
   float total =0;
   float load = 0;
     pcf.write(redled,LOW);//RED LED OFF
     pcf.write(greenled, HIGH);
   //BEEP SOUND
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
   //write solenoid high to open it
    pcf.write(solenoid, HIGH);
 // 2. CHECK IF LEVER IS LIFTED AND KEEP READING THE CURRENT WEIGHT UNTIL THE LEVER IS PUT DOWN
//check if level open switch is active i.e ready high when the lever is lifted
  while(!pcf.read(open_pos) || pcf.read(closed_pos)){
     //get weight reading and update screen here
        total= getweight();
        load = total - container;
        
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextSize(10);
        tft.setCursor(0, 0);
        tft.setTextColor(ST77XX_BLUE);
        tft.setTextWrap(true);
        tft.print(load);
        measurement_done_flag =1;
    }
    //return the value of weight measured when level is returned to the closed position
    return load;
}

 //total weight is equal to weight loadcell 1 +loadcell 2...this should be calibrated separately before rollout
 float getweight(){
    float weight1 = getweightdata(DT1,SCK1);
    float weight2 = getweightdata(DT2,SCK2);
    float weight = weight1 + weight2;
    
   return (weight); //the actual weight of the object
 }
 //NOTE: YOU CAN ALSO CALIBRATE TO CALIBRATE EACH CELL SEPARATELY

 void done(){
  pcf.write(greenled,LOW); //GREEN LED OFF
  pcf.write(redled,HIGH);//RED LED ON
  //BEEP SOUND
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
   //write solenoid high to open it
    pcf.write(solenoid, LOW);
  }
