#include "minesweeper_oo.h"

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
  m.setHiddenDisplay('a');
  m.setBombDisplay('b');
  m.setUnsureDisplay('u');
  m.play();
}