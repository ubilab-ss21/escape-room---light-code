from ortools.linear_solver import pywraplp
import paho.mqtt.client as mqtt
import json
import time
import sys
import os
import random

mqttBroker = "earth.informatik.uni-freiburg.de"
main_topic = "ubilab/colorcode/"

clearConsole = lambda: os.system('cls' if os.name in ('nt', 'dos') else 'clear')

def print_usage_and_exit():
    print('usage:\t\t', sys.argv[0], '--config\tSEED_CONFIG_FILE')
    print('   or \t\t', sys.argv[0], ' -c\tSEED_CONFIG_FILE')
    exit(1)

## CREATING EASY PUZZLE:
# each button controls a different number of lights, one controls one, the next two, then 3 and so on ...
def create_random_buttons_configuration():
    button_indices  = random.sample(range(nodecount), nodecount)
    lights_affected = random.sample(range(nodecount), nodecount)
    for button_index in range(nodecount):
        config['buttons'][button_indices[button_index]]['actions'] = \
                [ { "change_light_with_index" : lights_affected[light_index] ,
                    "by_this_many_steps"      : random.randrange(1,7)
                  } for light_index in range(button_index+1) ]
        config['code'][button_index] = random.randrange(1,7)

def check_if_config_is_infeasible():
    solver.Clear()
    effects_on_lights = [[] for _ in range(num_lights)]
    for button_num in range(num_buttons):
        for action in config['buttons'][button_num]['actions']:
            effects_on_lights[action['change_light_with_index']].append((button_num, action['by_this_many_steps']))

    button_presses = [ solver.IntVar(0, infinity, 'button_presses[%i]' % button_num)
                       for button_num in range(num_buttons) ]
    role_overs     = [ solver.IntVar(0, infinity, 'role_overs[%i]' % light_num)
                       for light_num  in range(num_lights)  ]

    for light_num in range(num_lights):
        constraint_expr = [ button_effect * button_presses[button_num]
                            for (button_num, button_effect) in effects_on_lights[light_num]  ]
        constraint_expr.append(  - len(config['lights'][light_num]['colors']) * role_overs[light_num]   )
        solver.Add(  sum(constraint_expr)  ==  config['code'][light_num]   )

    solver.Minimize(solver.Sum( button_presses ))
    status = solver.Solve()

    if status == pywraplp.Solver.OPTIMAL:
        return False
    return True


### MAIN ###
if len(sys.argv) < 3 or \
    sys.argv[1] != '-c' and sys.argv[1] != '--config':
    print_usage_and_exit()

if not os.path.isfile(sys.argv[2]):
    print(sys.argv[2], ' is not a file!\nprovide existing SEED_CONFIG_FILE')
    print_usage_and_exit()

with open(sys.argv[2], 'r') as json_file:
    data = json_file.read()
config = json.loads(data)

num_lights  = len(config['lights'])
num_buttons = len(config['buttons'])

if num_lights != num_buttons:
    print('different number of lights and buttons currently not supported!')
    exit()

clearConsole()

nodecount  = num_lights
solver = pywraplp.Solver.CreateSolver('SCIP')
infinity = solver.infinity()

infeasible = True
while infeasible:
    print('creating and checking random configuration ...')
    create_random_buttons_configuration()
    infeasible = check_if_config_is_infeasible()

print('feasible config found with this solution:')
print([int(button.solution_value()) for button in solver.variables()[:num_buttons]])

lights_cli = mqtt.Client("lights_publisher")
lights_cli.connect(mqttBroker)

for digit in config['code']: 
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
            for light in config['lights']:
                lights_cli.publish(main_topic + light['light_id'], "C",retain=False) 
            lights_cli.publish(main_topic + 'code', "C" , retain=True)
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

buttons_active = [button_init_state(index)  for index in range(num_buttons)]
buttons_cli    = [init_button_client(index) for index in range(num_buttons)]

print("Buttons Initialized....")
time.sleep(1)

print("Initializing Lights....")
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

loop = True
while loop:
    loop = input().lower()[0] != 'y'

for client in buttons_cli:
    client.loop_stop()
