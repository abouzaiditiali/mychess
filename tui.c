#include "logic.h"
#include <ncurses.h>

//Helper 
void printchars(player_perspective perspective, WINDOW* win) {
    wprintw(win, "  ");
    if (perspective == WHITES_PERSPECTIVE) {
        for (char c = 'a'; c <= 'h'; c++) {
            wprintw(win, "   %c", c);
        }
    } else {
        for (char c = 'h'; c >= 'a'; c--) {
            wprintw(win, "   %c", c);
        }
    }
    wprintw(win, "\n");
}

//Helper
char maybeupp(side side, char c) {
    if (side == WHITE_SIDE) {
        char new_c = c - ('a' - 'A');
        return new_c;
    }
    return c;
}

//Helper
char convertpiecetochar(Piece* piece) {
    char pk_chars[] = {'p', 'b', 'n', 'r', 'q', 'k'};
    if (piece == NULL) {
        return '.';
    }
    return maybeupp(piece->side, pk_chars[piece->kind]);
}


void boardshow(Board* board, player_perspective perspective, WINDOW* win) {
    wprintw(win, "\n\n");
    printchars(perspective, win);
    unsigned char mu = 8, md = 1, curr = 1;
    for (unsigned char i = 0; i < 17; i++) {
        if (i % 2 == 0) {
            wprintw(win, "   ");
            for (unsigned char j = 0; j < 8; j++) {
                wprintw(win, " ---");
            }
        } else {
            wprintw(win, " ");
            if (perspective == WHITES_PERSPECTIVE) {
                wprintw(win, "%hhu", mu);
            } else {
                wprintw(win, "%hhu", md);
            }
            wprintw(win, " |");
            for (unsigned char j = 0; j < 8; j++) {
                Pos wp_pos = pos_make(i - curr, j);
                Piece* piece;
                if (perspective == WHITES_PERSPECTIVE) {
                    piece = board_get(board, wp_pos);
                } else {
                    piece = board_get(board, pos_flip(wp_pos));
                }
                char c = convertpiecetochar(piece);
                wprintw(win, " %c |", c);
            }
            curr++;
            wprintw(win, " ");
            if (perspective == WHITES_PERSPECTIVE) {
                wprintw(win, "%hhu", mu--);
            } else {
                wprintw(win, "%hhu", md++);
            }
        }
        wprintw(win, "\n"); 
    }
    printchars(perspective, win);
    wprintw(win, "\n");
}

player_perspective perspectivefromturn(game_turn turn) {
    if (turn == WHITES_TURN) {
        return WHITES_PERSPECTIVE;
    }
    return BLACKS_PERSPECTIVE;
}

void squareshow(Square square, WINDOW* win) {
    wprintw(win, "%c%hhu", square.file, square.rank);
}

//Helper
char convertkindtochar(piece_kind kind) {
    piece_kind pk_char[6] = {'P', 'B', 'N', 'R', 'Q', 'K'};
    return pk_char[kind];
}

void moveshow(Move move, WINDOW* win) {
    if (move.moved != PAWN) {
        wprintw(win, "%c", convertkindtochar(move.moved));
    }
    if (move.moved == PAWN && (move.type == CAPTURE || 
                               move.type == EN_PASSANT)) {
        wprintw(win, "%c", move.from.file);
    }
    if (move.type == CAPTURE || move.type == EN_PASSANT) {
        wprintw(win, "x");
    }
    squareshow(move.to, win);
    if (move.type == EN_PASSANT) {
        wprintw(win, " e.p.");
    }
}

void movestackshow(Movestack* s, WINDOW* win, unsigned char skip) {
    wprintw(win, "MOVE COUNT: %hhu\n", s->len / 2);
    unsigned char i = 1, j = 1;
    Move_entry* curr = s->head;
    while (curr) {
        if (skip != 0) {
            if (i % 2 != 0) {
                j++;
            }
            skip--;
        } else if (i % 2 != 0) {
            wprintw(win, "%d. ", j++);
            moveshow(curr->move, win);
        } else {
            wprintw(win, " ");
            moveshow(curr->move, win);
            wprintw(win, "\n");
        }
        i++;
        curr = curr->next;
    }
    wprintw(win, "\n");
}

int main() {
    initscr();
    echo();
    refresh();

    WINDOW *board_win = newwin(30, 40, 0, 0);
    WINDOW *move_win = newwin(3, 20, 2, 45);

    Game* game = game_new();
    game_set(game);

    boardshow(game->board, perspectivefromturn(game->turn), board_win);
    wrefresh(board_win);

    Square from, to;
    unsigned char skip = 0;
    while (1) {
        move(0, 0);
        printw("Enter move: ");
        scanw("%c%hhu%c%hhu", &from.file, &from.rank, &to.file, &to.rank); 
        game_move(game, from, to);
        wclear(board_win);
        boardshow(game->board, perspectivefromturn(game->turn), board_win);
        wrefresh(board_win);
        wclear(move_win);
        if (game->moves->len == 2) {
            skip = 2;
        } else if (game->moves->len > 2) {
            skip += 2;
        }
        movestackshow(game->moves, move_win, skip);
        wrefresh(move_win);
    }

    endwin(); 
    return 0;
}

