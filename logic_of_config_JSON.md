Explanation of the example configuration data JSON
========

This JSON will store all informations necessary to define the behaviour of the system.

We're not 100% sure yet how to provide it to the logic unit (initially we'll just hardcode a similar example),
but it will make the system totally adaptable to different difficulties as well as different settings (more lights, buttons, etc).

JSON structure
---

The JSON basically contains 3 arrays:
 - buttons
 - lights
 - code

The code array is just the numerical representation of the code that has to be entered.

The lights and buttons each have unique IDs which will also serve as mqtt subtopics.

The lights also have arrays of hex color values.

The buttons have an initial state (which is only for integration into a real escape room and will be active for our demonstrator),
as well as an array of actions that tells which lights will be changed by how much.

How the logic unit would handle this information
---

- The first color in the array of each light will be the initial color that this light shows when the system starts up.
- This corresponds to the numerical representation [0,0,0]
- When the button with id BUTTON_0_ID is press, the logic unit will receive a message on the according subtopic
- it will then look up the actions to take, in this case changing lights with id's LIGHT_1_ID and LIGHT_2_ID by 1 and 3
- according to this rule, it will
  - update its internally saved state to [0,1,3]
  - publish the according colors on the according subtopics
  - and check if the correct code has been entered

Options for adaptability
---

first of all, it is really easy and quick to change the different colors, code and button press behaviour of a given system.

But the idea behind the IDs is that each button and light that is installed in the escape room has it's own ID.
If you want to ad a button, maybe even without adding a light, you just add an entry in the buttons array and specify the rules for this button. Same goes for adding lights.
