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

====  =========  ======  ==============================
Pin   Function   Type    Connected to
====  =========  ======  ==============================
PA0   ADC12_IN0  Input   Battery 1
PA1   ADC12_IN1  Input   Battery 2
PA2   ADC12_IN2  Input   Gyroscope Vref
PA3   ADC12_IN3  Input   Gyroscope Vout
PA4   ADC12_IN4  Input   Sensor left collector
PA5   ADC12_IN5  Input   Sensor front-left collector
PA6   ADC12_IN6  Input   Sensor front-right collector
PA7   ADC12_IN7  Input   Sensor right collector
PA8   GPIO       Output  Sensor emitter switch
PA9   TIM1_CH2   Output  Speaker PWM
PA10  GPIO       Input   Button
PA11  GPIO       Input   Button
PA12  GPIO       Output  Led
PA15  TIM2_CH1   Input   Left motor encoder, channel A
PB0   TIM3_CH3   Output  Left motor PWM
PB1   TIM3_CH4   Output  Right motor PWM
PB3   TIM2_CH2   Input   Left motor encoder, channel B
PB4   GPIO       Output  Led
PB5   GPIO       Output  Led
PB6   TIM4_CH1   Input   Right motor encoder, channel B
PB7   TIM4_CH2   Input   Right motor encoder, channel A
PB8   N/A        N/A     N/A
PB9   N/A        N/A     N/A
PB10  USART3_TX  Output  Bluetooth RX
PB11  USART3_RX  Input   Bluetooth TX
PB12  GPIO       Output  Left motor driver IN1
PB13  GPIO       Output  Left motor driver IN2
PB14  GPIO       Output  Right motor driver IN2
PB15  GPIO       Output  Right motor driver IN1
====  =========  ======  ==============================


References
==========

.. target-notes::

.. _`HC-05 serial bluetooth`:
  https://athena-robots.readthedocs.io/en/latest/hc05_bluetooth.html
.. _`IE2-512 encoder`:
  https://fmcc.faulhaber.com/resources/img/EN_IE2-1024_DFF.PDF
.. _`STM32F10xxx Reference Manual`:
  http://www.st.com/resource/en/reference_manual/cd00171190.pdf
