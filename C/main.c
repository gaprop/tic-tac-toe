#include <ncurses.h>
#include <stdlib.h>

#define SIZE 3

enum choice {
  X = 'X',
  O = 'O',
  Nothing = ' ',
};

typedef struct Cursors {
  int x;
  int y;
} Cursor;

enum side {
  Left,
  Right,
};

typedef struct ArrayWrapper {
  enum choice arr[SIZE];
} ArrayWrapper;

void init_ncurse() {
  initscr();
  clear();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);
  refresh();
}

Cursor init_board(WINDOW *board[SIZE][SIZE], int max_y, int max_x) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j ++) {
      board[i][j] = newwin(3, 5, (max_y / 2) + (i * 3), (max_x / 2) + (j * 5));
      box(board[i][j], 0, 0);
    }
  }

  Cursor cursor;
  cursor.x = 0;
  cursor.y = 0;
  return cursor;
}

void render_board(Cursor cursor, WINDOW *board[SIZE][SIZE], enum choice cells[SIZE][SIZE]) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j ++) {
      if ((i == cursor.y) && (j == cursor.x)) 
        wattron(board[i][j], A_REVERSE);

      mvwaddch(board[i][j], 1, 2, cells[i][j]);
      wrefresh(board[i][j]);

      wattroff(board[i][j], A_REVERSE);

      wrefresh(board[i][j]);
    }
  }
}

void set_cell(int row, int col, enum choice choice, enum choice cells[SIZE][SIZE]) {
  cells[row][col] = choice;
}

ArrayWrapper get_row(int row, enum choice cells[SIZE][SIZE]) {
  ArrayWrapper ret;
  for (int i = 0; i < SIZE; i++) {
    ret.arr[i] = cells[row][i];
  }
  return ret;
}

ArrayWrapper get_col(int col, enum choice cells[SIZE][SIZE]) {
  ArrayWrapper ret;
  for (int i = 0; i < SIZE; i++) {
    ret.arr[i] = cells[i][col];
  }
  return ret;
}

ArrayWrapper get_cross(enum side side, enum choice cells[SIZE][SIZE]) {
  ArrayWrapper ret;
  if (side == Left) {
    for (int i = 0; i < SIZE; i++) {
      ret.arr[i] = cells[i][i];
    }
  } else {
    for (int i = 0; i < SIZE; i++) {
      ret.arr[i] = cells[i][2 - i];
    }
  }
  return ret;
}

enum choice winner(ArrayWrapper arr) {
  int o_eq = 0;
  int x_eq = 0;

  for (int i = 0; i < SIZE; i++) {
    if (arr.arr[i] == O) o_eq++;
    if (arr.arr[i] == X) x_eq++;
  }

  if (o_eq == 3) return O;
  else if (x_eq == 3) return X;
  else return Nothing;
}

enum choice get_winner(enum choice cells[SIZE][SIZE]) {
  for (int i = 0; i < SIZE; i++) {
    ArrayWrapper row = get_row(i, cells);
    ArrayWrapper col = get_row(i, cells);

    enum choice row_win = winner(row);
    enum choice col_win = winner(col);

    if (row_win != Nothing) return row_win;
    if (col_win != Nothing) return col_win;
  }

  ArrayWrapper left_side = get_cross(Left, cells);
  ArrayWrapper right_side = get_cross(Right, cells);

  enum choice left_win = winner(left_side);
  enum choice right_win = winner(right_side);

  if (left_win != Nothing) return left_win;
  if (right_win != Nothing) return right_win;

  return Nothing;
}

int main() {

  int max_x = 0;
  int max_y = 0;
  int c;
  WINDOW *board[SIZE][SIZE];
  enum choice winner = Nothing;
  enum choice player = X;
  enum choice cells[SIZE][SIZE] = {Nothing, Nothing, Nothing,
                             Nothing, Nothing, Nothing,
                             Nothing, Nothing, Nothing};

  init_ncurse();
  getmaxyx(stdscr, max_y, max_x);

  Cursor cursor = init_board(board, max_y, max_x);

  while (winner == Nothing) {
    render_board(cursor, board, cells);

    mvprintw(max_y / 2 + 3 * 4, max_x / 2, "It is %c's turn", player);

    c = getch();
    switch(c) {
      case 'q':
        endwin();
        return 0;
      case KEY_UP:
        if (cursor.y <= 0)
          cursor.y = 2;
        else
          cursor.y--;
        break;

      case KEY_DOWN:
        if (cursor.y >= 2)
          cursor.y = 0;
        else
          cursor.y++;
        break;

      case KEY_LEFT:
        if (cursor.x <= 0)
          cursor.x = 2;
        else
          cursor.x--;
        break;

      case KEY_RIGHT:
        if (cursor.x >= 2)
          cursor.x = 0;
        else
          cursor.x++;
        break;

      case ' ':
        set_cell(cursor.y, cursor.x, player, cells);
        player = (player == X) ? O : X;
        break;
    }
    winner = get_winner(cells);
  }
  // We have to render the board one last time in case there is a winner, because otherwise the last choice will not be there.
  render_board(cursor, board, cells);
  mvprintw(max_y / 2 + 3 * 4, max_x / 2, "The winner is: %c", winner);
  mvprintw(max_y / 2 + 3 * 4 + 1, max_x / 2, "Press any key to quit");
  getch();
  endwin();
}
