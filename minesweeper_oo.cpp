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
  board_.resize(s_.rows + 2);
  for (auto &r : board_) r.resize(s_.cols + 2);

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
    int randomRow = rand() % (s_.rows) + 1;
    int randomCol = rand() % (s_.cols) + 1;

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
  for (int r = 1; r < board_.size() - 1; r++) {
    for (int c = 1; c < board_[0].size() - 1; c++) {
      if (board_[r][c].isBomb) {
        board_[r][c].bombsBordering = 0;
        continue;
      }
      for (int rOffset = -1; rOffset <= 1; rOffset++) {
        for (int cOffset = -1; cOffset <= 1; cOffset++) {
          if (board_[r + rOffset][c + cOffset].isBomb) {
            board_[r][c].bombsBordering++;
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
    if (row < 1 || col < 1 || row > board_.size() || col > board_[0].size()) {
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
    reveal(row, col);
    return true;
  case 'x':
    markBomb(row, col);
    return true;
  case '?':
    markQuestionMark(row, col);
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
  // Loop through surrounding
  for (int rOffset = -1; rOffset <= 1; rOffset++) {
    for (int cOffset = -1; cOffset <= 1; cOffset++) {
      int newR = r + rOffset;
      int newC = c + cOffset;
      if (newR == 0 || newR == board_.size() - 1 || newC == 0
          || newC == board_[0].size() - 1) continue;
      if (!board_[newR][newC].isRevealed && !board_[newR][newC].isBomb) {
        board_[r + rOffset][c + cOffset].isRevealed = true;
        if (board_[newR][newC].bombsBordering == 0)
          // Recursive call
          expand(newR, newC);

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
  Find what character to display for border positions in the board (WILL BE
    UPDATED IN THE FUTURE - board_ should not contain positions for border, will
    instead be displayed separately)

  @param r the row position to display
  @param c the column position to display
  @return the character to display for the border of the game board
*/
string Minesweeper::borderChar(int r, int c) {
  if (r == 0) {
    if (c == 0)
      return TOP_LEFT_BORDER_ + HORIZONTAL_BORDER_;
    else if (c == board_[0].size() - 1)
      return TOP_RIGHT_BORDER_;
    else
      return HORIZONTAL_BORDER_ + HORIZONTAL_BORDER_;
  }
  else if (r == board_.size() - 1) {
    if (c == 0)
      return BOTTOM_LEFT_BORDER_ + HORIZONTAL_BORDER_;
    else if (c == board_[0].size() - 1)
      return BOTTOM_RIGHT_BORDER_;
    else
      return HORIZONTAL_BORDER_ + HORIZONTAL_BORDER_;
  }
  else if (c == 0 || c == board_[0].size() - 1)
    return VERTICAL_BORDER_ + " ";
}

/**
  Displays the game board

  @param label whether or not to show row/column labels on output
  @param secret whether or not to show secret information (bomb locations)
*/
void Minesweeper::displayBoard(bool label, bool secret) {
  cout << endl;
  if (label) {
    // Column labels
    cout << "    ";
    for (int i = 0; i < board_[0].size() - 2; i++) {
      cout << setw(2) << (i + 1) % 10;
    }
    cout << endl;
  }

  // Board contents
  for (int r = 0; r < board_.size(); r++) {
    if (label) {
      // Row label
      if (r > 0 && r < board_.size() - 1)
        cout << setw(2) << r << " ";
      else cout << "   ";
    }

    for (int c = 0; c < board_[0].size(); c++) {
      if (secret && board_[r][c].isBomb) cout << "X ";
      else if (r == 0 || c == 0 || r == board_.size() - 1
               || c == board_[0].size() - 1) {
        cout << borderChar(r, c);
      } else cout << board_[r][c].display << " ";
    }
  }
  cout << endl;
}