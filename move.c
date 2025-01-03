#include "move.h"

Move move_make(Square from, Square to, piece_kind moved, move_type type, 
                        piece_kind captured) {
    Move move;
    move.from = from;
    move.to = to;
    move.moved = moved;
    move.type = type;
    move.captured = captured;
    return move;
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

//Helper
char convert_kind_to_char(piece_kind kind) {
    piece_kind pk_char[6] = {'P', 'B', 'N', 'R', 'Q', 'K'};
    return pk_char[kind];
}

void move_show(Move move) {
    if (move.moved != PAWN) {
        printf("%c", convert_kind_to_char(move.moved));
    }
    if (move.moved == PAWN && (move.type == CAPTURE || 
                               move.type == EN_PASSANT)) {
        printf("%c", move.from.file);
    }
    if (move.type == CAPTURE || move.type == EN_PASSANT) {
        printf("x");
    }
    square_show(move.to);
    if (move.type == EN_PASSANT) {
        printf(" e.p.");
    }
}

Movestack* movestack_new() {
    Movestack* s = (Movestack*)malloc(sizeof(Movestack));
    malloc_check(s);
    s->head = NULL;
    s->tail = NULL;
    s->len = 0;
    return s;
}

void movestack_add(Movestack* s, Move move) {
    pointer_check(s);
    Move_entry* e = (Move_entry*)malloc(sizeof(Move_entry));
    malloc_check(e);
    e->move = move;
    e->prev = s->tail;
    e->next = NULL;
    if (s->tail) {
        s->tail->next = e;
    }
    s->tail = e;
    if (s->head == NULL) {
        s->head = e;
    }
    s->len += 1;
}

Move movestack_pop(Movestack* s) {
    pointer_check(s);
    pointer_check(s->head);
    Move last = s->tail->move;
    Move_entry* tmp = s->tail;
    s->tail = s->tail->prev;
    free(tmp);
    if (s->tail == NULL) {
        s->head = NULL;
    } else {
        s->tail->next = NULL;
    }
    s->len -= 1;
    return last;
}

void movestack_free(Movestack* s) {
    Move_entry* tmp;
    while(s->head) {
        tmp = s->head;
        s->head = s->head->next;
        free(tmp);
    }
    free(s);
}

Move last_move(Movestack* s) {
    if (s->len == 0) {
        fprintf(stderr, "There is no last move\n");
        exit(1);
    }
    return s->tail->move;
}

void movestack_show(Movestack* s) {
    printf("MOVE COUNT: %hhu\n", s->len / 2);
    unsigned char i = 1, j = 1;
    Move_entry* curr = s->head;
    while (curr) {
        if (i % 2 != 0) {
            printf("%d. ", j++);
        }
        move_show(curr->move);
        printf(" ");
        i++;
        curr = curr->next;
    }
    printf("\n");
}

//Helper
bool move_recognized(Translation* tr, unsigned char len, Displacement d, 
                        Direction dir, unsigned char max_repeat) {
    for (unsigned char i = 0; i < len; i++) {
        Direction curr_dir;
        curr_dir.r = tr[i].r;
        curr_dir.c = tr[i].c;
        if (max_repeat == 1) {
            //here comparing displacement to translation (has to be same)
            if (d.r == tr[i].r && d.c == tr[i].c) {
                return true;
            }
        } else if (direction_cmp(dir, curr_dir)) {
            return true;
            //here comparing directions (rook, bishop, queen)
        }
    }
    return false;
}

//Helper
move_type* mts_allocate(move_type* copy_from, unsigned char len) {
    move_type* mts = (move_type*)malloc(sizeof(move_type) * len);
    malloc_check(mts);
    for (unsigned char i = 0; i < len; i++) {
        mts[i] = copy_from[i];
    }
    return mts;
}

move_type* plausible_mts(piece_kind kind, Displacement d, Direction dir,
                            side side, unsigned char* tylen) {
    if (kind == PAWN) {
        Translation tr_capture[2], tr_no_cap[1], tr_push_2[1];
        if (side == WHITE_SIDE) {
            tr_capture[0] = (Translation){-1, -1};
            tr_capture[1] = (Translation){-1, 1};
            tr_no_cap[0] = (Translation){-1, 0};
            tr_push_2[0] = (Translation){-2, 0};
        } else {
            tr_capture[0] = (Translation){1, -1};
            tr_capture[1] = (Translation){1, 1};
            tr_no_cap[0] = (Translation){1, 0};
            tr_push_2[0] = (Translation){2, 0};
        }
        if (move_recognized(tr_capture, 2, d, dir, 1)) {
            move_type mt_cap[3] = {CAPTURE, PAWN_PROMOTION_CAPTURE,EN_PASSANT};
            *tylen = 3;
            return mts_allocate(mt_cap, 3);
        }
        if (move_recognized(tr_no_cap, 1, d, dir, 1)) {
            move_type mt_no_cap[2] = {NO_CAPTURE, PAWN_PROMOTION_NO_CAPTURE};
            *tylen = 2;
            return mts_allocate(mt_no_cap, 2);
        }
        if (move_recognized(tr_push_2, 1, d, dir, 1)) {
            move_type mt_push_2[1] = {PAWN_PUSH_BY_TWO};
            *tylen = 1;
            return mts_allocate(mt_push_2, 1);
        }
    } else if (kind == KING) {
        Translation tr_king[8] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}, 
                                    {1, -1}, {-1, 1}, {1, 1}, {1, -1}};
        if (move_recognized(tr_king, 8, d, dir, 1)) {
            move_type mt_king[2] = {CAPTURE, NO_CAPTURE};
            *tylen = 2;
            return mts_allocate(mt_king, 2);
        }
        Translation tr_kcastle[1] = {{0, 2}};
        if (move_recognized(tr_kcastle, 1, d, dir, 1)) {
            move_type mt_kcastle[1] = {KINGSIDE_CASTLE};
            *tylen = 1;
            return mts_allocate(mt_kcastle, 1);
        }
        Translation tr_qcastle[1] = {{0, -2}};
        if (move_recognized(tr_qcastle, 1, d, dir, 1)) {
            move_type mt_qcastle[1] = {QUEENSIDE_CASTLE};
            *tylen = 1;
            return mts_allocate(mt_qcastle, 1);
        }
    } else {
        move_type mts[2] = {CAPTURE, NO_CAPTURE};
        *tylen = 2;
        Translation tr_knight[8] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, 
                                     {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        Translation tr_rook[4] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
        Translation tr_queen[8] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}, 
                                    {-1, -1}, {-1, 1}, {1, 1}, {1, -1}};
        Translation tr_bishop[4] = {{-1, -1}, {-1, 1}, {1, 1}, {1, -1}};
        if ((kind == KNIGHT && move_recognized(tr_knight, 8, d, dir, 1)) ||
            (kind == BISHOP && move_recognized(tr_bishop, 4, d, dir, 7)) ||
            (kind == QUEEN && move_recognized(tr_queen, 8, d, dir, 7)) ||
            (kind == ROOK && move_recognized(tr_rook, 4, d, dir, 7))) {
            return mts_allocate(mts, 2);
        }
    }
    return NULL;
}

move_type find_move_type(Movestack* s, Board* board, Piece* op, Piece* dp,  
            move_type* mts, Pos tpos, unsigned char tylen, Pos* captured_pos) {
    if (tylen == 1) {
        if (dp) {
            //fprintf(stderr, "WB PPush by 2, QK Castle, non-null dest\n");
            return UNRECOGNIZED;
        }
        if (op->moved == MOVED) {
            return UNRECOGNIZED;
            //fprintf(stderr, "WB PPush by 2, QK Castle, Pawn or King moved\n");
        }
        if (op->kind == KING) {
           Piece* nearrook = nrook_get(board, mts[0], op->side);
           if (nearrook == NULL || nearrook->kind != ROOK || 
                                                   nearrook->moved == MOVED) {
                //fprintf(stderr, "KQcastle not poss, no piece/rook or moved\n");
                return UNRECOGNIZED;
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
                if (tpos.r == 0 || tpos.r == 7) {
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
            Pos pbehind;
            if (op->side == WHITE_SIDE) {
                pbehind = pos_make(tpos.r + 1, tpos.c); 
            } else {
                pbehind = pos_make(tpos.r - 1, tpos.c); 
            }
            Square sbehind = pos_convert(pbehind, WHITES_PERSPECTIVE);
            Move last = last_move(s);
            if (last.type == PAWN_PUSH_BY_TWO && 
                                            square_cmp(last.to, sbehind)) {
                *captured_pos = pbehind;
                return EN_PASSANT;
            }
            return UNRECOGNIZED;
            //fprintf(stderr, "Pawn capture, null dp, not en-passant\n");
    }
}

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

