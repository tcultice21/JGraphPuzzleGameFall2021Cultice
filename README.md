# JGraphPuzzleGameFall2021Cultice
## Description/Details
This program will allow the user to use standard-in (STDIN) to play
a puzzle game, using output from JGraph.

![alt text][logo]

[logo]: https://github.com/AndrewCrittenden/comfortControl/raw/main/ClientCOMFORT/image.PNG "Typical output of program"

Save states can be used to allow saving and loading and the game
ends and presents results after 10 turns. Several savestates are
provided within the makefile.

To make a match, you must:
-Select 3+ tiles of the same color
-They must be adjacent

When a match is completed, adjacent shapes will grow in size. At 4x the size, they will break and chain react with
any other surrounding it for bonus score multiplier.

Tiles fall down from the top and are randomly generated using the system's TRNG. A randomly generated board will be used
if no save file is specified.

## Compilation
A simple compilation can be completed by using GNU G++ with C++11.
However, a makefile is provided that can compile.
To run, one must install jgraph (make install) and ImageMagick to their machine.

The major functions of this makefile are:
- make: compile the binary
- clean: remove all unnecessary generated files
- install: installs jgraph to PC (probably requires sudo)

## Running
Several examples are within the makefile, using the commands:
- play: generate game/binary and play without save
- victory: generate a game close to game over (1 turn left)
- redandblue: generate game with red and blue pattern
- green: generate game with size variation green pattern
- purpleandyellow: generate game of purple and yellow
- save: generate game and save it as testGame.txt
All Generated files by this makefile 
These examples also compile the binary.
