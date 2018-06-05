//
// Created by zachs on 6/3/2018.
//

#ifndef MINESWEEPER_MINESWEEPER_H
#define MINESWEEPER_MINESWEEPER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>

using std::tuple;
using std::cin;
using std::istringstream;
using std::getline;
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::setw;
using std::string;

// Shows bomb locations for debugging purposes
const bool SECRET = false;
// Shows row and column labels on all display functions
const bool LABEL = true;

const int NUM_ROWS = 16;
const int NUM_COLS = 16;
const int NUM_BOMBS = 40;

const string TOP_LEFT_BORDER = "┌";
const string TOP_RIGHT_BORDER = "┐";
const string BOTTOM_LEFT_BORDER = "└";
const string BOTTOM_RIGHT_BORDER = "┘";
const string HORIZONTAL_BORDER = "-";
const string VERTICAL_BORDER = "│";

const string ERR_INVALID_COMMAND = "That is not a valid command.";
const string ERR_OUT_OF_BOUNDS = "Please enter a row and column within the board dimensions.";
const string ERR_REVEAL = "That cell has already been revealed, you cannot reveal it again.";
const string ERR_BOMB = "That cell has already been revealed, you cannot mark it as a bomb.";
const string ERR_QMARK = "That cell has already been revealed, you cannot mark it as a question mark.";
const string GAME_OVER = "GAME OVER";
const string GAME_WON = "Congratulations, you have marked all the bombs!";

const string MENU = "----- WELCOME TO COMMAND LINE MINESWEEPER -----\n"
    "Please choose a difficulty level [Rows x Cols, # bombs]:\n"
    " (b)eginner      [9x9, 10]\n"
    " (i)ntermediate  [16x16, 40]\n"
    " (a)dvanced      [16x30, 99]\n"
    " (c)ustom\n";

struct Cell {
  bool isRevealed = false;
  string display = "*";
  bool isBomb = false;
  bool markedBomb = false;
  int bombsBordering = 0;
};

struct Settings {
  int rows;
  int cols;
  int bombs;
};

/**
 * Finds which character to use for the border of the cell
 *
 * @param r The row of the cell
 * @param c The column of the cell
 * @param rows The size of the game board
 * @return A string containing the character for the border cell (r,c)
 */
string borderChar(int r, int c, int rows, int cols) {
  if (r == 0) {
    if (c == 0)
      return TOP_LEFT_BORDER + HORIZONTAL_BORDER;
    else if (c == cols - 1)
      return TOP_RIGHT_BORDER;
    else
      return HORIZONTAL_BORDER + HORIZONTAL_BORDER;
  }
  else if (r == rows - 1) {
    if (c == 0)
      return BOTTOM_LEFT_BORDER + HORIZONTAL_BORDER;
    else if (c == cols - 1)
      return BOTTOM_RIGHT_BORDER;
    else
      return HORIZONTAL_BORDER + HORIZONTAL_BORDER;
  }
  else if (c == 0 || c == cols - 1)
    return VERTICAL_BORDER + " ";
}

/**
 * Displays the board game for play. Doesn't show cell values unless revealed.
 *
 * @param board The game board to be displayed
 * @param label Whether or not to display row and column labels
 */
void displayPlay(vector<vector<Cell>> board, bool label) {
  if (label) {
    // Column labels
    cout << "    ";
    for (int i = 0; i < board[0].size() - 2; i++) {
      cout << setw(2) << (i + 1) % 10;
    }
    cout << endl;
  }

  // Board contents
  for (int r = 0; r < board.size(); r++) {
    if (label) {
      // Row label
      if (r > 0 && r < board.size() - 1)
        cout << setw(2) << r << " ";
      else cout << "   ";
    }

    for (int c = 0; c < board[0].size(); c++) {
      if (r == 0 || c == 0 || r == board.size() - 1 || c == board[0].size() - 1) {
        cout << borderChar(r, c, board.size(), board[0].size());
      }
      else {
        cout << board[r][c].display << " ";
      }
    }
    cout << endl;
  }
}

/**
 * Displays the board game for play. Shows bomb locations and cell values for debugging purposes.
 *
 * @param board The game board to be displayed
 * @param label Whether or not to display row and column labels
 */
void displaySecret(vector<vector<Cell>> board, bool label) {

  if (label) {
    // Column labels
    cout << "    ";
    for (int i = 0; i < board[0].size() - 2; i++) {
      cout << setw(2) << (i + 1) % 10;
    }
    cout << endl;
  }

  for (int r = 0; r < board.size(); r++) {

    if (label) {
      // Row label
      if (r > 0 && r < board.size() - 1)
        cout << setw(2) << r << " ";
      else cout << "   ";
    }

    for (int c = 0; c < board[0].size(); c++) {
      if (r == 0 || c == 0 || r == board.size() - 1 || c == board.size() - 1) {
        cout << borderChar(r, c, board.size(), board[0].size());
      }
      else {
        Cell cell = board[r][c];
        if (cell.isBomb) cout << "X ";
        else cout << cell.bombsBordering << " ";
      }
    }
    cout << endl;
  }
  cout << endl;
}

/**
 * Updates the display fields of the game board
 *
 * @param board The current game board
 */
void updateDisplay(vector<vector<Cell>> &board) {
  for (int r = 0; r < board.size(); r++) {
    for (int c = 0; c < board[0].size(); c++) {

      if (board[r][c].isRevealed) {
        if (board[r][c].isBomb) {

          board[r][c].display = "X";
          board[r][c].bombsBordering = 0;
        }
        else if (board[r][c].bombsBordering != 0) board[r][c].display = std::to_string(board[r][c].bombsBordering);
        else if (board[r][c].bombsBordering == 0) board[r][c].display = " ";
      }
    }
  }
}

/**
 * Randomizes the location of bombs to setup the game board.
 *
 * @param board The current game board
 * @param numBombs The number of bombs to be randomly placed on the game board
 */
void randomBombs(vector<vector<Cell>> &board, int numBombs) {
  srand((uint) time(nullptr));
  for (int count = 0; count < numBombs; count++) {
    int randomRow = rand() % (board.size() - 2) + 1;
    int randomCol = rand() % (board[0].size() - 2) + 1;
    cout << randomRow << ", " << randomCol << endl;
    board[randomRow][randomCol].isBomb = true;
  }
}

/**
 * Calculates the number of bombs adjacent to each cell in the game board
 * @param board The current game board
 */
void calculateBombsBordering(vector<vector<Cell>> &board) {
  for (int r = 1; r < board.size() - 1; r++) {
    for (int c = 1; c < board[0].size() - 1; c++) {
      if (board[r][c].isBomb) {
        board[r][c].bombsBordering = 0;
        continue;
      }
      for (int rOffset = -1; rOffset <= 1; rOffset++) {
        for (int cOffset = -1; cOffset <= 1; cOffset++) {
          if (board[r + rOffset][c + cOffset].isBomb) {
            board[r][c].bombsBordering++;
          }
        }
      }
    }
  }
}

/**
 * Given a row and a column, recursively reveals all cells adjacent to it.
 *
 * @param board The current game board
 * @param r The row of the starting cell
 * @param c The column of the starting cell
 * @return The current game board, returned for recursive purposes
 */
vector<vector<Cell>> expand(vector<vector<Cell>> board, int r, int c) {
  // Loop through surrounding
  for (int rOffset = -1; rOffset <= 1; rOffset++) {
    for (int cOffset = -1; cOffset <= 1; cOffset++) {
      int newR = r + rOffset;
      int newC = c + cOffset;
      if (newR == 0 || newR == board.size() - 1 || newC == 0 || newC == board[0].size() - 1)
        continue;
      if (!board[newR][newC].isRevealed && !board[newR][newC].isBomb) {
        board[r + rOffset][c + cOffset].isRevealed = true;
        if (board[newR][newC].bombsBordering == 0)
          // Recursive call
          board = expand(board, newR, newC);

      }
    }
  }
  return board;
}

/**
 * Reveals a cell in the game board
 *
 * @param board The current game board
 * @param r The row of the cell to be revealed
 * @param c The column of the cell to be revealed
 * @return True if a bomb is revealed to stop the game in main
 *         False if a bomb is not revealed
 */
bool reveal(vector<vector<Cell>> &board, int r, int c) {
  if (board[r][c].isRevealed)
    cout << ERR_REVEAL << endl;
  else {
    board[r][c].isRevealed = true;
    if (board[r][c].isBomb) {
      return true;
    }
    else {
      if (board[r][c].bombsBordering == 0) {
        board = expand(board, r, c);
        return false;
      }
    }
  }
}

/**
 * Marks a cell with an "X" to denote it as a cell the player thinks has a bomb
 *
 * @param board The current game board
 * @param r The row of the cell to mark as a bomb
 * @param c The column of the cell to mark as a bomb
 * @return True if the cell was successfully marked as a bomb
 *         False if the cell was already revealed
 */
bool markBomb(vector<vector<Cell>> &board, int r, int c) {
  if (board[r][c].isRevealed) {
    cout << ERR_BOMB << endl;
    return false;
  }
  else {
    board[r][c].display = "X";
    board[r][c].markedBomb = true;
    return true;
  }
}

/**
 * Marks a cell with a "?" to denote it as a cell the player is unsure about
 *
 * @param board The current game board
 * @param r The row of the cell to mark as a bomb
 * @param c The column of the cell to mark as a bomb
 * @return True if the cell was successfully marked as a question mark
 *         False if the cell was already revealed
 */
bool markQuestionMark(vector<vector<Cell>> &board, int r, int c) {
  if (board[r][c].isRevealed) {
    cout << ERR_QMARK << endl;
    return false;
  }
  else {
    board[r][c].display = "?";
    return true;
  }
}

/**
 * Displays the game board and possibly the secret game board
 *
 * @param board The current game board
 * @param secret Whether or not the secret game board is displayed
 * @param label Wehter or not to display row and column labels with the game board
 */
void displayBoard(const vector<vector<Cell>> &board, bool secret, bool label) {
  cout << endl;
  if (secret)
    displaySecret(board, label);
  displayPlay(board, label);
}

/**
 * Displays the command menu
 */
void displayHelp() {
  cout << endl
       << "Available commands: " << endl
       << "  r row col - reveals the cell at (row,col)" << endl
       << "  x row col - marks the cell at (row,col) as a bomb" << endl
       << "  ? row col - marks the cell at (row,col) as a question mark" << endl
       << "  [h]elp    - display the command list" << endl
       << "  [q]uit    - end the game and close the program" << endl
       << endl;
}

/**
 * Drives the input for the game. Prompts the user to enter commands and calls the appropriate functions
 * to play the game.
 *
 * @param board The current game board
 * @param play A boolean value for whether or not the user wants to continue playing the game
 * @param bombRevealed A boolean value for whether or not a bomb was revealed by the user, ending the game if true
 * @return True if the user entered an acceptable command, false otherwise
 */
bool runCommand(vector<vector<Cell>> &board, bool &play, bool &bombRevealed) {
  char cmd;
  int row = -1, col = -1;

  cout << "Enter a command: ";
  cin >> cmd;

  if (!(cmd == 'h' || cmd == 'q')) {
    cin >> row;
    cin >> col;

    if (row < 1 || col < 1 || row > board.size() || col > board[0].size()) {
      cout << ERR_OUT_OF_BOUNDS << endl;
      return false;
    }
  }

  switch (cmd) {
    case 'h':displayHelp();
      return false;
    case 'q':cout << "Game quit" << endl;
      play = false;
      return true;
    case 'r':bombRevealed = reveal(board, row, col);
      return true;
    case 'x':markBomb(board, row, col);
      return true;
    case '?':markQuestionMark(board, row, col);
      return true;
    default:cout << ERR_INVALID_COMMAND << endl;
      return false;
  }
}

/**
 * A function to check whether or not the player has won the game
 *
 * @param board The current game board
 * @return True if the player has marked all bombs
 */
bool gameWon(vector<vector<Cell>> board) {
  for (int r = 0; r < board.size(); r++) {
    for (int c = 0; c < board[0].size(); c++) {
      if (board[r][c].isBomb && !board[r][c].markedBomb) {
        return false;
      }
    }
  }
  return true;
}

/**
 * Sets up the game board with the chosen difficulty (size and bombs)
 *
 * @return The game board with set size and randomly placed bombs
 */
vector<vector<Cell>> setup() {
  cout << MENU;
  cout << "Enter your choice: ";
  char choice;
  bool success;
  Settings s {};
  do {
    cin >> choice;
    switch (choice) {
      case 'b':
        cout << "Starting game with beginner difficulty" << endl;
        s = {9, 9, 10};
        success = true;
        break;
      case 'i':
        cout << "Starting game with intermediate difficulty" << endl;
        s = {16, 16, 40};
        success = true;
        break;
      case 'a':
        cout << "Starting game with advanced difficulty" << endl;
        s = {16, 30, 99};
        success = true;
        break;
      case 'c':
        int rows, cols, bombs;
        cout << "Enter [rows cols bombs]: ";
        cin >> rows >> cols >> bombs;
        s = {rows, cols, bombs};
        success = true;
        break;
      default:
        cout << "Not a valid choice, choose again: ";
        success = false;
    }
  } while (!success);

  // The game board
  vector<vector<Cell>> board(s.rows + 2, vector<Cell>(s.cols + 2));

  // Set up the board by placing random bombs and calculating the bombs adjacent to each cell
  randomBombs(board, s.bombs);
  calculateBombsBordering(board);

  return board;

}

/**
 * The main game loop to display the board, request input, and play the game
 *
 * @param board The gameboard created by setup()
 */
void loop(vector<vector<Cell>> board) {

  // Boolean value for whether or not the user entered an acceptable command
  bool success;

  // Boolean values for whether or not a bomb is revealed and whether or not the user wants to continue to play
  bool bombRevealed = false;
  bool play = true;

  // Main game loop. Continues to loop while a bomb isn't revealed and user wants to continue
  do {
    displayBoard(board, SECRET, LABEL);

    // Input loop. Continues to loop while the user enters invalid input
    do {
      success = runCommand(board, play, bombRevealed);
    } while (!success);

    // Update the cell display values
    updateDisplay(board);
  } while (!bombRevealed && play && !gameWon(board));

  // Display board one more time as long as they didn't quit
  if (play)
    displayBoard(board, SECRET, LABEL);

  // Output game over or game won message
  if (bombRevealed)
    cout << GAME_OVER << endl;
  else if (gameWon(board))
    cout << GAME_WON << endl;
}

/**
 * Starts and plays the game by calling setup() and loop()
 */
void play() {
  vector<vector<Cell>> board = setup();
  loop(board);
}

#endif //MINESWEEPER_MINESWEEPER_H
