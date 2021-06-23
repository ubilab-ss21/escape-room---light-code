#include "WiFi.h"
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


const char* ssid = "ZYXEL-036";
const char* password = "8991401155431659";

#define mqtt_server "earth.informatik.uni-freiburg.de"
#define PIN        23  //for ring
#define NUMPIXELS 16   //numbe of lights on ring

//for enabling light count and input signal pin
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int buttonState=0;
unsigned long previousMillis = 0, previousMillis1 = 0;
int interval = 300;

WiFiClient node3_saba;
PubSubClient client(node3_saba);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("node3_saba")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

int light[7][3] =  {{255,255,255},{255,255,0},{255,0,255},{255,0,0},{0,255,255},{0,255,0},{0,0,255}};
int l=0;
void callback(char* topic, byte* payload, unsigned int length){
  char k=(char)payload[0];
  Serial.println(k);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis1 >= interval) {
    previousMillis1 = currentMillis;
      if(k=='b'){
        for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
        pixels.setPixelColor(i, pixels.Color(light[l][0],light[l][1],light[l][2]));
        pixels.show();   // Send the updated pixel colors to the hardware.        
        delay(100);
      }
      l=(l+1)%7;
    }
  }
}

void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  pinMode (19 , INPUT);
  pinMode (23 , INPUT);
  pinMode (18 , INPUT);
  Serial.println("Connected to the WiFi network");
  client.setServer(mqtt_server, 1883);
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
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
      client.publish("ubilab/lightcode/button","a",false);
    }
    /*else{
      client.publish("ubilab/lightcode/button","none1",false);
    }*/
    client.subscribe("ubilab/lightcode/light");
  }
  
}
