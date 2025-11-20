# Terminal Tetris Prototype

A minimal ncurses-based playground for iterating toward a Tetris-style game. The first milestone is a tiny engine that boots ncurses, runs a fixed-timestep loop, and listens for keyboard input (press `q` to quit).

## Project Layout

```
.
├── README.md              # Project overview and build instructions
├── Makefile               # Builds the ncurses binary into ./build
├── include/
│   └── game.h             # Public engine API for main()
├── src/
│   ├── main.c             # Entry point wiring init/loop/shutdown
│   └── game.c             # Engine implementation (loop, input, draw)
└── build/                 # Out-of-tree object files & binaries
```

## Prerequisites

- macOS with Command Line Tools / clang
- `ncurses` (install via `brew install ncurses` if missing)

## Building & Running

```
make
./build/terminal_tetris
```

`make clean` removes the build artifacts.

## Next Steps

- Flesh out Tetromino representations and board state
- Implement collision/lockdown rules and line clears
- Add scoring, levels, and preview/hold pieces
- Create simple scene management (title, gameplay, pause)
