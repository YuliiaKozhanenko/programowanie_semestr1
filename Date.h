#ifndef DATA_HPP
#define DATA_HPP

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef struct
{
	int day;
	int month;
	int year;
} Date;

bool isOlder(Date firstDate, Date secondDate);

#endif