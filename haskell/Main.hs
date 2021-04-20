import Data.List

data Mark = X | O deriving(Eq)
-- data Position = N | NE | E | SE | S | SW | W | NW | Center
data Cell = Cell (Maybe Mark)
data Board = Board [[Cell]]
data Winner = Winner(Mark)

data State = State { board :: Board
                   , turn  :: Mark
                   , input :: String
                   }


-- Utilities
mapi :: (Int -> a -> b) -> [a] -> [b]
mapi f = map (\(i, x) -> f i x) . zip [0..]

-- All the tic tac toe stuff
showMark :: Mark -> Char
showMark X = 'X'
showMark O = 'O'

readMark :: Char -> Mark
readMark 'X' = X
readMark 'O' = O

showCell :: Cell -> Char
showCell (Cell (Just c)) = showMark c
showCell (Cell Nothing) = ' '

readCell :: Char -> Cell
readCell ' ' = Cell $ Nothing
readCell m = Cell . Just . readMark $ m

insertWalls :: String -> String
insertWalls s = unlines $ take (length walled - 1) walled
  where 
    len = 2 * (length . head $ lines s) - 1
    walled :: [String]
    walled = foldl (\acc x -> acc ++ [x, (take len $ repeat '-')]) [] sided

    sided :: [String]
    sided = map (foldr1 (++) . mapi wall) $ lines s

    wall :: Int -> Char -> String
    wall 0 x = [x]
    wall _ x = "|" ++ [x]

showBoard :: Board -> String
showBoard (Board cells) = unlines $ map (map showCell) cells

readBoard :: String -> Board
readBoard b = Board $ map (map readCell) $ lines b

initBoard :: Board
initBoard = readBoard . unlines $ ["   "
                                  ,"   "
                                  ,"   "]

hasWon :: Board -> Bool

-- All IO stuff and main game stuff
-- main = play

-- play = play' $ State initBoard X ""

