/*
  
 Created by Josh Sharrer
 For my Student Innovation Project at UAT 

*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

HTTPClient http;

const char *ssid = "test";             // SSID of the WiFi you are connecting to
const char *password = "testpassword"; // Password or PSK of the WiFi you are connecting to

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
    Serial.println(WiFi.localIP()); // Output the local IP to serial connection
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
        if (LastState == 0)
        {
            Serial.println("Your mailbox is Open");
            LastState = 1;
            http.begin("http://192.168.10.10/?status=Mailbox%20Opened");
            http.addHeader("Content-Type", "text/plain");
            int httpCode = http.POST("Message from Mailbox");
            String payload = http.getString();
            if (httpCode == 200)
            {
                Serial.println("Notification Email Sent!");
            }
            else 
            {
                Serial.println("Error sending notification email!");
                Serial.println("HTTP Code: ");
                Serial.println(httpCode);
            }
            http.end();
        }
    }
    delay(1);
}
