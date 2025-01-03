#include "logic.h"
#include <ncurses.h>
#include <locale.h>
#include <wchar.h>

void nprint_chars(player_perspective perspective, WINDOW* win) {
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

wchar_t nconvert_piece_to_unicode(Piece* piece) {
    if (piece == NULL) {
        return L' ';
    }
    wchar_t black_pieces[] = L"♙♗♘♖♕♔"; 
    wchar_t white_pieces[] = L"♟♝♞♜♛♚";
    if (piece->side == WHITE_SIDE) {
        return white_pieces[piece->kind];
    } else {
        return black_pieces[piece->kind];
    }
}

void nboard_show(Board* board, player_perspective perspective, WINDOW* win) {
    wattron(win, A_BOLD);
    wprintw(win, "\n\n");
    //nprint_chars(perspective, win);
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

                wchar_t c = nconvert_piece_to_unicode(piece);
                wprintw(win, " %lc |", c);
            }
            curr++;
            //wprintw(win, " ");
            //if (perspective == WHITES_PERSPECTIVE) {
            //    wprintw(win, "%hhu", mu--);
            //} else {
            //    wprintw(win, "%hhu", md++);
            //}
            mu--;
            md++;
        }
        wprintw(win, "\n"); 
    }
    nprint_chars(perspective, win);
    wprintw(win, "\n");
    wattroff(win, A_BOLD);
}

void nsquare_show(Square square, WINDOW* win) {
    wprintw(win, "%c%hhu", square.file, square.rank);
}

wchar_t nconvert_kind_to_char(piece_kind kind) {
    wchar_t pieces[] = L"♙♗♘♖♕♔"; 
    //piece_kind pk_char[6] = {'P', 'B', 'N', 'R', 'Q', 'K'};
    return pieces[kind];
}

void nmove_show(Move move, WINDOW* win) {
    if (move.moved != PAWN) {
        wprintw(win, "%lc", nconvert_kind_to_char(move.moved));
    }
    if (move.moved == PAWN && (move.type == CAPTURE || 
                               move.type == EN_PASSANT)) {
        wprintw(win, "%c", move.from.file);
    }
    if (move.type == CAPTURE || move.type == EN_PASSANT) {
        wprintw(win, "x");
    }
    nsquare_show(move.to, win);
    if (move.type == EN_PASSANT) {
        wprintw(win, " e.p.");
    }
}

void nmovestack_show(Move_entry* top_move, WINDOW* win, 
                                                    unsigned char top_mcount) {
    unsigned char i = 1, r = 2;
    while (top_move) {
        if (i % 2 != 0) {
            wmove(win, r, 0);
            wclrtoeol(win);
            wprintw(win, "%hhu. ", top_mcount++);
            nmove_show(top_move->move, win);
            wprintw(win, " ");
        } else {
            wmove(win, r++, 15);
            nmove_show(top_move->move, win);
            wprintw(win, "\n");
        }
        i++;
        top_move = top_move->next;
    }
    wprintw(win, "%hhu. ", top_mcount);
}

void update_board_win(Game* game, WINDOW* board_win) {
    wclear(board_win);
    nboard_show(game->board, perspective_from_turn(game->turn), board_win);
}

void update_move_win(Game* game, WINDOW* move_win, Pos* start, 
                        Move_entry** top_move, unsigned char* top_mcount) {
    unsigned char move_len = game->moves->len;
    unsigned char move_count = move_len / 2;
    wmove(move_win, 0, 12);
    wclrtoeol(move_win);
    mvwprintw(move_win, 0, 12, "%d", move_count);
    if (start->c == 3) {
        wmove(move_win, start->r, start->c);
        nmove_show(last_move(game->moves), move_win);
        wprintw(move_win, " ");
        start->c = 15;
        if (start->r == 2) {
            *top_move = game->moves->head; //first move in the game
        }
    } else {
        wmove(move_win, start->r, 15);
        nmove_show(last_move(game->moves), move_win);
        start->c = 3;
        if (start->r == 6) { //MAX INDEX of LINES of moves displayed
            *top_move = (*top_move)->next->next;
            *top_mcount += 1;
            nmovestack_show(*top_move, move_win, *top_mcount);
            wclrtoeol(move_win);
        } else {
            start->r += 1;
            mvwprintw(move_win, start->r, 0, "%hhu. ", move_count + 1);
        }
    }
}

int main() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    echo();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    wbkgd(stdscr, COLOR_PAIR(1));
    refresh();

    WINDOW *board_win = newwin(20, 37, 0, 0); 
    //minimum 20 height for screen (more appealing at 21), 37 width for board
    //width limit has to be one extra character for some reason 
    WINDOW *move_win = newwin(10, 27, 2, 39);
    //minimum 68 width for screen, 37 (board) + 3 (space) + 28 (move)

    Game* game = game_new();
    game_set(game);

    nboard_show(game->board, perspective_from_turn(game->turn), board_win);
    wrefresh(board_win);
    
    wprintw(move_win, "MOVE COUNT: %hhu\n\n", game->moves->len / 2);
    wprintw(move_win, "1. ");
    Pos start = pos_make(2, 3);
    Move_entry* top_move;
    unsigned char top_mcount = 1;
    wrefresh(move_win);

    Square from, to;
    while (1) {
        move(0, 0);
        printw("Enter move: ");
        scanw("%c%hhu%c%hhu", &from.file, &from.rank, &to.file, &to.rank); 
        if (from.file == 'q') {
            break;
        }
        game_move(game, from, to);

        update_board_win(game, board_win);
        wrefresh(board_win);

        update_move_win(game, move_win, &start, &top_move, &top_mcount);
        wrefresh(move_win);
    }

    delwin(board_win);
    delwin(move_win);
    endwin(); 
    return 0;
}

