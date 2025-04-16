#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Tools/Tools.c"

int main(void)
{

    srand(time(NULL));

    int numberOfEntries = 50;
    int minPrice = 10;
    int maxPrice = 50;

    FILE *itemsDB = fopen("Databases/items.csv", "a+");

    for (int i = 0; i < numberOfEntries; i++)
    {
        int randomNumber = minPrice + rand() % (maxPrice - minPrice + 1);

        fprintf(itemsDB, "Item Name %d|%d|%d|%d\n", i, generateItemIdentifier(), randomNumber, randomNumber);
    }

    return 0;
}