// Once you import this library into an app on the web based IDE, modify the code to look something like the following.
// This code is a heavily modified version of the MPU6050_raw.ino example found elsewhere in this repo.
// This code has been tested against the MPU-9150 breakout board from Sparkfun.

// This #include statement was automatically added by the Particle IDE.
#include "MPU6050.h"
#include "MQTT.h"

int ledPin = D7;

              //For ThingSpeak Broker

char mqttUserName[] = "Orangutan";           // Use any name.
char mqttPass[] = "HWW3ONP6MYDQ5XU7";        // Change this to your MQTT API key from Account > MyProfile.
char writeAPIKey[] = "6WP1KI6RP42GZKLN";     // Change this to your channel write API key.
long channelID = 834490;                     // Change this to your channel number.
char server[] = "192.168.1.102";       // Define the ThingSpeak MQTT broker.
static const char alphanum[] ="0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";                // This variable is used for random generation of the client ID.

// Define a callback function to initialize the MQTT client.
void callback(char* topic, byte* payload, unsigned int length) {
 }

MQTT client(server, 1883, callback);       // Initialize the MQTT client.


/*                //For Other Brokers
void callback(char* topic, byte* payload, unsigned int length);

MQTT client("iot.eclipse.org", 1883, callback);

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "RED"))
        RGB.color(255, 0, 0);
    else if (!strcmp(p, "GREEN"))
        RGB.color(0, 255, 0);
    else if (!strcmp(p, "BLUE"))
        RGB.color(0, 0, 255);
    else
        RGB.color(255, 255, 255);
    delay(1000);
}
*/


unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 50; // Post data every 0.5 seconds.

// MPU variables:
MPU6050 accelgyro1(0x68);
MPU6050 accelgyro2(0x69);
int16_t ax1, ay1, az1;
int16_t gx1, gy1, gz1;
int16_t ax2, ay2, az2;
int16_t gx2, gy2, gz2;


bool ledState = false;
void toggleLed() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
}

  int8_t L_red, L_green, L_blue;

void setup() {
    RGB.control(true);
    pinMode(ledPin, OUTPUT);

    Wire.begin();
    accelgyro1.initialize();
    accelgyro2.initialize();

}

void loop() {

    // If MQTT client is not connected then reconnect.
    if (client.isConnected())//burası yanıyor, publish toggle etmiyor 2 led var
    {
      L_green = 255;
    }
    if (accelgyro1.testConnection() & accelgyro2.testConnection())
    {
      L_blue = 255;
    }
    RGB.color(100, L_green, L_blue);

    if (!client.isConnected())
    {
      reconnect();
    }

    client.loop();  // Call the loop continuously to establish connection to the server.

    if (millis() - lastConnectionTime > postingInterval)
    {
        mqttpublish();
    }

}


void reconnect(){

     char clientID[9];

     Serial.println("Attempting MQTT connection");
        // Generate ClientID
        for (int i = 0; i < 8; i++) {
            clientID[i] = alphanum[random(51)];
        }
        clientID[8]='\0';

        // Connect to the ThingSpeak MQTT broker.
        if (client.connect(clientID /* ,mqttUserName,mqttPass */))  {
            Particle.publish("Conn:"+ String(server) + " cl: " + String(clientID)+ " Uname:" + String(mqttUserName));
        } else
        {
            Particle.publish("Failed to connect, Trying to reconnect in 5 seconds");
            delay(5000);
        }
}

void mqttpublish() {

    // read raw accel/gyro measurements from device
    accelgyro1.getMotion6(&ax1, &ay1, &az1, &gx1, &gy1, &gz1);
    accelgyro2.getMotion6(&ax2, &ay2, &az2, &gx2, &gy2, &gz2);

    // Create a data string to send data to ThingSpeak.

    //For multiple data string
    //String data = String("x1=" + String(ax1) + "&x2=" + String(ax2) + "&y1=" + String(ay1)
    // + "&y2=" + String(ay2) + "&z1=" + String(az1) + "&z2=" + String(az2));
    //String topic = String("channels/"+String(channelID)+ "/publish/"+String(writeAPIKey));

    //For only one data string
    String data = String((int)millis(), DEC);
    //String data = String(ax1);
    String topic = String("qwer"/*"channels/"+String(channelID)+ "/publish/fields/field1/"+String(writeAPIKey)*/);

    if(!client.publish(topic,data)==0)
    {
          toggleLed();
    }

    lastConnectionTime = millis();



}
