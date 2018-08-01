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
using std::find;
using std::vector;

struct Cell {
  bool isRevealed = false;
  char display = '*';
  bool isBomb = false;
  bool markedBomb = false;
  bool markedUnsure = false;
  int bombsBordering = 0;
};

struct Settings {
  int rows;
  int cols;
  int bombs;
};

using Board = vector<vector<Cell>>;

class Minesweeper {
public:
  void play();

  void setHiddenDisplay(char);
  char getHiddenDisplay();
  void setBombDisplay(char);
  char getBombdisplay();
  void setUnsureDisplay(char);
  char getUnsureDisplay();

// to be used for a minesweeper solver that will inherit from this class
protected:
  void setup();
  void reveal(int, int);
  void markBomb(int, int);
  void markUnsure(int, int);
  vector<vector<char>> getBoard();
  void displayBoard();

private:
  Board board_;
  Settings s_;

  bool quit_;
  bool win_;
  bool lose_;

  char hiddenDisplay_ = '*';
  char bombDisplay_ = 'x';
  char unsureDisplay_ = '?';

  void loop();
  void randomBombs();
  void calculateBombsBordering();
  void displayHelp();
  void runCommand();
  void expand(int, int);
  void checkWin();
  void updateDisplay();
  void displayBoard(bool, int);

  // const variables inherent to how the game plays
  const int MIN_ROWS_ = 1;
  const int MAX_ROWS_ = 100;
  const int MIN_COLS_ = 1;
  const int MAX_COLS_ = 100;
  const int MIN_BOMBS_ = 0;
  const string MENU_ = "----- WELCOME TO COMMAND LINE MINESWEEPER -----\n"
                       "Please choose a difficulty level [Rows x Cols, # bombs]"
                       ":\n"
                       " (b)eginner      [9x9, 10]\n"
                       " (i)ntermediate  [16x16, 40]\n"
                       " (a)dvanced      [16x30, 99]\n"
                       " (c)ustom\n"
                       "Enter your choice: ";

  const string QUIT_MSG_ = "Game quit.";
  const string TOP_LEFT_BORDER_ = "┌";
  const string TOP_RIGHT_BORDER_ = "┐";
  const string BOTTOM_LEFT_BORDER_ = "└";
  const string BOTTOM_RIGHT_BORDER_ = "┘";
  const string HORIZ_BORDER_ = "─";
  const string VERT_BORDER_ = "│";
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
};