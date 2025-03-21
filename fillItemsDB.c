#include <stdio.h>

int main(void)
{

    int numberOfEntries = 50;

    FILE *itemsDB = fopen("Databases/items.csv", "a+");

    for (int i = 0; i < numberOfEntries; i++)
    {
        fprintf(itemsDB, "Item Name %d|Item Identifier %d|Item Price %d\n", i, i, i);
    }

    return 0;
}