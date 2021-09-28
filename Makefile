###################################
# Makefile for JGraph Lab 1
# Created 9/2021 by Tyler Cultice
#
###################################
#
# Default is to compile the binary normally
#
# However, there are optional methods to get
# unique results from the program/JGraph:
#
#	clean -- removes all generated files
#			- This includes the JPGs!
#
#	install -- Installs JGraph
#
#	play -- Generate the game and play
#	 normally no save
#
#	victory -- Plays game using state that
#	 is 1 move from game over
#
#	redandblue -- Generates game with
#	 unique RedANDBlue pattern
#
#	green -- Generates game with all
#	 green tiles.
#
#	purpleandyellow -- Generate game of purple
#		and yellow
#
#	save -- Generate game and save it
#

TESTOUTPUTS = ./saveStates
STANDARD = -std=c++11
GAMEFILES = main.cpp

all: 
	g++ -o puzzle $(GAMEFILES) $(STANDARD)

clean:
	rm -f ./puzzle
	rm -f *.jpg
	rm -f testGame.txt
	rm -f ./green.txt
	rm -f ./victory.txt
	rm -f ./redandblue.txt
	rm -f ./purpleandyellow.txt

# Probably require sudo
install: 
	apt-get install jgraph

play:
	g++ -o puzzle $(GAMEFILES) $(STANDARD)
	./puzzle
	
victory:
	cp $(TESTOUTPUTS)/victory.txt ./victory.txt
	g++ -o puzzle $(GAMEFILES) $(STANDARD)
	./puzzle -s ./victory.txt
	
redandblue:
	cp $(TESTOUTPUTS)/redandblue.txt ./redandblue.txt
	g++ -o puzzle $(GAMEFILES) $(STANDARD)
	./puzzle -s ./redandblue.txt

green:
	cp $(TESTOUTPUTS)/green.txt ./green.txt
	g++ -o puzzle $(GAMEFILES) $(STANDARD)
	./puzzle -s ./green.txt

purpleandyellow:
	cp $(TESTOUTPUTS)/purpleandyellow.txt ./purpleandyellow.txt
	g++ -o puzzle $(GAMEFILES) $(STANDARD)
	./puzzle -s ./purpleandyellow.txt

save:
	rm -f testGame.txt
	g++ -o puzzle $(GAMEFILES) $(STANDARD)
	./puzzle -s testGame.txt