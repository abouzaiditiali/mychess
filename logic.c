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
    game->check.status = NO_CHECK;
}

void game_free(Game* game) {
    board_free(game->board);
    movestack_free(game->moves);
    //make sure to free tl
    free(game->check);
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

Piece* nearest_rook(Board* board, move_type mt, side side) {
    if (mt == KINGSIDE_CASTLE && side == WHITE_SIDE) {
        return board_get(board, pos_make(7, 7));
    } else if (mt == KINGSIDE_CASTLE && side == BLACK_SIDE) {
        return board_get(board, pos_make(0, 7));
    } else if (mt == QUEENSIDE_CASTLE && side == WHITE_SIDE) {
        return board_get(board, pos_make(7, 0));
    } else {
        return board_get(board, pos_make(0, 0));
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
           Piece* nearrook = nearest_rook(board, mts[0], op->side);
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

bool out_of_bounds(char r, char c) {
    return (r < 0 || r >= board_size || c < 0 || c >= board_size); 
}

bool piece_traj_encounter(Board* board, Direction dir, Pos fpos, 
                          piece_kind* pk, unsigned char pklen, side side, 
                          unsigned char max_repeat) {
    char curr_r = (char)fpos.r + dir.r, curr_c = (char)fpos.c + dir.c;
    Piece* curr_piece;
    for (unsigned char i = 0; i < max_repeat; i++) {
        if (out_of_bounds(curr_r, curr_c)) {
            return false;
        } //handle error is betteR?
        curr_piece = board_get(board, pos_make(curr_r, curr_c)); //handle error
        if (!curr_piece) {
            continue;
        }
        if (curr_piece->side != side) {
            return false;
        }
        for (unsigned char i = 0; i < pklen; i++) {
            if (curr_piece->kind == pk[i]) {
                return true;
            }
        }
        curr_r += dir.r;
        curr_c += dir.c;
    }
    return false;
}

//there is always only one piece that ever pins in each case
Piece* pin(Board* board, Piece* piece) {
    Pos kpos = kpos_get(board, piece->side);
    Pos fpos = piece->position;
    Displacement d = pos_displacement(kpos, fpos);
    if ((abs(d.r) == 1 && abs(d.c) == 2) || (abs(d.r) == 2 && abs(d.c) == 1)) {
        return NULL;
    }
    //handle error! all errors around the entire code
    Direction dir = displacement_direction(d); //raises error if not same line
    if (!void_traj_check(board, dir, kpos, fpos)) {
        return NULL;
    }
    if (dir.r == 0 || dir.c == 0) {
        piece_kind pk[2] = {ROOK, QUEEN};
    } else {
        piece_kind pk[2] = {BISHOP, QUEEN};
    }
    return !piece_traj_encounter(board, dir, fpos, pk, 2, 
                                                   opp_side(piece->side), 7);
}

//find a way to keep track of which kinds are left on board (count +-)
bool square_threatened(Board* board, Pos pos, side threatening) {
    Direction dr1[4] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}}; //along file/rank
    piece_kind pk1[2] = {QUEEN, ROOK};
    Direction dr2[4] = {{-1, -1}, {-1, 1}, {1, 1}, {1, -1}}; //diagonals
    piece_kind pk2[2] = {QUEEN, BISHOP};
    Direction dr3[8] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, 
                            {1, -2}, {1, 2}, {2, -1}, {2, 1}}; //L
    piece_kind pk3[1] = {KNIGHT};
    Direction dr4[8] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}, 
                            {1, -1}, {-1, 1}, {1, 1}, {1, -1}}; //cing
    piece_kind pk4[1] = {KING};
    if (threatening == WHITE_SIDE) {
        Direction dr5[2] = {{1, -1}, {1, 1}};
    } else {
        Direction dr5[2] = {{-1, -1}, {-1, 1}};
    }
    piece_kind pk5[1] = {PAWN};
    for (unsigned char i = 0; i < 4; i++) {
        if (piece_traj_encounter(board, dr1[i], pos, pk1, 2, threatening, 7)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 4; i++) {
        if (piece_traj_encounter(board, dr2[i], pos, pk2, 2, threatening, 7)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 8; i++) {
        if (piece_traj_encounter(board, dr3[i], pos, pk3, 1, threatening, 1)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 8; i++) {
        if (piece_traj_encounter(board, dr4[i], pos, pk4, 1, threatening, 1)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 2; i++) {
        if (piece_traj_encounter(board, dr5[i], pos, pk5, 1, threatening, 1)) {
            return true;
        }
    }
    return false;
}

Pos castle_through(move_type mt, Pos tpos) {
    if (mt == KINGSIDE_CASTLE) { 
        return pos_make(tpos.r, tpos.c + 1);
    } else {
        return pos_make(tpos.r, tpos.c - 1);
    }
}

bool legal_to_move(Game* game, Piece* op, Pos tpos, Pos captured_pos,
                                                               move_type mt) {
    switch (op->kind) {
        case QUEEN:
        case BISHOP:
        case ROOK:
        case KNIGHT:
        case PAWN:
            Piece* pinning = pin(game->board, op);
            if (!pinning) {
                return true;
            }
            Pos pinning_pos = pinning->position;
            if (mt == CAPTURE || mt == PAWN_PROMOTION_CAPTURE) {
                return pos_cmp(captured_pos, pinning_pos);
            }
            Pos kpos = kpos_get(game->board, op->side);
            Displacement d1 = pos_displacement(tpos, pinning_pos); 
            Displacement d2 = pos_displacement(kpos, pinning_pos); 
            Direction dir1 = displacement_direction(d1);
            Direction dir2 = displacement_direction(d2);
            return direction_cmp(dir1, dir2);
            //tpos in trajectory between king and pinning piece;
        case KING:
            side opp = opp_side(op->side);
            if (square_threatened(game->board, tpos, opp)) {
                return false;
            }
            Pos in_between;
           return square_threatened(game->board, in_between, opp);
    }
} 

bool pos_in_traj(Pos pos, Pos e1, Pos e2) {
    Displacement d1 = pos_displacement(pos, e2);
    Direction dir1 = displacement_direction(d1);
    Displacement d2 = pos_displacement(e1, e2);
    Direction dir2 = displacement_direction(d2);
    if (direction_cmp(dir1, dir2)) {
        if (d1.r == 0 && d2.r == 0) {
            return abs(d1.c) < abs(d2.c);
        } else if (d1.c == 0 && d2.c == 0) {
            return abs(d1.r) < abs(d2.r);
        } else if (abs(d1.r) == abs(d1.c) && abs(d2.r) == abs(d2.c)) {
            return abs(d1.r) < abs(d2.r);
        }
    }
    return false;
}

bool is_capture_move(move_type mt) {
    return mt == CAPTURE || 
           mt == PAWN_PROMOTION_CAPTURE || 
           mt == EN_PASSANT;
}

bool is_non_capture_move(move_type mt) {
    return mt == NO_CAPTURE || 
           mt == PAWN_PROMOTION_NO_CAPTURE || 
           mt == PAWN_PUSH_BY_TWO;
}

bool handled_check(Game* game, Piece* op, move_type mt, Pos cpos, Pos tpos,
                                                Pos captured_pos) { 
    if (mt == KINGSIDE_CASTLE || mt == QUEENSIDE_CASTLE) {
        return false;
    }
    Pos kpos = kpos_get(game->board, op->side);
    Piece* pchecking = board_get(game->board, check.checking);
    if (op->kind == KING) {
        return true;
    }
    if (is_capture_move(mt) && pos_cmp(captured_pos, cpos)) {
        return true;
    }
    if ((pchecking->kind == QUEEN || 
        pcheking->kind == BISHOP || 
        pchecking->kind == ROOK) && is_non_capture_move(mt) && 
                                    pos_in_traj(tpos, kpos, cpos)) {
        return true;
    }
    return false;
}

void update_moves(game, mt, fpos, tpos, captured_pos) {
    if (is_capture_move(mt)) {
        Piece* pcaptured = board_get(game->board, captured_pos);
        piece_kind kcaptured = pcaptured->kind;
    }
    Move move = move_make(fpos, tpos, mt, kcaptured);
    movestack_add(game->moves, move);
}

void update_board(game, op, tpos, mt) {
    if (mt == PAWN_PROMOTION_CAPTURE || mt == PAWN_PROMOTION_NO_CAPTURE) {
        char ch;
        scanf("%c%*c", &ch);
        if (ch < '1' || ch > '4') {
            fprintf(stderr, "cannot promote to other than Q, B, N, R\n");
            exit(1);
        }
        op->kind = ch - '0'; //can only be 1, 2, 3, 4
    }
    board_set(game->board, tpos, op); 
    board_set(game->board, fpos, NULL);
    if (mt == EN_PASSANT) {
        board_set(game->board, captured_pos, NULL);
    }
    if (mt == KINGSIDE_CASTLE || mt == QUEENSIDE_CASTLE) {
        Pos in_between = castle_through(mt, tpos);
        Piece* nearest_rook = nearest_rook(board, mt, op->side);
        Pos pos_rook = nearest_rook->position;
        board_set(game->board, in_between, nearest_rook);
        board_set(game->board, pos_rook, NULL);
    }
}
    
void update_turn(game) {
    if (game->turn == WHITES_TURN) {
        game->turn = BLACKS_TURN;
    } else {
        game->turn = WHITES_TURN;
    }
}

bool move(Game* game, Square from, Square to) {
    Pos fpos = square_convert(from), tpos = square_convert(to);
    Piece* op = board_get(game->board, fpos);
    Piece* dp = board_get(game->board, tpos);
    Check* check = game->check;
   
    if (check.status == DOUBLE_CHECK && check.threatened == op->side && 
                                                            op->kind != KING) {
        return false;
    } //quick check (obvious) 

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

    if (check.status == CHECK && check.threatened == op->side) {
        if (!handled_check(game, op, mt, cpos, tpos, captured_pos)) {
            return false; 
        }
    }
    //handle check scenario

    piece_kind kind = op->kind;
    if (kind == QUEEN || kind == BISHOP || kind == ROOK || tylen == 1) {
        if (!void_traj_check(game->board, dir, fpos, tpos)) {
            return false; 
        }
    }
    //check that there is nothing on the way from start to finish (exclusive)

    if (!legal_to_move(Game* game, Piece* op, Pos tpos, Pos captured_pos
                                                    move_type mt)) {
        return false;
    }
    //check if piece is allowed to move from its spot (pin)
    //for king, checks for trajectory safety in castle and dest square

    //AT THIS POINT, move can be made and all changes to game state carried
    update_moves(game, mt, fpos, tpos, captured_pos);
    update_board(game, op, tpos, mt);
    update_turn(game);

    //From this point onwards, check consequences of the move:
    //is it a check, is it a checkmate, is it a stalemate, all that
     
    update_check
    //how to check if it is a check, first check that the piece moved is
    //causing a threat, then take fpos and check if there could be a piece
    //further from the king in that line (double check, or discovered check)

    if (checking) {
        update Check structure;
    } else 
        set check status to NO_CHECK; leave others unchanged







    return 0;
}


void undo(Game* game);

game_outcome outcome(Game* game);

