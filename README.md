Light color code challenge
==========================

Goal of this project is to create a working prototype for an escape room challenge, consisting of several lights and buttons distributed through several rooms.
The objective of the challenge is to find out how to manipulate the color of the different lights and then make every light shine in  a certain color,
which is the code for revealing the next challenge in the escape room (e.g. open a door or hatch or exposing some information needed for a different challenge).
This color code will have to be obtained from the environment.

Setup and components
--------------------

The demonstration is going to consist of 5 ESP8266 boards that will be controlling one color LED light ring and one push button each, as well as one raspberry pi that will serve as the central logic unit.
Communication of the components will be achieved via mqtt with the raspberry running mosquitto.
The logic unit will listen to messages signaling a button press and send messages to change the color of the lights according to the rules provided during initialization.
A central concept is to make the system flexible so it can be easily initialized with different rules for how the lights change, making it easy to adjust the difficulty of the challenge.

Possible extensions
-------------------

There are several extensions that could be tackled if progress should be quick:
 - integration into Area 069
 - integration of off-the-shelf smart lighting systems
 - enhanced interface of master system for easy integration into an escape room (e.g. button activation by other riddle)
 - more advanced rules like behaviour dependent on simultanious button presses
 - ... _the sky is the limit :)_

Time plan for realization
-------------------------

calendar week | task
--------------|------
20	| creating rule initialization concept
21	| setup mosquitto
22	| build and code LED and button modules
23	| code logic unit
24	| code initialization interface	
25	| integrate init interface and logic
26	| test and assert first prototype
27	| extend interface for smart lighting
28	| integrate system into Area 069
29	| integrate system into Area 069

Evaluation plan
---------------

The project will be regarded a ...
 - ___minor success___     		>>>  if we manage to implement a hardcoded light color challenge prototype
 - ___moderate success___  		>>>  if we manage to implement an easily initializable prototpye
 - ___mayor success___     		>>>  if we manage to implement one of the possible extensions
 - ___magnificent success___	>>>  if we manage to implement at least two extensions
