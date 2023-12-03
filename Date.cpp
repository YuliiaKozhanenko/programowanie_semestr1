#include "Date.h"

#include<stdlib.h>

//funkcja w przypadku, gdy data nie musi być odczytywana z pliku
//void initializeDate(Date* date)
//{
//    date->year = rand() % 4 + 2019;
//    date->month = rand() % 12 + 1;
//
//    if (date->month == 2)
//    {
//        date->day = rand() % 28 + 1;
//    }
//    else if (date->month == 1
//        || date->month == 3
//        || date->month == 5
//        || date->month == 7
//        || date->month == 8
//        || date->month == 10
//        || date->month == 12
//        )
//    {
//        date->day = rand() % 31 + 1;
//    }
//    else
//    {
//        date->day = rand() % 30 + 1;
//    }
//}
//funkcja dla porównania dwóch dat
bool isOlder(Date firstDate, Date secondDate)
{
    if ((firstDate.year > secondDate.year) ||
        (firstDate.year == secondDate.year && firstDate.month > secondDate.month) ||
        (firstDate.year == secondDate.year && firstDate.month == secondDate.month && firstDate.day > secondDate.day))
    {
        return true;
    }
    else
    {
        return false;
    }
}