#include "minesweeper_oo.h"

Minesweeper::Minesweeper() {
  setup();
  play_ = true;
  gameOver_ = false;
  loop();
}

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

void Minesweeper::displayBoard() {
  cout << endl;
  if (label_) {
    // Column labels
    cout << "    ";
    for (int i = 0; i < board_[0].size() - 2; i++) {
      cout << setw(2) << (i + 1) % 10;
    }
    cout << endl;
  }

  // Board contents
  for (int r = 0; r < board_.size(); r++) {
    if (label_) {
      // Row label
      if (r > 0 && r < board_.size() - 1)
        cout << setw(2) << r << " ";
      else cout << "   ";
    }

    for (int c = 0; c < board_[0].size(); c++) {
      if (r == 0 || c == 0 || r == board_.size() - 1 || c == board_[0].size() - 1) {
        cout << borderChar(r, c);
      }
      else {
        if (secret_) {
          if (board_[r][c].isBomb) cout << "X ";
          else cout << board_[r][c].bombsBordering << " ";
        } else {
          cout << board_[r][c].display << " ";
        }
      }
    }
    cout << endl;
  }
}

void Minesweeper::reveal(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_REVEAL_ << endl;
  else {
    board_[r][c].isRevealed = true;
    if (board_[r][c].isBomb)
      gameOver_ = true;
    else if (board_[r][c].bombsBordering == 0)
      expand(r, c);
  }
}

void Minesweeper::expand(int r, int c) {
  // Loop through surrounding
  for (int rOffset = -1; rOffset <= 1; rOffset++) {
    for (int cOffset = -1; cOffset <= 1; cOffset++) {
      int newR = r + rOffset;
      int newC = c + cOffset;
      if (newR == 0 || newR == board_.size() - 1 || newC == 0 || newC == board_[0].size() - 1)
        continue;
      if (!board_[newR][newC].isRevealed && !board_[newR][newC].isBomb) {
        board_[r + rOffset][c + cOffset].isRevealed = true;
        if (board_[newR][newC].bombsBordering == 0)
          // Recursive call
          expand(newR, newC);

      }
    }
  }
}


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

      cout << "Starting game with custom difficulty (" << rows << " rows, " << cols << " cols, " << bombs << " bombs)";
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

  // setup the board by randomly placing bombs and calculating bombs adjacent to each cell
  randomBombs();
  calculateBombsBordering();
}

void Minesweeper::markBomb(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_BOMB_ << endl;
  else {
    board_[r][c].display = "X";
    board_[r][c].markedBomb = true;
  }
}

void Minesweeper::markQuestionMark(int r, int c) {
  if (board_[r][c].isRevealed)
    cout << ERR_QMARK_ << endl;
  else
    board_[r][c].display = "?";
}

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

void Minesweeper::updateDisplay() {
  for (int r = 0; r < board_.size(); r++) {
    for (int c = 0; c < board_[0].size(); c++) {

      if (board_[r][c].isRevealed) {
        if (board_[r][c].isBomb) {

          board_[r][c].display = "X";
          board_[r][c].bombsBordering = 0;
        }
        else if (board_[r][c].bombsBordering != 0) board_[r][c].display = std::to_string(board_[r][c].bombsBordering);
        else if (board_[r][c].bombsBordering == 0) board_[r][c].display = " ";
      }
    }
  }
}

bool Minesweeper::gameWon() {
  for (int r = 1; r < board_.size() - 1; r++) {
    for (int c = 1; c < board_[0].size() - 1; c++) {
      if (!board_[r][c].isBomb && !board_[r][c].isRevealed) {
        return false;
      }
    }
  }
  return true;
}

void Minesweeper::loop() {
  bool valid_command = false;
  bool play = true;

  // Main game loop - while a bomb isn't revealed and user doesn't quit
  do {
    displayBoard();
    displayBoard();

    // Input loop - while input is invalid
    do {
      valid_command = runCommand();
    } while (!valid_command);

    // Update the cell display values
    updateDisplay();
  } while (!gameOver_ && play_ && !gameWon());

  // Display board one more time as long as they didn't quit
  if (play)
    displayBoard();

  // Output game over or game won message
  if (gameOver_)
    cout << GAME_OVER_ << endl;
  else if (gameWon())
    cout << GAME_WON_ << endl;
}


vector<vector<Cell>> Minesweeper::getBoard() {
  return board_;
}