.. index:: brain

*****
Brain
*****


.. index:: board

Board
=====


.. index:: microcontroller

Microcontroller
===============


.. index:: workspace

Workspace
=========

We will be using `libopencm3`_ firmware library. Our workspace is based on 
the `reuse instructions
<https://github.com/libopencm3/libopencm3-examples#reuse>`_ from that project.

1. Clone the project including submodules to get libopencm3

  .. code-block:: none
  
     git clone --recursive git@github.com:Theseus/theseus.git

2. Grab a copy of the libopencm3 basic rules on .opencm3 folder

  .. code-block:: none
  
     cd .opencm3
  
     wget \ 
     
     https://raw.githubusercontent.com/libopencm3/libopencm3-examples/master/examples/rules.mk \
     
     -O libopencm3.rules.mk

3. Compile the libopencm3 library (See Compiling Section)

  .. code-block:: none
  
     make

4. Compile Theseus
   
  .. code-block:: none
  
     cd ../../src
     
     make

.. note:: The makefile on the src folder combines the libopencm3.target.mk file, 
	  the board STM32F1 makefile and the Theseus project relative paths.

.. index:: programmer

Programmer
==========

The programmer is a generic ST-Link V2. Connecting this programmer to the
board is very simple, just make sure to connect these pins together:

===========  ===========
Board        Programmer
===========  ===========
3V3          3.3V
SWIO         SWDIO
SWCLK        SWCLK
GND          GND
===========  ===========

.. note:: The programmer uses an USB interface, which means we might need to
   set the proper permissions for our user:

   .. code-block:: bash

      usermod -a -G dialout USER_NAME

   We may need to log out and back in for this change to take effect.

   Alternatively we could add some UDEV rules in
   `/etc/udev/rules.d/99-stlink.rules`:

   .. code-block:: none

      ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666"

   And then simply reload the rules without logging out:

   .. code-block:: bash

      udevadm control --reload-rules

To program the microcontroller we are using `OpenOCD`_. We need to specify
the interface and target configuration files:

.. code-block:: bash

   openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg

OpenOCD starts running as a server and waits for connections from clients
(Telnet, GDB, RPC) and processes the commands issued through those channels.

We can try and connect to the OpenOCD server with Telnet:

.. code-block:: bash

   telnet localhost 4444

Then we can, for example, display all registers and their values:

.. code-block:: none

   > reg

Or halt and re-run:

.. code-block:: none

   > reset halt
   > reset run


.. index:: compiling

Compiling
=========

We will be using GCC's ARM compiler:

.. code-block:: bash

   dnf install arm-none-eabi-gcc


.. index:: flashing

Flashing
========

We can flash the microcontroller using OpenOCD (assuming we are already
connected with `telnet`):

.. code-block:: none

   > program filename.elf verify reset

.. note:: The `program` command will automatically execute the `reset init`
   scripts, flash the image, verify it (if `verify` parameter is given) and
   run it if `reset` parameter is given.

Alternatively, we can launch OpenOCD and flash the program with a single
command:

.. code-block:: bash

   openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg \
       -c "program filename.elf verify reset exit"

Binary files need the flash address to be specified:

.. code-block:: none

   > program filename.bin verify reset 0x08000000


.. index:: debugging

Debugging
=========

We will be using GCC's ARM debugger:

.. code-block:: bash

   dnf install arm-none-eabi-gdb

We use `GDB`_ for debugging. Note that we connect to OpenOCD gdbserver using
the port `3333`, rather than the port `4444` used with `telnet`.

.. code-block:: none

   $ arm-none-eabi-gdb main.elf
   (gdb) target extended-remote localhost:3333

.. note:: Remember that you can shorten the commands: ``tar ext :3333`` is
   the same as ``target extended-remote localhost:3333``.

Once we are connected, we can execute OpenOCD commands simply prepending the
word `monitor`:

.. code-block:: none

   (gdb) monitor reset halt
   (gdb) monitor reset run

If we want to load the `.elf` file, we can simply execute the `load` command
now:

.. code-block:: none

   (gdb) load


References
==========

.. target-notes::

.. _`OpenOCD`:
  http://openocd.org/
.. _`GDB`:
  https://www.gnu.org/software/gdb/
.. _`libopencm3`:
  https://github.com/libopencm3/libopencm3

