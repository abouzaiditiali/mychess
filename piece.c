#include "piece.h"

void fill_transformation(char* into, char* from, unsigned char len) {
    for (unsigned char i = 0; i < len; i++) {
        into[i] = from[i];
    }
}

char* transformation(Piece* piece, char* t, unsigned char len) {
    char* res = (char*)malloc(sizeof(char)*len);
    fill_transformation(res, t, len);
    return res;
}

Piece* piece_new(piece_kind kind, piece_side side, Pos position) {
    Piece* piece = (Piece*)malloc(sizeof(Piece));
    piece->kind = kind;
    piece->side = side;
    piece->position = position;
    piece->moved = false;
    switch (kind) {
        case PAWN:
            piece->travel = TWO_TRANSFORMATIONS;
            piece->pc_travel = ONE_TRANSFORMATION;
            break;
        case KNIGHT:
            piece->travel = ONE_TRANSFORMATION;
            char tn[16] = {-2, -1, -2, 1, -1, -2, -1, 2, 
                           1, -2, 1, 2, 2, -1, 2, 1};
            piece->transformations = transformation(piece, tn, 16);
            break;
        case BISHOP:
            piece->travel = AS_MANY;
            char tb[8] = {-1, -1, -1, 1, 1, -1, 1, 1};
            piece->transformations = transformation(piece, tb, 8);
            break;
        case ROOK:
            piece->travel = AS_MANY;
            char tr[8] = {-1, 0, 0, -1, 0, 1, 1, 0};
            piece->transformations = transformation(piece, tr, 8);
            break;
        default: {
            char t[16] = {-1, -1, -1, 0, -1, 1, 0, -1, 
                           0, 1, 1, -1, 1, 0, 1, 1};
            piece->transformations = transformation(piece, t, 16);
            if (piece->kind == KING) {
                piece->travel = ONE_TRANSFORMATION;
            } else {
                piece->travel = AS_MANY;
            }
        }
    }
    return piece;
}

void set_pawn_transformation(Piece* piece, move_direction direction) {
    piece->direction = direction;
    if (direction == MOVING_UP) {
        char t1[2] = {-1, 0}; 
        piece->transformations = transformation(piece, t1, 2);
        char t2[4] = {-1, -1, -1, 1};
        piece->pc_transformations = transformation(piece, t2, 4);
    } else {
        char t1[2] = {1, 0}; 
        piece->transformations = transformation(piece, t1, 2);
        char t2[4] = {1, -1, 1, 1};
        piece->pc_transformations = transformation(piece, t2, 4);
    }
}
 
void piece_free(Piece* piece) {
    free(piece->transformations);
    if (piece->kind == PAWN) {
        free(piece->pc_transformations);
    }
    free(piece);
}

