#include <stdio.h>
#include "Tools/Tools.h"

int main(void)
{

    int numberOfEntries = 50;

    FILE *itemsDB = fopen("Databases/accounts.csv", "a+");

    for (int i = 0; i < numberOfEntries; i++)
    {
        fprintf(itemsDB, "Username %d|Password %d|Display Name %d|Status %d|%s\n", i, i, i, i, generateAccountIdentifier());
    }

    return 0;
}