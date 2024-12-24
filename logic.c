#include "logic.h"

Game* game_new() {
    Game* game = (Game*)malloc(sizeof(Game));
    game->board = board_new();
    game->moves = movestack_new();
    game->turn = WHITES_TURN;
    return game;
}

//Assumes WHITE'S PERSPECTIVE
void game_set(Game* game) {
    for (unsigned char c = 0; c < board_size; c++) {
        board_set(game->board, pos_make(1, c), 
                  piece_new(PAWN, BLACK_SIDE, NOT_MOVED, pos_make(1, c)));
        board_set(game->board, pos_make(6, c), 
                  piece_new(PAWN, WHITE_SIDE, NOT_MOVED, pos_make(6, c)));
    }
    piece_kind pk[8] = {ROOK, ROOK, KNIGHT, KNIGHT, 
                        BISHOP, BISHOP, QUEEN, KING};
    unsigned char positions[8] = {0, 7, 1, 6, 2, 5, 3, 4};
    for (unsigned char i = 0; i < 8; i++) {
        board_set(game->board, pos_make(0, positions[i]),
          piece_new(pk[i], BLACK_SIDE, NOT_MOVED, pos_make(0, positions[i])));
        board_set(game->board, pos_make(7, positions[i]),
          piece_new(pk[i], WHITE_SIDE, NOT_MOVED, pos_make(7, positions[i])));
    }
}

void game_free(Game* game) {
    board_free(game->board);
    movestack_free(game->moves);
    free(game);
}

bool move(Game* game, Square from, Square to) {
    Pos from = square_convert(from);
    Pos to = square_convert(to);



void undo(Game* game);

game_outcome outcome(Game* game);

