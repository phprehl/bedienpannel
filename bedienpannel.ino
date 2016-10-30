/************************************************************
ESP8266_Shield_Demo.h
SparkFun ESP8266 AT library - Demo
Jim Lindblom @ SparkFun Electronics
Original Creation Date: July 16, 2015
https://github.com/sparkfun/SparkFun_ESP8266_AT_Arduino_Library

This example demonstrates the basics of the SparkFun ESP8266
AT library. It'll show you how to connect to a WiFi network,
get an IP address, connect over TCP to a server (as a client),
and set up a TCP server of our own.

Development environment specifics:
  IDE: Arduino 1.6.5
  Hardware Platform: Arduino Uno
  ESP8266 WiFi Shield Version: 1.0

This code is released under the MIT license.

Distributed as-is; no warranty is given.
************************************************************/

//////////////////////
// Library Includes //
//////////////////////
// SoftwareSerial is required (even you don't intend on
// using it).
#include <SoftwareSerial.h> 
#include <SparkFunESP8266WiFi.h>

//////////////////////////////
// WiFi Network Definitions //
//////////////////////////////
// Replace these two character strings with the name and
// password of your WiFi network.
const char mySSID[] = "FRITZ!Box 7490";
const char myPSK[] = "";
//const char mySSID[] = "HONOR_PLK_E8CB";
//const char myPSK[] = "philippprehl";

//////////////////////////////
// ESP8266Server definition //
//////////////////////////////
// server object used towards the end of the demo.
// (This is only global because it's called in both setup()
// and loop()).
ESP8266Server server = ESP8266Server(80);

//////////////////
// HTTP Strings //
//////////////////
//const char destServer[] = "google.com";
const String htmlHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Connection: close\r\n\r\n"
                          "<!DOCTYPE HTML>\r\n"
                          "<html>\r\n";

//const String httpRequest = "GET / HTTP/1.1\n"
//                           "Host: google.com\n"
//                           "Connection: close\n\n";

// All functions called from setup() are defined below the
// loop() function. They modularized to make it easier to
// copy/paste into sketches of your own.
void setup() 
{
  // Serial Monitor is used to control the demo and view
  // debug information.
  Serial1.begin(115200);
  //serialTrigger(F("Press any key to begin."));

  // initializeESP8266() verifies communication with the WiFi
  // shield, and sets it up.
  initializeESP8266();

  // connectESP8266() connects to the defined WiFi network.
  connectESP8266();

  // displayConnectInfo prints the Shield's local IP
  // and the network it's connected to.
  displayConnectInfo();

  //serialTrigger(F("Press any key to connect client."));
  //clientDemo();
  
  serialTrigger(F("Press any key to test server."));
  serverSetup();
}

void loop() 
{
  serverDemo();
}

void initializeESP8266()
{
  // esp8266.begin() verifies that the ESP8266 is operational
  // and sets it up for the rest of the sketch.
  // It returns either true or false -- indicating whether
  // communication was successul or not.
  // true
  int test = esp8266.begin(115200,ESP8266_HARDWARE_SERIAL);
  if (test != true)
  {
    Serial1.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  Serial1.println(F("ESP8266 Shield Present"));
}

void connectESP8266()
{
  // The ESP8266 can be set to one of three modes:
  //  1 - ESP8266_MODE_STA - Station only
  //  2 - ESP8266_MODE_AP - Access point only
  //  3 - ESP8266_MODE_STAAP - Station/AP combo
  // Use esp8266.getMode() to check which mode it's in:
  int retVal = esp8266.getMode();
  if (retVal != ESP8266_MODE_STA)
  { // If it's not in station mode.
    // Use esp8266.setMode([mode]) to set it to a specified
    // mode.
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if (retVal < 0)
    {
      Serial1.println(F("Error setting mode."));
      errorLoop(retVal);
    }
  }
  Serial1.println(F("Mode set to station"));

  // esp8266.status() indicates the ESP8266's WiFi connect
  // status.
  // A return value of 1 indicates the device is already
  // connected. 0 indicates disconnected. (Negative values
  // equate to communication errors.)
  retVal = esp8266.status();
  if (retVal <= 0)
  {
    Serial1.print(F("Connecting to "));
    Serial1.println(mySSID);
    // esp8266.connect([ssid], [psk]) connects the ESP8266
    // to a network.
    // On success the connect function returns a value >0
    // On fail, the function will either return:
    //  -1: TIMEOUT - The library has a set 30s timeout
    //  -3: FAIL - Couldn't connect to network.
    retVal = esp8266.connect(mySSID, myPSK);
    if (retVal < 0)
    {
      Serial1.println(F("Error connecting"));
      errorLoop(retVal);
    }
  }
}

void displayConnectInfo()
{
  char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  // esp8266.getAP() can be used to check which AP the
  // ESP8266 is connected to. It returns an error code.
  // The connected AP is returned by reference as a parameter.
  int retVal = esp8266.getAP(connectedSSID);
  if (retVal > 0)
  {
    Serial1.print(F("Connected to: "));
    Serial1.println(connectedSSID);
  }

  // esp8266.localIP returns an IPAddress variable with the
  // ESP8266's current local IP address.
  IPAddress myIP = esp8266.localIP();
  Serial1.print(F("My IP: ")); Serial1.println(myIP);
}


void serverSetup()
{
  // begin initializes a ESP8266Server object. It will
  // start a server on the port specified in the object's
  // constructor (in global area)
  server.begin();
  Serial1.print(F("Server started! Go to "));
  Serial1.println(esp8266.localIP());
  Serial1.println();
}

void serverDemo()
{
  // available() is an ESP8266Server function which will
  // return an ESP8266Client object for printing and reading.
  // available() has one parameter -- a timeout value. This
  // is the number of milliseconds the function waits,
  // checking for a connection.
  ESP8266Client client = server.available(500);
  
  if (client) 
  {
    Serial1.println(F("Client Connected!"));
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) 
    {





    String req = client.readString();
    Serial1.println("request empfangen");
    Serial1.print(req);
    Serial1.println();
    //client.flush();
  
    // Match the request
    int val;
    if (req.indexOf("/gpio/0") != -1)
      val = 0;
    else if (req.indexOf("/gpio/1") != -1)
      val = 1;
    else {
      Serial1.println("invalid request");
      client.stop();
      return;
  }
  client.flush();
  Serial1.print("stelle GPIO: ");
  Serial1.println(val);
  client.print(htmlHeader);










      /*
      if (client.available()) 
      {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          Serial1.println(F("Sending HTML page"));
          // send a standard http response header:
          client.print(htmlHeader);
          String htmlBody;
          // output the value of each analog input pin
          for (int a = 0; a < 6; a++)
          {
            htmlBody += "A";
            htmlBody += String(a);
            htmlBody += ": ";
            htmlBody += String(analogRead(a));
            htmlBody += "<br>\n";
          }
          htmlBody += "</html>\n";
          client.print(htmlBody);
          break;
        }
        if (c == '\n') 
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }*/
    // give the web browser time to receive the data
    delay(1);
   
    // close the connection:
    client.stop();
    Serial1.println(F("Client disconnected"));
  }
  
}
}
// errorLoop prints an error code, then loops forever.
void errorLoop(int error)
{
  Serial1.print(F("Error: ")); Serial1.println(error);
  Serial1.println(F("Looping forever."));
  for (;;)
    ;
}

// serialTrigger prints a message, then waits for something
// to come in from the serial port.
void serialTrigger(String message)
{
  Serial1.println();
  Serial1.println(message);
  Serial1.println();
  while (!Serial1.available())
    ;
  while (Serial1.available())
    Serial1.read();
}
