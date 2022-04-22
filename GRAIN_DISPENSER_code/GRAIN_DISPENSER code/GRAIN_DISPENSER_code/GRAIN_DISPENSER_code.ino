/****************************************************************************************************************************
  WebClient.ino - Simple Arduino web server sample for Ethernet shield
  
  Ethernet_Generic is a library for the W5x00 Ethernet shields trying to merge the good features of
  previous Ethernet libraries
  
  Built by Khoi Hoang https://github.com/khoih-prog/Ethernet_Generic
 *****************************************************************************************************************************/
#include <EthernetHttpClient.h>
#include "defines.h"
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include <Arduino.h>
#include <MFRC522.h>




#include "basic.h"
#include "weight.h"



#define ECHO 16
#define TRIG 17



#define SS_PIN   4  // ESP32 pin GIOP5 
#define RST_PIN -1 // ESP32 pin GIOP27 



 //uid storage variable
  char tagdata[5] = {0};
 // char  packetBuffer[100];
MFRC522 rfid(SS_PIN);



int   readrfid    ();
int   getlevel    ();
float getweight   ();

//#define MOSI 19
//#define MISO 23
//#define SCK  18
//#define SS   5

char server[] = "arduino.cc";
char pathdata[] = "weight/";
char pathlevel[] = "level/";

// Initialize the Ethernet client object 
  // EthernetClient client;
//HTTPClient http(client);
 EthernetClient      client;
EthernetHttpClient  httpClient(client, server, 8080);

void setup()
{

  Serial.begin(115200);
  while (!Serial);
  btn.attachClick(handleClick);
  delay(500);

  Serial.print("\nStarting WebClient on "); Serial.print(BOARD_NAME);
  Serial.print(F(" with ")); Serial.println(SHIELD_TYPE); 
  
#if (USING_SPI2)
  #if defined(CUR_PIN_MISO)
    ETG_LOGWARN(F("Default SPI pinout:"));
    ETG_LOGWARN1(F("MOSI:"), CUR_PIN_MOSI);
    ETG_LOGWARN1(F("MISO:"), CUR_PIN_MISO);
    ETG_LOGWARN1(F("SCK:"),  CUR_PIN_SCK);
    ETG_LOGWARN1(F("SS:"),   CUR_PIN_SS);
    ETG_LOGWARN(F("========================="));
  #endif
#else
  ETG_LOGWARN(F("Default SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), MOSI);
  ETG_LOGWARN1(F("MISO:"), MISO);
  ETG_LOGWARN1(F("SCK:"),  SCK);
  ETG_LOGWARN1(F("SS:"),   SS);
  ETG_LOGWARN(F("========================="));
#endif

#if defined(ESP8266)
  // For ESP8266, change for other boards if necessary
  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   D2    // For ESP8266
  #endif

  ETG_LOGWARN1(F("ESP8266 setCsPin:"), USE_THIS_SS_PIN);

  // For ESP8266
  // Pin                D0(GPIO16)    D1(GPIO5)    D2(GPIO4)    D3(GPIO0)    D4(GPIO2)    D8
  // Ethernet_Generic   X                 X            X            X            X        0
  // D2 is safe to used for Ethernet_Generic libs
  //Ethernet.setCsPin (USE_THIS_SS_PIN);
  Ethernet.init (USE_THIS_SS_PIN);

#elif defined(ESP32)

  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   5   //22    // For ESP32
  #endif

  ETG_LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);

  // Must use library patch for Ethernet, EthernetLarge libraries
  // ESP32 => GPIO2,4,5,13,15,21,22 OK with Ethernet, Ethernet2, EthernetLarge
  // ESP32 => GPIO2,4,5,15,21,22 OK with Ethernet3

  //Ethernet.setCsPin (USE_THIS_SS_PIN);
  Ethernet.init (USE_THIS_SS_PIN);
  
#elif ETHERNET_USE_RPIPICO

  pinMode(USE_THIS_SS_PIN, OUTPUT);
  digitalWrite(USE_THIS_SS_PIN, HIGH);
  
  // ETHERNET_USE_RPIPICO, use default SS = 5 or 17
  #ifndef USE_THIS_SS_PIN
    #if defined(ARDUINO_ARCH_MBED)
      #define USE_THIS_SS_PIN   17     // For Arduino Mbed core
    #else  
      #define USE_THIS_SS_PIN   17    // For E.Philhower core
    #endif
  #endif

  ETG_LOGWARN1(F("RPIPICO setCsPin:"), USE_THIS_SS_PIN);

  // Must use library patch for Ethernet, EthernetLarge libraries
  // For RPI Pico using Arduino Mbed RP2040 core
  // SCK: GPIO2,  MOSI: GPIO3, MISO: GPIO4, SS/CS: GPIO5
  // For RPI Pico using E. Philhower RP2040 core
  // SCK: GPIO18,  MOSI: GPIO19, MISO: GPIO16, SS/CS: GPIO17
  // Default pin 5/17 to SS/CS

  //Ethernet.setCsPin (USE_THIS_SS_PIN);
  Ethernet.init (USE_THIS_SS_PIN);
  
#else   //defined(ESP8266)
  // unknown board, do nothing, use default SS = 10
  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   10    // For other boards
  #endif

  #if defined(BOARD_NAME)
    ETG_LOGWARN3(F("Board :"), BOARD_NAME, F(", setCsPin:"), USE_THIS_SS_PIN);
  #else
    ETG_LOGWARN1(F("Unknown board setCsPin:"), USE_THIS_SS_PIN);
  #endif

  // For other boards, to change if necessary 
  Ethernet.init (USE_THIS_SS_PIN);

#endif    // defined(ESP8266)

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;
  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  Ethernet.begin(mac[index]);

  //SPIClass SPI2(HSPI);
  //Ethernet.begin(mac[index], &SPI2);

  // Just info to know how to connect correctly
  // To change for other SPI
#if defined(CUR_PIN_MISO)
  ETG_LOGWARN(F("Currently Used SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), CUR_PIN_MOSI);
  ETG_LOGWARN1(F("MISO:"), CUR_PIN_MISO);
  ETG_LOGWARN1(F("SCK:"),  CUR_PIN_SCK);
  ETG_LOGWARN1(F("SS:"),   CUR_PIN_SS);
  ETG_LOGWARN(F("========================="));
#else
  ETG_LOGWARN(F("Currently Used SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), MOSI);
  ETG_LOGWARN1(F("MISO:"), MISO);
  ETG_LOGWARN1(F("SCK:"),  SCK);
  ETG_LOGWARN1(F("SS:"),   SS);
  ETG_LOGWARN(F("========================="));
#endif

  Serial.print(F("Using mac index = "));
  Serial.println(index);

  Serial.print(F("Connected! IP address: "));
  Serial.println(Ethernet.localIP());

  if (Ethernet.getChip() == w5500)
  {
    Serial.print(F("Speed: "));    Serial.print(Ethernet.speedReport());
    Serial.print(F(", Duplex: ")); Serial.print(Ethernet.duplexReport());
    Serial.print(F(", Link status: ")); Serial.println(Ethernet.linkReport());
  }

  Serial.println();
  Serial.println(F("Starting connection to server..."));
   //HTTPClient http(client, server);
  // if you get a connection, report back via serial
//  if (client.connect(server, 80))
//  {
//    Serial.println(F("Connected to server"));
//    // Make a HTTP request
//    client.println(F("GET /asciilogo.txt HTTP/1.1"));
//    client.println(F("Host: arduino.cc"));
//    client.println(F("Connection: close"));
//    client.println();
//  }
//}
// // Initialize the Ethernet client object DONE

  

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
  delay(1000);

  
  

}

int apicall(String Data){
   httpClient.get("/"+ Data);
 int statusCode = httpClient.responseStatusCode(); //if status code ==200 then card has been foun
  String response = httpClient.responseBody(); //if OKAY to buy in (value parameter)
  Serial.print("statusCode:"); Serial.print(statusCode);
  Serial.print("response:"); Serial.print(response);
  
////////json operation to stringify the data///////////////////////
  StaticJsonBuffer<300> JSONBuffer; //Memory pool decleration
  JsonObject& parsed = JSONBuffer.parseObject(response);
    if (!parsed.success()) {   //Check for errors in parsing
 
    Serial.println("Parsing failed");
    delay(5000);
    return 3;
 
  }
  String Dvalue = parsed["Value"];   //get the value parameter to determine what if what we can open the lever for the set weight amount
  int val = Dvalue.equals("OKAY"); //VAL IS TRUE IF THEY ARE EQUAL AND VICE VERSA
  
  if (statusCode == 200 && val){
     return 1;
  }
  else{
    return 0;
  }
}
void printoutData()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
    Serial.flush();  
  }
  if (!client.connected())
  {
    Serial.println();
    Serial.println(F("Disconnecting from server..."));
    client.stop();

    // do nothing forevermore
    while (true)
      yield();
  }
}


void loop()
{
    btn.tick();
   int x = getlevel();
   if(x==0){
          Serial.println("Silo level: EMPTY");
          tft.setTextSize(5);
          tft.setCursor(6, 6);
          tft.setTextColor(ST77XX_RED);
          tft.setTextWrap(true);
          tft.println("Silo level: EMPTY");
          delay(50);
     return;
    }
  tft.fillScreen(ST77XX_BLACK);
  // put your main code here, to run repeatedly:
   int updatestatus = readrfid(); //must be called before checking tag data
   float weightmeasured = 0;
 if(updatestatus && measurement_done_flag == 0) {//if there is an rfid update then
    
   Serial.print(tagdata[0]);
   Serial.print(tagdata[1]);
   Serial.print(tagdata[2]);
   Serial.print(tagdata[3]);
   Serial.println(tagdata[4]);
   Serial.println(tagdata);
   //MAKE AN API CALL TO THE SERVER TO SEE IF THE CARD IS VALID 
   int card_status = apicall(String(tagdata));
      if(card_status == 1){
        long current_time = millis();
        do{
        weightmeasured = basic();
        }while(millis() - current_time<= 20000);
        done();
        card_status=0;
        updatestatus=0;
        return;
        
        }
       else if(card_status == 3){
          updatestatus=0;
          Serial.println("Server or system ERROR!!");
          tft.setTextSize(5);
          tft.setCursor(6, 6);
          tft.setTextColor(ST77XX_RED);
          tft.setTextWrap(true);
          tft.println("System ERROR!!");
          delay(1000); //wait for 1 second then proceed to do something else
        }
       else{
          updatestatus = 0;
          Serial.println("RFID TAG ID NOT VALID!");
          tft.setTextSize(5);
          tft.setCursor(6, 6);
          tft.setTextColor(ST77XX_BLUE);
          tft.setTextWrap(true);
          tft.println("RFID TAG ID NOT VALID!!");
          tft.println("Register Please!");
          delay(1000); //wait for 1 second then proceed to do something else
        }
   
   }
  
  
//    tft.print("Tag UID: ");
//    tft.print(tagdata[0] + " ");
//    tft.print(tagdata[1] + " ");
//    tft.print(tagdata[2] + " ");
//    tft.print(tagdata[3] + " ");
//    tft.println(tagdata[4]);
  
      //we need to display the measured weight for 20 seconds and send the measured weight and uuid of the card to another api(server+newpath+data) for processing
     if(measurement_done_flag = 1){
          tft.fillScreen(ST77XX_BLACK);
          tft.setTextSize(10);
          tft.setCursor(0, 0);
          tft.setTextColor(ST77XX_BLUE);
          tft.setTextWrap(true);
          tft.print(weightmeasured); 
          pound ? tft.print("lb"): tft.print("g");
   //////////CALL DONE FUNCTION
          done();
   /////////SEND DATA TO SERVER VIA THE API/////////
           String packet =String(pathdata) + String(tagdata) +":"+ String(weightmeasured);
           apicall(String(tagdata));
           measurement_done_flag=0;
           delay(17000);//delay for 20seconds while displaying the most recent value
      }
  
}


int readrfid(){

     
    if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        tagdata[i] = char(rfid.uid.uidByte[i]);
        Serial.print(rfid.uid.uidByte[i], HEX);

      }
      
      Serial.println();
 
      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
      
    }
    return 1;
  }
  else{
    return 0;
    }
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
   if (distance>90)
  { 
    //send to allert api the storage is very low
    String datalevel = String(pathlevel) + "EMPTY";
    apicall(datalevel);
    return 0; //storage level is very low and refil is needed
  }
  else if (distance>70)
  { 
    //send to allert api the storage is empty
    String datalevel = String(pathlevel) + "LOW";
    apicall(datalevel);
    return 1; //storage level is very low and refil is needed
  }
  else if (distance>30 && distance <=70)
  {
    return 5; //storage level is okay
  }
   else(distance<=30);
  {
    return 10; //storage level is full
  }

}

// float getweight(){
//     float weight1 = getweightdata(DT1,SCK1);
//    float weight2 = getweightdata(DT2,SCK2);
//    float weight = weight1 + weight2;
//   return (weight); //the actual weight of the object
//
// }

void handleClick(){
  pound = !pound;
  }
