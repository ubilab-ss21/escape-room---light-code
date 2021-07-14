#include "WiFi.h"
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h> 

const char* ssid = "WIFI_SSID";
const char* password = "pass";

#define mqtt_server "earth.informatik.uni-freiburg.de"
#define PIN 23  // data pin for ring
#define NUMPIXELS 16   // number of lights on ring

//for enabling number of lights and input signal pin
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// variable for reading in push button state
int buttonState=0;

// definition for psuedo delay
unsigned long previousMillis = 0, previousMillis1 = 0;
int interval = 350;

WiFiClient node4_saba;
PubSubClient client(node4_saba);

// for setting all lights to off
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("node4_saba")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

//light values
int light[7][3] =  {{255,255,255},{255,255,0},{255,0,255},{255,0,0},{0,255,255},{0,255,0},{0,0,255}};
int l=0;

void callback(char* topic, byte* payload, unsigned int length){
  int l =(char)payload[0] - 48;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis1 >= interval) {
    previousMillis1 = currentMillis;
    colorWipe(pixels.Color(0, 0, 0), 50); 
    pixels.show();
    int arr[20]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    int i=0;
    while(i<l && l!=19)
    {
        int a = rand() % 16;
        if(arr[a]!=0)
        {
            arr[a]=0;
            pixels.setPixelColor(a, pixels.Color(light[l][0],light[l][1],light[l][2])); // Moderately bright green color.
            pixels.show();
            delay(100); // Delay for a period of time (in milliseconds).
            i++;
        }
    }
        if(l==19)
        {
          //animation
          for (int j=0; j<100; j++) {  
            for (int q=0; q < 3; q++) {
              for (int i=0; i < pixels.numPixels(); i=i+3) {
                  pixels.setPixelColor(i+q, pixels.Color(0,0,127)); 
              }
              pixels.show();
              delay(50);
              for (int i=0; i < pixels.numPixels(); i=i+3)
              {
                pixels.setPixelColor(i+q, 0); 
              }
            }
         }
         pixels.show();
       }
  }
}

void setup() { 
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  pinMode (19 , INPUT);
  pinMode (23 , INPUT);
  pinMode (18 , INPUT);
  Serial.println("Connected to the WiFi network");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pixels.setBrightness(150);
  pixels.begin(); //initializing the light ring
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  
    buttonState = digitalRead(19);
    if ( buttonState == HIGH){
      client.publish("ubilab/colorcode/BUTTON_4_ID","a",false);
    }
    client.subscribe("ubilab/colorcode/LIGHT_04_ID");
  }
}
