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

static void pageHeader();

const static int columnWidth = 35;

void RemoveItemPage(void)
{
    initializeDatabases();

    FILE *itemsDB = fopen(itemsDatabasePath, "a+");

    ItemsDatabaseEntry *itemsDBEntries = (ItemsDatabaseEntry *)malloc(sizeof(ItemsDatabaseEntry));

    int selectedEntryIndex = 0;
    const int maxEntriesToShow = 5;

    int numEntries = countEntries(itemsDB);
    int numItemsToShow = (numEntries >= maxEntriesToShow ? maxEntriesToShow : numEntries);

    int toShowStartIndex = 0;
    int toShowEndIndex = (toShowStartIndex + numItemsToShow);

    while (true)
    {
        clearTerminal();

        pageHeader();
        !itemsDB ? printf("Error opening Items Database\n") : printf("");
        printf("\n");

        int numEntries = countEntries(itemsDB);
        int numItemsToShow = (numEntries >= maxEntriesToShow ? maxEntriesToShow : numEntries);
        itemsDBEntries = realloc(itemsDBEntries, numEntries * sizeof(ItemsDatabaseEntry));
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

        char buffer[1024];
        rewind(itemsDB);
        fgets(buffer, sizeof(buffer), itemsDB);
        int index = 0;
        while (fgets(buffer, sizeof(buffer), itemsDB))
        {
            buffer[strcspn(buffer, "\n")] = '\0';

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

        printf("Press [enter] to remove item.\n");
        printf("  ");
        printf("|");
        printCentered("Item Name", columnWidth);
        printf("|");
        printCentered("Item Identifier", columnWidth);
        printf("|");
        printCentered("Item Price", columnWidth);
        printf("|");

        for (int i = toShowStartIndex; i < toShowEndIndex; i++)
        {

            const ItemsDatabaseEntry currentEntry = itemsDBEntries[i];

            printf("\n");

            if (i == selectedEntryIndex)
            {
                printColoredCursor();
                ansi_colorize_start((ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_BLACK, ANSI_BOLD}, 3);
            }
            else
                printf("  ");

            printf("|");
            printCentered(currentEntry.itemName, columnWidth);
            printf("|");
            printCentered(currentEntry.itemIdentifier, columnWidth);
            printf("|");
            printCentered(inttoascii(currentEntry.itemPrice), columnWidth);
            printf("|");

            ansi_colorize_end();
        }
        printf("\n");

        KeyboardKey key = getKeyPressInsensitive();
        if (key == KEY_ENTER)
        {
            while (true)
            {

                clearTerminal();

                pageHeader();
                printf("\n");

                printf("Press [enter] to remove item.\n");
                for (int i = toShowStartIndex; i < toShowEndIndex; i++)
                {

                    const ItemsDatabaseEntry currentEntry = itemsDBEntries[i];

                    if (i == selectedEntryIndex)
                    {
                        printColoredCursor();
                        ansi_colorize_start((ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_BLACK, ANSI_BOLD}, 3);
                    }
                    else
                        printf("  ");
                    printf("|");
                    printCentered(currentEntry.itemName, columnWidth);
                    printf("|");
                    printCentered(currentEntry.itemIdentifier, columnWidth);
                    printf("|");
                    printCentered(inttoascii(currentEntry.itemPrice), columnWidth);
                    printf("|");
                    ansi_colorize_end();

                    printf("\n");
                }
                printf("\n");

                printf("Are you sure you want to remove this item? Please review before proceeding.\n");
                printf("Press [y] for yes, [n] for no.\n");

                KeyboardKey key2 = getKeyPressInsensitive();

                if (key2 == KEY_y)
                {
                    fclose(itemsDB);
                    removeItemsDatabaseEntryByIdentifier(strdup(itemsDBEntries[selectedEntryIndex].itemIdentifier));
                    itemsDB = fopen(itemsDatabasePath, "a+");
                    break;
                }
                else if (key2 == KEY_n)
                    break;

                refreshDelay();
            }
        }
        if (key == KEY_ESCAPE)
            break;

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

static void pageHeader()
{
    printf("Remove Item Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to go back.\n");
}