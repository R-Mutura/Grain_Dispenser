# Grain_Dispenser

The repo contains all the firmware codes for the project and instructions on how to load them to the hardware

## Requirements
1) Set up the IDE, for this application you will need to download the Arduino ide [here](https://www.arduino.cc/en/software) if you don't have it already installed for your distributions
2) Instal the board support package for ESP32 by goint to ` Tools > Board:xx > Board Manager`. In the search-bar type ESP32 and select install ESP32 by Espressid Systems.  
3) Now select DOIT ESP32 DEVKITT V1 from ` Tools > Board:xx > ESP32 Arduino > DOIT ESP32 DEVKITT`
 
 Once done proceed to the next instructions.
## Download the files 
4)  Download all the files on the `Code`:arrow_down_small: on this platform, In your downloads you will get the a folder :file_folder:  `GRAIN_DISPENSER_code`this will have:
     - GRAIN_DISPENSER_code.ino
     - defines.h
     - basic.h
     - weight.h 
</br> and `Libraries` zip file -this contains all the related libraries used in the project
## Library Installation
5)  On library installation unzip the folder :file_folder: `libraries` in the downloaded folder :file_folder:, copy and paste all the folders to your directory `documents/arduino/library` folder :file_folder:
6)  Open the `GRAIN_DISPENSER_code.ino`  in `GRAIN_DISPENSER_code.ino`folder :file_folder: with the downloaded ide, compile and upload to the hardware `(procedure on how to use the ide is on the tutorial link)`:link:    [tutorial](https://www.youtube.com/watch?v=nL34zDTPkcs&t=3s)

# Uploading the codes  :arrow_up:
You can buy the FTDI from here :link: [buy ftdi](https://www.amazon.com/HiLetgo-FT232RL-Converter-Adapter-Breakout/dp/B00IJXZQ7C/ref=sr_1_3?keywords=FTDI&qid=1650483928&sr=8-3)
or here 
 :link: [buy other FTDI](https://www.amazon.com/CP2102-Module-Converter-Downloader-Compatible/dp/B092YMT52G_)

1) Insert the FTDI programmer cable socket onto UART headers. ...
2) Open Arduino IDE software. ...
3) Next, call out the source code. ...
4) Select “FTDI” from the Tools > Programmer menu.
5) Select “Upload using Programmer” from the File menu.

You can also explore more from the following tutorial :link:[How to Use FTDI Programmer to upload Code](https://www.youtube.com/watch?v=JYchUapoqzc)
## Debugging 
To see the serial data as sent or received from either of the boards we can use the FTDI programmer or any USB to TTL converter so that you can have access to a serial port. Check out the headers on the board for uart communications 

## GRAIN_DISPENSER

![WhatsApp Image 2022-04-20 at 4 52 58 PM](https://user-images.githubusercontent.com/85989401/164726362-7c01db98-b902-4366-b0fd-c977ebaefdde.jpeg)

![Grain dispenser 1](https://user-images.githubusercontent.com/85989401/164727106-0635dc3a-2305-4025-9ec7-77a50d3a150c.png)

![Grain dispenser3](https://user-images.githubusercontent.com/85989401/164727123-5a369b41-59ad-41a8-ab32-bf23504b9e62.png)

### Features
- The Grain Dispenser is fitted with POE capabilities that enable plug and play functionality
- TFT 1.8" to dsiplay user data
- Server connection to perform data processing and verification
- loadcell for weight measurement of products
- RFID reader and writter for authentication
- And a mechanical design to enable smooth user self service experience

### HOW TO USE
- Ensure that u receive a registered RFID tag r register the one in your possesion
- Tap the tag/card on the rfid reader. If authentication is succesful
- the Green led comes on and a beep sound is heard for 300ms
- The weight of the container is taken and saved
- Lift the lever to dispense the amount of gramms/pounds you need and then return the lever to its initial position
- The data of true weight shown on the screen is automatically taken and sent to the server where you will be charged accordingly making it a seemless operation
- If you are not registered an invalid card error is shown on the screen
- Silo level is also monitored to allow for prompt refil by the supplier.


## GRAIN DISPENSER MACHINE.
