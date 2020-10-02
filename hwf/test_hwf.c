#include "hwf.c"

#include <stdio.h>
#include <stdlib.h>

#define MAX_GAME (5000 + 1)
#define GAME_RPT (10)

typedef int (*move_func)(int, int);

// Random player only tries to not make moves that
// cause him to lose on the next turn
int random_next_turn(int total, int possible) {
    assert(total > 0);
    assert(possible > 0);

    int max_move = (possible - 1) / 3;
    if (max_move == 0) {
        return 1;
    }

    return rand() % max_move + 1;
}

// Simulate a game of Fibonacci nim between two decision functions
int simulate_game(int total, move_func player0_func, move_func player1_func) {
    assert(total > 1);
    assert(player0_func);
    assert(player1_func);

    int playerid = 1;
    int possible = total - 1;
    while (total > 0) {
        playerid = !playerid;
        int move;
        if (!playerid) {
            move = player0_func(total, possible);
        }
        else {
            move = player1_func(total, possible);
        }
        assert(move <= possible);
        assert(move > 0);
        total -= move;
        possible = 2 * move;
    }
    
    return playerid;
}

void play_game(int start_count, int* wins_first, int* wins_second, int* games) {
    assert(wins_first);
    assert(wins_second);
    assert(games);
    
    int go_first_res = !simulate_game(start_count, next_turn, random_next_turn);
    if (!go_first_res) {
        printf("Lost game %d when going first\n", start_count);
    }
    *wins_first += go_first_res;

    int go_second_res = simulate_game(start_count, random_next_turn, next_turn);
    if (!go_second_res) { 
        printf("Lost game %d when going second\n", start_count);
    }
    *wins_second += go_second_res;

    (*games)++;
}

int main() {
    int wins_first = 0;
    int wins_second = 0;
    int games = 0;
    for (int i = 2; i <= MAX_GAME; i++) {
        for (int j = 0; j < GAME_RPT; j++) {
            play_game(i, &wins_first, &wins_second, &games);
        }
    }

    printf("Won %d games out of %d when going first\n", wins_first, games);
    printf("Won %d games out of %d when going second\n", wins_second, games);

    return 0;
}
