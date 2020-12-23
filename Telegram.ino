/* 
***************************************************************************  
**  Program  : DSMRloggerAPI
**  Version  : v2.2.0
**
**  Copyright (c) 2020 Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//ALLEEN VOOR TESTEN - DOOR TSL EN BEPERKTE HEAP SIZE WERKT DIT VAAK NIET. NIET STABIEL GENOEG.

#ifdef USE_TELEGRAM

#include <WiFiClientSecure.h> 

#define BOTtoken "<fill token here>"
#define msg "SMR API test"

//Link to read data from https://jsonplaceholder.typicode.com/comments?postId=7
//Web/Server address to read/write from 
const char* host = "api.telegram.org";
const char* fingerprint = "47 BE AB C9 22 EA E8 0E 78 78 34 62 A7 9F 45 C2 54 FD E6 8B";
BearSSL::WiFiClientSecure https;

//=======================================================================

void Bot_setup() {
//  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
//  delay(1000);
//  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
//  WiFi.begin("Bali", "JijBentLief#5");     //Connect to your WiFi router
  Debugln("wifi status:");
  WiFi.printDiag(Serial);
  Debugln(WiFi.status());

  Debug("Connecting ");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Debug(".");
  }
  Debugln();
  //If connection successful show IP address in serial monitor
  DebugT("Connected to: ");  Debugln(WiFi.SSID());
  DebugT("IP address: ");  Debugln(WiFi.localIP());  //IP address assigned to your ESP
  
  
} //end bot_setup

//=======================================================================

void Bot_loop() {
 DebugT("connecting to "); Debugln(host);
 int con_result = 0;
  https.setInsecure();
  con_result = https.connect(host, 443);
  if (!con_result) {
    DebugT("connection failed code: "); Debugln(con_result);
    delay(500);
    return;
  }
  
  if (https.verify(fingerprint, host)) {
    Debugln("certificate matches");
  } else {
    Debugln("certificate doesn't match");
  }
  
  https.print(String("GET ") + BOTtoken + msg + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  
  while (https.connected()) {
    String line = https.readStringUntil('\n');
    if (line == "\r") {
      Debugln("headers received");
      break;
    }
  }
  
  String line = https.readStringUntil('\n');
  if (line.startsWith("{\"ok\":true")) {
    Debugln("Message successfull send!");
  } else {  Debugln("failed"); }

  https.stop();
  delay(500);
}

#endif

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
