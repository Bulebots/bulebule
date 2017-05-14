.. index:: locomotion

**********
Locomotion
**********


.. index:: motors

Motors
======

The motors are a pair of `Faulhaber 1524B009SR`_, which have the following
basic characteristics (at 22ºC and nominal voltage):

====================  =====  ======
Name                  Value  Unit
====================  =====  ======
Diameter              15     mm
Length                24     mm
Nominal voltage       9      V
Terminal resistance   10.6   Ω
No-load speed         10000  rpm
No-load current       0.009  A
====================  =====  ======

With the following rated values for continuous operation:

====================  =====  ======
Name                  Value  Unit
====================  =====  ======
Rated torque          2.9    mm
Rated current         0.38   mm
Rated speed           4500   V
====================  =====  ======


.. index:: encoders

Encoders
========

Each motor has also an integrated `IE2-512 encoder`_. This is an incremental,
two-channel magnetic encoder with 512 lines per revolution. Requires a 5 V
supply voltage and outputs two digital square waves that can be used for the
indication and control of both shaft velocity and direction of rotation as
well as for positioning.


.. index:: gears, tires

Gears and tires
===============

The motors we have found come with a pinion attached to the shaft. The pinion
has a 4.5 mm reference diameter and 15 teeth, which means it has modulo 0.3.

.. math:: M = \frac{D_r}{Z} = 0.3

.. figure:: figures/gear_diameters.svg
   :width: 30%
   :align: center

   External, reference and internal diameters in a gear.

The external diameter can be calculated with:

.. math:: D_e = M (Z + 2) = D_r + 2 M

For our robot, we would like to have two wheels with gears attached to them
and driven by the pinion:

.. figure:: figures/gears.svg
   :width: 50%
   :align: center

   Pinion, gears and wheels in the robot.

Note we have two important restrictions:

- The wheel must have a diameter greater than the external gear diameter
  :math:`D_e^{gear}`. Otherwise the gear would be in contact with the floor.
- The wheel must have a diameter smaller than the reference diameter of the
  gear plus the reference diameter of the pinion :math:`D_r^{gear} +
  D_r^{pinion}`. Otherwise the two wheels would be in contact with eachother.

Taking into account the target robot size and also the availability of tires
and gears in local hobby/slot shops, we decided to go with:

- 60 teeth gears (18 mm reference diameter and less than 19 mm external
  diameter).
- 20 mm tires (`Slot.it PT18`_; suitable for smooth and clean surfaces).


.. index:: bearings, axis

Bearings and axis
=================

The bearings are some `MR63ZZ`_ (3 mm inner diameter, 6 mm outter diameter
and 2.5 mm width).


.. index:: wheel

Wheel
=====


.. index:: mounting

Mounting
========


References
==========

.. target-notes::

.. _`Faulhaber 1524B009SR`:
  https://fmcc.faulhaber.com/resources/img/EN_1524_SR_DFF.PDF
.. _`IE2-512 encoder`:
  https://fmcc.faulhaber.com/resources/img/EN_IE2-1024_DFF.PDF
.. _`MR63ZZ`:
   http://www.rcbearings.com/products/mr63zz-3-x-6-x-2-5-bearing.html
.. _`Slot.it PT18`:
   http://slot.it/immagini/KitPart/tires/Slot-it_tabella-2015.jpg
