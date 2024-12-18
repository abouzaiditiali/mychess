#include "piece.h"

Piece* piece_new(piece_kind kind, piece_side side, Pos position) {
    Piece* piece = (Piece*)malloc(sizeof(Piece));
    piece->kind = kind;
    piece->side = side;
    piece->position = position;
    piece->moved = false;
    return piece;
}

void piece_free(Piece* piece) {
    free(piece);
}

//Helper
Transformation transformation_new(char* from, unsigned char len, 
                                        unsigned char max_repeat) {
    Transformation t;
    t.max_repeat = max_repeat;
    t.transformations = (char*)malloc(sizeof(char)*len);
    for (unsigned char i = 0; i < len; i++) {
        t.transformations[i] = from[i];
    }
    return t;
}

Transformation transformation_get(piece_kind kind) {
    switch (kind) {
        case KNIGHT:
            char tn[16] = {-2, -1, -2, 1, -1, -2, -1, 2, 
                           1, -2, 1, 2, 2, -1, 2, 1};
            return transformation_new(tn, 16, 1);
        case BISHOP:
            char tb[8] = {-1, -1, -1, 1, 1, -1, 1, 1};
            return transformation_new(tb, 8, 7);
        case ROOK:
            char tr[8] = {-1, 0, 0, -1, 0, 1, 1, 0};
            return transformation_new(tr, 8, 7);
        case QUEEN:
            char tq[16] = {-1, -1, -1, 0, -1, 1, 0, -1, 
                           0, 1, 1, -1, 1, 0, 1, 1};
            return transformation_new(tq, 16, 7);
        case KING:
            char tk[16] = {-1, -1, -1, 0, -1, 1, 0, -1, 
                           0, 1, 1, -1, 1, 0, 1, 1};
            return transformation_new(tk, 16, 1);
    }
    return NULL;
}

Transformation* ptransformation_get(move_direction direction, bool capture) {
    if (direction == MOVING_UP) {
        if (!capture) {
            char tpu[2] = {-1, 0}; 
            return transformation_new(tpu, 2, 2);
        }
        char tpcu[4] = {-1, -1, -1, 1};
        return transformation_new(tpcu, 4, 1);
    } else {
        if (!capture) {
            char tpd[2] = {1, 0}; 
            return transformation_new(tpd, 2, 2);
        }
        char tpcd[4] = {1, -1, 1, 1};
        return transformation_new(tpcd, 4, 1);
    }
}
 
