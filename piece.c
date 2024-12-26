#include "piece.h"

Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos) {
    Piece* piece = (Piece*)malloc(sizeof(Piece));
    malloc_check(piece);
    piece->kind = kind;
    piece->side = side;
    piece->position = pos;
    piece->moved = moved;
    return piece;
}

void piece_show(Piece* piece) {
    if (piece == NULL) {
        fprintf(stderr, "Cannot show an empty cell\n");
        exit(1);
    }
    char* kind;
    char* pk_strings[] = {"PAWN", "BISHOP", "KNIGHT", "ROOK", "QUEEN", "KING"};
    kind = pk_strings[piece->kind];
    char* side; 
    if (piece->side == WHITE_SIDE) {
        side = "WHITE";
    } else {
        side = "BLACK";
    }
    char* moved;
    if (piece->moved == MOVED) {
        moved = "MOVED";
    } else {
        moved = "NOT MOVED";
    }
    printf("Kind: %s, ", kind);
    printf("Side: %s, ", side);
    printf("Position: ");
    square_show(pos_convert(piece->position, WHITES_PERSPECTIVE));
    printf(", ");
    printf("Status: %s\n", moved);
}

void piece_free(Piece* piece) {
    free(piece);
}

Translationlist* translationlist_new(piece_kind kind) {
    Translationlist* tl = (Translationlist*)malloc(sizeof(Translationlist));
    tl->kind = kind;
    if (kind == PAWN) {
        tl->len = 6;
    } else if (kind == KNIGHT || kind == BISHOP || kind == ROOK || kind == QUEEN) {
        tl->len = 1;
    } else if (kind == KING) {
        tl->len = 3;
    } else {
        fprintf(stderr, "Unrecognized kind\n");
        exit(1);
    }
    tl->mtt = (MoveTypeTranslation**)malloc(sizeof(MoveTypeTranslation*) * tl->len);
    if (kind == PAWN) {
        //White Pawn Capture
        tl->mtt[0] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[0]->translations = (Translation*)malloc(sizeof(Translation) * 2);
        tl->mtt[0]->translations[0] = (Translation){-1, -1};
        tl->mtt[0]->translations[1] = (Translation){-1, 1};
        tl->mtt[0]->trlen = 2;
        tl->mtt[0]->max_repeat = 1;
        tl->mtt[0]->type = (move_type*)malloc(sizeof(move_type) * 3);
        tl->mtt[0]->type[0] = CAPTURE;
        tl->mtt[0]->type[1] = PAWN_PROMOTION_CAPTURE;
        tl->mtt[0]->type[2] = EN_PASSANT;
        tl->mtt[0]->tylen = 3;
        tl->mtt[0]->side = WHITE_SIDE;
        //White Pawn No Capture
        tl->mtt[1] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[1]->translations = (Translation*)malloc(sizeof(Translation) * 1);
        tl->mtt[1]->translations[0] = (Translation){-1, 0};
        tl->mtt[1]->trlen = 1;
        tl->mtt[1]->max_repeat = 1;
        tl->mtt[1]->type = (move_type*)malloc(sizeof(move_type) * 2);
        tl->mtt[1]->type[0] = NO_CAPTURE;
        tl->mtt[1]->type[1] = PAWN_PROMOTION_NO_CAPTURE;
        tl->mtt[1]->tylen = 2;
        tl->mtt[1]->side = WHITE_SIDE;
        //White Pawn Push by Two
        tl->mtt[2] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[2]->translations = (Translation*)malloc(sizeof(Translation) * 1);
        tl->mtt[2]->translations[0] = (Translation){-2, 0};
        tl->mtt[2]->trlen = 1;
        tl->mtt[2]->max_repeat = 1;
        tl->mtt[2]->type = (move_type*)malloc(sizeof(move_type) * 1);
        tl->mtt[2]->type[0] = PAWN_PUSH_BY_TWO;
        tl->mtt[2]->tylen = 1;
        tl->mtt[2]->side = WHITE_SIDE;
        //Black Pawn Capture
        tl->mtt[3] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[3]->translations = (Translation*)malloc(sizeof(Translation) * 2);
        tl->mtt[3]->translations[0] = (Translation){1, -1};
        tl->mtt[3]->translations[1] = (Translation){1, 1};
        tl->mtt[3]->trlen = 2;
        tl->mtt[3]->max_repeat = 1;
        tl->mtt[3]->type = (move_type*)malloc(sizeof(move_type) * 3);
        tl->mtt[3]->type[0] = CAPTURE;
        tl->mtt[3]->type[1] = PAWN_PROMOTION_CAPTURE;
        tl->mtt[3]->type[2] = EN_PASSANT;
        tl->mtt[3]->tylen = 3;
        tl->mtt[3]->side = BLACK_SIDE;
        //Black Pawn No Capture
        tl->mtt[4] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[4]->translations = (Translation*)malloc(sizeof(Translation) * 1);
        tl->mtt[4]->translations[0] = (Translation){1, 0};
        tl->mtt[4]->trlen = 1;
        tl->mtt[4]->max_repeat = 1;
        tl->mtt[4]->type = (move_type*)malloc(sizeof(move_type) * 2);
        tl->mtt[4]->type[0] = NO_CAPTURE;
        tl->mtt[4]->type[1] = PAWN_PROMOTION_NO_CAPTURE;
        tl->mtt[4]->tylen = 2;
        tl->mtt[4]->side = BLACK_SIDE;
        //Black Pawn push by two
        tl->mtt[5] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[5]->translations = (Translation*)malloc(sizeof(Translation) * 1);
        tl->mtt[5]->translations[0] = (Translation){2, 0};
        tl->mtt[5]->trlen = 1;
        tl->mtt[5]->max_repeat = 1;
        tl->mtt[5]->type = (move_type*)malloc(sizeof(move_type) * 1);
        tl->mtt[5]->type[0] = PAWN_PUSH_BY_TWO;
        tl->mtt[5]->tylen = 1;
        tl->mtt[5]->side = BLACK_SIDE;
    } else if (kind == KNIGHT) {
        tl->mtt[0] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[0]->translations = (Translation*)malloc(sizeof(Translation) * 8);
        tl->mtt[0]->translations[0] = (Translation){-2, -1};
        tl->mtt[0]->translations[1] = (Translation){-2, 1};
        tl->mtt[0]->translations[2] = (Translation){-1, -2};
        tl->mtt[0]->translations[3] = (Translation){-1, 2};
        tl->mtt[0]->translations[4] = (Translation){1, -2};
        tl->mtt[0]->translations[5] = (Translation){1, 2};
        tl->mtt[0]->translations[6] = (Translation){2, -1};
        tl->mtt[0]->translations[7] = (Translation){2, 1};
        tl->mtt[0]->trlen = 8;
        tl->mtt[0]->max_repeat = 1;
        tl->mtt[0]->type = (move_type*)malloc(sizeof(move_type) * 2);
        tl->mtt[0]->type[0] = CAPTURE;
        tl->mtt[0]->type[1] = NO_CAPTURE;
        tl->mtt[0]->tylen = 2;
        tl->mtt[0]->side = WHITE_SIDE;
    } else if (kind == BISHOP) {
        tl->mtt[0] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[0]->translations = (Translation*)malloc(sizeof(Translation) * 4);
        tl->mtt[0]->translations[0] = (Translation){-1, -1};
        tl->mtt[0]->translations[1] = (Translation){-1, 1};
        tl->mtt[0]->translations[2] = (Translation){1, -1};
        tl->mtt[0]->translations[3] = (Translation){1, 1};
        tl->mtt[0]->trlen = 4;
        tl->mtt[0]->max_repeat = 7;
        tl->mtt[0]->type = (move_type*)malloc(sizeof(move_type) * 2);
        tl->mtt[0]->type[0] = CAPTURE;
        tl->mtt[0]->type[1] = NO_CAPTURE;
        tl->mtt[0]->tylen = 2;
        tl->mtt[0]->side = WHITE_SIDE;
    } else if (kind == ROOK) {
        tl->mtt[0] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[0]->translations = (Translation*)malloc(sizeof(Translation) * 4);
        tl->mtt[0]->translations[0] = (Translation){-1, 0};
        tl->mtt[0]->translations[1] = (Translation){1, 0};
        tl->mtt[0]->translations[2] = (Translation){0, -1};
        tl->mtt[0]->translations[3] = (Translation){0, 1};
        tl->mtt[0]->trlen = 4;
        tl->mtt[0]->max_repeat = 7;
        tl->mtt[0]->type = (move_type*)malloc(sizeof(move_type) * 2);
        tl->mtt[0]->type[0] = CAPTURE;
        tl->mtt[0]->type[1] = NO_CAPTURE;
        tl->mtt[0]->tylen = 2;
        tl->mtt[0]->side = WHITE_SIDE;
    } else if (kind == QUEEN) {
        tl->mtt[0] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[0]->translations = (Translation*)malloc(sizeof(Translation) * 8);
        tl->mtt[0]->translations[0] = (Translation){-1, -1};
        tl->mtt[0]->translations[1] = (Translation){-1, 0};
        tl->mtt[0]->translations[2] = (Translation){-1, 1};
        tl->mtt[0]->translations[3] = (Translation){0, -1};
        tl->mtt[0]->translations[4] = (Translation){0, 1};
        tl->mtt[0]->translations[5] = (Translation){1, -1};
        tl->mtt[0]->translations[6] = (Translation){1, 0};
        tl->mtt[0]->translations[7] = (Translation){1, 1};
        tl->mtt[0]->trlen = 8;
        tl->mtt[0]->max_repeat = 7;
        tl->mtt[0]->type = (move_type*)malloc(sizeof(move_type) * 2);
        tl->mtt[0]->type[0] = CAPTURE;
        tl->mtt[0]->type[1] = NO_CAPTURE;
        tl->mtt[0]->tylen = 2;
        tl->mtt[0]->side = WHITE_SIDE;
    } else if (kind == KING) {
        //King Standard Move
        tl->mtt[0] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[0]->translations = (Translation*)malloc(sizeof(Translation) * 8);
        tl->mtt[0]->translations[0] = (Translation){-1, -1};
        tl->mtt[0]->translations[1] = (Translation){-1, 0};
        tl->mtt[0]->translations[2] = (Translation){-1, 1};
        tl->mtt[0]->translations[3] = (Translation){0, -1};
        tl->mtt[0]->translations[4] = (Translation){0, 1};
        tl->mtt[0]->translations[5] = (Translation){1, -1};
        tl->mtt[0]->translations[6] = (Translation){1, 0};
        tl->mtt[0]->translations[7] = (Translation){1, 1};
        tl->mtt[0]->trlen = 8;
        tl->mtt[0]->max_repeat = 1;
        tl->mtt[0]->type = (move_type*)malloc(sizeof(move_type) * 2);
        tl->mtt[0]->type[0] = CAPTURE;
        tl->mtt[0]->type[1] = NO_CAPTURE;
        tl->mtt[0]->tylen = 2;
        tl->mtt[0]->side = WHITE_SIDE;
        //King QueenSide Castle
        tl->mtt[1] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[1]->translations = (Translation*)malloc(sizeof(Translation) * 1);
        tl->mtt[1]->translations[0] = (Translation){0, -2};
        tl->mtt[1]->trlen = 1;
        tl->mtt[1]->max_repeat = 1;
        tl->mtt[1]->type = (move_type*)malloc(sizeof(move_type) * 1);
        tl->mtt[1]->type[0] = QUEENSIDE_CASTLE;
        tl->mtt[1]->tylen = 1;
        tl->mtt[1]->side = WHITE_SIDE;
        //King KingSide Castle 
        tl->mtt[2] = (MoveTypeTranslation*)malloc(sizeof(MoveTypeTranslation));
        tl->mtt[2]->translations = (Translation*)malloc(sizeof(Translation) * 1);
        tl->mtt[2]->translations[0] = (Translation){0, 2};
        tl->mtt[2]->trlen = 1;
        tl->mtt[2]->max_repeat = 1;
        tl->mtt[2]->type = (move_type*)malloc(sizeof(move_type) * 1);
        tl->mtt[2]->type[0] = KINGSIDE_CASTLE;
        tl->mtt[2]->tylen = 1;
        tl->mtt[2]->side = WHITE_SIDE;
    }
    return tl;
}

move_type* translationlist_retrieve(Translationlist* tl, Displacement d, 
                              Direction dir, side side, unsigned char* tylen) {
    MoveTypeTranslation* mtt;
    Translation* tr;
    for (unsigned char i = 0; i < tl->len; i++) {
        mtt = tl->mtt[i]; 
        if (tl->kind == PAWN && mtt->side != side) {
            continue;
        }
        tr = mtt->translations;
        for (unsigned char j = 0; j < mtt->trlen; j++) {
            Direction curr_dir = {tr[j].r, tr[j].c};
            if (mtt->max_repeat == 1) {
                //here comparing displacement to translation (has to be same)
                if (d.r != tr[j].r || d.c != tr[j].c) {
                    continue;
                }
                *tylen = mtt->tylen;
                return mtt->type;
            } else if (direction_cmp(dir, curr_dir)) {
                //here comparing directions (rook, bishop, queen)
                *tylen = mtt->tylen;
                return mtt->type;
            }
        }
    }
    return NULL;
}

Piecelist* piecelist_new() {
    Piecelist* piecelist = (Piecelist*)malloc(sizeof(Piecelist));
    malloc_check(piecelist);
    piecelist->head = NULL;
    piecelist->len = 0;
    return piecelist;
}

void piecelist_show(Piecelist* piecelist) {
    Piece_entry* head = piecelist->head;
    if (head) {
        while (head) {
            piece_show(head->piece);
            head = head->next;
        }
    }
    printf("\n");
}

void piecelist_insert(Piecelist* piecelist, Piece* piece) {
    Piece_entry* piece_entry = (Piece_entry*)malloc(sizeof(Piece_entry));
    malloc_check(piece_entry);
    piece_entry->piece = piece;
    if (piece->kind == KING) {
        piece_entry->next = piecelist->head;
        piecelist->head = piece_entry;
    } else if (piecelist->len == 0) {
        piece_entry->next = NULL;
        piecelist->head = piece_entry;
    } else {
        Piece_entry* tmp = piecelist->head->next;
        piecelist->head->next = piece_entry;
        piece_entry->next = tmp;
    }
    (piecelist->len)++;
}

//Helper
void piece_entry_free(Piece_entry* piece_entry) {
    piece_free(piece_entry->piece);
    free(piece_entry);
}

void piecelist_delete(Piecelist* piecelist, Piece* piece) {
    if (piecelist->len == 0) {
        fprintf(stderr, "No pieces to delete from\n");
        exit(1);
    }
    Piece_entry* head = piecelist->head; 
    if (head->piece == piece) {
        Piece_entry* to_delete = head;
        piecelist->head = head->next;
        piece_entry_free(to_delete);
    } else {
        Piece_entry* curr = head;
        while (curr->next) {
            if (curr->next->piece == piece) {
                Piece_entry* to_delete = curr->next; 
                curr->next = curr->next->next;
                piece_entry_free(to_delete);
            }
            curr = curr->next;
        }
    }
}

void piecelist_free(Piecelist* piecelist) {
    Piece_entry* to_delete;
    while (piecelist->head) {
        to_delete = piecelist->head;
        piecelist->head = piecelist->head->next;
        piece_entry_free(to_delete);
    }
    free(piecelist);
}

