use std::env;
use std::fmt;
use std::ops;
use std::io;

#[derive(Clone, Eq, PartialEq, Copy)]
enum Mark {
    X,
    O,
}

struct Player<'a> {
    name: &'a str,
    mark: Mark,
}

impl Player<'_> {
    fn new(name: &str, mark: Mark) -> Player {
        Player {
            name,
            mark,
        }
    }
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

    fn place(&mut self, x: usize, y: usize, mark: Mark) {
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
    let players = vec!["mig", "x", "ham", "o"];
    let (x, y) = (x.parse::<usize>().unwrap(), y.parse::<usize>().unwrap());

    let state = setup((x, y), players);
    run(state);
}

fn setup((x, y): (usize, usize), mut players: Vec<&str>) -> State {
    let board = Board::new((x, y));
    let mut iter_players = players.iter();
    let mut players = Vec::new();
    while let Some(p) = iter_players.next() {
        let name = p;
        let mark = match iter_players.next() {
            Some(&"x") => Mark::X,
            Some(&"o") => Mark::O,
            _          => panic!("not a mark"),
        };

        players.push(Player::new(name, mark));
    }
    State {
        board,
        players,
    }
}

fn run(state: State) {
    let mut winner: Option<Player> = None;
    let mut players = state.players;
    let mut board = state.board;
    while let None = winner {
        // Stop if there is a winner
        if let Some(m) = get_winner(&board) {
            while let Some(player) = players.pop() {
                if player.mark == *m {
                    winner = Some(player);
                    break;
                }
            }
        }

        let player = players.remove(0);
        board.render();
        println!("It is {}'s turn, who is {}", player.name, player.mark);

        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();
        let mut input = input.chars();

        match input.next() {
            Some('h') => println!("Help should be here"),
            Some('p') => board.render(),
            Some('m') => {
                input.next();
                let x = input.next().unwrap().to_digit(10).unwrap();
                input.next();
                let y = input.next().unwrap().to_digit(10).unwrap();

                board.place(x as usize, y as usize, player.mark);
                players.push(player);
                continue;
            }
            _ => (),
        }
        players.insert(0, player);
    }
    let winner = winner.unwrap();
    println!("The winner is: {}, who where: {}", winner.name, winner.mark);
}

fn get_winner(board: &Board) -> &Option<Mark> {
    let (x, y) = board.size;
    for i in 0..x {
        let mut winner: &Option<Mark> = &None;
        for j in 0..y {
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

        for j in 0..y {
            let cell = board.get(j, i);
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

        for _ in x..0 {
            let cell = board.get(i, i);
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

        for j in x-1..0 {
            let cell = board.get(j, j);
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
}
