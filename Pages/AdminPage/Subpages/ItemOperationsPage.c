#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static const char *pageTitle = "Item Operations Page";
static const int numColumns = 4;
static const int columnWidth = 35;
static const int maxEntriesToShow = 7;

static void pageHeader();
static void fillItemsDBEntries();
static void adjustEntriesToShow();
static void showItemsDBEntries();

static void confirmRemoval();

static FILE *itemsDB;
static ItemsDatabaseEntry *itemsDBEntries;
static int numItemsDBEntries;

static int numItemsToShow;
static int toShowStartIndex = 0;
static int toShowEndIndex;

static int selectedEntryIndex = 0;

void ItemOperationsPage()
{

    itemsDB = fopen(itemsDatabasePath, "a+");

    while (true)
    {
        clearTerminal();

        numItemsDBEntries = countEntries(itemsDB);
        itemsDBEntries = (ItemsDatabaseEntry *)malloc(numItemsDBEntries * sizeof(ItemsDatabaseEntry));
        fillItemsDBEntries();

        numItemsToShow = numItemsDBEntries >= maxEntriesToShow ? maxEntriesToShow : numItemsDBEntries;
        toShowEndIndex = toShowStartIndex + numItemsToShow - 1;

        adjustEntriesToShow();

        pageHeader();
        printf("selectedEntryIndex: %d\n", selectedEntryIndex);
        printf("\n");

        if (numItemsDBEntries == 0)
        {
            printf("No items yet. Add some items to remove.\n");
            goto last;
        }
        showItemsDBEntries();

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_ESCAPE)
            break;
        else if (key == KEY_ENTER)
            confirmRemoval();
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

            case KEY_HOME:
                selectedEntryIndex = 0;
                break;

            case KEY_END:
                selectedEntryIndex = numItemsDBEntries - 1;
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
    printf("%s\n", pageTitle);
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
    printRow(columnWidth, numColumns,
             "Item Name",
             "Item Identifier",
             "Stocks",
             "Item Price");
    printf("\n");
    ansi_colorize_end();
    for (int i = toShowStartIndex; i <= toShowEndIndex; i++)
    {
        const ItemsDatabaseEntry entry = itemsDBEntries[i];

        if (i == selectedEntryIndex)
        {
            printColoredCursor();
            ansi_colorize_start((ANSI_SGR[]){ANSI_FG_BLACK, ANSI_BG_WHITE, ANSI_BOLD}, 3);
        }
        else
            printf("  ");

        if (i == toShowEndIndex)
            ansi_colorize_start((ANSI_SGR[]){ANSI_UNDERLINE}, 1);

        printRow(columnWidth, numColumns,
                 entry.itemName,
                 entry.itemIdentifier,
                 inttoascii(entry.numStocks),
                 inttoascii(entry.itemPrice));
        printf("\n");

        ansi_colorize_end();
    }
    printf("\n");
}

void confirmRemoval()
{
    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("selectedEntryIndex: %d\n", selectedEntryIndex);
        printf("\n");

        showItemsDBEntries();

        printf("Are you sure you want to remove this item? Please review before proceeding.\n");
        printf("Press [y] for yes, [n] for no.\n");

        KeyboardKey key2 = getKeyPressInsensitive();

        if (key2 == KEY_y)
        {
            fclose(itemsDB);

            char *itemIdentifier = strdup(itemsDBEntries[selectedEntryIndex].itemIdentifier);

            removeItemsDatabaseEntryByIdentifier(itemIdentifier);
            if (selectedEntryIndex > 0)
                selectedEntryIndex--;
            if (toShowStartIndex > 0)
                toShowStartIndex--;
            toShowEndIndex--;
            itemsDB = fopen(itemsDatabasePath, "a+");

            break;
        }
        else if (key2 == KEY_n)
            break;

        refreshDelay();
    }
}