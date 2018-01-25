.. index:: utils

*****
Utils
*****

.. index:: utils, dependencies

Installing Python dependencies
==============================

To install the required dependencies for developing the Python code of Theseus, you can
make use of the provided ``requirements.txt`` file:

.. code-block:: bash

   pip install -r requirements.txt


.. index:: utils, documentation

Generating documentation
========================

Documentation is generated with Sphinx. In order to generate the documentation
locally you need to run ``make`` from the ``docs`` directory:

.. code-block:: bash

   make html

