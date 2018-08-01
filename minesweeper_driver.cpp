#include "minesweeper.h"

void display2D(vector<vector<char>> b) {
  for (auto &r : b) {
    for (auto &c : r) {
      cout << c << " ";
    }
    cout << endl;
  }
}

int main() {
  Minesweeper m;
  m.play();
}