# AdhesionRandomWalk
Code to run the stochastic simulations in "A space-jump derivation for non-local models of cell-cell adhesion and non-local chemotaxis"

Requirements:

1) C++ compiler with C++11 support
2) cxxtest for the unit tests
3) CMake
4) SWig

4) python3
5) SqlAlchemy
6) SQL server (MySQL or MariaDB has been tested)
7) Alembic if changes to the database layout are made
8) pandas and numpy
9) matplotlib

For the moment this only compiles on a X86 cpu with the AVX2 instruction set. Future work includes making sure that this works on all CPUs.

ToDos:
1) Tell the compiler consistently that we are using aligned data types!
2) Remove some of the hand written AVX code, and see if the compiler can take care of optimizing those where those things are supported.

Quick guide:

The main C++ simulations are implemented in the src folder.
  - The Simulator class is the external interface which is wrapped with SWig

The Player for simulations is implemented in the python folder in adhesion_1d.py.
  - The Player class talks to the c++ simulator class, and will start simulations. 
  - Further an interval can be defined at which the C++ simulator sends data back to the player
  - The Player starts N (CPUs) + 1 processes which run simulations + 1 processes which processes data received from 
  each of the simulation processes. This process writes the received data into the database.

Code for creating the plots is in plot.py

import.py is a quick and dirty method to read matlab simulation data and import it into the SQL database.

The database layout is defined in models.py

Updating database:
This is done using alembic. See documentation somewhere ...


