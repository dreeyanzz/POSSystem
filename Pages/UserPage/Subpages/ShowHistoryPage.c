#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static int columnWidth = 35;
static int maxEntriesToShow = 7;

static void pageHeader();
int getMonthNumber(const char *monthName);

static char currentCashierFilePath[2048];

static FILE *cashierFile;

static SaleEntry *saleEntries;
static int numSaleEntry;

static char *textToSearch;

static SaleEntry *entriesToShow;
static int numSearched;

static int selectedEntryIndex;
static int numItemsToShow;
static int toShowStartIndex;
static int toShowEndIndex;

void ShowHistoryPage()
{
    initializeDatabases();

    snprintf(currentCashierFilePath,
             sizeof(currentCashierFilePath),
             "%s/%s%s",
             transactionsFolderPath,
             userData.identifier,
             ".csv");

    cashierFile = fopen(currentCashierFilePath, "a+");

    saleEntries = (SaleEntry *)malloc(sizeof(SaleEntry));
    numSaleEntry = 0;

    textToSearch = (char *)malloc(sizeof(char));
    textToSearch[0] = '\0';

    entriesToShow = (SaleEntry *)malloc(sizeof(SaleEntry));
    numSearched = 0;

    selectedEntryIndex = 0;
    numItemsToShow = 0;
    toShowStartIndex = 0;

    while (true)
    {
        clearTerminal();

        numSaleEntry = countEntries(cashierFile);
        saleEntries = realloc(saleEntries, numSaleEntry * sizeof(SaleEntry));

        rewind(cashierFile);
        char dbSaleEntry[2048];
        fgets(dbSaleEntry, sizeof(dbSaleEntry), cashierFile);
        int index = 0;
        while (fgets(dbSaleEntry, sizeof(dbSaleEntry), cashierFile))
        {
            dbSaleEntry[strcspn(dbSaleEntry, "\n")] = '\0';

            char *cashierName = strdup(userData.name);
            char *dbItemName = strtok(dbSaleEntry, "|");
            char *dbItemIdentifier = strtok(NULL, "|");
            char *dbStocksSold = strtok(NULL, "|");
            char *dbDateRaw = strtok(NULL, "|");

            struct tm tm = {0};
            char month[20];
            int day, year;

            sscanf(dbDateRaw, "%s %d, %d", month, &day, &year);

            // Convert month name to month number (0-11)
            int monthNumber = getMonthNumber(month);

            tm.tm_mday = day;
            tm.tm_mon = monthNumber;
            tm.tm_year = year - 1900;

            saleEntries[index] = (SaleEntry){
                .cashierName = strdup(cashierName),
                .itemName = strdup(dbItemName),
                .itemIdentifier = strdup(dbItemIdentifier),
                .stocksSold = atoi(dbStocksSold),
                .dateRaw = strdup(dbDateRaw),
                .timestamp = mktime(&tm),
            };

            index++;
        }

        qsort(entriesToShow, numItemsToShow, sizeof(SaleEntry), compareDates);

        pageHeader();
        printf("\n");

        if (numSaleEntry == 0)
        {
            printf("No transaction history yet. Make a transaction first.\n");
            goto input;
        }

        numSearched = 0;
        for (int i = 0; i < numSaleEntry; i++)
        {
            SaleEntry *currentEntry = &saleEntries[i];
            char *itemName = strdup(currentEntry->itemName);
            char *itemIdentifier = strdup(currentEntry->itemIdentifier);

            bool matchItemName = strstr(toLowercase(itemName), toLowercase(textToSearch)) != NULL;
            bool matchItemIdentifier = strstr(toLowercase(itemIdentifier), toLowercase(textToSearch)) != NULL;

            if (matchItemName || matchItemIdentifier)
            {
                numSearched++;
                entriesToShow = realloc(entriesToShow, numSearched * sizeof(SaleEntry));

                entriesToShow[numSearched - 1] = *currentEntry;
            }

            free(itemName);
            free(itemIdentifier);
        }

        printf("Search Item name / Item Identifier: %s", textToSearch);
        printWhiteHighlight();
        printf("\n");

        if (numSearched == 0)
        {
            printf("No items matched.\n");
            goto input;
        }

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

        printf("  ");
        ansi_colorize_start((ANSI_SGR[]){ANSI_UNDERLINE, ANSI_OVERLINE, ANSI_BOLD}, 3);
        printRow(columnWidth, 4,
                 "Item Name",
                 "Item Identifier",
                 "Stocks Sold",
                 "Date");
        ansi_colorize_end();
        printf("\n");

        for (int i = toShowStartIndex; i <= toShowEndIndex; i++)
        {
            SaleEntry *currentSaleEntry = &entriesToShow[i];

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
                     currentSaleEntry->itemName,
                     currentSaleEntry->itemIdentifier,
                     inttoascii(currentSaleEntry->stocksSold),
                     currentSaleEntry->dateRaw);
            ansi_colorize_end();
            printf("\n");
        }

    input:
        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ESCAPE)
        {
            break;
        }
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

        refreshDelay();
    }

    fclose(cashierFile);

    for (int i = 0; i < numSaleEntry; i++)
    {
        SaleEntry *currentEntry = &saleEntries[i];

        free(currentEntry->cashierName);
        free(currentEntry->itemName);
        free(currentEntry->itemIdentifier);
        free(currentEntry->dateRaw);
    }
    free(saleEntries);
    free(entriesToShow);
    free(textToSearch);
}

void pageHeader()
{
    printf("Show History Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to go back.\n");
}

int getMonthNumber(const char *monthName)
{
    static const char *months[] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December",
    };
    for (int i = 0; i < 12; i++)
    {
        if (strcasecmp(monthName, months[i]) == 0)
            return i;
    }
    return -1; // Invalid month
}
