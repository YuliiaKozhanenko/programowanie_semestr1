#ifndef GAMES_STORE_HPP
#define GAMES_STORE_HPP

#define GAMES_COUNT 24
#define ILOSC_KAT 6
#define ILOSC_GIER 4

#include "Game.h"

typedef struct
{
    Game* games;
    Game** games2;
    size_t countGames2Categories;
    size_t countGames;
    char** gamesCategories;
    size_t countGamesCategories;
    double currentSum;
} GamesStore;

bool initializeGamesStore(GamesStore* gamesStore, long);

void runGameStore(GamesStore* gamesStore);

void clearGamesStore(GamesStore* gamesStore);

void writeGamesStoreToFile(GamesStore gamesStore);

#endif