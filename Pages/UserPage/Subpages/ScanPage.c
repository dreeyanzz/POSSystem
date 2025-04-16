#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

typedef struct
{
    ItemsDatabaseEntry entry;
    int quantity;
} SelectedItem;

static const int columnWidth = 35;
static const int maxEntriesToShow = 7;

static void pageHeader();
static void fillItemsDBEntries();
static void adjustEntriesToShow();
static void showItemsDBEntries();

static FILE *itemsDB;
static ItemsDatabaseEntry *itemsDBEntries;
static ItemsDatabaseEntry *itemsToShow;
static int numItemsDBEntries;

static char *itemToSearch;
static int numSearched;

static SelectedItem *selectedItems;
static int numSelectedItems;

static int numItemsToShow;
static int toShowStartIndex = 0;
static int toShowEndIndex;

static int selectedEntryIndex = 0;

void ScanPage()
{

    itemsDB = fopen(itemsDatabasePath, "a+");
    itemToSearch = (char *)malloc(sizeof(char));

    selectedItems = (SelectedItem *)malloc(sizeof(SelectedItem));

    itemToSearch[0] = '\0';

    while (true)
    {
        clearTerminal();

        numItemsDBEntries = countEntries(itemsDB);
        itemsDBEntries = (ItemsDatabaseEntry *)malloc(numItemsDBEntries * sizeof(ItemsDatabaseEntry));
        fillItemsDBEntries();

        pageHeader();
        printf("selectedEntryIndex: %d\n", selectedEntryIndex);
        printf("numSelectedItems: %d\n", numSelectedItems);
        printf("\n");

        if (numItemsDBEntries == 0)
        {
            printf("No items yet. Please ask an administrator to add some.\n");
            goto last;
        }

        numSearched = 0;
        for (int i = 0; i < numItemsDBEntries; i++)
        {
            ItemsDatabaseEntry currentEntry = itemsDBEntries[i];
            char *itemName = strdup(currentEntry.itemName);

            if (strstr(toLowercase(itemName), toLowercase(itemToSearch)) != NULL)
            {
                numSearched++;

                itemsToShow = realloc(itemsToShow, numSearched * sizeof(ItemsDatabaseEntry));

                itemsToShow[numSearched - 1] = currentEntry;
            }
        }

        printf("Search Item: %s", itemToSearch);
        printWhiteHighlight();
        printf("\n");

        if (numSearched == 0)
        {
            printf("No items matched.\n");
            goto input;
        }

        adjustEntriesToShow();
        showItemsDBEntries();

    input:
        KeyboardKey key = getKeyPressInsensitive();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_UP:
                selectedEntryIndex == 0 ? selectedEntryIndex = numSearched - 1 : selectedEntryIndex--;
                break;

            case KEY_TAB:
            case KEY_DOWN:
                selectedEntryIndex == numSearched - 1 ? selectedEntryIndex = 0 : selectedEntryIndex++;
                break;

            case KEY_ENTER:
                break;

            case KEY_BACKSPACE:
                if (strlen(itemToSearch) > 0)
                {
                    size_t stringLength = strlen(itemToSearch);
                    itemToSearch[stringLength - 1] = '\0';
                    itemToSearch = realloc(itemToSearch, stringLength);

                    selectedEntryIndex = 0;
                }
                break;

            case KEY_HOME:
                selectedEntryIndex = 0;
                break;

            case KEY_END:
                selectedEntryIndex = numSearched > 0 ? numSearched - 1 : 0;
                break;

            default:
            {
                bool emptyTemp = temp[0] == '\0';

                if (!emptyTemp && !isdigit(temp[0]))
                {
                    size_t stringLength = strlen(itemToSearch) + strlen(temp) + 1;
                    itemToSearch = realloc(itemToSearch, stringLength);
                    strcat(itemToSearch, temp);

                    selectedEntryIndex = 0;
                }
                if (c >= '0' && c <= '9')
                {
                    ItemsDatabaseEntry selectedEntry = itemsToShow[selectedEntryIndex];

                    bool isSelected = false;

                    int i;
                    for (i = 0; i < numSelectedItems; i++)
                    {
                        SelectedItem currentSelectedItem = selectedItems[i];

                        if (strcmp(selectedEntry.itemIdentifier, currentSelectedItem.entry.itemIdentifier) == 0)
                        {
                            isSelected = true;
                            break;
                        }
                    }

                    if (isSelected)
                        selectedItems[i].quantity = (selectedItems[i].quantity * 10) + (c - '0');
                    else
                    {
                        numSelectedItems++;
                        selectedItems = realloc(selectedItems, numSelectedItems * sizeof(SelectedItem));
                        selectedItems[numSelectedItems - 1] = (SelectedItem){
                            {
                                .itemName = strdup(selectedEntry.itemName),
                                .itemIdentifier = strdup(selectedEntry.itemIdentifier),
                                .numStocks = selectedEntry.numStocks,
                                .itemPrice = selectedEntry.itemPrice,
                            },
                            c - '0',
                        };
                    }
                }
            }
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
    numItemsToShow = numSearched >= maxEntriesToShow ? maxEntriesToShow : numSearched;
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
    if (selectedEntryIndex >= numSearched)
    {
        while (selectedEntryIndex >= numSearched)
        {
            if (selectedEntryIndex > 0)
                selectedEntryIndex--;
            if (toShowStartIndex > 0)
                toShowStartIndex--;
            toShowEndIndex--;
        }
    }
}

void showItemsDBEntries()
{
    printf("  ");
    ansi_colorize_start((ANSI_SGR[]){ANSI_UNDERLINE, ANSI_OVERLINE, ANSI_BOLD}, 3);
    printRow(columnWidth, 5,
             "Item Name",
             "Item Identifier",
             "Remaining Stocks",
             "Item Price",
             "Quantity");
    ansi_colorize_end();
    printf("\n");

    for (int i = toShowStartIndex; i <= toShowEndIndex; i++)
    {
        const ItemsDatabaseEntry currentEntry = itemsToShow[i];

        bool isSelected = false;

        int j;
        SelectedItem currentSelectedItem;
        for (j = 0; j < numSelectedItems; j++)
        {
            currentSelectedItem = selectedItems[j];

            if (strcmp(currentEntry.itemIdentifier, currentSelectedItem.entry.itemIdentifier) == 0)
            {
                isSelected = true;
                break;
            }
        }

        if (i == selectedEntryIndex)
        {
            printColoredCursor();
            ansi_colorize_start((ANSI_SGR[]){ANSI_FG_BLACK, ANSI_BG_WHITE, ANSI_BOLD}, 3);
        }
        else
            printf("  ");

        if (i == toShowEndIndex)
            ansi_colorize_start((ANSI_SGR[]){ANSI_UNDERLINE}, 1);
        printRow(columnWidth, 5,
                 currentEntry.itemName,
                 currentEntry.itemIdentifier,
                 inttoascii(currentEntry.numStocks),
                 inttoascii(currentEntry.itemPrice),
                 inttoascii(isSelected ? currentSelectedItem.quantity : 0));
        printf("\n");
        ansi_colorize_end();
    }
    printf("\n");
}