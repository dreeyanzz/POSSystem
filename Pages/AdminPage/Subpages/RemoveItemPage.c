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

int countEntries(FILE *database);

void RemoveItemPage(void)
{
    initializeDatabases();

    FILE *itemsDatabase = fopen(itemsDatabasePath, "a+");
    if (!itemsDatabase)
    {
        perror("Failed to open itemsDatabase");
        return;
    }

    const int maxNumEntriesToShow = 5;
    int columnWidth = 30;

    size_t numberOfEntries = countEntries(itemsDatabase);
    ItemsDatabaseEntry *itemsDatabaseEntries = (ItemsDatabaseEntry *)malloc(sizeof(ItemsDatabaseEntry));
    if (!itemsDatabaseEntries)
    {
        perror("Memory allocation error");
        fclose(itemsDatabase);
        return;
    }

    int selectedEntryIndex = 0;
    int toShowStartIndex = 0;
    int toShowEndIndex = (numberOfEntries > maxNumEntriesToShow) ? maxNumEntriesToShow : numberOfEntries;

    while (true)
    {
        clearTerminal();

        printf("Remove Item Page\n");
        printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
        printf("Press [enter] to remove item.\n");
        printf("Press [esc] to go back.\n");
        printf("\n");

        char buffer[1024];
        numberOfEntries = countEntries(itemsDatabase);
        rewind(itemsDatabase);
        itemsDatabaseEntries = realloc(itemsDatabaseEntries, (numberOfEntries * sizeof(ItemsDatabaseEntry)));
        fgets(buffer, sizeof(buffer), itemsDatabase); // Skip header
        for (size_t i = 0; i < numberOfEntries; i++)
        {
            if (!fgets(buffer, sizeof(buffer), itemsDatabase))
                break;

            char *dbItemName = strtok(buffer, "|");
            char *dbItemIdentifier = strtok(NULL, "|");
            char *dbItemPrice = strtok(NULL, "|");

            itemsDatabaseEntries[i].itemName = strdup(dbItemName);
            itemsDatabaseEntries[i].itemIdentifier = strdup(dbItemIdentifier);
            itemsDatabaseEntries[i].itemPrice = atoi(dbItemPrice);
        }

        // Adjust the visible window based on the selected entry
        if (selectedEntryIndex < toShowStartIndex)
        {
            toShowStartIndex = selectedEntryIndex;
            toShowEndIndex = toShowStartIndex + maxNumEntriesToShow;
        }
        else if (selectedEntryIndex >= toShowEndIndex)
        {
            toShowEndIndex = selectedEntryIndex + 1;
            toShowStartIndex = toShowEndIndex - maxNumEntriesToShow;
        }

        // Ensure the window indices stay within bounds
        if (toShowStartIndex < 0)
        {
            toShowStartIndex = 0;
            toShowEndIndex = (numberOfEntries > maxNumEntriesToShow) ? maxNumEntriesToShow : numberOfEntries;
        }
        if (toShowEndIndex > (int)numberOfEntries)
        {
            toShowEndIndex = numberOfEntries;
            toShowStartIndex = (numberOfEntries >= maxNumEntriesToShow) ? numberOfEntries - maxNumEntriesToShow : 0;
        }

        while (selectedEntryIndex > numberOfEntries - 1)
            selectedEntryIndex--;

        // Print header row
        printf("  ");
        printf("|");
        printCentered("Item Name", columnWidth);
        printf("|");
        printCentered("Item Identifier", columnWidth);
        printf("|");
        printCentered("Item Price", columnWidth);
        printf("|");
        printf("\n");

        // Print entries in the visible window
        for (int i = toShowStartIndex; i < toShowEndIndex; i++)
        {
            if (i == selectedEntryIndex)
            {
                printf("%s", coloredCusor());
                ansi_colorize_start((ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_BLACK}, 2);
            }
            else
                printf("  ");

            printf("|");
            printCentered(itemsDatabaseEntries[i].itemName, columnWidth);
            printf("|");
            printCentered(itemsDatabaseEntries[i].itemIdentifier, columnWidth);
            printf("|");
            printCentered(inttoascii(itemsDatabaseEntries[i].itemPrice), columnWidth);
            printf("|");
            ansi_colorize_end();

            printf("\n");
        }

        KeyboardKey key = getKeyPress();
        if (key == KEY_ESCAPE)
            break;

        switch (key)
        {
        case KEY_UP:
        {
            (selectedEntryIndex == 0) ? selectedEntryIndex = (numberOfEntries - 1) : selectedEntryIndex--;
            break;
        }

        case KEY_DOWN:
        case KEY_TAB:
        {
            (selectedEntryIndex == (numberOfEntries - 1)) ? selectedEntryIndex = 0 : selectedEntryIndex++;
            break;
        }
        default:
            break;
        }

        refreshDelay();
    }

    // Free allocated memory
    for (size_t i = 0; i < numberOfEntries; i++)
    {
        free(itemsDatabaseEntries[i].itemName);
        free(itemsDatabaseEntries[i].itemIdentifier);
    }
    free(itemsDatabaseEntries);
    fclose(itemsDatabase);
}

int countEntries(FILE *database)
{
    int numberOfEntries = 0;
    char buffer[1024];
    rewind(database);
    fgets(buffer, sizeof(buffer), database); // Skip header
    while (fgets(buffer, sizeof(buffer), database))
        numberOfEntries++;
    rewind(database);

    return numberOfEntries;
}
