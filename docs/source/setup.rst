*****
Setup
*****


User interface
==============

Bulebule user interface uses as input:

- A single button (differentiates between long and short press)
- The IR sensors (detects when your finger passes near the sensor)

And as output:

- The speaker
- Blue Pill's integrated green LED
- On-board red LEDs

Right after it is turned on, Bulebule can be configured and instructed to
perform different tasks following the diagram bellow:

.. graphviz::

   digraph finite_state_machine {
       size="8,14"; ratio = fill;
       node [shape = doublecircle,style=filled,fillcolor=lightblue]; Idle "Go!";
       node [shape = diamond,style=filled,fillcolor=white]; "Saved\nmaze?";
       node [shape = ellipse,style=filled,fillcolor=lightgrey]; "Start?" "Reuse?" "Goal?" "Speed?" "Ready?";
       node [shape = box,style=filled,fillcolor=white];
       Idle -> "Start?";
       "Start?" -> "Start?" [ label = "No input" ];
       "Start?" -> "Saved\nmaze?" [ label = "Press" ];
       "Saved\nmaze?" -> "Green LED on" [ label = "Yes" ];
       "Saved\nmaze?" -> "Green LED off" [ label = "No" ];
       "Green LED on" -> "Reuse?";
       "Reuse?" -> "Reuse?" [ label = "No input" ];
       "Reuse?" -> "Clear saved maze" [ label = "Long press" ];
       "Reuse?" -> "Set running mode" [ label = "Short press" ];
       "Set running mode" -> "Speed?";
       "Clear saved maze" -> "Green LED off";
       "Green LED off" -> "Goal?";
       "Goal?" -> "Goal?" [ label = "No input" ];
       "Goal?" -> "Set training goal" [ label = "Short press" ];
       "Goal?" -> "Set competition goal" [ label = "Long press" ];
       "Set competition goal" -> "Play epic theme";
       "Set training goal" -> "Set searching mode";
       "Play epic theme" -> "Set searching mode";
       "Set searching mode" -> "Speed?";
       "Speed?" -> "Speed?" [ label = "No input" ];
       "Speed?" -> "Increase speed" [ label = "Short press" ];
       "Increase speed" -> "Speed?";
       "Speed?" -> "Delay (robot placement)" [ label = "Long press" ];
       "Delay (robot placement)" -> "Ready?";
       "Ready?" -> "Ready?" [ label = "No input" ];
       "Ready?" -> "Go!" [ label = "Front sensor close reading" ];
   }

.. note:: After ``Go!`` (run/explore), the robot will end up in the ``Idle``
   state back again.


Encoders calibration
====================

If you are using slightly different wheels, motors or encoders than the ones
suggested in the :ref:`building` section, you will probably need to calibrate
the ``MICROMETERS_PER_COUNT`` constant. You may need to calibrate it even if
you use the same suggested components, since little differences in the wheels'
diameter can affect its value.


IR sensors calibration
======================

It is important to properly calibrate the IR sensors. Not only your sensors
will behave differently than sensors in another Bulebule build, but also,
readings can vary a lot from maze to maze, depending on the characteristics of
the walls.

To calibrate the IR sensors, create a 2-cell closed maze like the following:

.. code:: text

   o---o
   | G |
   o   o
   | S |
   o---o

Place the robot at the starting cell (``S``), with its tail touching the back
wall and facing towards the goal cell (``G``).

Then, using the ``connect_bluetooth.py`` script, connect to the robot and send
the command:

.. code:: text

   run front_sensors_calibration

The robot should travel across the two cells and end up with its nose touching
the front wall at ``G``.

During that travel, the robot has sent information about the IR sensors
readings to the console, so make sure to save it from the console with:

.. code:: text

   log save

Then run the ``front_sensors_calibration.ipynb`` notebook to obtain the IR
configuration parameters, which you can replace in ``config.h``.
