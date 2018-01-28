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

The microcontroller reference is STM32F103CBT6 LQFP64,
that means:

STM32
  ST microcontroller with 32-bit

F1
  ARM Cortex M3-based, performance line

03
  72MHz, with motor control, USB and CAN

CB
  Medium-density up to 1 Mbyte

LQFP64
  Pinout


.. index:: workspace

Workspace
=========

We will be using the `libopencm3`_ firmware library. Our workspace is based on
the `reuse instructions
<https://github.com/libopencm3/libopencm3-examples#reuse>`_ from that project.

#. Clone the project including submodules to get libopencm3:

   .. code-block:: bash

      git clone --recursive git@github.com:Theseus/bulebule.git

#. Setup libopencm3:

   .. code-block:: bash

      cd bulebule/
      ./scripts/setup_libopencm3.sh

#. Compile Bulebule

   .. code-block:: bash

      cd src/
      make

.. note:: The makefile on the :code:`src` folder combines the
   :code:`libopencm3.target.mk` file, the board STM32F1 makefile and the
   Bulebule project relative paths.

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

      sudo usermod -a -G dialout USER_NAME

   We may need to log out and back in for this change to take effect.

   Alternatively we could add some UDEV rules in
   `/etc/udev/rules.d/99-stlink.rules`::

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

Then we can, for example, display all registers and their values::

   > reg

Or halt and re-run::

   > reset halt
   > reset run


.. index:: compiling

Compiling
=========

We will be using GCC's ARM compiler:

.. code-block:: bash

   dnf install arm-none-eabi-gcc

We need also to install a C standard library implementation:

.. code-block:: bash

   dnf install arm-none-eabi-newlib

.. index:: flashing

Flashing
========

We can flash the microcontroller using OpenOCD (assuming we are already
connected with `telnet`)::

   > program filename.elf verify reset

.. note:: The `program` command will automatically execute the `reset init`
   scripts, flash the image, verify it (if `verify` parameter is given) and
   run it if `reset` parameter is given.

Alternatively, we can launch OpenOCD and flash the program with a single
command:

.. code-block:: bash

   openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg \
       -c "program filename.elf verify reset exit"

Binary files need the flash address to be specified::

   > program filename.bin verify reset 0x08000000


.. index:: debugging

Debugging
=========

We will be using GCC's ARM debugger:

.. code-block:: bash

   dnf install arm-none-eabi-gdb

We use `GDB`_ for debugging. Note that we connect to OpenOCD gdbserver using
the port `3333`, rather than the port `4444` used with `telnet`::

   $ arm-none-eabi-gdb main.elf
   (gdb) target extended-remote localhost:3333

.. note:: Remember that you can shorten the commands: ``tar ext :3333`` is
   the same as ``target extended-remote localhost:3333``.

Once we are connected, we can execute OpenOCD commands simply prepending the
word `monitor`::

   (gdb) monitor reset halt
   (gdb) monitor reset run

If we want to load the `.elf` file, we can simply execute the `load` command
now::

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
