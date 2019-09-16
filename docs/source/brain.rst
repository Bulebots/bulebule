*****
Brain
*****


Microcontroller
===============

The microcontroller reference is STM32F103C8T6, that means:

STM32
  ARM-based 32-bit ST microcontroller

F
  General purpose

103
  Performance line

C
  Pin count, 48 pins

8
  Medium-density, 64 Kbytes of Flash memory

T
  Package type, LQFP

6
  Temperature range, -40 to 85 ºC


Compiling
=========

This project requires some tools and dependencies:

- The GCC ARM compiler and debugger.
- A C standard library implementation.
- `OpenOCD`_ for programing and debugging.

We will also be using the `libopencm3`_ firmware library. Our workspace is
based on the `simple template
<https://github.com/libopencm3/libopencm3-template>`_ from that project.

We can manually install all those tools and dependencies in our system or we
can use containers, which is a much more convenient approach. Just make sure
you have `Podman`_ or `Docker`_ installed in your system.

#. Clone the project including submodules to get `libopencm3`_:

   .. code-block:: bash

      git clone --recursive git@github.com:Bulebots/bulebule.git
      cd bulebule

#. Create the container image:

   .. code-block:: bash

      make image

#. Setup and compile `libopencm3`_:

   .. code-block:: bash

      make libopencm3

#. Compile the source code with:

   .. code-block:: bash

      make

#. We can clean the compilation files with:

   .. code-block:: bash

      make clean


.. note:: The makefile on the :code:`src` folder combines the
   :code:`libopencm3.target.mk` file, the board STM32F1 makefile and the
   Bulebule project relative paths.


.. index:: programmer

Programmer
==========

The programmer is a generic ST-Link V2. Connecting this programmer to the
board is very simple, just make sure to connect these pins together:

===================  ===================
Board                Programmer
===================  ===================
3V3                  **<unconnected>**
**<unconnected>**    3.3V
SWIO                 SWDIO
SWCLK                SWCLK
GND                  GND
===================  ===================

.. warning:: Note that the 3V3 line is not connected. We will be using the
   battery to power the mouse while flashing it. This way we avoid having two
   power sources on the microcontroller board while programming.

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


Flashing
========

Having the programmer connected, we can flash the microcontroller with::

   make -C src/ flash


OpenOCD
=======

To program the microcontroller we can also directly use `OpenOCD`_. We need to
specify the interface and target configuration files:

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

We can flash the microcontroller using OpenOCD as well (assuming we are already
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


Debugging
=========

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
.. _`Podman`:
  https://podman.io/
.. _`Docker`:
  https://www.docker.com/
