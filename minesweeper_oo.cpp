#include "minesweeper_oo.h"

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC
////////////////////////////////////////////////////////////////////////////////
/**
  Sets the game up and starts the play loop
*/
void Minesweeper::play() {
  setup();
  play_ = true;
  win_ = false;
  loop();
}


////////////////////////////////////////////////////////////////////////////////
/// PROTECTED
////////////////////////////////////////////////////////////////////////////////
/**
  Reveals a cell in the game board. Updates win_ and play_ to stop the game if
    the player reveals a bomb

  @param r the row number to reveal
  @param c the column number to reveal
*/
void Minesweeper::reveal(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_REVEAL_ << endl;
  else {
    board_[r][c].isRevealed = true;
    if (board_[r][c].isBomb) {
      win_ = false;
      play_ = false;
    }
    else if (board_[r][c].bombsBordering == 0)
      expand(r, c);
  }
}

/**
  Marks a cell in the board as a bomb
  @param r the row number to mark
  @param c the column number to mark
*/
void Minesweeper::markBomb(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_BOMB_ << endl;
  else {
    board_[r][c].display = "X";
    board_[r][c].markedBomb = true;
  }
}

/**
  Marks a cell in the board as a '?'

  @param r the row number to mark
  @param c the column number to mark
*/
void Minesweeper::markQuestionMark(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_QMARK_ << endl;
  else
    board_[r][c].display = "?";
}

/**
  Returns the current game board

  @return the current game board
*/
Board Minesweeper::getBoard() {
  return board_;
}

/**
  Displays the current game board with labels on and secret off
*/
void Minesweeper::displayBoard() {
  displayBoard(true, false);
}


////////////////////////////////////////////////////////////////////////////////
/// PRIVATE
////////////////////////////////////////////////////////////////////////////////
/**
  Prompts the user to choose game type (num rows, num columns, num bombs), then
    sets up the board
*/
void Minesweeper::setup() {
  cout << MENU_;
  char choice;
  bool success;
  string line;
  do {
    getline(cin, line);
    istringstream iss(line);
    iss >> choice;
    switch (choice) {
    case 'b':
      cout << "Starting game with beginner difficulty" << endl;
      s_ = {9, 9, 10};
      success = true;
      break;
    case 'i':
      cout << "Starting game with intermediate difficulty" << endl;
      s_ = {16, 16, 40};
      success = true;
      break;
    case 'a':
      cout << "Starting game with advanced difficulty" << endl;
      s_ = {16, 30, 99};
      success = true;
      break;
    case 'c': {
      int rows, cols, bombs;
      bool rowsValid = false;
      bool colsValid = false;
      bool bombsValid = false;

      do {
        cout << "Enter [rows cols bombs]: ";
        getline(cin, line);
        istringstream iss2(line);
        iss2 >> rows >> cols >> bombs;

        if (rows < MIN_ROWS_) cout << ERR_ROWS_MIN_ << endl;
        else if (rows > MAX_ROWS_) cout << ERR_ROWS_MAX_ << endl;
        else rowsValid = true;

        if (cols < MIN_COLS_) cout << ERR_COLS_MIN_ << endl;
        else if (cols > MAX_COLS_) cout << ERR_COLS_MAX_ << endl;
        else colsValid = true;

        if (bombs < MIN_BOMBS_) cout << ERR_BOMBS_MIN_ << endl;
        else if (bombs > rows * cols) cout << ERR_BOMBS_MAX_ << endl;
        else bombsValid = true;
      } while (!(rowsValid && colsValid && bombsValid));

      cout << "Starting game with custom difficulty (" << rows << " rows, "
           << cols << " cols, " << bombs << " bombs)";
      s_ = {rows, cols, bombs};
      success = true;
      break;
    }
    default:
      cout << "Not a valid choice, choose again: ";
      success = false;
    }
  } while (!success);

  // resize board_ to have rows_ rows and cols_ cols
  board_.resize(s_.rows);
  for (auto &r : board_) r.resize(s_.cols);

  // setup the board - randomly place bombs, calculating bombs adjacent to cells
  randomBombs();
  calculateBombsBordering();
}

/**
  The game loop. Prompts user to enter commands and checks for win condition.
*/
void Minesweeper::loop() {
  bool valid_command = false;

  // Main game loop - while a bomb isn't revealed and user doesn't quit
  do {
    displayBoard();

    // Input loop - while input is invalid
    do {
      valid_command = runCommand();
    } while (!valid_command);

    // Update the cell display values
    updateDisplay();
    checkWin();
  } while (play_);

  // Display board one more time
  displayBoard(true, true);

  // Output game over or game won message
  if (win_)
    cout << GAME_WON_ << endl;
  else
    cout << GAME_OVER_ << endl;
}

/**
  Randomly places bombs in the bord according to how many were specified in the
    settings
*/
void Minesweeper::randomBombs() {
  srand((uint) time(nullptr));

  int count = 0;
  while (count < s_.bombs) {
    int randomRow = rand() % (s_.rows);
    int randomCol = rand() % (s_.cols);

    if (!board_[randomRow][randomCol].isBomb) {
      board_[randomRow][randomCol].isBomb = true;
      count++;
    }
  }
}

/**
  Calculates and updates the number of bombs adjacent to each cell in the board
*/
void Minesweeper::calculateBombsBordering() {
  for (int r = 0; r < s_.rows; r++) {
    for (int c = 0; c < s_.cols; c++) {

      if (!board_[r][c].isBomb) {
        for (int rOffset = -1; rOffset <= 1; rOffset++) {
          for (int cOffset = -1; cOffset <= 1; cOffset++) {
            int newR = r + rOffset;
            int newC = c + cOffset;
            if (newR < 0 || newR > s_.rows - 1 || newC < 0
                || newC > s_.cols - 1) continue;
            else if (board_[r + rOffset][c + cOffset].isBomb) {
              board_[r][c].bombsBordering++;
            }
          }
        }
      }
    }
  }
}

/**
  Prompts the user to enter a command and processes their response

  @return true if a command was successfully run, false otherwise
*/
bool Minesweeper::runCommand() {
  char cmd;
  int row = -1, col = -1;

  cout << "Enter a command: ";
  string line;
  getline(cin, line);
  istringstream iss(line);
  iss >> cmd >> row >> col;

  if (!(cmd == 'h' || cmd == 'q')) {
    if (row < 1 || col < 1 || row > s_.rows || col > s_.cols) {
      cout << ERR_OUT_OF_BOUNDS_ << endl;
      return false;
    }
  }

  switch (cmd) {
  case 'h':
    cout << HELP_;
    return false;
  case 'q':
    cout << "Game quit" << endl;
    play_ = false;
    return true;
  case 'r':
    reveal(row - 1, col - 1);
    return true;
  case 'x':
    markBomb(row - 1, col - 1);
    return true;
  case '?':
    markQuestionMark(row - 1, col - 1);
    return true;
  default:
    cout << ERR_INVALID_COMMAND_ << endl;
    return false;
  }
}

/**
  Recursively expand around a cell if it has no adjacent bombs

  @param r the row of the cell to expand at
  @param c the column of the cell to expand at
*/
void Minesweeper::expand(int r, int c) {
  printf("expand(%d, %d)\n", r + 1, c + 1);
  for (int rOffset = -1; rOffset <= 1; rOffset++) {
    for (int cOffset = -1; cOffset <= 1; cOffset++) {
      if (rOffset == 0 && cOffset == 0) continue;
      int newR = r + rOffset;
      int newC = c + cOffset;
      if (newR < 0 || newR > s_.rows - 1 || newC < 0 || newC > s_.cols - 1)
        continue;
      else if (!board_[newR][newC].isRevealed && !board_[newR][newC].isBomb) {
        board_[newR][newC].isRevealed = true;
        printf("  (%d, %d) revealed\n", newR + 1, newC + 1);
        if (board_[newR][newC].bombsBordering == 0) expand(newR, newC);
      }
    }
  }
}

/**
  Check if the player has won. Update win_ and play_.
*/
void Minesweeper::checkWin() {
  for (int r = 1; r < board_.size() - 1; r++) {
    for (int c = 1; c < board_[0].size() - 1; c++) {
      if (!board_[r][c].isBomb && !board_[r][c].isRevealed) {
        win_ = false;
        return;
      }
    }
  }
  win_ = true;
  play_ = false;
}

/**
  Update the display characters in the board_ corresponding to whether or not
    they have been revealed
*/
void Minesweeper::updateDisplay() {
  for (int r = 0; r < board_.size(); r++) {
    for (int c = 0; c < board_[0].size(); c++) {

      if (board_[r][c].isRevealed) {
        if (board_[r][c].isBomb) {

          board_[r][c].display = "X";
          board_[r][c].bombsBordering = 0;
        }
        else if (board_[r][c].bombsBordering != 0)
          board_[r][c].display = std::to_string(board_[r][c].bombsBordering);
        else if (board_[r][c].bombsBordering == 0)
          board_[r][c].display = " ";
      }
    }
  }
}

/**
  Displays the game board

  @param label whether or not to show row/column labels and borders on display
  @param secret whether or not to show secret information (bomb locations)
*/
void Minesweeper::displayBoard(bool label, bool secret) {
  cout << endl;
  for (int r = 0; r < s_.rows; r++) {
    if (label && r == 0) {
      // column numbers
      cout << "     ";
      for (int c = 0; c < s_.cols; c++) {
        cout << (c + 1) % 10 << " ";
      }
      cout << endl;

      // top border
      cout << "   ";
      for (int c = 0; c < s_.cols; c++) {
        if (c == 0)
          cout << TOP_LEFT_BORDER_ << HORIZ_BORDER_ << HORIZ_BORDER_;
        else if (c < s_.cols - 1)
          cout << HORIZ_BORDER_ << HORIZ_BORDER_;
        else
          cout << HORIZ_BORDER_ << HORIZ_BORDER_ << HORIZ_BORDER_
               << TOP_RIGHT_BORDER_ << endl;
      }

    }
    // row labels
    cout << setw(2) << (r + 1) % 100 << " " << VERT_BORDER_ << " ";

    // cells
    for (int c = 0; c < s_.cols; c++) {
      if (secret && board_[r][c].isBomb) cout << "X ";
      // else if (secret) cout << board_[r][c].bombsBordering << " ";
      else cout << board_[r][c].display << " ";
    }

    cout << VERT_BORDER_;

    if (label && r == s_.rows - 1) {
      cout << endl << "   ";
      for (int c = 0; c < s_.cols; c++) {
        if (c == 0)
          cout << BOTTOM_LEFT_BORDER_ << HORIZ_BORDER_ << HORIZ_BORDER_;
        else if (c < s_.cols - 1)
          cout << HORIZ_BORDER_ << HORIZ_BORDER_;
        else
          cout << HORIZ_BORDER_ << HORIZ_BORDER_ << HORIZ_BORDER_
               << BOTTOM_RIGHT_BORDER_;
      }
    }
    cout << endl;
  }
}