# Terminal Tetris

Tiny curses playground for evolving a Tetris clone. Runs with `ncurses` on macOS/Linux and `pdcurses` on Windows.

## Features

- Score tracking with persistent high score (`highscore.dat`)
- Next-piece preview plus hard drop for faster play
- Seven-bag randomization, lock delay, and level-based gravity
- Automated logic tests via `make test`

## Build & Run

**macOS / Linux**
```
make
./build/terminal_tetris
make test   # logic tests
```

**Windows (MinGW + PDCurses)**
```
set CC=x86_64-w64-mingw32-gcc
mingw32-make LDFLAGS=-lpdcurses
build\terminal_tetris.exe
mingw32-make test
```

`make clean` (or `mingw32-make clean`) removes `build/`.

## Controls
```
- Arrow keys or A/D: move piece
- Up or W: rotate
- Down or S: soft drop
- Space: hard drop
- R: restart after game over
- Q: quit
```
