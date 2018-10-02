/*
  
 Created by Josh Sharrer
 For my Student Innovation Project at UAT
 This code notifys the owner that their mailbox 
 has being opened via email.   

*/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <base64.h>

HTTPClient http;
WiFiClientSecure client;

const int SMTP_PORT = 465;
const char *SMTP_SERVER = "smtp.gmail.com";
const char *ssid = "SSID";             // SSID of the WiFi you are connecting to
const char *password = "PASSWORD"; // Password or PSK of the WiFi you are connecting to
String error_message, Login_base64, Passwrd_base64, ServerResponse;
String Senders_Login = "USERNAME@GMAIL.COM";
String Senders_Password = "PASSWORD";
String From = "Mailbox";
String To = "USERNAME@SERVER.COM";
String Subject = "Mailbox Opened";
String Message = "Mailbox has been opened. If this was not you, look into it immediately!";

int ledOpen = 4;     // Green LED
int ledClose = 5;    // RED LED
int switchReed = 16; // Reed Switch
int LastState = 0;   // 0 is closed, 1 is open

void setup()
{
  Serial.begin(9600); // Start the serial connection at the specified baud rate
  delay(100);         // Add a delay
  Serial.println();   // Print a blank line
  Serial.println();   // Print a blank line
  Serial.println("Starting the mailbox notifier...");

  pinMode(ledOpen, OUTPUT);   // Setup GPIO pin 4 as an output device
  pinMode(ledClose, OUTPUT);  // Setup GPIO pin 5 as an output device
  pinMode(switchReed, INPUT); // Setup GPIO pin 16 as an input device

  Serial.println();
  Serial.println();
  Serial.println("Connecting to the SSID:");
  Serial.println(ssid); // Output the SSID to serial

  WiFi.begin(ssid, password); // Start the WiFi connection

  while (WiFi.status() != WL_CONNECTED) // While the WiFi is not connected, wait and print a line
  {
    delay(500);
    Serial.println();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());      // Output the local IP to serial connection
  if (digitalRead(switchReed) == HIGH) // If the door is opened
  {
    Serial.println("Your mailbox is currently Closed");
  }
  if (digitalRead(switchReed) == LOW) // If the door is closed
  {
    Serial.println("Your mailbox is currently Open");
  }
}

void loop()
{
  if (digitalRead(switchReed) == HIGH) // If the door is opened
  {
    digitalWrite(ledOpen, LOW);
    digitalWrite(ledClose, HIGH);
    if (LastState == 1)
    {
      Serial.println("Your mailbox is Closed");
      LastState = 0;
    }
  }
  else
  {
    digitalWrite(ledOpen, HIGH);
    digitalWrite(ledClose, LOW);
    if (LastState == 0);
    {
      Serial.println("Your mailbox is Open");
      LastState = 1;
      SendMail(To, Subject, Message);
      if (error_message != "") Serial.println(error_message);
    }
  }
  delay(1);
}

void SendMail(String To, String Subject, String Message)
{
  if (!client.connect(SMTP_SERVER, SMTP_PORT))
  {
    error_message = "SMTP responded that it could not connect to the mail server";
    return;
  }
  if (ErrorWhileWaitingForSMTP_Response("220", 500))
  {
    error_message = "SMTP responded with a Connection Error";
    return;
  }
  client.println("HELO server");
  if (ErrorWhileWaitingForSMTP_Response("250", 500))
  {
    error_message = "SMTP responded with an Identification error";
    return;
  }
  client.println("AUTH LOGIN");
  WaitSMTPResponse(ServerResponse, 500);
  client.println(base64::encode(Senders_Login));
  WaitSMTPResponse(ServerResponse, 500);
  client.println(base64::encode(Senders_Password));
  ;
  if (ErrorWhileWaitingForSMTP_Response("235", 500))
  {
    error_message = "SMTP responded with an Authorisation error";
    return;
  }
  String mailFrom = "MAIL FROM: <" + String(From) + '>';
  client.println(mailFrom);
  WaitSMTPResponse(ServerResponse, 500);
  String recipient = "RCPT TO: <" + To + '>';
  client.println(recipient);
  WaitSMTPResponse(ServerResponse, 500);
  client.println("DATA");
  if (ErrorWhileWaitingForSMTP_Response("354", 500))
  {
    error_message = "SMTP DATA error";
    return;
  }
  client.println("From: <" + String(From) + '>');
  client.println("To: <" + String(To) + '>');
  client.print("Subject: ");
  client.println(String(Subject));
  client.println("Mime-Version: 1.0");
  client.println("Content-Type: text/html; charset=\"UTF-8\"");
  client.println("Content-Transfer-Encoding: 7bit");
  client.println();
  String body = "<!DOCTYPE html><html lang=\"en\">" + Message + "</html>";
  client.println(body);
  client.println(".");
  if (ErrorWhileWaitingForSMTP_Response("250", 1000))
  {
    error_message = "SMTP responded with a Message error";
    return;
  }
  client.println("QUIT");
  if (ErrorWhileWaitingForSMTP_Response("221", 1000))
  {
    error_message = "SMTP responded with a QUIT error";
    return;
  }
  client.stop();
  Serial.println("Message Sent");
}

bool ErrorWhileWaitingForSMTP_Response(String Error_Code, int TimeOut)
{
  int timer = millis();
  while (!client.available())
  {
    if (millis() > (timer + TimeOut))
    {
      error_message = "SMTP responsed that a Timeout occurred";
      return true;
    }
  }
  ServerResponse = client.readStringUntil('\n');
  if (ServerResponse.indexOf(Error_Code) == -1)
    return true;
  return false;
}

bool WaitSMTPResponse(String Error_Code, int TimeOut)
{
  int timer = millis();
  while (!client.available())
  {
    if (millis() > (timer + TimeOut))
    {
      error_message = "SMTP responded that a Timeout occurred";
      return false;
    }
  }
  ServerResponse = client.readStringUntil('\n');
  if (ServerResponse.indexOf(Error_Code) == -1)
    return false;
  return true;
}