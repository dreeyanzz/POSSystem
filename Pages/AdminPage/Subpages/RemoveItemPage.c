#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "header_files/RemoveItemPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

typedef struct
{
    char *itemName;
    char *itemIdentifier;
    int itemPrice;
} ItemsDatabaseEntry;

void printHeader();

const int columnWidth = 30;

void RemoveItemPage(void)
{
    initializeDatabases();

    FILE *itemsDB = fopen(itemsDatabasePath, "a+");

    ItemsDatabaseEntry *itemsDBEntries = (ItemsDatabaseEntry *)malloc(sizeof(ItemsDatabaseEntry));
    ItemsDatabaseEntry *itemsToShow = (ItemsDatabaseEntry *)malloc(sizeof(ItemsDatabaseEntry));

    int cursor = 0;
    int selectedEntryIndex = 0;
    const int maxEntriesToShow = 5;

    int numEntries = countEntries(itemsDB);
    int numItemsToShow = (numEntries >= maxEntriesToShow ? maxEntriesToShow : numEntries);

    int toShowStartIndex = 0;
    int toShowEndIndex = (toShowStartIndex + numItemsToShow);

    while (true)
    {
        clearTerminal();

        printHeader();

        int numEntries = countEntries(itemsDB);
        int numItemsToShow = (numEntries >= maxEntriesToShow ? maxEntriesToShow : numEntries);
        itemsDBEntries = realloc(itemsDBEntries, numEntries * sizeof(ItemsDatabaseEntry));
        itemsToShow = realloc(itemsToShow, numItemsToShow * sizeof(ItemsDatabaseEntry));
        int toShowEndIndex = (toShowStartIndex + numItemsToShow);

        if (selectedEntryIndex >= toShowEndIndex)
        {
            toShowEndIndex++;
            toShowStartIndex++;
        }
        if (selectedEntryIndex < toShowStartIndex)
        {
            toShowStartIndex--;
            toShowEndIndex--;
        }

        printf("itemsDB entries Count: %d\n", countEntries(itemsDB));
        printf("selectedEntryIndex = %d\n", selectedEntryIndex);

        char buffer[1024];
        rewind(itemsDB);
        fgets(buffer, sizeof(buffer), itemsDB);
        int index = 0;
        while (fgets(buffer, sizeof(buffer), itemsDB))
        {
            char *dbItemName = strtok(buffer, "|");
            char *dbItemIdentifier = strtok(NULL, "|");
            char *dbItemPrice = strtok(NULL, "|");

            itemsDBEntries[index] = (ItemsDatabaseEntry){
                .itemName = strdup(dbItemName),
                .itemIdentifier = strdup(dbItemIdentifier),
                .itemPrice = atoi(dbItemPrice),
            };

            index++;
        }

        for (int i = toShowStartIndex; i < toShowEndIndex; i++)
        {
            i == selectedEntryIndex ? printColoredCursor() : printf("  ");

            const ItemsDatabaseEntry currentEntry = itemsDBEntries[i];

            printf("|");
            printCentered(currentEntry.itemName, columnWidth);
            printf("|");
            printCentered(currentEntry.itemIdentifier, columnWidth);
            printf("|");
            printCentered(inttoascii(currentEntry.itemPrice), columnWidth);
            printf("|");

            printf("\n");
        }

        KeyboardKey key = getKeyPressInsensitive();
        if (key == KEY_ENTER)
        {
            break;
        }
        if (key == KEY_ESCAPE)
        {
            break;
        }

        switch (key)
        {
        case KEY_TAB:
        case KEY_DOWN:
        {
            selectedEntryIndex = (selectedEntryIndex == (numEntries - 1) ? 0 : (selectedEntryIndex + 1));

            break;
        }

        case KEY_UP:
        {
            selectedEntryIndex = ((selectedEntryIndex - 1) < 0 ? (numEntries - 1) : (selectedEntryIndex - 1));

            break;
        }

        default:
            break;
        }

        refreshDelay();
    }

    free(itemsDBEntries);
    fclose(itemsDB);
}

void printHeader()
{
    printf("Remove Item Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [enter] to remove item.\n");
    printf("Press [esc] to go back.\n");
}