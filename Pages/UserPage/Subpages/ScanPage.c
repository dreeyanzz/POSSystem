#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static const int columnWidth = 35;
static const int maxEntriesToShow = 7;

static void pageHeader();
static void fillItemsDBEntries();
static void adjustEntriesToShow();
static void showItemsDBEntries();

static FILE *itemsDB;
static ItemsDatabaseEntry *itemsDBEntries;
static int numItemsDBEntries;

static int numItemsToShow;
static int toShowStartIndex = 0;
static int toShowEndIndex;

static int selectedEntryIndex = 0;

void ScanPage()
{

    itemsDB = fopen(itemsDatabasePath, "a+");

    while (true)
    {
        clearTerminal();

        numItemsDBEntries = countEntries(itemsDB);
        itemsDBEntries = (ItemsDatabaseEntry *)malloc(numItemsDBEntries * sizeof(ItemsDatabaseEntry));
        fillItemsDBEntries();

        adjustEntriesToShow();

        pageHeader();
        printf("selectedEntryIndex: %d\n", selectedEntryIndex);
        printf("\n");
        if (numItemsDBEntries == 0)
        {
            printf("No items yet. Please ask an administrator to add some.\n");
            goto last;
        }
        showItemsDBEntries();

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_UP:
                selectedEntryIndex == 0 ? selectedEntryIndex = numItemsDBEntries - 1 : selectedEntryIndex--;
                break;

            case KEY_TAB:
            case KEY_DOWN:
                selectedEntryIndex == numItemsDBEntries - 1 ? selectedEntryIndex = 0 : selectedEntryIndex++;
                break;

            default:
                break;
            }
    last:
        for (int i = 0; i < numItemsDBEntries; i++)
        {
            free(itemsDBEntries[i].itemName);
            free(itemsDBEntries[i].itemIdentifier);
        }
        free(itemsDBEntries);
        refreshDelay();
    }

    fclose(itemsDB);
}

void pageHeader()
{
    printf("Scan Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to go back.\n");
}

void fillItemsDBEntries()
{
    char buffer[2048];
    rewind(itemsDB);
    fgets(buffer, sizeof(buffer), itemsDB);
    int index = 0;
    while (fgets(buffer, sizeof(buffer), itemsDB))
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        char *dbItemName = strtok(buffer, "|");
        char *dbItemIdentifer = strtok(NULL, "|");
        char *dbNumStocks = strtok(NULL, "|");
        char *dbItemPrice = strtok(NULL, "|");

        itemsDBEntries[index] = (ItemsDatabaseEntry){
            .itemName = strdup(dbItemName),
            .itemIdentifier = strdup(dbItemIdentifer),
            .numStocks = atoi(dbNumStocks),
            .itemPrice = atoi(dbItemPrice),
        };

        index++;
    }
}

void adjustEntriesToShow()
{

    numItemsToShow = numItemsDBEntries >= maxEntriesToShow ? maxEntriesToShow : numItemsDBEntries;
    toShowEndIndex = toShowStartIndex + numItemsToShow - 1;

    if (selectedEntryIndex > toShowEndIndex)
    {
        while (selectedEntryIndex > toShowEndIndex)
        {
            toShowStartIndex++;
            toShowEndIndex++;
        }
    }
    if (selectedEntryIndex < toShowStartIndex)
    {
        while (selectedEntryIndex < toShowStartIndex)
        {
            toShowStartIndex--;
            toShowEndIndex--;
        }
    }
}

void showItemsDBEntries()
{
    printf("  ");
    ansi_colorize_start((ANSI_SGR[]){ANSI_UNDERLINE, ANSI_OVERLINE, ANSI_BOLD}, 3);
    printRow(columnWidth, 4,
             "Item Name",
             "Item Identifier",
             "Remaining Stocks",
             "Item Price");
    ansi_colorize_end();
    printf("\n");

    for (int i = toShowStartIndex; i <= toShowEndIndex; i++)
    {
        const ItemsDatabaseEntry currentEntry = itemsDBEntries[i];

        if (i == selectedEntryIndex)
        {
            printColoredCursor();
            ansi_colorize_start((ANSI_SGR[]){ANSI_FG_BLACK, ANSI_BG_WHITE, ANSI_BOLD}, 3);
        }
        else
            printf("  ");

        if (i == toShowEndIndex)
            ansi_colorize_start((ANSI_SGR[]){ANSI_UNDERLINE}, 1);
        printRow(columnWidth, 4,
                 currentEntry.itemName,
                 currentEntry.itemIdentifier,
                 inttoascii(currentEntry.numStocks),
                 inttoascii(currentEntry.itemPrice));
        printf("\n");
        ansi_colorize_end();
    }
    printf("\n");
}