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
    game->check = (Check*)malloc(sizeof(Check));
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
    game->check.check = false;
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

bool void_traj_check(Board* board, Direction dir, Pos fpos, Pos tpos) {
    Pos curr_pos == pos_make(fpos.r + dir.r, fpos.c + dir.c);
    Piece* curr_piece;
    while (!pos_cmp(curr_pos, tpos)) {
        curr_piece = board_get(board, curr_pos);
        if (curr_piece) {
            return false;
        }
        curr_pos.r += dir.r;
        curr_pos.c += dir.c;
    }
    return true;
}

//Helper (returns true if it finds the piece looked for)
Piece* encounter(Board* board, piece_kind kind, side side, Pos pos, 
                                            Transformation transformation) {
    char* transformations = transformation.transformations;
    unsigned char max_repeat = transformation.max_repeat; 
    Pos curr_pos;
    for (unsigned char i = 0; i < transformation.len; i += 2) {
        char tr = transformations[i], tc = transformations[i + 1];
        curr_pos = pos;
        for (unsigned char j = 0; j < max_repeat; j++) {
            curr_pos.r += tr;
            curr_pos.c += tc;
            if (curr_pos.r < 0 || curr_pos.r >= board_size || curr_pos.c < 0 ||
                    curr_pos.c >= board_size) {
                break;
            }
            Piece* piece = board_get(board, curr_pos);
            if (piece == NULL) {
                continue;
            }
            if (piece->side != side || piece->kind != kind) {
                break;
            }
            return piece;
        }
    }
    return NULL;
}

//Helper (t is always of length 2)
void reverse_transformation(char* transformation) {
    transformation[0] = -transformation[0];
    transformation[1] = -transformation[1];
}

//there is always only one piece that ever pins in each case
Piece* pin(Game* game, Piece* piece) {
    Pos kpos = kpos_get(board, piece->side);
    Pos ppos = piece->position;
    if (!(kpos.r == ppos.r) && !(kpos.c == ppos.c) && 
               !(abs(kpos.r - ppos.r) == abs(kpos.c - ppos.c))) {
        return NULL; //not in same line
    }
    Displacement k_to_p = pos_displacement(

    
    t.len = 2;
    t.max_repeat = 7;
    t.transformations = (char*)malloc(sizeof(char) * 2); 
    malloc_check(t.transformations);
    fill_transformation(t.transformations, ppos, kpos);
    piece_kind pk[2]; //pieces maybe pinning
    if (abs(kpos.r - ppos.r) == abs(kpos.c - ppos.c)) {
        pk[0] = BISHOP;
        pk[1] = QUEEN;
    } else {
        pk[0] = ROOK;
        pk[1] = QUEEN;
    }
    if (!encounter(board, KING, piece->side, ppos, t)) {
        transformation_free(t);
        return NULL; //line between king and piece is not empty
    }
    reverse_transformation(t.transformations); //checking opp direction now
    for (unsigned char i = 0; i < 2; i++) {
        Piece* pinning = encounter(board, pk[i], opp_side(piece->side), ppos, t);
        if (pinning) {
            transformation_free(t);
            return pinning;
        }
    }
    transformation_free(t);
    return NULL; //line is not empty between piece and the maybe-pinning pcs
}

bool legal_to_move(Game* game, Piece* op, Pos tpos, Pos captured_pos,
                                                               move_type mt) {
    switch (op->kind) {
        case QUEEN:
        case BISHOP:
        case ROOK:
        case KNIGHT:
        case PAWN:
            Piece* pinning = pin(game, op);
            if (!pinning) {
                return true;
            }
            Pos pinning_pos = pinning->position;
            if (mt == CAPTURE || mt == PAWN_PROMOTION_CAPTURE) {
                return pos_cmp(captured_pos, pinning_pos);
            }
            Pos kpos = kpos_get(game->board, op->side);
            Displacement d1 = pos_displacement(pinning_pos, tpos); 
            Displacement d2 = pos_displacement(pinning_pos, kpos); 
            Direction dir1 = displacement_direction(d1);
            Direction dir2 = displacement_direction(d2);
            return direction_cmp(dir1, dir2);
            //tpos in trajectory between king and pinning piece;
        case KING:
            if (game->check.check && (mt == QUEENSIDE_CASTLE ||
                                      mt == KINGSIDE_CASTLE)) {
                return false;
            }
            //handle dest square not safe (for all 3 possible moves)
            //for KQcastle, handle square safety in between
    }
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

    piece_kind kind = op->kind;
    if (kind == QUEEN || kind == BISHOP || kind == ROOK || tylen == 1) {
        if (!void_traj_check(game->board, dir, fpos, tpos)) {
            return false; 
    }
    //check that there is nothing on the way from start to finish (exclusive)

    if (!legal_to_move(Game* game, Piece* op, Pos tpos, Pos captured_pos
                                                    move_type mt)) {
        return false;
    }
    //check if piece is allowed to move from its spot
    //for king, additional checks for trajectory in castle and dest square









    return 0;
}


void undo(Game* game);

game_outcome outcome(Game* game);

