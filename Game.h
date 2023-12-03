#ifndef GAME_HPP
#define GAME_HPP

#include <stdio.h>

#include "Date.h"

#define STRING_SIZE 30


typedef struct
{
	int id; 
	char* name;
	char* category;
	double price;
	Date releaseDate;
} Game;

#endif