# ChessEngine
Development of a Chess Engine in C++ inclusive of game logic (wiht special moves), interactive gameplay and engine with a custom evaluation function. 

## Usage 
### Setup (MacOS)
1) CMake, GCC, GoogleTest
```shell
brew install gcc cmake
git clone https://github.com/DevrajK721/ChessEngine.git
cd ChessEngine
git clone https://github.com/google/googletest.git
```

2) Build project
```shell
mkdir build && cd build
cmake .. 
cmake --build .
```

3) Run CTest to confirm environment and validate functions 
```
ctest --output-on-failure
```

Should get output of something like:
```shell
Test project /Users/devrajkatkoria/Documents/ChessEngine/build
    Start 1: EnvSanityCheck
1/4 Test #1: EnvSanityCheck ...................   Passed    0.01 sec
    Start 2: BoardInitialization
2/4 Test #2: BoardInitialization ..............   Passed    0.15 sec
    Start 3: MoveGeneration
3/4 Test #3: MoveGeneration ...................   Passed    0.14 sec
    Start 4: Engine
4/4 Test #4: Engine ...........................   Passed    0.12 sec

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   0.43 sec
```

4) If no errors;
```shell
./ChessEngine
```

`./ChessEngine` will bring up the interactive game for two players or a user to play chess against the engine. 

### Gameplay
To interact with the pieces, we use standard chess notation in the form of start square and destination square, for example `e2e4`. 

If you want the engine to find a move for you, simply type `ai` (same goes for if you want to play it as an opponent).

### Tunable Parameters
The Chess Engine can be further tuned and a lot of `engine.cpp` is intuitively alterable.

#### Piece-Square Tables 
```cpp
static const std::array<int,64> pawnTable = {
      0,   0,   0,   0,   0,   0,   0,   0,
     78,  83,  86,  73, 102,  82,  85,  90,
      7,  29,  21,  44,  40,  31,  44,   7,
    -17,  16,  -2,  15,  14,   0,  15, -13,
    -26,   3,  10,   9,   6,   1,   0, -23,
    -22,   9,   5, -11, -10,  -2,   3, -19,
    -31,   8,  -7, -37, -36, -14,   3, -31,
      0,   0,   0,   0,   0,   0,   0,   0
};
```

#### Piece Values
```cpp
static const int pieceValue[6] = {
    100,   // pawn
    280,   // knight
    320,   // bishop
    479,   // rook
    929,   // queen
    60000  // king (very high to discourage losing it)
};
```

#### Depth 
```cpp
// src/main.cpp
...
        if (input == "ai") {
            auto res = Engine::search(board, N); // Choose N to be your desired depth (CAREFUL, every increase in depth exponentially increases the runtime for the engine), I have tested up to N=5
...
```

### Bug Reports
I have hopefully fixed most of the bugs to do with gameplay logic but if the engine plays an illegal move or doesn't allow you to play a legal move, please submit a PR adding a test case in the following form:
```cpp
// tests/movegen_test.cpp
/*
All previous tests in the file
...
*/
TEST(MoveGen, CaseToTest) {
    init_attacks();
    Board b; b.init_startpos();
    std::vector<std::string> seq = {"....", "...."}; // Sequence of moves to play up until you get to the issue
    for(const auto &mv : seq){
        auto legal = generate_legal_moves(b);
        ASSERT_TRUE(contains_move(legal,mv,b)) << "Illegal move in sequence: " << mv;
        Move m = parse_move(mv,b);
        for(const auto &l : legal){
            if(l.from==m.from && l.to==m.to && l.promotion==m.promotion && l.isCastling==m.isCastling && l.isEnPassant==m.isEnPassant){
                m = l; break;
            }
        }
        make_move(b,m);
    }
    auto legal = generate_legal_moves(b);
    EXPECT_FALSE(contains_move(legal,"movetocheck",b)); // Change EXPECT_FALSE to EXPECT_TRUE if you expect a move to be legal but it isn't
}
```

For example, 
```cpp
TEST(MoveGen, IllegalMoveInCheck) {
    init_attacks();
    Board b; b.init_startpos();
    std::vector<std::string> seq = {
        "a2a4","e7e5","b2b4","d8h4","d2d4","d7d6",
        "c2c4","b8c6","g2g3","h4e4","e2e3","e4h1",
        "f2f4","h1g1","d1e2","e5d4","e2f2","g1f2",
        "e1f2","d4e3"
    };
    for(const auto &mv : seq){
        auto legal = generate_legal_moves(b);
        ASSERT_TRUE(contains_move(legal,mv,b)) << "Illegal move in sequence: " << mv;
        Move m = parse_move(mv,b);
        for(const auto &l : legal){
            if(l.from==m.from && l.to==m.to && l.promotion==m.promotion && l.isCastling==m.isCastling && l.isEnPassant==m.isEnPassant){
                m = l; break;
            }
        }
        make_move(b,m);
    }
    auto legal = generate_legal_moves(b);
    EXPECT_FALSE(contains_move(legal,"h2h4",b));
}
```

The testing framework used for this project is `GoogleTest`. If you think you have spotted the issue in the source code, please feel free to amend bits of the code to add to your PR but please ensure that before sending it you rerun `ctest --output-on-failure` to make sure the test case you've added is fixed and that all of the other test cases still pass. Full credit will be given to those who find bugs and suggest successful fixes! :)

### Referencing 
If you use this Repo as a start-point or do any further development for your own project please reference this repo with:

Katkoria, Devraj (2025). ChessEngine. Github. https://github.com/DevrajK721/ChessEngine

