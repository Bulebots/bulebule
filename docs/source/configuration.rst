.. index:: configuration

*************
Configuration
*************


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


.. index:: pinout

Pinout
======

Bluepill
--------

====  =========  ======  ==============================
Pin   Function   Type    Connected to
====  =========  ======  ==============================
PA0   ADC12_IN0  Input   Battery
PA1   N/A        N/A     N/A
PA2   ADC12_IN2  Input   Left side phototransistor
PA3   ADC12_IN3  Input   Left front phototransistor
PA4   ADC12_IN4  Input   Right front phototransistor
PA5   ADC12_IN5  Input   Right side phototransistor
PA6   TIM3_CH1   Input   Motor driver IN1 (left)
PA7   TIM3_CH2   Input   Motor driver IN2 (left)
PA8   GPIO       Output  Left front emitter switch
PA9   GPIO       Output  Left side emitter switch
PA10  TIM1_CH3   Output  Speaker PWM
PA11  GPIO       Input   Left button
PA12  GPIO       Input   Right button
PA15  TIM2_CH1   Input   Left motor encoder, channel A
PB0   TIM3_CH3   Output  Motor driver IN3 (right)
PB1   TIM3_CH4   Output  Motor driver IN4 (right)
PB3   TIM2_CH2   Input   Left motor encoder, channel B
PB4   GPIO       Output  Left LED
PB5   GPIO       Output  Right LED
PB6   TIM4_CH1   Input   Right motor encoder, channel B
PB7   TIM4_CH2   Input   Right motor encoder, channel A
PB8   GPIO       Output  Right side emmiter switch
PB9   GPIO       Output  Right front emmiter switch
PB10  USART3_TX  Output  Bluetooth RX
PB11  USART3_RX  Input   Bluetooth TX
PB12  XXXX       ??????  Digital gyro
PB13  XXXX       ??????  Digital gyro
PB14  XXXX       ??????  Digital gyro
PB15  XXXX       ??????  Digital gyro
PC13  N/A        N/A     N/A
PC14  N/A        N/A     N/A
PC15  N/A        N/A     N/A
====  =========  ======  ==============================

Motor driver
------------

====  ==============================
Pin   Connected to
====  ==============================
OUT1  Left motor + (pin 2)
OUT2  Left motor - (pin 1)
OUT3  Right motor - (pin 1)
OUT4  Right motor + (pin 2)
EEP   3V3 (J2 jumper is open)
ULT   Not connected
====  ==============================


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

.. index:: references

References
==========

.. target-notes::

.. _`HC-05 serial bluetooth`:
  https://athena-robots.readthedocs.io/en/latest/hc05_bluetooth.html
.. _`IE2-512 encoder`:
  https://fmcc.faulhaber.com/resources/img/EN_IE2-1024_DFF.PDF
.. _`STM32F10xxx Reference Manual`:
  http://www.st.com/resource/en/reference_manual/cd00171190.pdf
.. _`STM32F10xxx/20xxx/21xxx/L1xxxx Cortex®-M3 programming manual`:
  http://www.st.com/content/ccc/resource/technical/document/programming_manual/5b/ca/8d/83/56/7f/40/08/CD00228163.pdf/files/CD00228163.pdf/jcr:content/translations/en.CD00228163.pdf
