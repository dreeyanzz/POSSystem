#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

typedef struct
{
    char *operationName;
    ItemsEntryOperationType operationType;
    void *operationFunc;
} ItemOperationsFieldEntry;

static const char *pageTitle = "Item Operations Page";
static const int numColumns = 4;
static const int columnWidth = 35;
static const int maxEntriesToShow = 7;

static void pageHeader();
static void fillItemsDBEntries();
static void adjustEntriesToShow();
static void showItemsDBEntries();

static void showOperations();

static void removeItemEntry();
static void changeItemName();
static void changeStocks();
void changeItemPrice();

static FILE *itemsDB;
static ItemsDatabaseEntry *itemsDBEntries;
static int numItemsDBEntries;

static char *textToSearch;
static int numSearched;

static ItemsDatabaseEntry *itemsToShow;
static int numItemsToShow;
static int toShowStartIndex;
static int toShowEndIndex;

static int selectedEntryIndex;
static int selectedOperationIndex;

const int numOperations = 4;
static const ItemOperationsFieldEntry operations[] = {
    {
        "Remove Item Entry",
        REMOVE_ITEM_ENTRY,
        &removeItemEntry,
    },
    {
        "Change Item Name",
        CHANGE_ITEM_NAME,
        &changeItemName,
    },
    {
        "Change Stocks",
        CHANGE_ITEM_STOCKS,
        &changeStocks,
    },
    {
        "Change Price",
        CHANGE_ITEM_PRICE,
        &changeItemPrice,
    },
};

void ItemOperationsPage()
{
    itemsDB = fopen(itemsDatabasePath, "a+");

    numItemsDBEntries = 0;

    textToSearch = (char *)malloc(sizeof(char));
    textToSearch[0] = '\0';
    numSearched = 0;

    itemsToShow = (ItemsDatabaseEntry *)malloc(sizeof(ItemsDatabaseEntry));
    numItemsToShow = 0;
    toShowStartIndex = 0;

    selectedEntryIndex = 0;
    selectedOperationIndex = 0;

    while (true)
    {
        clearTerminal();

        numItemsDBEntries = countEntries(itemsDB);
        itemsDBEntries = (ItemsDatabaseEntry *)malloc(numItemsDBEntries * sizeof(ItemsDatabaseEntry));
        fillItemsDBEntries();

        pageHeader();
        printf("\n");
        if (numItemsDBEntries == 0)
        {
            printf("No items yet. Add some items to remove.\n");
            goto input;
        }

        numSearched = 0;
        for (int i = 0; i < numItemsDBEntries; i++)
        {
            ItemsDatabaseEntry *currentEntry = &itemsDBEntries[i];
            char *itemName = strdup(currentEntry->itemName);
            char *itemIdentifier = strdup(currentEntry->itemIdentifier);

            bool matchItemName = strstr(toLowercase(itemName), toLowercase(textToSearch)) != NULL;
            bool matchItemIdentifier = strstr(toLowercase(itemIdentifier), toLowercase(textToSearch)) != NULL;

            if (matchItemName || matchItemIdentifier)
            {
                numSearched++;

                itemsToShow = realloc(itemsToShow, numSearched * sizeof(ItemsDatabaseEntry));

                itemsToShow[numSearched - 1] = *currentEntry;
            }

            free(itemName);
            free(itemIdentifier);
        }

        printf("Total number of items: %d\n", numItemsDBEntries);
        printf("Number of results: %d\n", numSearched);
        printf("\n");

        printf("Search Item name / Item Identifier: %s", textToSearch);
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
        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ESCAPE)
            break;
        else if (key == KEY_ENTER)
            showOperations();
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

            case KEY_HOME:
                selectedEntryIndex = 0;
                break;

            case KEY_END:
                selectedEntryIndex = numSearched > 0 ? numSearched - 1 : 0;
                break;

            case KEY_BACKSPACE:
                if (strlen(textToSearch) > 0)
                {
                    size_t stringLength = strlen(textToSearch);
                    textToSearch[stringLength - 1] = '\0';
                    textToSearch = realloc(textToSearch, stringLength);

                    selectedEntryIndex = 0;
                }
                break;

            default:
            {
                bool emptyTemp = temp[0] == '\0';

                if (!emptyTemp)
                {
                    size_t stringLength = strlen(textToSearch) + strlen(temp) + 1;
                    textToSearch = realloc(textToSearch, stringLength);
                    strcat(textToSearch, temp);

                    selectedEntryIndex = 0;
                }
            }
            break;
            }

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
    printRow(columnWidth, numColumns,
             "Item Name",
             "Item Identifier",
             "Stocks",
             "Item Price");
    ansi_colorize_end();
    printf("\n");

    for (int i = toShowStartIndex; i <= toShowEndIndex; i++)
    {
        ItemsDatabaseEntry *entry = &itemsToShow[i];

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
                 entry->itemName,
                 entry->itemIdentifier,
                 inttoascii(entry->numStocks),
                 inttoascii(entry->itemPrice));
        ansi_colorize_end();
        printf("\n");
    }
    printf("\n");
}

void showOperations()
{

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        showItemsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        printf("Press [enter] to proceed.\n");
        for (int i = 0; i < numOperations; i++)
        {
            ItemOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_ENTER)
        {
            ((void (*)(void))operations[selectedOperationIndex].operationFunc)();
            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_TAB:
            case KEY_DOWN:
                selectedOperationIndex == numOperations - 1 ? selectedOperationIndex = 0 : selectedOperationIndex++;
                break;

            case KEY_UP:
                selectedOperationIndex == 0 ? selectedOperationIndex = numOperations - 1 : selectedOperationIndex--;
                break;
            }

        refreshDelay();
    }

    selectedOperationIndex = 0;
}

void removeItemEntry()
{
    fclose(itemsDB);

    char *itemIdentifier = strdup(itemsToShow[selectedEntryIndex].itemIdentifier);

    removeItemsDatabaseEntryByIdentifier(itemIdentifier);
    if (selectedEntryIndex > 0)
        selectedEntryIndex--;
    if (toShowStartIndex > 0)
        toShowStartIndex--;
    toShowEndIndex--;
    itemsDB = fopen(itemsDatabasePath, "a+");
}

void changeItemName()
{
    char *newItemName = (char *)malloc(sizeof(char));
    newItemName[0] = '\0';

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        showItemsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        for (int i = 0; i < numOperations; i++)
        {
            ItemOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }
        printf("\n");

        printf("Enter new item name: %s", newItemName);
        printWhiteHighlight();
        printf("\n");

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {
            fclose(itemsDB);
            char *itemIdentifier = strdup(itemsToShow[selectedEntryIndex].itemIdentifier);
            changeItemPropertyByIdentifier(itemIdentifier, CHANGE_ITEM_NAME, newItemName);
            itemsDB = fopen(itemsDatabasePath, "a+");

            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_BACKSPACE:
                if (strlen(newItemName) > 0)
                {
                    size_t stringLength = strlen(newItemName);
                    newItemName[stringLength - 1] = '\0';
                    newItemName = realloc(newItemName, stringLength);
                }
                break;

            default:
            {
                bool emptyTemp = temp[0] == '\0';

                if (!emptyTemp)
                {
                    int stringLength = strlen(newItemName) + strlen(temp) + 1;
                    newItemName = realloc(newItemName, stringLength);
                    strcat(newItemName, temp);
                }
            }
            break;
            }

        refreshDelay();
    }

    free(newItemName);
}

void changeStocks()
{

    int newNumStocks = 0;

    while (true)
    {

        clearTerminal();

        pageHeader();
        printf("\n");

        showItemsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        for (int i = 0; i < numOperations; i++)
        {
            ItemOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }
        printf("\n");

        printf("Enter number of stocks: %d", newNumStocks);
        printWhiteHighlight();
        printf("\n");

        KeyboardKey key = getKeyPressInsensitive();
        char c = mappedAlNum(key);

        if (key == KEY_ENTER)
        {
            fclose(itemsDB);
            char *itemIdentifier = strdup(itemsToShow[selectedEntryIndex].itemIdentifier);
            changeItemPropertyByIdentifier(itemIdentifier, CHANGE_ITEM_STOCKS, newNumStocks);
            itemsDB = fopen(itemsDatabasePath, "a+");

            free(itemIdentifier);
            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_BACKSPACE:
                newNumStocks /= 10;
                break;

            default:
                if (c >= '0' && c <= '9')
                    newNumStocks = ((newNumStocks) * 10) + (c - '0');
                break;
            }

        refreshDelay();
    }
}

void changeItemPrice()
{
    int newItemPrice = 0;

    while (true)
    {

        clearTerminal();

        pageHeader();
        printf("\n");

        showItemsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        for (int i = 0; i < numOperations; i++)
        {
            ItemOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }
        printf("\n");

        printf("Enter number of stocks: %d", newItemPrice);
        printWhiteHighlight();
        printf("\n");

        KeyboardKey key = getKeyPressInsensitive();
        char c = mappedAlNum(key);

        if (key == KEY_ENTER)
        {
            fclose(itemsDB);
            char *itemIdentifier = strdup(itemsToShow[selectedEntryIndex].itemIdentifier);
            changeItemPropertyByIdentifier(itemIdentifier, CHANGE_ITEM_PRICE, newItemPrice);
            itemsDB = fopen(itemsDatabasePath, "a+");

            free(itemIdentifier);
            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_BACKSPACE:
                newItemPrice /= 10;
                break;

            default:
                if (c >= '0' && c <= '9')
                    newItemPrice = ((newItemPrice) * 10) + (c - '0');
                break;
            }

        refreshDelay();
    }
}
