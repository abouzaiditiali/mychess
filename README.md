# Terminal-Based Chess Game

## Overview
A terminal-based chess game built in C using the `ncurses` library. This project demonstrates strong system design, backend logic implementation, and a passion for combining technical skills with chess gameplay.

## Features
- **Text-Based Interface:** Dynamic and intuitive user interface powered by `ncurses`.
- **Complete Gameplay Rules:** Supports all standard chess moves, including special moves like castling, pawn promotion, and en-passant (excluding draw scenarios).
- **Efficient Algorithms:** Optimized game logic for move validation and board updates.
- **Interactive Gameplay:** Allows two players to compete on a single terminal.

## Installation
### Prerequisites
- A C compiler (e.g., GCC)
- `ncurses` library installed on your system

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/abouzaiditiali/mychess.git
   cd mychess
   ```
2. Build the game using `make`:
   ```bash
   make tui
   ```
3. Run the game:
   ```bash
   ./tui
   ```

## How to Play
- **Navigation:** Use text commands to specify moves (e.g., `e2e4` to move a piece from e2 to e4).
  - Press `q` then enter to quit the game.
