/*
-----------------------------
Puzzle Game utilizing JGraph
by Tyler Cultice
-----------------------------

Requires ImageMagick 6 on machine for "Convert" utility
and JGraph -- both acquirable through apt-get

Compile using g++ -o Puzzle main.cpp -std=c++11
Use by calling:

./Puzzle [-s fileName]
where -s fileName is the save where you would like to load or save to (does not require to exist)

Takes standard in for moves, formatted as {(x0,x0),(x1,x1),(x2,x2)....}
*/

#include "JGraph.h"
#include <vector>
#include <random>
#include <iostream>
#include <queue>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

// Tile class -- Contains type information, size information, and methods to initalize them and reset them
class Tile {
public:
	enum class TileType {
		red = 0,
		green = 1,
		blue = 2,
		purple = 3,
		yellow = 4,
		BLOCKED = 5,
		empty = 6
	};

	inline string tileTypeToString() {
		switch (type) {
		case TileType::red: return "red";
		case TileType::green: return "green";
		case TileType::blue: return "blue";
		case TileType::purple: return "purple";
		case TileType::yellow: return "yellow";
		default: return "";
		}
	}

	TileType type;
	int size;

	Tile() {
		initTile();
	}


	Tile(TileType typ, int state) {
		type = typ;
		size = state;
	}

	void resetTile() {
		random_device rd;
		size = 0;
		type = (TileType)(rd() % 5);
	}

	void initTile() {
		random_device rd;
		size = (rd() % 3);
		type = (TileType)(rd() % 5);
	}

};

#define BOARD_LEN 9
#define BOARD_HEIGHT 6

vector<vector<Tile>> board;
string file;

// Score and number of Turns
// Number of turns < 10 and > 0
long score;
int numTurns;

// Init the board, create the proper length vectors
void boardInit() {
	board.resize(BOARD_LEN);
	for (int i = 0; i < board.size(); i++) {
		board[i].resize(BOARD_HEIGHT);
	}

	// Block Top left
	board[0][0].type = Tile::TileType::BLOCKED;
	board[0][0].size = 0;

	// Block Bottom Left
	board[0][BOARD_HEIGHT-1].type = Tile::TileType::BLOCKED;
	board[0][BOARD_HEIGHT - 1].size = 0;

	// Block Top Right
	board[BOARD_LEN-1][0].type = Tile::TileType::BLOCKED;
	board[BOARD_LEN - 1][0].size = 0;

	// Block Bottom Right
	board[BOARD_LEN - 1][BOARD_HEIGHT - 1].type = Tile::TileType::BLOCKED;
	board[BOARD_LEN - 1][BOARD_HEIGHT - 1].size = 0;
}

// Init game, if new game has been created
void gameInit() {
	boardInit();
	score = 0;
	numTurns = 10;
}

// Save game currently in progress
int gameSave(string fileName) {
	// 3 Statuses:
	// 0 Reads successfully
	// 1 Unable to save
	// 2 Format of file is not correct (Actual error)
	ofstream saveFile(fileName, ios::trunc);
	if (!saveFile.is_open())
		return 1;

	// Top game identifier
	saveFile << "-JGRAPHFALL2021CULTICE SCORE-" << endl;

	// Score
	saveFile << "#" << score << endl;

	// Turns
	saveFile << "#" << numTurns << endl;
	
	// Load board
	// 0-2 = Red, size 1 to 3
	// 3-5 = Green, size 1 to 3
	// 6-8 = Blue, size 1 to 3
	// 9-B = Purple, size 1 to 3
	// C-E = Yellow, size 1 to 3
	// F = BLOCKED TILE
	for (int i = 0; i < BOARD_LEN; i++) {
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			saveFile << hex << 3 * (int)board[i][j].type + board[i][j].size;
		}
		saveFile << endl;
	}

	saveFile.close();

	return 0;
}

// Save file format:
// Score
// Num Turns
// Board in decimal form

// Read Save Game
int gameRead(string fileName) {
	// 3 Statuses:
	// 0 Reads successfully
	// 1 Unable to open it, will try to save later
	// 2 Format of file is not correct (Actual error)
	ifstream saveFile(fileName);
	if (!saveFile.is_open())
		return 1;

	string tempString;
	const string TopIdent = "-JGRAPHFALL2021CULTICE SCORE-";

	// Top game identifier
	getline(saveFile, tempString);

	if (tempString.compare(TopIdent) != 0) return 2;

	// Load Score
	getline(saveFile, tempString);
	if (tempString[0] != '#') return 2;
	tempString = tempString.erase(0, 1);

	if (tempString.empty() || !(find_if(tempString.begin(), tempString.end(),
		[](char ch) { return !std::isdigit(ch); }) == tempString.end())) return 2;

	score = stoi(tempString);

	// Load Turns
	getline(saveFile, tempString);
	if (tempString[0] != '#') return 2;
	tempString = tempString.erase(0, 1);

	if (tempString.empty() || !(find_if(tempString.begin(), tempString.end(),
		[](char ch) { return !std::isdigit(ch); }) == tempString.end())) return 2;

	numTurns = stoi(tempString);
	if (numTurns > 10 || numTurns <= 0) return 2;

	// Load board
	// 0-2 = Red, size 1 to 3
	// 3-5 = Green, size 1 to 3
	// 6-8 = Blue, size 1 to 3
	// 9-B = Purple, size 1 to 3
	// C-E = Yellow, size 1 to 3
	// F = BLOCKED TILE
	boardInit();

	// Convert board using above mapping
	for (int i = 0; i < BOARD_LEN; i++) {
		if (!saveFile) return 2;
		getline(saveFile, tempString);
		if (tempString.length() != BOARD_HEIGHT) return 2;
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			char mapValue;
			// Change to decimal value for easy conversion
			if (tempString[j] <= '9' && tempString[j] >= '0') mapValue = tempString[j] - '0';
			else if (tempString[j] >= 'a' && tempString[j] <= 'f') mapValue = tempString[j] - 'a' + 10;
			else return 2;
			// divide by 3 gives type, modulo 3 gives size
			board[i][j].type = (Tile::TileType)(mapValue / 3);
			board[i][j].size = (mapValue % 3);
		}
	}

	saveFile.close();
	return 0;
}

// If you run out of turns, finish game w/ Jgraph output and save file write
void gameFinish(bool isSaved,string fileName) {

	// Canvas, contains graphs, set to boundaries required
	JGraph::Canvas testcanvas;
	testcanvas.bounding_box.X = 0;
	testcanvas.bounding_box.Y = -3;
	testcanvas.bounding_box.width = 6 * 72;
	testcanvas.bounding_box.height = 5 * 72;
	testcanvas.size.height = 4;
	testcanvas.size.width = 6;
	testcanvas.graphs.push_back(JGraph::Graph());
	JGraph::Graph& testgraph = testcanvas.graphs[0];

	// X axis has no axis, only marks to build grid
	JGraph::Axis& xaxis = testgraph.xaxis;
	xaxis.size_inches = 6;
	xaxis.min = 0;
	xaxis.max = 9;
	xaxis.hash_spacing = 1;
	xaxis.minor_hash_count = 0;
	xaxis.grid_lines = true;
	xaxis.minor_grid_lines = false;
	xaxis.mgrid_color = JGraph::Gray(.625);
	xaxis.draw = false;

	// Y axis has no axis, only marks to build grid
	JGraph::Axis& yaxis = testgraph.yaxis;
	yaxis.size_inches = 4;
	yaxis.min = 0;
	yaxis.max = 6;
	yaxis.hash_spacing = 1;
	yaxis.minor_hash_count = 0;
	yaxis.grid_lines = true;
	yaxis.minor_grid_lines = false;
	yaxis.mgrid_color = JGraph::Gray(.625);
	yaxis.draw = false;

	// Black Background
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[0].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[0].curveColor = JGraph::Color(0, 0, 0);
	testgraph.curves[0].points = { {3,3} };
	JGraph::ShapeMark* blackBackground = new JGraph::ShapeMark();
	blackBackground->type = JGraph::ShapeMark::Type::box;
	blackBackground->size = { 50, 50 };
	blackBackground->pattern = JGraph::ShapeMark::FillPattern::solid;
	blackBackground->color = JGraph::Color(0, 0, 0);

	// Score text
	testgraph.curves.push_back(JGraph::Curve());
	JGraph::Curve& Scoretext = testgraph.curves.back();
	Scoretext.lineType = JGraph::Curve::LineType::none;
	Scoretext.points = { {4.5, 3} };
	JGraph::TextMark* textMark = new JGraph::TextMark();
	textMark->text.font = "Arial";
	textMark->text.size = 40;
	textMark->text.color.R = 1;
	textMark->text.color.G = 1;
	textMark->text.color.B = 1;
	textMark->text.line_spacing = 20;
	textMark->text.content = "GAME OVER \n Score: ";
	textMark->text.content += to_string(score);

	testgraph.curves[0].marks.reset(blackBackground);
	Scoretext.marks.reset(textMark);

	// Time to terrify anyone by making it look like
	// Their computer is compromised
	// This looks that sketchy.
	JGraph::jgraphToJPG(testcanvas, "gameOutput.jpg");

	// Rewrite the save file w/ score
	ofstream saveFile(fileName, ios::trunc);
	if (!saveFile.is_open())
		return;

	// Top game identifier
	saveFile << "SAVE COMPLETE, GAME OVER" << endl
		<< "SCORE: " << score << endl;

	saveFile.close();
}

// TileFall to allow tiles to fall down the board from top to bottom
void tileFall(vector<int> columnsToConsider) {
	for (int i = 0; i < columnsToConsider.size(); i++) {
		// Ignore if column is untouched
		if (columnsToConsider[i] == -1) continue;
		// Start at the lowest point
		int numEmpty = 0;
		for (int j = columnsToConsider[i]; j >= 0; j--) {
			if (board[i][j].type == Tile::TileType::BLOCKED) {
				numEmpty++;
				continue;
			}	

			else if (board[i][j].type == Tile::TileType::empty) {
				numEmpty++;
			}
			// Special case: blocked spaces, ignore spot
			else if (board[i][j+numEmpty].type == Tile::TileType::BLOCKED) {
				numEmpty--;
				board[i][j + numEmpty] = board[i][j];
			}
			// Move down next by number of empty
			else {
				board[i][j + numEmpty] = board[i][j];
			}
		}
		// Now we know the top numEmpty's should be empty, fill them with reset
		for (int j = 0; j < numEmpty; j++) {
			if (board[i][j].type != Tile::TileType::BLOCKED)
				board[i][j].resetTile();
		}
	}
}

// Perform the basic game mechanics
void gameProcedure(vector<JGraph::Point<int>> moves) {
	queue<JGraph::Point<int> > popQueue;
	vector<int> lowestinColumn = { -1,-1,-1,-1,-1,-1,-1, -1, -1};
	JGraph::Point<int> lastLocation = moves[0];
	Tile::TileType moveType = board[lastLocation.x][lastLocation.y].type;

	// Begin move processing
	// Multiplier for score is increased for each acquired tile
	// After move, all tiles will grow in size, if size exceeds 3x, it will pop
	// Popped tiles will provide an extra 0.2x of base score + 2x every cascaded pop
	// Popped tiles can cause chain reactions

	int moveScore = 0;
	for (int i = 0; i < moves.size(); i++) {
		if (board[moves[i].x][moves[i].y].type == Tile::TileType::empty) continue;
		// Pop tile, add score, flip boolean flag
		board[moves[i].x][moves[i].y].type = Tile::TileType::empty;
		
		// If popped tile is lower than recorded "lowest", replace it
		//if (lowestinColumn[moves[i].x] < moves[i].y) 
		//	lowestinColumn[moves[i].x] = moves[i].y;

		moveScore += 10 * ((board[moves[i].x][moves[i].y].size+1) * moves.size())/4;
		popQueue.push(moves[i]);
	}
	
		// Grow outside and if they are about to pop, add to pop stack
	int chainMultiplier = 0;

	while (!popQueue.empty()) {
		JGraph::Point<int> tileToPop = popQueue.front();
		popQueue.pop();

		// Pop it, change its type, add to multiplier and score
		board[tileToPop.x][tileToPop.y].type = Tile::TileType::empty;
		if (lowestinColumn[tileToPop.x] < tileToPop.y) {
			lowestinColumn[tileToPop.x] = tileToPop.y;
		}

		chainMultiplier++;

		// Analyze and add to the left
		if (tileToPop.x > 0 && board[tileToPop.x - 1][tileToPop.y].type != Tile::TileType::BLOCKED && board[tileToPop.x - 1][tileToPop.y].type != Tile::TileType::empty) {
			board[tileToPop.x - 1][tileToPop.y].size++;
			if (board[tileToPop.x - 1][tileToPop.y].size == 3) {
				popQueue.push({ tileToPop.x - 1,tileToPop.y });
			}
		}

		// Analyze and add to right
		if (tileToPop.x < BOARD_LEN - 1 && board[tileToPop.x + 1][tileToPop.y].type != Tile::TileType::BLOCKED && board[tileToPop.x + 1][tileToPop.y].type != Tile::TileType::empty) {
			board[tileToPop.x + 1][tileToPop.y].size++;
			if (board[tileToPop.x + 1][tileToPop.y].size == 3) {
				popQueue.push({ tileToPop.x + 1, tileToPop.y });
			}
		}

		// Analyze and add below
		if (tileToPop.y < BOARD_HEIGHT - 1 && board[tileToPop.x][tileToPop.y + 1].type != Tile::TileType::BLOCKED && board[tileToPop.x][tileToPop.y + 1].type != Tile::TileType::empty) {
			board[tileToPop.x][tileToPop.y + 1].size++;
			if (board[tileToPop.x][tileToPop.y + 1].size == 3) {
				popQueue.push({ tileToPop.x,tileToPop.y + 1 });
			}
		}

		// Analyze and add above
		if (tileToPop.y > 0 && board[tileToPop.x][tileToPop.y - 1].type != Tile::TileType::BLOCKED && board[tileToPop.x][tileToPop.y-1].type != Tile::TileType::empty) {
			board[tileToPop.x][tileToPop.y - 1].size++;
			if (board[tileToPop.x][tileToPop.y - 1].size == 3) {
				popQueue.push({ tileToPop.x,tileToPop.y - 1 });
			}
		}

		
	}

	score += moveScore + moveScore*chainMultiplier/5;

	// Drop tiles down and fill the top
	tileFall(lowestinColumn);

	numTurns--;
}

// Draw the board using JGraph
void drawBoard() {
	// Canvas, contains graphs, set to boundaries required
	JGraph::Canvas testcanvas;
	testcanvas.bounding_box.X = 0;
	testcanvas.bounding_box.Y = -3;
	testcanvas.bounding_box.width = 6*72;
	testcanvas.bounding_box.height = 5*72;
	testcanvas.size.height = 4;
	testcanvas.size.width = 6;
	testcanvas.graphs.push_back(JGraph::Graph());
	JGraph::Graph& testgraph = testcanvas.graphs[0];

	// X axis has no axis, only marks to build grid
	JGraph::Axis& xaxis = testgraph.xaxis;
	xaxis.size_inches = 6;
	xaxis.min = 0;
	xaxis.max = 9;
	xaxis.hash_spacing = 1;
	xaxis.minor_hash_count = 0;
	xaxis.grid_lines = true;
	xaxis.minor_grid_lines = false;
	xaxis.mgrid_color = JGraph::Gray(.625);
	xaxis.draw = false;

	// Y axis has no axis, only marks to build grid
	JGraph::Axis& yaxis = testgraph.yaxis;
	yaxis.size_inches = 4;
	yaxis.min = 0;
	yaxis.max = 6;
	yaxis.hash_spacing = 1;
	yaxis.minor_hash_count = 0;
	yaxis.grid_lines = true;
	yaxis.minor_grid_lines = false;
	yaxis.mgrid_color = JGraph::Gray(.625);
	yaxis.draw = false;

	///////////////////// Small Shapes //////////////////////////
	// Red Small
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[0].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[0].curveColor = JGraph::Color(0.2, 0, 0);
	testgraph.curves[0].points = { };
	JGraph::GeneralMark* redSmallMark = new JGraph::GeneralMark();
	redSmallMark->type = JGraph::GeneralMark::Type::general;
	redSmallMark->points = { {-1,-0.25},{-1,0.25},{-0.25,0.25},{-0.25,1},{0.25,1},{0.25,0.25},{1,0.25},{1,-0.25},{0.25,-0.25},{0.25,-1},{-0.25,-1},{-0.25,-0.25} };
	redSmallMark->size = { .925/3, .925/3 };
	redSmallMark->pattern = JGraph::GeneralMark::FillPattern::solid;
	redSmallMark->fill_rotate_angle = 15;
	redSmallMark->color = JGraph::Color(1,0.2,0.20);

	// Green Small
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[1].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[1].curveColor = JGraph::Color(0, 0.2, 0);
	testgraph.curves[1].points = { };
	JGraph::ShapeMark* greenSmallMark = new JGraph::ShapeMark();
	greenSmallMark->type = JGraph::ShapeMark::Type::triangle;
	greenSmallMark->size = { .925/3, .925/3 };
	greenSmallMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	greenSmallMark->fill_rotate_angle = 30;
	greenSmallMark->color = JGraph::Color(0, 0.5, 0.17);
	
	// Blue Small
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[2].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[2].curveColor = JGraph::Color(0, 0, 0.2);
	testgraph.curves[2].points = { };
	JGraph::ShapeMark* blueSmallMark = new JGraph::ShapeMark();
	blueSmallMark->type = JGraph::ShapeMark::Type::circle;
	blueSmallMark->size = { .850/3, .850/3 };
	blueSmallMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	blueSmallMark->color = JGraph::Color(0, 0.47, 0.7);
	
	// Purple Small
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[3].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[3].curveColor = JGraph::Color(0.2, 0, 0.2);
	testgraph.curves[3].points = { };
	JGraph::ShapeMark* purpleSmallMark = new JGraph::ShapeMark();
	purpleSmallMark->type = JGraph::ShapeMark::Type::diamond;
	purpleSmallMark->size = { .925/3, .925/3 };
	purpleSmallMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	purpleSmallMark->color = JGraph::Color(0.9, 0, 0.75);

	// Yellow Small
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[4].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[4].curveColor = JGraph::Color(0.2, 0.2, 0);
	testgraph.curves[4].points = { };
	JGraph::GeneralMark* yellowSmallMark = new JGraph::GeneralMark();
	yellowSmallMark->type = JGraph::GeneralMark::Type::general;
	yellowSmallMark->points = { {-1,-1}, {-0.5,0}, {-1,1}, {0,0.5}, {1,1}, {0.5,0}, {1,-1}, {0,-0.5} };
	yellowSmallMark->size = { .925/3, .925/3 };
	yellowSmallMark->pattern = JGraph::GeneralMark::FillPattern::solid;
	yellowSmallMark->color = JGraph::Color(1, 0.9, 0.0);

	//////////////// Medium Shapes /////////////////////////

	// Red Medium
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[5].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[5].curveColor = JGraph::Color(0.2, 0, 0);
	testgraph.curves[5].points = { };
	JGraph::GeneralMark* redMediumMark = new JGraph::GeneralMark();
	redMediumMark->type = JGraph::GeneralMark::Type::general;
	redMediumMark->points = { {-1,-0.25},{-1,0.25},{-0.25,0.25},{-0.25,1},{0.25,1},{0.25,0.25},{1,0.25},{1,-0.25},{0.25,-0.25},{0.25,-1},{-0.25,-1},{-0.25,-0.25} };
	redMediumMark->size = { .925/1.5, .925/ 1.5 };
	redMediumMark->pattern = JGraph::GeneralMark::FillPattern::solid;
	redMediumMark->fill_rotate_angle = 15;
	redMediumMark->color = JGraph::Color(1, 0.2, 0.20);

	// Green Medium
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[6].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[6].curveColor = JGraph::Color(0, 0.2, 0);
	testgraph.curves[6].points = { };
	JGraph::ShapeMark* greenMediumMark = new JGraph::ShapeMark();
	greenMediumMark->type = JGraph::ShapeMark::Type::triangle;
	greenMediumMark->size = { .925/ 1.5, .925/ 1.5 };
	greenMediumMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	greenMediumMark->fill_rotate_angle = 30;
	greenMediumMark->color = JGraph::Color(0, 0.5, 0.17);

	// Blue Medium
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[7].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[7].curveColor = JGraph::Color(0, 0, 0.2);
	testgraph.curves[7].points = { };
	JGraph::ShapeMark* blueMediumMark = new JGraph::ShapeMark();
	blueMediumMark->type = JGraph::ShapeMark::Type::circle;
	blueMediumMark->size = { .850/ 1.5, .850/ 1.5 };
	blueMediumMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	blueMediumMark->color = JGraph::Color(0, 0.47, 0.7);

	// Purple Medium
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[8].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[8].curveColor = JGraph::Color(0.2, 0, 0.2);
	testgraph.curves[8].points = { };
	JGraph::ShapeMark* purpleMediumMark = new JGraph::ShapeMark();
	purpleMediumMark->type = JGraph::ShapeMark::Type::diamond;
	purpleMediumMark->size = { .925/ 1.5, .925/ 1.5 };
	purpleMediumMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	purpleMediumMark->color = JGraph::Color(0.9, 0, 0.75);

	// Yellow Medium
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[9].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[9].curveColor = JGraph::Color(0.2, 0.2, 0);
	testgraph.curves[9].points = { };
	JGraph::GeneralMark* yellowMediumMark = new JGraph::GeneralMark();
	yellowMediumMark->type = JGraph::GeneralMark::Type::general;
	yellowMediumMark->points = { {-1,-1}, {-0.5,0}, {-1,1}, {0,0.5}, {1,1}, {0.5,0}, {1,-1}, {0,-0.5} };
	yellowMediumMark->size = { .925/ 1.5, .925/ 1.5 };
	yellowMediumMark->pattern = JGraph::GeneralMark::FillPattern::solid;
	yellowMediumMark->color = JGraph::Color(1, 0.9, 0.0);

	/////////////// Large Shapes ///////////////

	// Red Large
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[10].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[10].curveColor = JGraph::Color(0.2, 0, 0);
	testgraph.curves[10].points = { };
	JGraph::GeneralMark* redLargeMark = new JGraph::GeneralMark();
	redLargeMark->type = JGraph::GeneralMark::Type::general;
	redLargeMark->points = { {-1,-0.25},{-1,0.25},{-0.25,0.25},{-0.25,1},{0.25,1},{0.25,0.25},{1,0.25},{1,-0.25},{0.25,-0.25},{0.25,-1},{-0.25,-1},{-0.25,-0.25} };
	redLargeMark->size = { .925, .925 };
	redLargeMark->pattern = JGraph::GeneralMark::FillPattern::solid;
	redLargeMark->fill_rotate_angle = 15;
	redLargeMark->color = JGraph::Color(1, 0.2, 0.20);

	// Green Large
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[11].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[11].curveColor = JGraph::Color(0, 0.2, 0);
	testgraph.curves[11].points = { };
	JGraph::ShapeMark* greenLargeMark = new JGraph::ShapeMark();
	greenLargeMark->type = JGraph::ShapeMark::Type::triangle;
	greenLargeMark->size = { .925, .925 };
	greenLargeMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	greenLargeMark->fill_rotate_angle = 30;
	greenLargeMark->color = JGraph::Color(0, 0.5, 0.17);

	// Blue Large
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[12].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[12].curveColor = JGraph::Color(0, 0, 0.2);
	testgraph.curves[12].points = { };
	JGraph::ShapeMark* blueLargeMark = new JGraph::ShapeMark();
	blueLargeMark->type = JGraph::ShapeMark::Type::circle;
	blueLargeMark->size = { .850, .850 };
	blueLargeMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	blueLargeMark->color = JGraph::Color(0, 0.47, 0.7);

	// Purple Large
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[13].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[13].curveColor = JGraph::Color(0.2, 0, 0.2);
	testgraph.curves[13].points = { };
	JGraph::ShapeMark* purpleLargeMark = new JGraph::ShapeMark();
	purpleLargeMark->type = JGraph::ShapeMark::Type::diamond;
	purpleLargeMark->size = { .925, .925 };
	purpleLargeMark->pattern = JGraph::ShapeMark::FillPattern::solid;
	purpleLargeMark->color = JGraph::Color(0.9, 0, 0.75);

	// Yellow Large
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[14].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[14].curveColor = JGraph::Color(0.2, 0.2, 0);
	testgraph.curves[14].points = { };
	JGraph::GeneralMark* yellowLargeMark = new JGraph::GeneralMark();
	yellowLargeMark->type = JGraph::GeneralMark::Type::general;
	yellowLargeMark->points = { {-1,-1}, {-0.5,0}, {-1,1}, {0,0.5}, {1,1}, {0.5,0}, {1,-1}, {0,-0.5} };
	yellowLargeMark->size = { .925, .925 };
	yellowLargeMark->pattern = JGraph::GeneralMark::FillPattern::solid;
	yellowLargeMark->color = JGraph::Color(1, 0.9, 0.0);


	/////////////////////////////////
	// White Space
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[15].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[15].curveColor = JGraph::Color(1, 1, 1);
	testgraph.curves[15].points = { {0,0}, {0,6}, {9,0}, {9,6} };
	JGraph::ShapeMark* whitespace = new JGraph::ShapeMark();
	whitespace->type = JGraph::ShapeMark::Type::box;
	whitespace->size = { 1.975, 1.975 };
	whitespace->pattern = JGraph::ShapeMark::FillPattern::solid;
	whitespace->color = JGraph::Color(1, 1, 1);

	// Score space
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[16].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[16].curveColor = JGraph::Color(0, 0, 0);
	testgraph.curves[16].points = { {0,6.5} };
	JGraph::GeneralMark* Scorespace = new JGraph::GeneralMark();
	Scorespace->type = JGraph::GeneralMark::Type::general;
	Scorespace->points = { {0,0},{0,5},{5,5},{3,0} };
	Scorespace->size = { 1.975, 1.975 };
	Scorespace->pattern = JGraph::GeneralMark::FillPattern::solid;
	Scorespace->color = JGraph::Color(0.8, 0.7, 1);

	// Turn Space
	testgraph.curves.push_back(JGraph::Curve());
	testgraph.curves[17].lineType = JGraph::Curve::LineType::none;
	testgraph.curves[17].curveColor = JGraph::Color(0, 0, 0);
	testgraph.curves[17].points = { {4.25,6.5} };
	JGraph::GeneralMark* TurnSpace = new JGraph::GeneralMark();
	TurnSpace->type = JGraph::GeneralMark::Type::general;
	TurnSpace->points = { {5,5},{0,5},{3,0},{5,0} };
	TurnSpace->size = { 1.975, 1.975 };
	TurnSpace->pattern = JGraph::GeneralMark::FillPattern::solid;
	TurnSpace->color = JGraph::Color(0.8, 0.7, 1);

	// Score text
	testgraph.curves.push_back(JGraph::Curve());
	JGraph::Curve& Scoretext = testgraph.curves.back();
	Scoretext.lineType = JGraph::Curve::LineType::none;
	Scoretext.points = { {1.5, 7} };
	JGraph::TextMark* textMark = new JGraph::TextMark();
	textMark->text.font = "Arial";
	textMark->text.size = 20;
	textMark->text.line_spacing = 20;
	textMark->text.content = "Score: \n";
	textMark->text.content += to_string(score);

	// Text showing turn count
	testgraph.curves.push_back(JGraph::Curve());
	JGraph::Curve& Turntext = testgraph.curves.back();
	Turntext.lineType = JGraph::Curve::LineType::none;
	Turntext.points = { {8, 7} };
	JGraph::TextMark* turnTextMark = new JGraph::TextMark();
	turnTextMark->text.font = "Arial";
	turnTextMark->text.size = 20;
	turnTextMark->text.line_spacing = 20;
	turnTextMark->text.content = "Turns: \n";
	turnTextMark->text.content += to_string(numTurns);
	
	// add points to each curve based on their tile type and size
	for (int i = 0; i < BOARD_LEN; i++) {
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			switch (board[i][j].type) {
			case Tile::TileType::red: testgraph.curves[0 + board[i][j].size * 5].points.push_back({ i+0.5F,(BOARD_HEIGHT-1 - j) + 0.5F });
				break;
			case Tile::TileType::green: testgraph.curves[1 + board[i][j].size * 5].points.push_back({ i + 0.5F,(BOARD_HEIGHT-1 - j) + 0.5F });
				break;
			case Tile::TileType::blue: testgraph.curves[2 + board[i][j].size * 5].points.push_back({ i + 0.5F,(BOARD_HEIGHT-1 - j) + 0.5F });;
				break;
			case Tile::TileType::purple: testgraph.curves[3 + board[i][j].size * 5].points.push_back({ i + 0.5F,(BOARD_HEIGHT-1 - j) + 0.5F });
				break;
			case Tile::TileType::yellow: testgraph.curves[4 + board[i][j].size * 5].points.push_back({ i + 0.5F,(BOARD_HEIGHT-1 -j) + 0.5F });
				break;
			default: break;
			}
		}
	}

	// add ALL THE CURVES to their unique_ptr child of Marks
	testgraph.curves[0].marks.reset(redSmallMark);
	testgraph.curves[1].marks.reset(greenSmallMark);
	testgraph.curves[2].marks.reset(blueSmallMark);
	testgraph.curves[3].marks.reset(purpleSmallMark);
	testgraph.curves[4].marks.reset(yellowSmallMark);
	testgraph.curves[5].marks.reset(redMediumMark);
	testgraph.curves[6].marks.reset(greenMediumMark);
	testgraph.curves[7].marks.reset(blueMediumMark);
	testgraph.curves[8].marks.reset(purpleMediumMark);
	testgraph.curves[9].marks.reset(yellowMediumMark);
	testgraph.curves[10].marks.reset(redLargeMark);
	testgraph.curves[11].marks.reset(greenLargeMark);
	testgraph.curves[12].marks.reset(blueLargeMark);
	testgraph.curves[13].marks.reset(purpleLargeMark);
	testgraph.curves[14].marks.reset(yellowLargeMark);
	testgraph.curves[15].marks.reset(whitespace);
	testgraph.curves[16].marks.reset(Scorespace);
	testgraph.curves[17].marks.reset(TurnSpace);
	Scoretext.marks.reset(textMark);
	Turntext.marks.reset(turnTextMark);

	// Convert to JPG
	JGraph::jgraphToJPG(testcanvas, "gameOutput.jpg");
}

// Main
int main(int argc, char* argv[]) {
	bool saveGame = false;
	bool loadedGame = false;

	// Check incoming call flags
	// Read file in, if it exists
	if (argc != 1 && argc != 3) {
		cout << "Provided " << argc << " arguments..." << endl;
		cout << "Usage: ./puzzleGame [-s fileName]" << endl
			<< "-s fileName --- Use Saved Board from fileName Location" << endl;
		return -1;
	}

	// If file flag is set, handle it
	else if (argc == 3 && string(argv[1]) == "-s") {
		file = string(argv[2]);
		int status = gameRead(string(argv[2]));
		if (status == 2) {
			cout << "Error reading file; Invalid savefile syntax." << endl;
			return 1;
		}
		else if (status == 1) {
			loadedGame = false;
		}
		else {
			loadedGame = true;
		}
		saveGame = true;
	}
	
	// If the game is not loaded
	if (!loadedGame) {
		gameInit();
	}

	// pre-draw board in case it didn't exist, so users can see what's going on
	drawBoard();

	// Endless loop, broken by "quit" (saves) or Ctrl-C (won't save)
	while (true) {
		vector<JGraph::Point<int>> moves;
		string playerMoves;

		moves.clear();
		// Acquire Player Move or choice
		cout << "Provide next move in the format: " << endl
			<< "{(x0,y0),(x1,y1),(x2,y2)....}" << endl
			<< "To exit, type quit or Quit" << endl;
	
		getline(cin,playerMoves);

		// Keywords "quit" and "Quit" will exit the program and properly save if needed.
		if (playerMoves == "quit" || playerMoves == "Quit") {
			if (saveGame) gameSave(file);
			return 0;
		}
		
		// Remove any white space within the input
		playerMoves.erase(remove_if(playerMoves.begin(), playerMoves.end(),
			::isspace), playerMoves.end());

		// Process the move into proper format

		// Check overall format and length
		if (playerMoves.length() < 7 || playerMoves.length() % 6 != 1 || playerMoves[0] != '{' || playerMoves[playerMoves.length()-1] != '}') {
			cout << "Format of move is incorrect. Try again. " << endl;
			continue;
		}

		// Process point 1 and set as base:
		string baseText = playerMoves.substr(1, 6);
		Tile::TileType moveType;

		// Parenthesis check
		if (baseText[0] != '(' || baseText[4] != ')') {
			cout << "Format of move is incorrect. Try again. " << endl;
			continue;
		}

		int x = baseText[1] - '0';
		int y = baseText[3] - '0';
		
		// Comma check
		if (baseText[2] != ',') {
			cout << "Format of move is incorrect. Try again. " << endl;
			continue;
		}

		// Valid coordinates check
		if (!(x >= 0 && x < BOARD_LEN && y >= 0 && y < BOARD_HEIGHT)) {
			cout << "Format of move is incorrect. Try again. " << endl;
			continue;
		}

		// Add this to the move
		moves.push_back({ x,y });
		moveType = board[x][y].type;
		if (moveType == Tile::TileType::BLOCKED) {
			cout << "Format of move is incorrect. Try again. " << endl;
			continue;
		}

		// Can probably become a regex in a future version... works for now.
		// Splice other points into proper point format
		bool validInput = true;
		for (int i = 1; (i * 6 + 1) < playerMoves.length(); i++) {
			string pointText = playerMoves.substr(i * 6 + 1, 6);
			
			// Parenthesis check
			if (pointText[0] != '(' || pointText[4] != ')') {
				cout << "Format of move is incorrect. Try again. " << endl;
				validInput = false;
				break;
			}

			int x = pointText[1] - '0';
			int y = pointText[3] - '0';
			
			// Comma check
			if (pointText[2] != ',') {
				cout << "Format of move is incorrect. Try again. " << endl;
				validInput = false;
				break;
			}

			// Valid coordinates check
			if (!(x >= 0 && x < BOARD_LEN && y >= 0 && y < BOARD_HEIGHT)) {
				cout << "Format of move is incorrect. Try again. " << endl;
				validInput = false;
				break;
			}

			moves.push_back({ x,y });

			// Ensure distance to last one is sufficient
			if ((abs(moves[i].x - moves[i-1].x) > 1 || abs(moves[i].y -  moves[i-1].y) > 1)) {
				cout << "Move " << i + 1 << " not adjacent tiles. Cannot do move." << endl;
				validInput = false;
				break;
			}

			// Check color
			if (board[moves[i].x][moves[i].y].type != moveType) {
				cout << "Move " << i + 1 << " not same type. Cannot do move." << endl;
				validInput = false;
				break;
			}



			// After move-trail validity check;
			//lastLocation = moves[i];

		}

		// Force reentry of move if not valid input.
		if (!validInput) continue;

		// Moves check
		if (moves.size() < 3) {
			cout << "Moves should be 3+ tiles." << endl;
			continue;
		}

		// Game Logic/Procedures
		gameProcedure(moves);

		// Out of turns, game over.
		if (numTurns == 0) {
			gameFinish(saveGame, file);
			cout << "Game over! Score : " << score << endl;
			return 0;
		}

		// Draw updated board
		drawBoard();

	}
	
}