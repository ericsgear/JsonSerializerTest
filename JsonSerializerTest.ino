/*
 * 
 * This is designed to work with a computer hooked to the USB port (Serial) of an 
 * Arduino and another serial device on pins 2 and 3 (BTSerial).  
 * 
 * The original testing device was a BlueTooth module.  I was getting garbled text over
 * 120-130 characters.  I think this was due to a memory issue with an ATMega 328, so
 * instead of BTSerial.readString(), I'm using a read() method to bring in the string.
 * 
 * I'm assuming there is not any nested JSON objects, so the home grown method kills
 * it's loop after any "}" character.
 * 
 * Test data to put into the Serial Monitor window: {test:"data"}
 * 
 *  BTW, you will need ArduinoJson.h from : https://github.com/bblanchon/ArduinoJson/
 *  https://bblanchon.github.io/ArduinoJson/
 *
 *  The license for ArduinoJson.h is also the MIT license.
 *
 */

#include <ArduinoJson.h>
#include <SoftwareSerial.h>

String serialData;
String resultsString;
String btData;
SoftwareSerial BTSerial(2, 3); // RX | TX

void setup() {
  BTSerial.begin(57600);
  //BTSerial.begin(9600);
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to initialize
  }

  /*
  BTSerial.print("AT+NAMESCOREBOARD_EG");  //  renames the serial (BT) device
  
  delay(1200);
  BTSerial.print("AT+PIN4321");  //  resets the (BT) device password
  BTSerial.begin(57600);
  */
  
  //delay(1200);
  //BTSerial.print("AT+UART=4");  //  change the 4 to any hex number in the below list to change your baud rate  // I have not had any luck with this on an HC-06
  //BTSerial.begin(9600);
  
  //  all standard serial connection speeds are listed
  //  1=1200, 2=2400, 3=4800, 4=9600 (default), 5=19200, 6=38400, 7=57600, 8=115200, 9=230400, A=460800, B=921600, C=1382400 
  Serial.println("Ready");
}

void loop() {
  
  if (Serial.available()) {  //  looks for a string from the computer to send back to the computer and to pass onto the 2nd serial device
    resultsString = "";  //  clear the string of any previous data
    serialData = Serial.readString(); 
    DynamicJsonBuffer jsonBuffer1;  //  get a new JsonBuffer for each use
    JsonObject& root = jsonBuffer1.parse(serialData);
    if (root.success()){
      root.printTo(resultsString);
      Serial.println(resultsString);
      BTSerial.print(resultsString);
    }
  }
  
  if (BTSerial.available()) {  //  looks for a string from the other serial device to send back to the 2nd serial device and to pass onto the computer
    resultsString = "";  //  clear the string of any previous data
    readBuffer(btData);
    DynamicJsonBuffer jsonBuffer2;  //  get a new JsonBuffer for each use
    JsonObject& root2 = jsonBuffer2.parse(btData);
    if (root2.success()){
      root2.printTo(resultsString);
      Serial.println("JSON: " + resultsString);
      BTSerial.print("JSON: " + resultsString);
    }
    btData = "";
  }
}

void readBuffer(String &data){
  //  reads from the 2nd serial device one character at a time and kills the loop after finding a "}"
  //  this tries to prevent the JsonBuffer from getting too large a string and not parsing anything, if the device sends multiple JSON strings at once
  char c = ' ';
  while (BTSerial.available() && (c != '}')){
    c = BTSerial.read();
    data += c;
  }
}
