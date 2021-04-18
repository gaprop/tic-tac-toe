use std::env;
use std::fmt;
use std::ops;

#[derive(Clone)]
enum Mark {
    X,
    O,
}

enum Player<'a> {
    Player(&'a str),
}

type Cell = Option<Mark>;

struct Board {
    board: Vec<Vec<Cell>>,
    size: (usize, usize),
}

impl fmt::Display for Mark {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Mark::X => write!(f, "X"),
            Mark::O => write!(f, "O"),
        }
    }
}

impl Board {
    fn new((x, y): (usize, usize)) -> Board {
        let row = vec![None; y];
        let board = vec![row; x];
        Board {
            board,
            size: (x, y),
        }
    }

    fn place(&mut self, mark: Mark, x: usize, y: usize) {
        self.board[x][y] = Some(mark);
    }

    fn get(&self, x: usize, y: usize) -> &Cell {
        &self.board[x][y]
    }

    fn render(&self) {
        let board = &self.board;
        let (x, y) = &self.size;

        // Create a top row with the index to that column
        print!("   ");
        for i in 0..*x {
            print!("|{} ", i);
        }
        print!("|");
        print!("\n");

        print!("----");
        for i in 0..*x {
            print!("---");
        }
        print!("\n");

        // Print the board
        for row in 0..*y {
            print!("|{} ", row);
            for col in 0..*x {
                let mark = &board[col][row];
                match mark {
                    None => print!("|  "),
                    Some(mark) => print!("|{} ", mark),
                }
            }
            print!("|");
            print!("\n");

            // Print underline
            print!("----");
            for _ in 0..*x {
                print!("---");
            }

            print!("\n");
        }
    }
}

struct State<'a> {
    players: Vec<Player<'a>>,
    board: Board,
}

fn main() {
    let x = env::args().nth(1).expect("Needs a horizontal size");
    let y = env::args().nth(2).expect("Needs a horizontal size");
    let players = vec!["mig", "ham"];
    let (x, y) = (x.parse::<usize>().unwrap(), y.parse::<usize>().unwrap());

    let state = setup((x, y), players);
    // run(state);
}

fn setup((x, y): (usize, usize), players: Vec<&str>) -> State {
    let board = Board::new((x, y));
    let players = players.iter().map(|p| Player::Player(p)).collect();
    State {
        board,
        players,
    }
}

// fn run(state: State) {
    // let winner = None;
    // while let None = winner {
        // let player = state.players.pop();
    // }
// }

fn has_winner(board: &Board) -> &Option<Mark> {

    let get_winner_down = |x_range: ops::Range<usize>, y_range: ops::Range<usize>| -> &Option<Mark> {
        for i in x_range {
            let mut winner: &Option<Mark> = &None;
            for j in y_range {
                let cell = board.get(i, j);
                match (winner, cell) {
                    (None, Some(_))    => winner = cell,
                    (Some(_), Some(_)) => continue,
                    _ => {
                            winner = &None;
                            break;
                         },
                }
            }
            if winner.is_some() {
                return &winner;
            }
        }
        &None
    };

    let (x, y) = board.size;

    let winner = get_winner_down(0..x, 0..y);
    if let Some(_) = winner {
        return &winner;
    }

    let winner = get_winner_down(0..y, 0..x);
    if let Some(_) = winner {
        return &winner;
    }

    let winner = get_winner_down(0..x, 0..x);
    if let Some(_) = winner {
        return &winner;
    }

    let winner = get_winner_down(x..0, x..0);
    if let Some(_) = winner {
        return &winner;
    }

    return &None
}
