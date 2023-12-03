#include<stdio.h>
#include<time.h>
#include <stdlib.h>

#include "GamesStore.h"

int main()
{
	system("cls");
	srand(time(NULL));

	GamesStore gamesStore;
	long place_in_file = 0;
	bool resultOfInitialization = initializeGamesStore(&gamesStore, place_in_file);
	
	if (resultOfInitialization)
	{
		runGameStore(&gamesStore);
		writeGamesStoreToFile(gamesStore);
		clearGamesStore(&gamesStore);
	}

	return 0;
}