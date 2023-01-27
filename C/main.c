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

void init_ncurse();
Cursor init_board(WINDOW *board[SIZE][SIZE], int max_y, int max_x);
void render_board(Cursor cursor, WINDOW *board[SIZE][SIZE], enum choice cells[SIZE][SIZE]);
void set_cell(int row, int col, enum choice choice, enum choice cells[SIZE][SIZE]);
enum choice get_winner(enum choice cells[SIZE][SIZE]);

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
      /* WINDOW* cell = board[i][j]; */

      board[i][j] = newwin(3, 5, (max_y / 2) + (i * 3), (max_x / 2) + (j * 5));
      box(board[i][j], 0, 0);

      // FIXME: Do not think these two lines are necessary
      /* mvwaddch(board[i][j], 1, 2, cells[i][j]); */
      /* wrefresh(board[i][j]); */
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

// Make better
enum choice get_winner(enum choice cells[SIZE][SIZE]) {

  // First check the rows and see if they are all the same choice
  int O_row_eq = 0;
  int O_col_eq = 0;

  int X_row_eq = 0;
  int X_col_eq = 0;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (cells[i][j] == O) O_row_eq++;
      if (cells[j][i] == O) O_col_eq++;
      if (cells[i][j] == X) X_row_eq++;
      if (cells[j][i] == X) X_col_eq++;
    }

    if (O_row_eq == SIZE) return O;
    if (O_col_eq == SIZE) return O;
    if (X_row_eq == SIZE) return X;
    if (X_col_eq == SIZE) return X;

    O_row_eq = 0;
    O_col_eq = 0;
    X_row_eq = 0;
    X_col_eq = 0;
  }

  // Afterwards check across the board and see if they are equal
  int O_lcross_eq = 0;
  int O_rcross_eq = 0;
  int X_lcross_eq = 0;
  int X_rcross_eq = 0;

  for (int i = 0; i < SIZE; i++) {
    if (cells[i][i] == O) O_lcross_eq++;
    if (cells[i][2 - i] == O) O_rcross_eq++;
    if (cells[i][i] == X) X_lcross_eq++;
    if (cells[i][2 - i] == X) X_rcross_eq++;
  }

  if (O_lcross_eq == SIZE) return O;
  if (O_rcross_eq == SIZE) return O;
  if (X_lcross_eq == SIZE) return X;
  if (X_rcross_eq == SIZE) return X;

  // In any other case return Nothing
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
