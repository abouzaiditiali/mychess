#include "logic.h"

Game* game_new() {
    Game* game = (Game*)malloc(sizeof(Game));
    malloc_check(game);
    game->board = board_new();
    game->moves = movestack_new();
    game->turn = WHITES_TURN;
    game->check = (Check*)malloc(sizeof(Check));
    malloc_check(game->check);
    return game;
}

//Assumes WHITE'S PERSPECTIVE
void game_set(Game* game) {
    for (unsigned char c = 0; c < board_size; c++) {
        board_set(game->board, pos_make(1, c), 
                  piece_new(PAWN, BLACK_SIDE, NOT_MOVED, pos_make(1, c)), NEW);
        board_set(game->board, pos_make(6, c), 
                  piece_new(PAWN, WHITE_SIDE, NOT_MOVED, pos_make(6, c)), NEW);
    }
    piece_kind pk[8] = {ROOK, ROOK, KNIGHT, KNIGHT, 
                        BISHOP, BISHOP, QUEEN, KING};
    unsigned char positions[8] = {0, 7, 1, 6, 2, 5, 3, 4};
    for (unsigned char i = 0; i < 8; i++) {
        board_set(game->board, pos_make(0, positions[i]),
      piece_new(pk[i], BLACK_SIDE, NOT_MOVED, pos_make(0, positions[i])), NEW);
        board_set(game->board, pos_make(7, positions[i]),
      piece_new(pk[i], WHITE_SIDE, NOT_MOVED, pos_make(7, positions[i])), NEW);
    }
    game->check->status = NO_CHECK;
}


void game_free(Game* game) {
    board_free(game->board);
    movestack_free(game->moves);
    free(game->check);
    free(game);
}

void turn_show(game_turn turn) {
    printf("\n");
    if (turn == WHITES_TURN) {
        printf("White:\n");
    } else {
        printf("Black:\n");
    }
    printf("\n");
}

void check_show(check_status status) {
    printf("Check Status: ");
    if (status == CHECK) {
        printf("CHECK\n");
    } else if (status == DOUBLE_CHECK) {
        printf("DOUBLE CHECK\n");
    } else {
        printf("NO CHECK\n");
    }
    printf("\n");
}

void game_show(Game* game, player_perspective perspective) {
    board_show(game->board, perspective);
    check_show(game->check->status);
    movestack_show(game->moves);
    turn_show(game->turn);
}

bool handled_check(Game* game, Piece* op, move_type mt, Pos cpos, Pos tpos,
                                                Pos captured_pos) { 
    if (mt == KINGSIDE_CASTLE || mt == QUEENSIDE_CASTLE) {
        return false;
    }
    Pos kpos = kpos_get(game->board, op->side);
    Piece* pchecking = board_get(game->board, game->check->checking);
    if (op->kind == KING) {
        return true;
    }
    if (is_capture_move(mt) && pos_cmp(captured_pos, cpos)) {
        return true;
    }
    if ((pchecking->kind == QUEEN || 
        pchecking->kind == BISHOP || 
        pchecking->kind == ROOK) && is_non_capture_move(mt) && 
                                    pos_in_traj(tpos, kpos, cpos)) {
        return true;
    }
    return false;
}

void update_moves(Game* game, piece_kind moved, move_type mt, Square from, 
                        Square to, Pos captured_pos) {
    piece_kind kind_captured;
    if (is_capture_move(mt)) {
        Piece* pcaptured = board_get(game->board, captured_pos);
        kind_captured = pcaptured->kind;
    } else {
        kind_captured = PAWN; //trivial
    }
    Move move = move_make(from, to, moved, mt, kind_captured);
    movestack_add(game->moves, move);
}

void update_board(Game* game, Piece* op, Pos fpos, Pos tpos, move_type mt,
                                Pos captured_pos) {
    if (mt == PAWN_PROMOTION_CAPTURE || mt == PAWN_PROMOTION_NO_CAPTURE) {
        char ch;
        scanf("%c%*c", &ch);
        if (ch < '1' || ch > '4') {
            fprintf(stderr, "cannot promote to other than Q, B, N, R\n");
            exit(1);
        }
        op->kind = ch - '0'; //can only be 1, 2, 3, 4
    }
    board_set(game->board, tpos, op, EXISTING); 
    if (mt == EN_PASSANT) {
        board_set(game->board, captured_pos, NULL, NEW); //NEW is trivial
    }
    if (mt == KINGSIDE_CASTLE || mt == QUEENSIDE_CASTLE) {
        Pos crossed = crossedpos_get(mt, tpos);
        Piece* nrook = nrook_get(game->board, mt, op->side);
        board_set(game->board, crossed, nrook, EXISTING);
    }
}
    
void update_turn(Game* game) {
    if (game->turn == WHITES_TURN) {
        game->turn = BLACKS_TURN;
    } else {
        game->turn = WHITES_TURN;
    }
}

void update_check(Game* game, Pos fpos, Pos tpos, move_type mt, 
                                                    Pos captured_pos) {
    Piece* maybe_checking = board_get(game->board, tpos);
    if (mt == KINGSIDE_CASTLE || mt == QUEENSIDE_CASTLE) {
        Pos crossed = crossedpos_get(mt, tpos);
        maybe_checking = board_get(game->board, crossed); //rook at new pos
    }
    game->check->status = NO_CHECK;
    piece_kind ckind = maybe_checking->kind;
    side side = maybe_checking->side;
    if (ckind != KING) {
        TranslationToKind tk = translation_get(ckind, side, CAPTURE);
        bool check;
        piece_kind pk[1] = {KING};
        Direction dir;
        for (unsigned char i = 0; i < tk.len; i++) {
            dir = direction_make(tk.translations[i].r, tk.translations[i].c);
            check = piece_traj_encounter(game->board, dir, 
                                        maybe_checking->position, pk, 1,
                                        opp_side(side), tk.max_repeat);
            if (check) {
                game->check->checking = maybe_checking->position;
                game->check->status = CHECK;
                game->check->threatened = opp_side(side);
                break;
            }
        }
    }
    if (mt != KINGSIDE_CASTLE && mt != QUEENSIDE_CASTLE) {
        //can also get the second piece checking if i want, think about it
        if (pin(game->board, fpos, side)) {
            if (game->check->status == CHECK) {
                game->check->status = DOUBLE_CHECK;
            } else {
                game->check->status = CHECK;
            }
        }
    }
    if (mt == EN_PASSANT && game->check->status == NO_CHECK) {
        if (pin(game->board, captured_pos, side)) {
            game->check->status = CHECK;
        }
    }
}

bool game_move(Game* game, Square from, Square to) {
    Pos fpos = square_convert(from), tpos = square_convert(to);
    Piece* op = board_get(game->board, fpos); //handle out of bounds
    Piece* dp = board_get(game->board, tpos);
    Check* check = game->check;
   
    if (check->status == DOUBLE_CHECK && check->threatened == op->side && 
                                                            op->kind != KING) {
        return false;
    } //quick check (obvious) 

    valid_move_check(fpos, tpos, op, dp, game->turn);
    //checks valid move (primitive, only raises errors, basic)

    Displacement d = pos_displacement(fpos, tpos);
    Direction dir = displacement_direction(d); //trivial for non-sliding pieces
    if (is_sliding_piece(op->kind) && dir.r == 0 && dir.c == 0) {
        //checks direction (has to be either file, rank, diagonal)
        return false;
    }

    unsigned char tylen;
    move_type* mts = plausible_mts(op->kind, d, dir, op->side, &tylen);
    if (mts == NULL) {
        return false;
    }
    //returns plausible moves (if displacement or direction don't match, false)

    Pos captured_pos;
    move_type mt = find_move_type(game->moves, game->board, op, dp, mts, tpos, 
                                                    tylen, &captured_pos);
    if (mt == UNRECOGNIZED) {
        return false;
    }
    //check plausible moves in context now (get down to one move type only)

    if (check->status == CHECK && check->threatened == op->side) {
        if (!handled_check(game, op, mt, check->checking, tpos, 
                                                              captured_pos)) {
            return false; 
        }
    }
    //handle check scenario

    if (is_sliding_piece(op->kind) || tylen == 1) {
        if (!void_traj_check(game->board, dir, fpos, tpos)) {
            return false; 
        }
    }
    //check that there is nothing on the way from start to finish (exclusive)

    if (!legal_to_move(game->board, op, tpos, captured_pos, mt)) {
        return false;
    }
    //check if piece is allowed to move from its spot (pin)
    //for king, checks for trajectory safety in castle and dest square

    //AT THIS POINT, move can be made and all changes to game state carried
    update_moves(game, op->kind, mt, from, to, captured_pos);
    update_board(game, op, fpos, tpos, mt, captured_pos);
    update_turn(game);

    //From this point onwards, check consequences of the move:
    //is it a check, is it a checkmate, is it a stalemate, all that
    update_check(game, fpos, tpos, mt, captured_pos);
    return true;
}

void undo(Game* game);

game_outcome outcome(Game* game);

