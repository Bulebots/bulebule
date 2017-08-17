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

====  =========  ==============================
Pin   Function   Connected to
====  =========  ==============================
PA8   TIM1_CH1   Left motor encoder, channel A
PA9   TIM1_CH2   Left motor encoder, channel B
PB6   TIM4_CH1   Right motor encoder, channel B
PB7   TIM4_CH2   Right motor encoder, channel A
PB10  USART3_TX  Bluetooth RX
PB11  USART3_RX  Bluetooth TX
====  =========  ==============================


References
==========

.. target-notes::

.. _`HC-05 serial bluetooth`:
  https://athena-robots.readthedocs.io/en/latest/hc05_bluetooth.html
.. _`IE2-512 encoder`:
  https://fmcc.faulhaber.com/resources/img/EN_IE2-1024_DFF.PDF
.. _`STM32F10xxx Reference Manual`:
  http://www.st.com/resource/en/reference_manual/cd00171190.pdf
