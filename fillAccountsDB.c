#include <stdio.h>
#include "Tools/Tools.h"

int main(void)
{

    int numberOfEntries = 50;

    FILE *itemsDB = fopen("Databases/accounts.csv", "a+");

    for (int i = 0; i < numberOfEntries; i++)
    {
        int randomBool = rand() % 2; // Generates 0 (false) or 1 (true)

        fprintf(itemsDB, "Username%d|%s|Display Name %d|%s|%s\n",
                i,
                generateRandomString(5),
                i,
                randomBool ? "admin" : "user",
                generateAccountIdentifier());
    }

    return 0;
}