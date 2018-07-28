#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>

using std::cin;
using std::getline;
using std::istringstream;
using std::cout;
using std::endl;
using std::setw;
using std::string;
using std::to_string;
using std::vector;

/**
 * Holds information about a cell in the game board
 */
struct Cell {
	bool isRevealed = false;
	string display = "*";
	bool isBomb = false;
	bool markedBomb = false;
	int bombsBordering = 0;
};

/**
 * Holds information about settings (more to be added later)
 */
struct Settings {
	int rows;
	int cols;
	int bombs;
};

class Minesweeper {
private:
	vector<vector<Cell>> board_;
	Settings s_;

	bool label_ = true;
	bool secret_ = false;

	bool gameOver_;
	bool play_;

	void updateDisplay();
	bool gameWon();
	string borderChar(int, int);
	void randomBombs();
	void calculateBombsBordering();
	void expand(int, int);
	void markBomb(int, int);
	void markQuestionMark(int, int);
	bool runCommand();
	void loop();

	const int MIN_ROWS_ = 1;
	const int MAX_ROWS_ = 100;
	const int MIN_COLS_ = 1;
	const int MAX_COLS_ = 100;
	const int MIN_BOMBS_ = 0;
	const string MENU_ = "----- WELCOME TO COMMAND LINE MINESWEEPER -----\n"
	                     "Please choose a difficulty level [Rows x Cols, # bombs]:\n"
	                     " (b)eginner      [9x9, 10]\n"
	                     " (i)ntermediate  [16x16, 40]\n"
	                     " (a)dvanced      [16x30, 99]\n"
	                     " (c)ustom\n"
	                     "Enter your choice: ";
	const string HELP_ = "\nAvailable commands: "
	                     "\n  r row col - reveals the cell at (row,col)"
	                     "\n  x row col - marks the cell at (row,col) as a bomb"
	                     "\n  ? row col - marks the cell at (row,col) as a question mark"
	                     "\n  [h]elp    - display the command list"
	                     "\n  [q]uit    - end the game and close the program";
	const string TOP_LEFT_BORDER_ = "┌";
	const string TOP_RIGHT_BORDER_ = "┐";
	const string BOTTOM_LEFT_BORDER_ = "└";
	const string BOTTOM_RIGHT_BORDER_ = "┘";
	const string HORIZONTAL_BORDER_ = "─";
	const string VERTICAL_BORDER_ = "│";
	const string ERR_INVALID_COMMAND_ = "That is not a valid command.";
	const string ERR_OUT_OF_BOUNDS_ = "Please enter a row and column within the board dimensions.";
	const string ERR_REVEAL_ = "That cell has already been revealed, you cannot reveal it again.";
	const string ERR_BOMB_ = "That cell has already been revealed, you cannot mark it as a bomb.";
	const string ERR_QMARK_ = "That cell has already been revealed, you cannot mark it as a question mark.";
	const string ERR_ROWS_MIN_ = "Number of rows must be greater than " + to_string(MIN_ROWS_) + ".";
	const string ERR_ROWS_MAX_ = "Number of rows must be less than " + to_string(MAX_ROWS_) + ".";
	const string ERR_COLS_MIN_ = "Number of columns must be greater than " + to_string(MIN_COLS_) + ".";
	const string ERR_COLS_MAX_ = "Number of columns must be less than " + to_string(MAX_COLS_) + ".";
	const string ERR_BOMBS_MIN_ = "Number of bombs must be greater than " + to_string(MIN_BOMBS_) + ".";
	const string ERR_BOMBS_MAX_ = "Number of bombs must be less than ROWS * COLS.";
	const string GAME_OVER_ = "You revealed a bomb, game over!";
	const string GAME_WON_ = "Congratulations, you won!";

protected:
	void reveal(int, int);

public:
	Minesweeper();
	void displayBoard();
	void setup();
	vector<vector<Cell>> getBoard();
};