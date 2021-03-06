Light Color Code Challenge
==========================

Goal of this project was to create a working prototype for an escape room challenge, consisting of several lights and buttons distributed through several rooms.
The objective of the challenge is to find out how to manipulate the number's appearing on the different lights and then make every light shine with a fixed number of LED's (correspoding to a sequence), which results in the next challenge in the escape room (e.g. open a door or hatch or exposing some information needed for a different challenge).
This code sequence will either be displayed:
- on a display
- deciphered via animated light sequence


Setup and components
--------------------

The demonstration will consist of 4 ESP-32 boards that will be controlling one color LED light ring (Grove LED light rings in our case) and one push button each, and one additionally ESP will be displaying the sequence to be obtained.
Communication of the components takes place via MQTT connected through the central broker at the University.
The logic unit will listen to messages signaling a button press and send messages to change the color of the lights according to the rules provided during initialization.
A central concept was to make the system flexible so it can be easily initialized with different rules for how the lights change, making it easy to adjust the difficulty of the challenge.
A detailed guide on how to setup the Config file for different configurations is present in  the [How_To_Configure_JSON.md](https://github.com/ubilab-ss21/escape-room---light-code/blob/main/config_files/How_To_Configure_JSON.md) file.

Right now, the digits in the sequence are in the range [0-6].

There are three setups:
- **puzzle_logic.py**       : Uses preset config files to solve a preset code found in the config file. Works only with the display.
- **puzzle_code_with_light.py**       : Uses preset config files to define a preset code found in the config file. Works with the display and the animated light sequence.
- **puzzle_randomized_code_with_light.py**       : Uses preset config files to determine a random code generated using a petri net. Works only with  the display and the animated light sequence.

Pythin Libraries Required:
- [ortools](https://developers.google.com/optimization/install)
- [paho-mqtt](https://www.eclipse.org/paho/index.php?page=clients/python/index.php)

Arduino Libraries Required:
- [Adafruit-Neopixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [pubsub-client](https://www.arduino.cc/reference/en/libraries/pubsubclient/) 


Instructions
--------------------

1. Decide which setup file to use.
2. Based on your setup, load the ESP Node's with the required code from [Arduino Node Files](https://github.com/ubilab-ss21/escape-room---light-code/tree/main/Arduino_Node_Files).
3. Select a configuration and modify it based on your requirements according to the [How_To_Configure_JSON.md](https://github.com/ubilab-ss21/escape-room---light-code/blob/main/config_files/How_To_Configure_JSON.md) file.
4. Run the code with the following command: 
 ```
      python puzzle_setup_file.py -c config_file.json
 ```
5. Have fun!!!


What we could have done differently
--------------------
- Made use of an OLED display to display the sequence.
- Possibly integrated it with other puzzles.
- Mdae use of small e-ink displays to show node number at the ESP's (increase randomness).
