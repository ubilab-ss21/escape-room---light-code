#include "WiFi.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
 
#include "local_wifi.h"
#include "mqtt_config.h"

WiFiClient espClient;
PubSubClient client(espClient);

// in case we want to enable software reset (from online search):
//  void(* resetFunc) (void) = 0; //declare reset function @ address 0

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

void initialize_wifi_mqtt() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqtt_server, 1883);
}

void setup() {
  Serial.begin(115200);
 
  Serial.println("deserializing config JSON ...");
  StaticJsonDocument<1024> doc;	//TODO make this somehow flexible for different json sizes
  char json[] =
    "{\"buttons\":[{\"button_id\":\"BUTTON_0_ID\",\"inital_state\":\"active\",\"actions\":[{\"change_light_id\":\"LIGHT_1_ID\",\"change\":1},{\"change_light_id\":\"LIGHT_2_ID\",\"change\":3}]},{\"button_id\":\"BUTTON_1_ID\",\"inital_state\":\"active\",\"actions\":[{\"change_light_id\":\"LIGHT_2_ID\",\"change\":1}]},{\"button_id\":\"BUTTON_2_ID\",\"inital_state\":\"active\",\"actions\":[{\"change_light_id\":\"LIGHT_0_ID\",\"change\":4},{\"change_light_id\":\"LIGHT_1_ID\",\"change\":2},{\"change_light_id\":\"LIGHT_2_ID\",\"change\":1}]}],\"lights\":[{\"light_id\":\"LIGHT_0_ID\",\"colors\":[\"ff0000\",\"ff00ff\",\"0000ff\",\"00ffff\",\"00ff00\",\"ffff00\",\"ffffff\"]},{\"light_id\":\"LIGHT_1_ID\",\"colors\":[\"ff0000\",\"00ffff\",\"ffff00\",\"ffffff\"]},{\"light_id\":\"LIGHT_2_ID\",\"colors\":[\"0000ff\",\"00ff00\",\"ff0000\"]}],\"code\":[4,2,0]}";
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    /* maybe use this to reset the controller in this case: ... or even better: TODO loop dezerialization until successful
      Serial.println("reseting in 5 seconds ...");
      delay(1000);
      for (int sec = 4 ; sec > 0 ; sec++ ) {
	Serial.print(sec);
	delay(500);
	Serial.println(" ...");
	delay(500);
      }
      Serial.println("calling resetFunc() ...");
      resetFunc();  //call reset
    */
  }

  Serial.println("config read ... initializing logic ...");

  byte num_buttons = doc["buttons"].size();
  bool buttons_active[num_buttons];
  /*  NOTE: this is still work in progress
  i want an array that tells me:
    if button b is pressed,
    take buttons_numActions[b] actions,
    that is, for each action a in buttons_actions[b]:
      advance light no buttons_actions[b][a][0] by buttons_actions[b][a][1] steps
  it's gonna be tough :( ...
  byte buttons_numActions[num_buttons];
  byte* buttons_actions[num_buttons];
  for (byte b = 0 ; b < num_buttons ; b++ ) {
    buttons_numActions[b] = doc["buttons"][b]["actions"].size();
    byte buttons_actions[b]
    buttons_actions[b] = (byte *) malloc(2*buttons_numActions[b]);
  }
  */

  byte num_lights = doc["lights"].size();
  byte lights_state[num_lights];
  byte lights_maxvalue[num_lights];
  String topic;

  Serial.print("number of lights: ");
  Serial.println(num_lights);

  initialize_wifi_mqtt();
  for (byte l = 0 ; l < num_lights ; l++) {
    lights_state[l] = 0;
    lights_maxvalue[l] = doc["lights"][l]["colors"].size();
    topic = (String) mqtt_main_topic + (const char*) doc["lights"][l]["light_id"];
    if (!client.connected()) {
      reconnect();
    }
    client.publish(topic.c_str(),(const char*) doc["lights"][l]["colors"][0],true);
  }
}
 
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
}
