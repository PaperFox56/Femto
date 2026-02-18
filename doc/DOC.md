This file will be used to document the code as much as possible, keeping track of the projects structure.


### Overview of the code structure

The project is written in the C programming language and all the source files lie in the `src/` directory.
Note that the header files are the ones defining the code structure, with the .c files only there for implementing the functions. That's the reason why the implementations of functions in a single header file sometimes appears in differents source files. 
As this is the case, this documentation presents the header files and their funtions while mentioning which .c file contains the implementation.


## terminal.h
Low level funtions to interact with the terminal.

**int getwindow_size(int* rows, int* cols)**
