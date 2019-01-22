# grid
Path-finding in a grid.

The Grid class represents a terrain in the form of a rectangular grid. The cells of the grid are indexed with pairs of integers, where (0,0) represents the lower left cell, and (m-1,n-1) represents the top right cell, where m is the width and n is the height of the grid.   Some of the cells of the grid are blocked by obstacles. We are interested in determining whether there is a path to reach a given goal cell from a given start cell by moving one cell at a time in each of the four cardinal directions: North, East, West, South, and never entering a blocked cell or stepping outside of the grid. You will write functions that compute and output such a path if it exists. A description is given in the pdf file provided above.  
Implementation of the methods are in the file Grid.cpp.   
The signatures of the classes and their methods are given in the header file Grid.h.   
The file main.cpp includes a basic usage.   
The provided makefile will attempt to build the main file.   
Example format of a grid to read in is provided in "grid".
