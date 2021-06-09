#include "WiFi.h"
#include <PubSubClient.h>
 
const char* ssid = "ZYXEL-036";
const char* password = "8991401155431659";

#define mqtt_server "earth.informatik.uni-freiburg.de"

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Test")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
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
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  if (digitalRead(19) == HIGH){
      client.publish("a/b","a",true);
      delay(500);
  }
  if (digitalRead(23) == HIGH){
      client.publish("a/b","b",true);
      delay(500);
  }
  
  if (digitalRead(18) == HIGH){
      client.publish("a/b","c",true);
      delay(500);
  }

  delay(1000);  
}
