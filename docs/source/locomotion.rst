.. index:: locomotion

**********
Locomotion
**********


.. index:: motors, encoders

Motors and encoders
===================

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
Rated current         0.38   A
Rated speed           4500   V
====================  =====  ======

The motors have attached `IE2-512 quadrature encoders`_, with 512 lines per
revolution.


.. index:: gears, tires

Design
======

We are following a `Tetra locomotion design`_. The motors we have found come
with a pinion attached to the shaft. The pinion has a 4.5 mm reference diameter
and 15 teeth, which means it has modulo 0.3.

Taking into account the target robot size, the design restrictions and also the
availability of tires and gears in local hobby/slot shops, we decided to go
with:

- `60 teeth gears`_ (18 mm reference diameter and less than 19 mm external
  diameter).
- 20 mm tires (`Slot.it PT18`_; suitable for smooth and clean surfaces).


.. index:: bearings, axis

Bearings and axis
=================

The bearings are some `MR63ZZ`_ (3 mm inner diameter, 6 mm outter diameter
and 2.5 mm width).


.. index:: rim

Rim
===

The rim has been designed with `CadQuery`_. See the `Bulebule 3D designs`.


.. index:: mounting

Mounting
========

The mounting has been designed with `CadQuery`_. See the `Bulebule 3D designs`.


References
==========

.. target-notes::

.. _`CadQuery`:
  https://github.com/dcowden/cadquery
.. _`Faulhaber 1524B009SR`:
  https://fmcc.faulhaber.com/resources/img/EN_1524_SR_DFF.PDF
.. _`IE2-512 quadrature encoders`:
  https://athena-robots.readthedocs.io/en/latest/ie2-1024_encoder_family.html
.. _`MR63ZZ`:
   http://www.rcbearings.com/products/mr63zz-3-x-6-x-2-5-bearing.html
.. _`60 teeth gears`:
   http://www.mootio-components.com/engranajes-de-plastico/modulo-0.300/dientes-60z
.. _`Slot.it PT18`:
   http://slot.it/immagini/KitPart/tires/Slot-it_tabella-2015.jpg
.. _`Tetra locomotion design`:
  https://athena-robots.readthedocs.io/en/latest/tetra.html
.. _`Bulebule 3D designs`:
  https://github.com/Theseus/bulebule/tree/master/3d
