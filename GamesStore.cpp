#include "GamesStore.h"
#include "Date.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define buffer_size 500
long place_in_file = 0;
size_t getCountOfCategories(const char* filename,const long place_in_file);
size_t getCountOfGames(char* filename, long* place_in_file);
static void outputListOfGames(const Game resultGames[], const size_t countResultGames);
static bool chooseSortTypeMenu();
static void findGamesByCategory(Game* games, size_t gamesCount, char** categories, size_t sizeCategories, Game** gamesResult, size_t* sizeGamesResult);
static void findGameByPartOfName(const Game* games, const size_t gamesCount, Game** gamesResult, size_t* sizeGamesResult);
static void sortGamesByPrice(Game* games, const size_t gamesCount, bool sortLowToHigh);
static void sortGamesById(Game* games, const size_t gamesCount);
static void sortGamesByReleaseDate(Game* games, const size_t gamesCount, bool sortLowToHigh);
static void findGameById(const Game* games, const size_t gamesCount, Game** gameResult);
static void addGamePriceToSum(const Game* game, double* sum);
static int countDiscountForSum(const GamesStore* gamesStore);
static void fromOneDimensionalToTwoDimensional(Game* games, Game*** Games,char** GamesCategories,size_t countGamesCategories, size_t countGames);
static int getCountOfGamesInCategory(const Game* games, const char* gameCategory, const size_t countGames);
void addNewGame(Game** games, size_t* countGames, char*** gamesCategories, size_t* countGamesCategories);
void deleteGame(Game** games, size_t* countGames);


enum UserCommands {
	FIND_GAMES_BY_CATEGORY = 1,
	FIND_GAMES_BY_NAME,
	SORT_GAMES_BY_ID,
	SORT_GAMES_BY_PRICE,
	SORT_GAMES_BY_RELEASE_DATE,
	ADD_GAME_TO_THE_CART_USING_ID,
	ADD_NEW_GAME_TO_THE_STORE,
	DELETE_GAME_FROM_STORE,
	CALCULATE_DISCOUNT_AND_BUY = 0,
	UNDEFIND = -1
};

//funkcja inicjalizująca gamesStore
bool initializeGamesStore(GamesStore* gamesStore, long place_in_file)
{
	bool resultOfInitialization = false;

	gamesStore->currentSum = 0.0;

	FILE* file = NULL;

	errno_t err = fopen_s(&file, "GamesStore.txt", "r");

	if (err != 0)
	{
		printf("\ncan't read file from path %s\n", "GamesStore.txt");
	}

	gamesStore->countGames = getCountOfGames((char*)"GamesStore.txt", &place_in_file);
	//zliczającej wiersze w pliku do znaku '&'
	//printf("%Iu = gamesStore->countGames\n", gamesStore->countGames);
	Game* games12 = (Game*)malloc(sizeof(Game) * gamesStore->countGames);

	//odczytywanie z pliku w temp games12 za pomocą strtok_s
	if (games12 != NULL)
	{
		char string1[500];
		if (err == 0)
		{
			for (int k = 0; k < gamesStore->countGames; k++)
			{
				fgets(string1, 500, file);
				char seps[] = ",|";//ograniczniki
				char* token1 = NULL;
				char* next_token1 = NULL;
				token1 = strtok_s(string1, seps, &next_token1);//pobranie pierwszego stringa przed
				//ogranicznikiem

				games12[k].id = k + 1;

				if (token1 != NULL)
				{
					//rezervacja pamięci
					games12[k].name = (char*)malloc(sizeof(char) * (strlen(token1) + 1));
					//kopiowanie nazwy ze stringa w name
					strcpy_s(games12[k].name, (strlen(token1) + 1), token1);
					token1 = strtok_s(NULL, seps, &next_token1);

					games12[k].category = (char*)malloc(sizeof(char) * (strlen(token1) + 1));
					strcpy_s(games12[k].category, (strlen(token1) + 1), token1);
					token1 = strtok_s(NULL, seps, &next_token1);

					games12[k].price = atof(token1);
					//przypisywanie ceny ze stringa i konwersja znaków
					//z typu char na typ zmiennoprzecinkowy
					token1 = strtok_s(NULL, seps, &next_token1);

					games12[k].releaseDate.day = atoi(token1);//konwersja na typ int
					token1 = strtok_s(NULL, seps, &next_token1);

					games12[k].releaseDate.month = atoi(token1);
					token1 = strtok_s(NULL, seps, &next_token1);

					games12[k].releaseDate.year = atoi(token1);
					token1 = strtok_s(NULL, seps, &next_token1);
				}
			}

		}
		//uzyskanie liczby kategorii za pomocą funkcji zliczającej liczbę wierszy po '&'
		gamesStore->countGamesCategories = getCountOfCategories((char*)"GamesStore.txt", place_in_file);
		//printf("%Iu = noun of categories\n", gamesStore->countGamesCategories);
		gamesStore->gamesCategories = (char**)malloc(sizeof(char*) * gamesStore->countGamesCategories);


		if (gamesStore->gamesCategories != NULL)
		{
			for (int i = 0; i < gamesStore->countGamesCategories; i++)
			{
				gamesStore->gamesCategories[i] = (char*)malloc(sizeof(char) * STRING_SIZE);
			}
			if (err == 0 && gamesStore->gamesCategories != NULL)
			{
				//przesunięcie w pliku na miejsce po '&', żeby odczytać nazwy kategorii
				fseek(file, place_in_file, 0);
				for (int i = 0; i < gamesStore->countGamesCategories; i++)
				{
					if (gamesStore->gamesCategories[i] != NULL)
					{
						fgets(gamesStore->gamesCategories[i], STRING_SIZE, file);
						gamesStore->gamesCategories[i][strcspn(gamesStore->gamesCategories[i], "\n")] = '\0';
						//za pomocą strcspn, jaka zwraca długość stringa przed '\n'
					}
				}
			}
		}
		//jeśli wszystko zostało zrobiono, inicjalizacja się udała
		resultOfInitialization = true;
	}
	else
	{
		puts("\ninitializeGamesStore:: Memory not allocated for gamesStore->games!\n");
	}

	if (file)
	{
		//zamykanie pliku
		fclose(file);

		if (err == 0)
		{
			
		}
		else
		{
			printf("The file was not closed\n");
		}
	}
	//przekazanie wskaźnika do tablicy z grami
	(*gamesStore).games = games12;
	//tworzenie tablicy structur 2D na podstawie tablicy 1D w celu użycia potrójnego wskaźnika w funkcji
	fromOneDimensionalToTwoDimensional(gamesStore->games, &gamesStore->games2, gamesStore->gamesCategories, gamesStore->countGamesCategories, gamesStore->countGames);
	gamesStore->countGames2Categories = gamesStore->countGamesCategories;


	return resultOfInitialization;
}

static void fromOneDimensionalToTwoDimensional(Game* games, Game*** Games, char** GamesCategories, size_t countGamesCategories, size_t countGames)
{
	Game** buffer;
	int rg = 0;
	//rezerwacja pamięci
	buffer = (Game**)malloc(sizeof(Game*) * countGamesCategories);
	for (int i = 0; i < countGamesCategories; i++)
	{
		//tworzenie dwuwymiarowej dynamicznej tablivy gier, każdy wiersz to nowa kategoria
		int CountOfGamesInCategory = getCountOfGamesInCategory(games, GamesCategories[i], countGames);
		if (buffer != NULL)
		{
			buffer[i] = (Game*)malloc(sizeof(Game) * CountOfGamesInCategory);

			if (buffer[i] != NULL && games != NULL)
			{
				for (int j = 0; j < CountOfGamesInCategory; j++)
				{
					buffer[i][j] = games[rg];
					rg++;
				}
			}
		}
	}

	*Games = buffer;
}

static int getCountOfGamesInCategory(const Game* games, const char* gameCategory, const size_t countGames)
{

	int countOfGamesInCategory = 0;
	for (unsigned i = 0; i < countGames; i++)
	{
		if (_strnicmp(gameCategory, games[i].category, strlen(gameCategory)) == 0)
		{
			countOfGamesInCategory++;
		}
	}

	return countOfGamesInCategory;
}
//Liczenie liczby gier w pliku
static size_t getCountOfGames(char* filename, long* place_in_file)
{

	FILE* filePointer = NULL;
	errno_t err = fopen_s(&filePointer, filename, "r");

	size_t rec_count = 0;
	if (err == NULL)
	{
		char dataToBeRead[buffer_size];
		while (fgets(dataToBeRead, buffer_size, filePointer) != NULL)
		{
			if (strncmp(dataToBeRead, "&", 1) == 0)
			{
				*place_in_file = ftell(filePointer);
				break;

			}
			++rec_count;
		}
	}
	else
	{
		printf("\ncan't read file from path %s\n", filename);
	}
	if (filePointer)
	{
		err = fclose(filePointer);
		if (err != 0)
		{
			printf("The file was not closed\n");
		}
	}

	return rec_count;
}
//liczenie liczby kategorii w pliku
static size_t getCountOfCategories(const char* filename, const long place_in_file)
{

	FILE* filePointer = NULL;
	errno_t err = fopen_s(&filePointer, filename, "r");
	size_t rec_count1 = 0;
	if (err == NULL)
	{
		//przemieszczenie na miejsce po '&'
		fseek(filePointer, place_in_file, 0);
		char dataToBeRead[buffer_size];
		while (fgets(dataToBeRead, buffer_size, filePointer) != NULL)
		{
			++rec_count1;
		}
	}
	else
	{
		printf("\ncan't read file from path %s\n", filename);
	}
	if (filePointer)
	{
		err = fclose(filePointer);
		if (err != 0)
		{
			printf("The file was not closed\n");
		}
	}

	return rec_count1;
}
//oczyszczenie pamięci po gamesStore
void clearGamesStore(GamesStore* gamesStore)
{
	//oczyszczenie wszystkich gier z tablicy 1D
	for (int i = 0; i < gamesStore->countGames; ++i)
	{
		free(gamesStore->games[i].name);
		free(gamesStore->games[i].category);
	}
	free(gamesStore->games);
	//oczyszczenie wszystkich kategorii
	for (int i = 0; i < gamesStore->countGamesCategories; ++i)
	{
		free(gamesStore->gamesCategories[i]);
	}
	free(gamesStore->gamesCategories);

	//oczyszczenie wszystkich gier z tablicy 2D
	for (int i = 0; i < gamesStore->countGames2Categories; ++i)
	{
		gamesStore->games2[i];
	}
	free(gamesStore->games2);

}

//funkcja uruchamiająca menu konsoli użytkownika
void runGameStore(GamesStore* gamesStore)
{
	//tworzenie zmiennej enum
	UserCommands userCommand = UserCommands::UNDEFIND;
	bool howToSort;

	printf("\nWellcome to our store!\n");
	//początkowa lista gier
	outputListOfGames(
		gamesStore->games,
		gamesStore->countGames
	);
	//Tworzenie tablicy dla punktu menu 1,2,6
	Game* resultGames = NULL;
	size_t countResultGames = 0;

	while (userCommand != 0)
	{
		printf("\nMenu: \n\n");
		printf("1. Find games by category\n");
		printf("2. Find games by name\n");
		printf("3. Sort games by id\n");
		printf("4. Sort games by price\n");
		printf("5. Sort games by release dates\n");
		printf("6. Add the game to the cart using id\n");
		printf("7. Add new game to the store\n");
		printf("8. Delete game from the store\n");
		printf("0. Calculate the discount and buy\n");
		printf("Input command: ");
		scanf_s("%d", &userCommand);

		switch (userCommand)
		{
		case UserCommands::FIND_GAMES_BY_CATEGORY:
		{
			findGamesByCategory(
				gamesStore->games,
				gamesStore->countGames,
				gamesStore->gamesCategories,
				gamesStore->countGamesCategories,
				&resultGames,
				&countResultGames
			);
			//jeśli znalezione gry, pokazać
			if (resultGames != NULL)
			{
				outputListOfGames(resultGames, countResultGames);

				free(resultGames);
				resultGames = NULL;
				countResultGames = size_t(0);
			}
			else
			{

			}

			break;
		}
		case UserCommands::FIND_GAMES_BY_NAME:
		{
			findGameByPartOfName(gamesStore->games, gamesStore->countGames, &resultGames, &countResultGames);

			if (countResultGames == 0)
			{
				puts("Did not find");
			}
			else
			{
				outputListOfGames(resultGames, countResultGames);

				free(resultGames);
				resultGames = NULL;
				countResultGames = size_t(0);
			}
			break;
		}
		case UserCommands::SORT_GAMES_BY_ID:
		{
			sortGamesById(gamesStore->games, gamesStore->countGames);
			outputListOfGames(gamesStore->games, gamesStore->countGames);
			break;
		}
		case UserCommands::SORT_GAMES_BY_PRICE:
		{
			howToSort = chooseSortTypeMenu();
			sortGamesByPrice(gamesStore->games, gamesStore->countGames, howToSort);
			outputListOfGames(gamesStore->games, gamesStore->countGames);
			break;
		}
		case UserCommands::SORT_GAMES_BY_RELEASE_DATE:
		{
			howToSort = chooseSortTypeMenu();
			sortGamesByReleaseDate(gamesStore->games, gamesStore->countGames, howToSort);
			outputListOfGames(gamesStore->games, gamesStore->countGames);
			break;
		}
		case UserCommands::ADD_GAME_TO_THE_CART_USING_ID:
		{
			findGameById(gamesStore->games, gamesStore->countGames, &resultGames);
			outputListOfGames(resultGames, 1);
			addGamePriceToSum(resultGames, &gamesStore->currentSum);

			free(resultGames);
			resultGames = NULL;

			printf("\nYour cart contains products with a total price: %.2lf\n", gamesStore->currentSum);
			break;
		}
		case UserCommands::ADD_NEW_GAME_TO_THE_STORE:
		{
			addNewGame(&gamesStore->games, &gamesStore->countGames, &gamesStore->gamesCategories, &gamesStore->countGamesCategories);
			break;
		}
		case UserCommands::DELETE_GAME_FROM_STORE:
		{
			deleteGame(&gamesStore->games, &gamesStore->countGames);
			break;
		}
		case UserCommands::CALCULATE_DISCOUNT_AND_BUY:
		{
			//tworzenie i znalezienie zniżki
			unsigned discount;
			discount = countDiscountForSum(gamesStore);
			printf("\nThe random discount is %d%%\n", discount);
			//odejmowanie zniżki
			gamesStore->currentSum -= (gamesStore->currentSum * discount / 100);
			printf("Taking into account the discount, you spent %.2lf PLN\n", gamesStore->currentSum);
			break;
		}
		default:
		{
			printf("\nIncorrect command. Try again\n");
		}
		}
	}
}


static void outputListOfGames(const Game resultGames[], const size_t countResultGames)
{
	printf("\n-------------------------------------------------------------------------------------------\n");
	printf("| %-3s | %-30s | %-20s | %-10s | %-12s |\n", "ID", "Name of game", "Category", "Price", "Release date");
	printf("-------------------------------------------------------------------------------------------\n");

	for (size_t i = 0; i < countResultGames; ++i)
	{
		printf("| %-3d | %-30s | %-20s | %-10.2f |  %-.2d.%-.2d.%-4d  |\n",
			resultGames[i].id,
			resultGames[i].name,
			resultGames[i].category,
			resultGames[i].price,
			resultGames[i].releaseDate.day,
			resultGames[i].releaseDate.month,
			resultGames[i].releaseDate.year
		);
	}
	printf("-------------------------------------------------------------------------------------------\n");
}

//funkcja dla punktów menu 3,4,5 (wybór sortowania malejąco lub rosnąco)
static bool chooseSortTypeMenu()
{
	bool resultType = true;

	int userCommand = -1;

	puts("\nChoose how to sort:");
	puts("1. Low to high");
	puts("2. High to low");
	printf("Input command: ");
	scanf_s("%d", &userCommand);
	while (userCommand < 1 && userCommand > 2)
	{
		printf("Incorrect command. Try again\n");
		scanf_s("%d", &userCommand);
	}
	if (userCommand == 2)
	{
		resultType = false;
	}

	return resultType;
}


static void findGamesByCategory(Game* games, size_t gamesCount, char** categories, size_t sizeCategories, Game** gamesResult, size_t* sizeGamesResult)
{
	Game* resultGamesTempArray = NULL;

	//output exsit game categories
	printf("\nChoose category:\n");
	for (size_t i = 0; i < sizeCategories; ++i)
	{
		printf("%Iu. %s\n", i + 1, categories[i]);
	}

	//resive category from user
	int userCategoryNumber = -1;
	printf("Input category number: ");
	scanf_s("%d", &userCategoryNumber);

	while (userCategoryNumber < 1 || userCategoryNumber > sizeCategories)
	{
		printf("Incorrect command. Try again\n");
		scanf_s("%d", &userCategoryNumber);
	}

	//find and fill result games to the temporary buffer
	Game* tempGamesBuffer = (Game*)malloc(sizeof(Game) * gamesCount);
	size_t tempGamesBufferRealSize = 0;

	if (tempGamesBuffer != NULL)
	{
		for (size_t i = 0; i < gamesCount; ++i)
		{
			//jeśli kategoria w structurze games[i] jest taka sama, jak kategoria w structurze gamesStore, dodać grę do bufferu, poźniej do resultGames
			if (strncmp(categories[userCategoryNumber - 1], games[i].category, STRING_SIZE) == 0 && tempGamesBufferRealSize < gamesCount)
			{
				tempGamesBuffer[tempGamesBufferRealSize] = games[i];
				++tempGamesBufferRealSize;
			}
		}

		// copy result game from temporary buffer to result array
		resultGamesTempArray = (Game*)malloc(sizeof(Game) * tempGamesBufferRealSize);

		if (resultGamesTempArray != NULL)
		{
			for (size_t i = 0; i < tempGamesBufferRealSize; ++i)
			{
				resultGamesTempArray[i] = tempGamesBuffer[i];
			}
		}
		else
		{
			printf_s("\nfindGameByPartOfName:: Memory not allocated for resultGamesTempArray!\n");
		}

		free(tempGamesBuffer);
		tempGamesBuffer = NULL;
	}
	else
	{
		printf_s("\nfindGameByPartOfName:: Memory not allocated for tempGamesBuffer!\n");
	}

	*sizeGamesResult = tempGamesBufferRealSize;
	*gamesResult = resultGamesTempArray;
}


static void findGameByPartOfName(const Game* games, const size_t gamesCount, Game** gamesResult, size_t* sizeGamesResult)
{
	Game* resultGamesTempArray = NULL;
	char userSearchQuery[100];
	//resive category from user

	puts("\nInput part of name or full name: ");
	while ((getchar()) != '\n');
	gets_s(userSearchQuery);

	int count_of_chars = 0;
	for (int i = 0; userSearchQuery[i] != '\0'; i++)
	{
		count_of_chars++;
	}

	//find and fill result games to the temporary buffer
	Game* tempGamesBuffer = (Game*)malloc(sizeof(Game) * gamesCount);
	size_t tempGamesBufferRealSize = 0;

	if (tempGamesBuffer != NULL)
	{
		for (size_t i = 0; i < gamesCount; ++i)
		{
			//jeśli żądanie wyszukiwania jest takie same, jak nazwa gry, dodać do buffera
			if (strncmp(games[i].name, userSearchQuery, count_of_chars) == 0 && tempGamesBufferRealSize < gamesCount)
			{
				tempGamesBuffer[tempGamesBufferRealSize] = games[i];
				++tempGamesBufferRealSize;
			}
		}

		// copy result game from temporary buffer to result array
		resultGamesTempArray = (Game*)malloc(sizeof(Game) * tempGamesBufferRealSize);

		if (resultGamesTempArray != NULL)
		{
			for (size_t i = 0; i < tempGamesBufferRealSize; ++i)
			{
				resultGamesTempArray[i] = tempGamesBuffer[i];
			}

			free(tempGamesBuffer);
			tempGamesBuffer = NULL;
		}
		else
		{
			printf_s("\nfindGameByPartOfName:: Memory not allocated for resultGamesTempArray!\n");
		}

		*sizeGamesResult = tempGamesBufferRealSize;
		*gamesResult = resultGamesTempArray;
	}
	else
	{
		puts("\nfindGameByPartOfName:: Memory not allocated for tempGamesBuffer!\n");
	}
}

//sortowanie według ceny
static void sortGamesByPrice(Game* games, const size_t gamesCount, bool sortLowToHigh)
{
	Game tmp;
	for (size_t i = 0; i < gamesCount - 1; i++) {
		for (size_t j = gamesCount - 1; j > i; j--) {
			if (sortLowToHigh)
			{
				if (games[j].price < games[j - 1].price)
				{
					tmp = games[j - 1];
					games[j - 1] = games[j];
					games[j] = tmp;
				}
			}
			else {
				if (games[j].price > games[j - 1].price)
				{
					tmp = games[j - 1];
					games[j - 1] = games[j];
					games[j] = tmp;
				}
			}
		}
	}
}

//sortowanie według id
static void sortGamesById(Game* games, const size_t gamesCount)
{
	Game tmp;
	for (size_t i = 0; i < gamesCount - 1; i++) {
		for (size_t j = gamesCount - 1; j > i; j--) {
			if (games[j].id < games[j - 1].id)
			{
				tmp = games[j - 1];
				games[j - 1] = games[j];
				games[j] = tmp;
			}

		}
	}
}

//sortowanie według daty wydania
static void sortGamesByReleaseDate(Game* games, const size_t gamesCount, bool sortLowToHigh)//sortLowToHigh to low
{
	Game tmp;
	for (size_t i = 0; i < gamesCount - 1; i++) {
		for (size_t j = gamesCount - 1; j > i; j--) {
			if (sortLowToHigh)
			{
				if (isOlder(games[j].releaseDate, games[j - 1].releaseDate))
				{
					tmp = games[j - 1];
					games[j - 1] = games[j];
					games[j] = tmp;
				}
			}
			else {
				if (!isOlder(games[j].releaseDate, games[j - 1].releaseDate))
				{
					tmp = games[j - 1];
					games[j - 1] = games[j];
					games[j] = tmp;
				}
			}
		}
	}
}

//poszukiwanie wśriód gier za pomocą id
static void findGameById(const Game* games, const size_t gamesCount, Game** gameResult)
{
	Game* resultGame;

	int userSearchIdQuerry = -1;
	printf("\nInput id number to find the game: ");
	scanf_s("%d", &userSearchIdQuerry);
	while (userSearchIdQuerry < 1 || userSearchIdQuerry > gamesCount)
	{
		printf("Incorrect command. Try again\n");
		scanf_s("%d", &userSearchIdQuerry);
	}


	resultGame = (Game*)malloc(sizeof(Game));

	if (resultGame != NULL)
	{
		for (size_t i = 0; i < gamesCount; ++i)
		{
			if (userSearchIdQuerry == games[i].id)
			{
				*resultGame = games[i];
				break;
			}
		}
	}
	else
	{
		puts("\nfindGameByPartOfName:: Memory not allocated for tempGamesBuffer!\n");
	}

	*gameResult = resultGame;
}

//dodawanie ceny do sumy
static void addGamePriceToSum(const Game* game, double* sum)
{
	*sum += game->price;
}
//znalerzenie zniżki
static int countDiscountForSum(const GamesStore* gamesStore)
{
	int discount = 0;

	if (100 < (gamesStore->currentSum) && (gamesStore->currentSum) <= 150)
	{
		discount = rand() % 5 + 5;
	}
	else if (150 < (gamesStore->currentSum) && (gamesStore->currentSum) <= 200)
	{
		discount = rand() % 5 + 10;
	}
	else if (200 < (gamesStore->currentSum) && (gamesStore->currentSum) <= 250)
	{
		discount = rand() % 5 + 15;
	}
	else if (250 < (gamesStore->currentSum) && (gamesStore->currentSum) <= 300)
	{
		discount = rand() % 5 + 20;
	}
	else if (300 < (gamesStore->currentSum))
	{
		discount = rand() % 5 + 25;
	}

	return discount;
}
//zapis wszystkich gier do pliku
void writeGamesStoreToFile(GamesStore gamesStore)
{

	FILE* file = NULL;

	errno_t err = fopen_s(&file, "CopyOfGamesStore.txt", "w");

	if (err == 0)
	{
		for (int i = 0; i < gamesStore.countGames; i++)
		{
			fprintf(file, "%s,%s,%lf,%d|%d|%d\n",
				gamesStore.games[i].name,
				gamesStore.games[i].category,
				gamesStore.games[i].price,
				gamesStore.games[i].releaseDate.day,
				gamesStore.games[i].releaseDate.month,
				gamesStore.games[i].releaseDate.year
			);
		}
		fprintf(file, "&\n");
		for (int i = 0; i < gamesStore.countGamesCategories; i++)
		{
			fprintf(file, "%s\n", gamesStore.gamesCategories[i]);
		}
	}
	else
	{
		printf("\ncan't write to file from path %s\n", "Cart.txt");
	}

	if (file)
	{
		err = fclose(file);
		if (err != 0)
		{
			printf("The file was not closed\n");
		}
	}
}
//dodawanie nowej gry do tablicy gier
void addNewGame(Game** games, size_t* countGames, char*** gamesCategories, size_t* countGamesCategories)
{
	*countGames = *countGames + 1;
	Game* copyOfGames = (Game*)malloc(sizeof(Game) * (*countGames));
	//kopiowanie zawartości starej tablicy 
	for (int i = 0; i < *countGames - 1; i++)
	{
		copyOfGames[i] = (*games)[i];
	}

	char buffer[500];
	printf("Write Name of Game,Name of Category,price and releaze date (necessarily as in the example)\n");
	printf("Like that: Name of game,Name of category,xxx.xx,xx|xx|xxxx\n");
	char ch = getchar();
	gets_s(buffer, 500);

	char seps[] = ",|";//znaki interpuncyjne oddzielające informacje o nowej grze
	char* token1 = NULL;//będzie przyjmował część stringu do znaku
	char* next_token1 = NULL;//będzie wkazywa miejsce po następnym znaku
	token1 = strtok_s(buffer, seps, &next_token1);
	//przypisanie identyfikatora do nowej gry
	copyOfGames[*countGames - 1].id = *countGames;
	printf("%d id of new game\n", copyOfGames[*countGames - 1].id);
	if (token1 != NULL)
	{
		//rezerwacja pamięci dla nazwy gry
		copyOfGames[*countGames - 1].name = (char*)malloc(sizeof(char) * (strlen(token1) + 1));
		//kopiowanie nazwy gry
		strcpy_s(copyOfGames[*countGames - 1].name, (strlen(token1) + 1), token1);
		//następny element przed znakiem interpuncyjnym
		token1 = strtok_s(NULL, seps, &next_token1);
		//jeżeli kategoria już istnieje, dodać do istniejącej
		bool NeedNewCategory = true;
		for (int i = 0; i < *countGamesCategories; i++)
		{
			if (_strnicmp(token1, (*gamesCategories)[i], strlen((*gamesCategories)[i])) == 0)
			{
				NeedNewCategory = false;
			}
		}
		//jeżeli użytkownik dodaje grę do nieistniejącej kategorii, stworzyć nową kategorię
		if (NeedNewCategory)
		{
			*countGamesCategories = *countGamesCategories + 1;
			char** copyCategory = (char**)malloc(sizeof(char*) * (*countGamesCategories));
			for (int i = 0; i < *countGamesCategories - 1; i++)
			{
				copyCategory[i] = (*gamesCategories)[i];
			}
			if (copyCategory[*countGamesCategories - 1] != NULL)
			{
				copyCategory[*countGamesCategories - 1] = (char*)malloc(sizeof(char) * strlen(token1) + 1);

				strcpy_s(copyCategory[*countGamesCategories - 1], (strlen(token1) + 1), token1);
			}

			free(*gamesCategories);
			*gamesCategories = copyCategory;
		}
		//rezerwacja pamięci do nazwy kategorii nowej gry
		copyOfGames[*countGames - 1].category = (char*)malloc(sizeof(char) * (strlen(token1) + 1));
		//kopiowanie nazwy kategorii do kategorii w środku structury Game
		if (copyOfGames[*countGames - 1].category != NULL)
			strcpy_s(copyOfGames[*countGames - 1].category, (strlen(token1) + 1), token1);
		token1 = strtok_s(NULL, seps, &next_token1);

		copyOfGames[*countGames - 1].price = atof(token1);
		token1 = strtok_s(NULL, seps, &next_token1);

		copyOfGames[*countGames - 1].releaseDate.day = atoi(token1);
		token1 = strtok_s(NULL, seps, &next_token1);

		copyOfGames[*countGames - 1].releaseDate.month = atoi(token1);
		token1 = strtok_s(NULL, seps, &next_token1);

		copyOfGames[*countGames - 1].releaseDate.year = atoi(token1);
		token1 = strtok_s(NULL, seps, &next_token1);

		free(*games);
		*games = copyOfGames;

	}
}

//funcja dla usuwania gry, wybranej za pomocą id
void deleteGame(Game** games, size_t* countGames)
{

	size_t newCountGames = *countGames - 1;
	Game* newGamesCopy = (Game*)malloc(sizeof(Game) * newCountGames);

	size_t userChoice;

	printf("Input id of the game that you want to remove: ");
	scanf_s("%Iu", &userChoice);

	while (userChoice < 1 || userChoice > *countGames)
	{
		printf("Incorrect input! Try again: ");
		scanf_s("%d", &userChoice);
	}
	//sortujemy według id żeby prawidlowo zmienić indeksy innych gier
	sortGamesById(*games, *countGames);

	bool gameIdWasFound = false;
	size_t shift = 1;
	for (int i = 0; i < *countGames; ++i)
	{
		//jeżęli znaleźliśmy grę, wprowadzoną przez użytkownika
		if (((*games) + i)->id == userChoice)
		{
			gameIdWasFound = true;
			//zwolnić pamięć
			free((*games)[i].name);
			free((*games)[i].category);
			//przejść do następnej iteracji
			continue;
		}
		//do gry, ktorą trzeba usunąć kopiujemy wszystkie gry
		if (!gameIdWasFound)
		{
			newGamesCopy[i] = (*games)[i];
		}
		else
		{
			//po usunięciu gry kopiujemy wszystkie gry z przesunięciem
			newGamesCopy[i - shift] = (*games)[i];
			newGamesCopy[i - shift].id = (*games)[i].id - 1;

		}
	}



	free(*games);
	*games = newGamesCopy;
	*countGames = newCountGames;
}