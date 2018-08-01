#include "minesweeper_oo.h"

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC
////////////////////////////////////////////////////////////////////////////////
/**
  Sets the game up and starts the play loop
*/
void Minesweeper::play() {
  setup();
  quit_ = false;
  win_ = false;
  lose_ = false;
  updateDisplay();
  loop();
}


////////////////////////////////////////////////////////////////////////////////
/// PROTECTED
////////////////////////////////////////////////////////////////////////////////
/**
  Reveals a cell in the game board. Updates lose_ to stop the game if the
    player reveals a bomb

  @param r the row number to reveal
  @param c the column number to reveal
*/
void Minesweeper::reveal(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_REVEAL_ << endl;
  else {
    board_[r][c].isRevealed = true;
    if (board_[r][c].isBomb) {
      lose_ = true;
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
    board_[r][c].display = 'X';
    board_[r][c].markedBomb = true;
  }
}

/**
  Marks a cell in the board as a '?'

  @param r the row number to mark
  @param c the column number to mark
*/
void Minesweeper::markUnsure(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_QMARK_ << endl;
  else {
    board_[r][c].display = unsureDisplay_;
    board_[r][c].markedUnsure = true;
  }
}

/**
  Returns just the display values of the current game board, not the bomb
    locations or other secret information

  @return the display values of the current game board
*/
vector<vector<char>> Minesweeper::getBoard() {
  vector<vector<char>> boardDisplay;
  for (auto &row : board_) {
    vector<char> rowDisplay;
    for (auto &cell : row) {
      rowDisplay.push_back(cell.display);
    }
    boardDisplay.push_back(rowDisplay);
  }
  return boardDisplay;
}

/**
  Displays the current game board with labels on and secret off
*/
void Minesweeper::displayBoard() {
  displayBoard(true, 0);
}

// getters and setters for board display characters
void Minesweeper::setHiddenDisplay(char c) {
  hiddenDisplay_ = c;
}

char Minesweeper::getHiddenDisplay() {
  return hiddenDisplay_;
}

void Minesweeper::setBombDisplay(char c) {
  bombDisplay_ = c;
}

char Minesweeper::getBombdisplay() {
  return bombDisplay_;
}

void Minesweeper::setUnsureDisplay(char c) {
  unsureDisplay_ = c;
}

char Minesweeper::getUnsureDisplay() {
  return unsureDisplay_;
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
    case 'B':
    case 'b':
      cout << "Starting game with beginner difficulty" << endl;
      s_ = {9, 9, 10};
      success = true;
      break;
    case 'I':
    case 'i':
      cout << "Starting game with intermediate difficulty" << endl;
      s_ = {16, 16, 40};
      success = true;
      break;
    case 'A':
    case 'a':
      cout << "Starting game with advanced difficulty" << endl;
      s_ = {16, 30, 99};
      success = true;
      break;
    case 'C':
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
           << cols << " cols, " << bombs << " bombs)" << endl;
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
  // Main game loop - while a bomb isn't revealed and user doesn't quit
  do {
    displayBoard();
    runCommand();
    updateDisplay();
    checkWin();
  } while (!quit_ && !win_ && !lose_);

  // Display board one more time
  displayBoard(true, 1);

  // Output game over or game won message
  if (win_) cout << GAME_WON_ << endl;
  else if (lose_) cout << GAME_OVER_ << endl;
  else if (quit_) cout << QUIT_MSG_ << endl;
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

void Minesweeper::displayHelp() {
  cout << "\nAvailable commands: "
       "\n  r row col - reveals the cell at (row,col)"
       "\n  " << bombDisplay_ << " row col - marks the cell at (row,col) as a bomb"
       "\n  " << unsureDisplay_ << " row col - marks the cell at (row,col) as a question mark"
       "\n  [h]elp    - display the command list"
       "\n  [q]uit    - end the game and close the program\n";
}

/**
  Prompts the user to enter a command and processes their response
*/
void Minesweeper::runCommand() {
  char cmd;
  int row = -1, col = -1;

  // all this input validation feels really awkward, can't think of a cleaner
  // way to do it
  do {
    cout << "Enter a command: ";
    string line;
    getline(cin, line);
    istringstream iss(line);
    iss >> cmd >> row >> col;

    // convert cmd to lowercase
    cmd = tolower(cmd);

    // if command is valid
    if (cmd == 'h') displayHelp();
    else if (cmd == 'q') {
      quit_ = true;
      return;
    }
    else if (cmd == 'r' || cmd == tolower(bombDisplay_) || cmd == tolower(unsureDisplay_)) {
      if (row < 1 || col < 1 || row > s_.rows || col > s_.cols)
        cout << ERR_OUT_OF_BOUNDS_ << endl;
      else if (cmd == 'r') {
        reveal(row - 1, col - 1);
        return;
      }
      else if (cmd == tolower(bombDisplay_)) {
        markBomb(row - 1, col - 1);
        return;
      }
      else if (cmd == tolower(unsureDisplay_)) {
        markUnsure(row - 1, col - 1);
        return;
      }
    }
    else cout << ERR_INVALID_COMMAND_ << endl;
  } while (true);
}

/**
  Recursively expand around a cell if it has no adjacent bombs

  @param r the row of the cell to expand at
  @param c the column of the cell to expand at
*/
void Minesweeper::expand(int r, int c) {
  for (int rOffset = -1; rOffset <= 1; rOffset++) {
    for (int cOffset = -1; cOffset <= 1; cOffset++) {
      if (rOffset == 0 && cOffset == 0) continue;
      int newR = r + rOffset;
      int newC = c + cOffset;
      if (newR < 0 || newR > s_.rows - 1 || newC < 0 || newC > s_.cols - 1)
        continue;
      else if (!board_[newR][newC].isRevealed && !board_[newR][newC].isBomb) {
        board_[newR][newC].isRevealed = true;
        if (board_[newR][newC].bombsBordering == 0) expand(newR, newC);
      }
    }
  }
}

/**
  Check if the player has won. Update win_.
*/
void Minesweeper::checkWin() {
  for (int r = 0; r < s_.rows; r++) {
    for (int c = 0; c < s_.cols; c++) {
      if (!board_[r][c].isBomb && !board_[r][c].isRevealed) {
        win_ = false;
        return;
      }
    }
  }
  win_ = true;
}

/**
  Update the display characters in the board_ corresponding to whether or not
    they have been revealed
*/
void Minesweeper::updateDisplay() {
  for (int r = 0; r < s_.rows; r++) {
    for (int c = 0; c < s_.cols; c++) {

      if (board_[r][c].isRevealed) {
        if (board_[r][c].isBomb) {

          board_[r][c].display = bombDisplay_;
          board_[r][c].bombsBordering = 0;
        }
        else if (board_[r][c].bombsBordering != 0)
          board_[r][c].display = '0' + board_[r][c].bombsBordering;
        else if (board_[r][c].bombsBordering == 0)
          board_[r][c].display = ' ';
      }
      else if (board_[r][c].markedBomb) board_[r][c].display = bombDisplay_;
      else if (board_[r][c].markedUnsure) board_[r][c].display = unsureDisplay_;
      else board_[r][c].display = hiddenDisplay_;
    }
  }
}

/**
  Displays the game board

  @param label whether or not to show row/column labels and borders on display
  @param secret whether or not to show secret information (bomb locations)
*/
void Minesweeper::displayBoard(bool label, int secret) {
  cout << endl;
  for (int r = 0; r < s_.rows; r++) {
    if (label && r == 0) {
      // column numbers
      if (s_.cols >= 10) {
        cout << "     ";
        for (int c = 0; c < s_.cols; c++) {
          if ((c + 1) % 10 == 0) cout << (c + 1) / 10 << " ";
          else cout << "  ";
        }
      }
      cout << endl;
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
      if ((secret == 1 || secret == 3) && board_[r][c].isBomb) cout << "X ";
      else if (secret == 2) cout << board_[r][c].bombsBordering << " ";
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