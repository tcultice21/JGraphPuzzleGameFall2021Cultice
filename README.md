# JGraph Tiled Puzzle Game (Fall 2021, Lab 1)
## Description/Details
This program will allow the user to use standard-in (STDIN) to play
a puzzle game, using output from JGraph.

![alt text][logo]

[logo]: https://github.com/AndrewCrittenden/comfortControl/raw/main/ClientCOMFORT/image.PNG "Typical output of program"

Save states can be used to allow saving and loading and the game
ends and presents results after 10 turns. Several savestates are
provided within the makefile.

To make a match, you must:
- Select 3+ tiles of the same color
- They must be adjacent

When a match is completed, adjacent shapes will grow in size. At 4x the size, they will break and chain react with
any other surrounding it for bonus score multiplier.

Tiles fall down from the top and are randomly generated using the system's TRNG. A randomly generated board will be used
if no save file is specified.
## JGraph CPP interface
In order to efficiently interface my code with JGraph, Joseph Clark and I collaborated to create Jgraph.h, which is a
functional API/interface to JGraph commands. Using cpp classes, such as Canvas/Graph, we can generate object oriented
code that is later printed out as script required for JGraph. This is then tunneled through convert to create JPGs from
within the code itself, allowing easy image creation without leaving the binary.

The interface contains classes for major features, such as:
-Axis
-Mark (polymorphic with various child classes)
  - ShapeMark
  - TextMark
  - PostscriptRawMark
  - PostscriptFileMark
  - GeneralMark
- Arrows
- Curves
- Legends
- Graph
- Canvas

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
All Generated files by these examples are removed by 'make clean' as well.
These examples also compile the binary.

To manually run the program, one can run with two sets of input arguments:
./puzzle
OR
./puzzle -s fileName
This chooses between saving and not saving; if the fileName doesn't exist, it will create a new game and save it there.
Otherwise, it will load fileName, and error out of the file is not in the right format.

### To input moves, one must follow the format:
{(x0,y0),(x1,y1),(x2,y2)....}
White space is acceptable.

## Examples

### Red and Blue:
This is a fancy pattern using red and green tiles to demonstrate save state editing.
![alt text][logo]

[logo]: https://github.com/AndrewCrittenden/comfortControl/raw/main/ClientCOMFORT/image.PNG "Red and Blue"

### Purple and Yellow:
This is a pattern generated by the toJgraph draw function of the puzzle.
![alt text][logo]

[logo]: https://github.com/AndrewCrittenden/comfortControl/raw/main/ClientCOMFORT/image.PNG "Purple and Yellow"

### Green:
This is a full green pattern, which would generate quite a few points.
![alt text][logo]

[logo]: https://github.com/tcultice21/JGraphPuzzleGameFall2021Cultice/blob/main/Images/green.jpg "Green"

### Typical Game:
This is a typical game look, with randomly generated sizes and shapes scattered around.
![alt text][logo]

[logo]: https://github.com/tcultice21/JGraphPuzzleGameFall2021Cultice/blob/main/Images/victory.jpg "Victory"

### Game over screen:
This is the game over screen shown when the game is completed.
![alt text][logo]

[logo]: https://github.com/tcultice21/JGraphPuzzleGameFall2021Cultice/blob/main/Images/gameover.jpg "Game over"

## Save files
Save file format can be seen in the /saveStates/ folder.

They contain the general structure, for generating your own:
-JGRAPHFALL2021CULTICE SCORE-
#Score
#Turns
6x9 board*


* encoding for the board is as follows:
0-2 = Red, size 1 to 3
3-5 = Green, size 1 to 3
6-8 = Blue, size 1 to 3
9-B = Purple, size 1 to 3
C-E = Yellow, size 1 to 3
F = BLOCKED TILE
