# Spiral Galaxy Simulation

## Description
The program solves a 2D, n-body problem using a gravitational potential to simulate spiral galaxies with the Fast Multipole Methed (FMM).

The header `Setup.h` contains presets.
They can be chosen in `main.cpp`.
Feel free to add additional configurations there.

There are three methods implemented, the FMM, the Linked Cell Method and a direct calculation.

The relevant files for the FMM are: `{ Fmm.h, Tree.h, Expansion.h }`

## Dependencies
* __make 4.3__
* __gcc 11.1__ or higher (C++ 20 is used)
* a terminal emulator with ANSI 4-bit color support
* paraview

## Compile
* build with `make`
* clean with `make clean`

## Execute
To execute, just type `./space` into the command line.
* The `.vtk` files used for the visualization will be stored in the directory `data/`.
* The benchmark data will be stored in a `.txt` file in the directory `benchmark/`.

## Visualization
* open paraview
* load the file `visualization-config.pvsm` by clicking `File`→`Load State...`: select `Search files under specified directory` and click `OK`.

If you need help using paraview, please consider this
[paraview tutorial](https://www.bu.edu/tech/support/research/training-consulting/online-tutorials/paraview/)
.
