#include "WiFi.h"
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h> 

const char* ssid = "SSID";
const char* password = "PASS";

#define mqtt_server "earth.informatik.uni-freiburg.de"
#define PIN 23  // data pin for ring
#define NUMPIXELS 16   // number of lights on ring

//for enabling number of lights and input signal pin
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// variable for reading in push button state
int buttonState=0;

// definition for psuedo delay
unsigned long previousMillis = 0, previousMillis1 = 0;
int interval = 200;

WiFiClient light_node_saba;
PubSubClient client(light_node_saba);

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
    if (client.connect("light_node_saba")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

//light values
int light[7][3] =  {{255,255,255},{255,255,0},{255,0,255},{255,0,0},{0,255,255},{0,255,0},{0,0,255}};
char code[5]={};
int j=0;
int start_display=0;
void callback(char* topic, byte* payload, unsigned int length){
  char l =(char)payload[0];
  Serial.println(l);
  if(l=='D')
  {
      start_display=1;
      code[j]='\0';
      Serial.println(code[j]);
      j=0;
  }
  if(l!='D' && l!='C')
  {
      code[j]=l;
      Serial.println(code[j]);
      j++;
  }
  if(l=='C')
  {
      client.publish("ubilab/colorcode/code","",true);
      start_display=0;
      l='\0';
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

void setup() { 
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  client.publish("ubilab/colorcode/code","",true);
  pinMode (19 , INPUT);
  pinMode (23 , INPUT);
  pinMode (18 , INPUT);
  Serial.println("Connected to the WiFi network");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pixels.setBrightness(150);
  pixels.begin(); //initializing the light ring
}


int light_number=0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  
    client.subscribe("ubilab/colorcode/code");
  }
  
  if(start_display==1)
  {
    for (int j=0; j<10; j++) {  
      for (int q=0; q < 3; q++) {
        for (int i=0; i < pixels.numPixels(); i=i+3) {
          pixels.setPixelColor(i+q, pixels.Color(255,255,255)); 
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
    int p=0;
    Serial.println(code);
    while(code[p]!='\0')
    {
      light_number = (int)code[p] - 48;
      int arr[20]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
      int i=0;
      int color_value = rand () % 7;
      while(i<light_number && light_number!=0)
      {
        int a = rand() % 16;
        if(arr[a]!=0)
        {
          arr[a]=0;
          pixels.setPixelColor(a, pixels.Color(light[color_value][0],light[color_value][1],light[color_value][2])); // Moderately bright green color.
          pixels.show();
          delay(1000); // Delay for a period of time (in milliseconds).
          i++;
        }
      }
      if(light_number==0)
      {
        colorWipe(pixels.Color(light[color_value][0],light[color_value][1],light[color_value][2]), 50);
      }
      delay(1000);
      colorWipe(pixels.Color(0, 0, 0), 50);
      p++;
      
    }
  }
}
