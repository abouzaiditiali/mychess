#include "logic.h"

Game* game_new() {
    Game* game = (Game*)malloc(sizeof(Game));
    game->board = board_new();
    game->moves = movestack_new();
    game->turn = WHITES_TURN;
    game->tls = (Translationlist**)malloc(sizeof(Translationlist*) * 6);
    piece_kind pk[6] = {PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING};
    for (unsigned char i = 0; i < 6; i++) {
        game->tls[i] = translationlist_new(pk[i]);
    }
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

//Helper
side side_from_turn(game_turn turn) {
    if (turn == WHITES_TURN) {
        return WHITE_SIDE;
    } else {
        return BLACK_SIDE;
    }
}

//Helper
void valid_move_check(Pos from, Pos to, Piece* op, Piece* dp, game_turn turn) {
    if (op == NULL) {
        fprintf(stderr, "Empty square cannot be moved\n");
        exit(1);
    }
    if (op->side != side_from_turn(turn)) {
        fprintf(stderr, "Turn does not match piece side\n");
        exit(1);
    }
    if (pos_cmp(from, to)) {
        fprintf(stderr, "Origin square cannot be destination\n");
        exit(1);
    }
    if (dp && (op->side == dp->side)) {
        fprintf(stderr, "OP and DP cannot be of same side\n");
        exit(1);
    }
}

//any piece not moved should be in its original standard position
move_type find_move_type(Game* game, Piece* op, Piece* dp, move_type* mts,
                        Square to, unsigned char tylen, Pos* captured_pos) {
    if (tylen == 1) {
        if (dp) {
            fprintf(stderr, "WB PPush by 2, QK Castle, non-null dest\n");
            exit(1);
        }
        if (op->moved == MOVED) {
            fprintf(stderr, "WB PPush by 2, QK Castle, Pawn or King moved\n");
            exit(1);
        }
        if (op->kind == KING) {
            Piece* nearrook;
            if (mts[0] == KINGSIDE_CASTLE && op->side == WHITE_SIDE) {
                nearrook = board_get(game->board, pos_make(7, 7));
            } else if (mts[0] == KINGSIDE_CASTLE && op->side == BLACK_SIDE) {
                nearrook = board_get(game->board, pos_make(0, 7));
            } else if (mts[0] == QUEENSIDE_CASTLE && op->side == WHITE_SIDE) {
                nearrook = board_get(game->board, pos_make(7, 0));
            } else {
                nearrook = board_get(game->board, pos_make(0, 0));
            }
            if (nearrook == NULL || nearrook->kind != ROOK || 
                                                   nearrook->moved == MOVED) {
                fprintf(stderr, "KQcastle not poss, no piece/rook or moved\n");
                exit(1);
            }
        }
        return mts[0];
    }
    switch (op->kind) {
        case QUEEN:
        case KING:
        case KNIGHT:
        case ROOK:
        case BISHOP:
            //no need for mts, see how to improve that later
            if (dp) {
                *captured_pos = pos_make(dp->position.r, dp->position.c);
                return CAPTURE;
            }
            return NO_CAPTURE;
        case PAWN:
            if (tylen == 2) {
                if (dp->position.r == 0 || dp->position.r == 7) {
                    return PAWN_PROMOTION_NO_CAPTURE;
                }
                return NO_CAPTURE;
            }
            if (dp) {
                *captured_pos = pos_make(dp->position.r, dp->position.c);
                if (dp->position.r == 0 || dp->position.r == 7) {
                    return PAWN_PROMOTION_CAPTURE;
                }
                return CAPTURE;
            }
            Move last = last_moves(game->moves);
            Pos pbehind;
            if (op->side == WHITE_SIDE) {
                pbehind = pos_make(dp->position.r + 1, dp->position.c); 
            } else {
                pbehind = pos_make(dp->position.r - 1, dp->position.c); 
            }
            Square sbehind = pos_convert(pbehind, WHITES_PERSPECTIVE);
            if (last->type == PAWN_PUSH_BY_TWO && 
                                            square_cmp(last->to, sbehind)) {
                *captured_pos = pbehind;
                return EN_PASSANT;
            }
            fprintf(stderr, "Pawn capture, null dp, not en-passant\n");
            exit(1);
    }
}

bool void_traj_check(Board* board, Piece* op, Direction dir, Pos tpos, 
                                                        unsigned char tylen) {
    piece_kind kind = op->kind;
    Pos fpos = op->position;
    if (kind == QUEEN || kind == BISHOP || kind == ROOK || tylen == 1) {
        Pos curr_pos == pos_make(fpos.r + dir.r, fpos.c + dir.c);
        Piece* curr_piece;
        while (!pos_cmp(curr_pos, tpos)) {
            curr_piece = board_get(board, curr_pos);
            if (curr_piece) {
                fprintf(stderr, "Piece cannot jump over pieces\n");
                exit(1);
                //maybe return false?
            }
            curr_pos.r += dir.r;
            curr_pos.c += dir.c;
        }
    }
    return true;
}

bool move(Game* game, Square from, Square to) {
    Pos fpos = square_convert(from), tpos = square_convert(to);
    Piece* op = board_get(game->board, fpos);
    Piece* dp = board_get(game->board, tpos);

    valid_move_check(fpos, tpos, op, dp, game->turn);
    //checks valid move (primitive, only raises errors, basic)

    Displacement d = pos_displacement(fpos, tpos);
    Direction dir = displacement_direction(d);
    //checks direction (has to be either file, rank, diagonal or the L)

    unsigned char tylen;
    move_type* mts = translationlist_retrieve(game->tls[op->kind], d, dir, 
                                                            op->side, &tylen);
    //returns plausible moves (if displacement or direction don't match, false)

    Pos captured_pos;
    move_type mt = find_move_type(game, op, dp, mts, to, tylen, &captured_pos);
    //check plausible moves in context now (get down to one move type only)

    if (!void_traj_check(game->board, op, dir, tpos, tylen)) {
       return false; 
    }
    //check that there is nothing on the way from start to finish (exclusive)

    //check piece is allowed to make that move mt
    //for q b r n pawn, check if piece is pinned and if it is, check that
    //tpos is between king and piece_pinning (if capture, pppos has to be ==
    //captured pos) else (anything on the line between king and ppinning excl)
    //ONE EXCEPTION: for en-passant, check tpos in line not captured pos);
    //for king, king cannot be in check and castle

    //special: if king, check that tpos is safe, if KQcastle, check in between








    return 0;
}


void undo(Game* game);

game_outcome outcome(Game* game);

