import paho.mqtt.client as mqtt
import json
import time

mqttBroker = "earth.informatik.uni-freiburg.de"
main_topic = "ubilab/colorcode/"
lights_cli = mqtt.Client("lights_publisher")
lights_cli.connect(mqttBroker)

def button_init_state(index):
    return config['buttons'][index]['initial_state'] == "active"
def init_button_client(index):
    def on_message(client, userdata, message):
        for action in config['buttons'][index]['actions']:
            light_index = action['change_light_with_index']
            light = config['lights'][light_index]
            next_state = ( lights_state[light_index] + action['by_this_many_steps'] ) % len(light['colors'])
            lights_state[light_index] = next_state
            lights_cli.publish(main_topic + light['light_id'], light['colors'][next_state],retain=False)
        print("\n\t current state is {}".format(lights_state))
        if config['code'] == lights_state:
            print("\n\t\tCONGRATS, riddle solved!!!")
            loop = False

    client = mqtt.Client("buttons_listener_" + str(index))
    client.on_message = on_message
    client.connect(mqttBroker)
    client.subscribe(main_topic + config['buttons'][index]['button_id'])
    client.loop_start()
    return client

with open('example_configuration_data.json', 'r') as json_file:
    data = json_file.read()

config = json.loads(data)

print("initializing buttons ...")
num_buttons    = len(config['buttons'])
buttons_active = [button_init_state(index)  for index in range(num_buttons)]
buttons_cli    = [init_button_client(index) for index in range(num_buttons)]
print("buttons initialized ...")

print("initializing lights ...")
num_lights     = len(config['lights'])
lights_state   = [0] * num_lights
for light in config['lights']:
    lights_cli.publish(main_topic + light['light_id'], light['colors'][0],retain=False)
print("lights initialized ...")

loop = True
while loop:
    loop = input("end program? [y/n]").lower()[0] != 'y'

for client in buttons_cli:
    client.loop_stop()




# i'll probably not need this any longer, but won't hurt to have it here for now ...
'''
  /*
  i want an array that tells me:
    if button b is pressed,
    take buttons_numActions[b] actions,
    that is, for each action a in buttons_actions[b]:
      advance light no buttons_actions[b][a][0] by buttons_actions[b][a][1] steps
      { { { light changed , how much it changes }==action 1 of button 1 ,
          { action 2 of button 1 }
	} all actions of button 1 ,
	{ all actions of button 2 }
	...
      }

  it's gonna be tough :( ...
  byte buttons_numActions[num_buttons];
  byte* buttons_actions[num_buttons];
  for (byte b = 0 ; b < num_buttons ; b++ ) {
    buttons_numActions[b] = doc["buttons"][b]["actions"].size();
    byte buttons_actions[b]
    buttons_actions[b] = (byte *) malloc(2*buttons_numActions[b]);
  }
  */
'''
