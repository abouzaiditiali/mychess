#include "logic.h"

player_perspective perspective_from_turn(game_turn turn) {
    if (turn == WHITES_TURN) {
        return WHITES_PERSPECTIVE;
    }
    return BLACKS_PERSPECTIVE;
}

char* file_handling(FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        exit(1);
    }
    fseek(file, -1, SEEK_END); 
    long file_size = ftell(file);
    if ((file_size % 4) != 0) {
        fprintf(stderr, "Incorrect move annotation\n");
        exit(1);
    }
    rewind(file);
    char* annotations = malloc(file_size + 1);
    malloc_check(annotations);
    fread(annotations, 1, file_size, file);
    annotations[file_size] = '\0';
    return annotations;
}

void play_game(Game* game, char* moves) {
    Square from, to;
    unsigned int i = 0;
    while (moves[i]) {
        sscanf(moves, "%c%hhu%c%hhu", &from.file, &from.rank, &to.file, &to.rank); 
        printf("From: "); square_show(from); printf("\n");
        printf("To: "); square_show(to); printf("\n");
        game_move(game, from, to);
        game_show(game, perspective_from_turn(game->turn));
        moves += 4;
    }
}

int main() {
    Game* game = game_new();
    game_set(game);
    
    game_show(game, perspective_from_turn(game->turn));

    FILE* file = fopen("annotations.txt", "r");
    char* annotations = file_handling(file);
    fclose(file);

    play_game(game, annotations);
    free(annotations);

    game_free(game);
    return 0;
}

