import paho.mqtt.client as mqtt
import json
import time
import os

clearConsole = lambda: os.system('cls' if os.name in ('nt', 'dos') else 'clear')
clearConsole()
loop = True

mqttBroker = "earth.informatik.uni-freiburg.de"
main_topic = "ubilab/colorcode/"
lights_cli = mqtt.Client("lights_publisher")
lights_cli.connect(mqttBroker)

with open('example_configuration_data1.json', 'r') as json_file:
    data = json_file.read() 
config = json.loads(data)     

for digit in config['code'][::-1]: 
    lights_cli.publish(main_topic + 'code', digit, retain=False)
lights_cli.publish(main_topic + 'code', 'D', retain=False)

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
        clearConsole()
        print("Target \t\t{}".format(config['code']))
        print("Current State \t{}".format(lights_state))
        if config['code'] == lights_state:
            print("CONGRATS, You are out of here!!!")
            for light in config['lights']:                                                                                                                                                            lights_cli.publish(main_topic + light['light_id'], "C",retain=False) 
            lights_cli.publish(main_topic + 'code', "C" , retain=False)
            loop = False

    client = mqtt.Client("buttons_listener_" + str(index))
    client.on_message = on_message
    client.connect(mqttBroker)
    client.subscribe(main_topic + config['buttons'][index]['button_id'])
    client.loop_start()
    return client


print("ARE YOU READY TO BE TRAPPED ???")
time.sleep(1)

print("Initializing Buttons....")
time.sleep(1)

num_buttons    = len(config['buttons'])
buttons_active = [button_init_state(index)  for index in range(num_buttons)]
buttons_cli    = [init_button_client(index) for index in range(num_buttons)]

print("Buttons Initialized....")
time.sleep(1)

print("Initializing Lights....")
num_lights     = len(config['lights'])
lights_state   = [0] * num_lights
for light in config['lights']:
    lights_cli.publish(main_topic + light['light_id'], light['colors'][0],retain=False)

time.sleep(1)
print("Lights Initialized ...")
time.sleep(1)
print("\nHERE WE GO")
time.sleep(1)
clearConsole()

print("Target \t\t{}".format(config['code']))     
print("Current State \t{}".format(lights_state))   

while loop:
    loop = input().lower()[0] != 'y'

for client in buttons_cli:
    client.loop_stop()
