# Terminal-Based Chess Game
## Overview
A terminal-based chess game built in C using the `ncurses` library. 
FULLY CUSTOMIZABLE (NEW PLAY MODES COMING OUT SOON).
CURRENT MODES:
- **WIN-BY-CHECK MODE**

## Features
- **Text-Based Interface:** Dynamic and intuitive user interface powered by 
                            `ncurses`.
- **Complete Gameplay Rules:** Supports all standard chess moves, 
                                including special moves like castling, 
                                pawn promotion, and en-passant.
- **Efficient Algorithms:** Optimized game logic for move validation and 
                            board updates.
- **Interactive Gameplay:** Allows two players to compete on a single terminal.
                            (LOCAL SERVER GAMEPLAY COMING UP SOON).

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
   For Current Mode: option to specify number of checks to win as
                    a command-line argument
   ```

## How to Play
- **Navigation:** Use text commands to specify moves 
                    (e.g., `e2e4` to move a piece from e2 to e4).

## Installation
### Prerequisites
- A C compiler (e.g., GCC)
- `ncurses` library installed on your system

