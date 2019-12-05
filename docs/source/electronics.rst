***********
Electronics
***********


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


.. index:: clocks

Clocks
======

The microcontroller is configured to use the External High Speed clock (HSE),
at 8 MHz, and set the SYSCLK at 72 MHz. The AHB is at 72 MHz as well (the AHB
divider is set to 1 by default). The AHB clock is used to generate the SysTick
interruptions. For more information have a look at the `STM32F10xxx Reference
Manual`_.


.. index:: bluetooth

Bluetooth
=========

The robot can communicate with the world using an `HC-05 serial bluetooth`_
module. This module is configured with a baud rate of 921600, 8 bits data, 1
stop bit, no parity and no flow control.


.. index:: encoders

Encoders
========

Timers 1 and 4 are configured to read quadrature encoders (and thus, connected
to the `IE2-512 encoder`_ channels A and B). They are both configured to count
on both TI1 and TI2 edges.


.. index:: gyroscope

Gyroscope
=========

Gyros are selected as the clock source for accuracy (versus the internal
relaxation oscillator suitable for lower power consumption when only
accelerometer is being used).

The MPU-6500 gyro features the following SPI operational features:

- Data is delivered MSB first and LSB last.
- Data is latched on the rising edge of SCLK.
- Data should be transitioned on the falling edge of SCLK.
- The maximum frequency of SCLK is 1 MHz. Although the sensor and interrupt
  registers may be read using SPI at 20 MHz.
- Read/write operations are completed in 16+ cycles (2+ bytes).
- First byte contains the SPI address, and the following the SPI data.
- First bit of the first byte contains the Read (1) / Write (0) bit.
- The following 7 bits contain the Register Address.
- In cases of multiple-byte read/writes, data is two or more bytes.

.. index:: pinout

Pinout
======

Bluepill
--------

====  =========  ======  ==============================
Pin   Function   Type    Connected to
====  =========  ======  ==============================
PA0   ADC12_IN0  Input   Battery voltage
PA1   N/A        N/A     *Not connected*
PA2   ADC12_IN2  Input   Receiver front right
PA3   ADC12_IN3  Input   Receiver side right
PA4   ADC12_IN4  Input   Receiver side left
PA5   ADC12_IN5  Input   Receiver front left
PA6   TIM3_CH1   Input   Motor driver IN1 (left)
PA7   TIM3_CH2   Input   Motor driver IN2 (left)
PA8   GPIO       Output  Emitter left front
PA9   GPIO       Output  Emitter left side
PA10  TIM1_CH3   Output  Speaker signal
PA11  GPIO       Input   Button left
PA12  GPIO       Input   Button right
PA15  TIM2_CH1   Input   Encoder left, channel A
PB0   TIM3_CH3   Output  Motor driver IN3 (right)
PB1   TIM3_CH4   Output  Motor driver IN4 (right)
PB3   TIM2_CH2   Input   Encoder left, channel B
PB4   GPIO       Output  LED left
PB5   GPIO       Output  LED right
PB6   TIM4_CH1   Input   Encoder right, channel B
PB7   TIM4_CH2   Input   Encoder right, channel A
PB8   GPIO       Output  Emitter right side
PB9   GPIO       Output  Emitter right front
PB10  USART3_TX  Output  Bluetooth RX
PB11  USART3_RX  Input   Bluetooth TX
PB12  NSS2       Output  Gyroscope NCS (SPI-CS)
PB13  SCK2       Output  Gyroscope SCL (SPI-SCLK)
PB14  MISO2      Input   Gyroscope AD0 (SPI-SDO)
PB15  MOSI2      Output  Gyroscope SDA (SPI-SDI)
PC13  N/A        N/A     *Not connected*
PC14  N/A        N/A     *Not connected*
PC15  N/A        N/A     *Not connected*
====  =========  ======  ==============================

Motor driver
------------

====  ========================================
Pin   Connected to
====  ========================================
IN1   *Defined in Bluepill pinout*
IN2   *Defined in Bluepill pinout*
IN3   *Defined in Bluepill pinout*
IN4   *Defined in Bluepill pinout*
OUT1  Motor left + (pin 2)
OUT2  Motor left - (pin 1)
OUT3  Motor right - (pin 1)
OUT4  Motor right + (pin 2)
EEP   50 kΩ pull-up to 5V (J2 jumper is open)
ULT   *Not connected*
VCC   Vmot
GND   GND
====  ========================================


.. index:: interruptions

Exceptions
==========

The configuration for interruptions (IRQ) and SysTick exceptions has been
deduced taking into account that:

- The priority is configurable, the exception and the IRQ numbers are fixed.

- If the software does not configure any priority, then all exceptions with a
  configurable priority have a priority of 0. We didn't find any reference on
  the documentation but for experience it seems that by default SysTick
  exception has higher priority than any interruption.

- For System Handlers, the lowest priority value has precedence over the
  lowest exception number value.

- For Interrupt service routines (ISR) the precedence is: lowest group priority
  value, lowest subpriority value and lowest IRQ number value. For now, there
  are not any subpriority value configured for this project.

- The preemption happens just for higher priority exceptions (nothing to do
  with exception number or subpriority).

For more information have a look at the `STM32F10xxx/20xxx/21xxx/L1xxxx
Cortex®-M3 programming manual`_.

==========  ========  =========  =======  ========  ======================
Exceptions  Handler   Excep num  IRQ num  Priority  Functionality
==========  ========  =========  =======  ========  ======================
SysTick     System    15         -1       1         Control and algorithm
ADC1_2      ISR       N/A        18       1         Battery low level
TIM1_UP     ISR       N/A        25       0         Infrared state machine
USART3      ISR       N/A        39       1         Bluetooth
==========  ========  =========  =======  ========  ======================

Gyroscope
---------

=====  ========  ==============================
Pin    Function  Connected to
=====  ========  ==============================
SCL    SPI-SCLK  *Defined in Bluepill pinout*
SDA    SPI-SDI   *Defined in Bluepill pinout*
EDA    N/A       *Not connected*
ECL    N/A       *Not connected*
AD0    SPI-SDO   *Defined in Bluepill pinout*
INT    N/A       *Not connected*
NCS    SPI-CS    *Defined in Bluepill pinout*
FSYNC  N/A       *Not connected*
VCC    Power     5V
GND    Power     GND
=====  ========  ==============================

.. index:: references

References
==========

.. target-notes::

.. _`HC-05 serial bluetooth`:
   https://athena-robots.readthedocs.io/en/latest/hc05_bluetooth.html
.. _`IE2-512 encoder`:
   https://www.faulhaber.com/fileadmin/Import/Media/EN_IE2-1024_DFF.pdf
.. _`STM32F10xxx Reference Manual`:
   http://www.st.com/resource/en/reference_manual/cd00171190.pdf
.. _`STM32F10xxx/20xxx/21xxx/L1xxxx Cortex®-M3 programming manual`:
   http://www.st.com/resource/en/programming_manual/cd00228163.pdf
