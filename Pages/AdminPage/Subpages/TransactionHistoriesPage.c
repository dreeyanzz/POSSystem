#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

typedef enum
{
    DEFAULT,
    BY_DATE,
} SortMode;

static const int columnWidth = 30;
static const int numOfColumns = 5;
static const int numMaxEntriesToShow = 7;

static void pageHeader();

static void initializeSaleEntries();
void fillSaleEntries(const char *dbDisplayName, const char *dbIdentifier);

static SaleEntry *saleEntries;
static int numSaleEntries;

static char *textToSearch;
static SaleEntry *entriesToShow;
static int numEntriesSearched;

static int selectedEntryIndex;
static int numEntriesToShow;
static int toShowStartIndex;
static int toShowEndIndex;

void TransactionHistoriesPage()
{
    saleEntries = (SaleEntry *)malloc(sizeof(SaleEntry));
    numSaleEntries = 0;

    textToSearch = (char *)malloc(sizeof(char));
    textToSearch[0] = '\0';
    entriesToShow = (SaleEntry *)malloc(sizeof(SaleEntry));
    numEntriesSearched = 0;

    selectedEntryIndex = 0;
    numEntriesToShow = 0;
    toShowStartIndex = 0;

    while (true)
    {
        clearTerminal();

        numSaleEntries = 0;
        initializeSaleEntries();

        pageHeader();
        printf("numSaleEntries: %d\n", numSaleEntries);
        printf("\n");

        if (numSaleEntries == 0)
        {
            printf("No sales yet.\n");
            goto input;
        }

        printf("Search cashier name, item name, or date: %s", textToSearch);
        printWhiteHighlight();
        printf("\n");

        numEntriesSearched = 0;
        for (int i = 0; i < numSaleEntries; i++)
        {
            SaleEntry *currentEntry = &saleEntries[i];
            char *cashierName = strdup(currentEntry->cashierName);
            char *itemName = strdup(currentEntry->itemName);
            char *date = strdup(currentEntry->dateRaw);

            bool matchCashierName = strstr(toLowercase(cashierName), toLowercase(textToSearch)) != NULL;
            bool matchItemName = strstr(toLowercase(itemName), toLowercase(textToSearch)) != NULL;
            bool matchDate = strstr(toLowercase(date), toLowercase(textToSearch)) != NULL;

            if (matchCashierName || matchItemName || matchDate)
            {
                numEntriesSearched++;
                entriesToShow = realloc(entriesToShow, numEntriesSearched * sizeof(SaleEntry));
                entriesToShow[numEntriesSearched - 1] = *currentEntry;
            }

            free(cashierName);
            free(itemName);
            free(date);
        }

        if (numEntriesSearched == 0)
        {
            printf("No results matched.\n");
            goto input;
        }

        numEntriesToShow = numEntriesSearched >= numMaxEntriesToShow ? numMaxEntriesToShow : numEntriesSearched;
        toShowEndIndex = toShowStartIndex + numEntriesToShow - 1;

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
        if (selectedEntryIndex >= numEntriesSearched)
        {
            while (selectedEntryIndex >= numEntriesSearched)
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
        printRow(columnWidth, numOfColumns,
                 "Cashier name",
                 "Item name",
                 "Item identifier",
                 "Stocks sold",
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
            printRow(columnWidth, 5,
                     currentSaleEntry->cashierName,
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
            break;
        else
            switch (key)
            {
            case KEY_UP:
                selectedEntryIndex == 0 ? selectedEntryIndex = numEntriesSearched - 1 : selectedEntryIndex--;
                break;

            case KEY_TAB:
            case KEY_DOWN:
                selectedEntryIndex == numEntriesSearched - 1 ? selectedEntryIndex = 0 : selectedEntryIndex++;
                break;

            case KEY_HOME:
                selectedEntryIndex = 0;
                break;

            case KEY_END:
                selectedEntryIndex = numEntriesSearched > 0 ? numEntriesSearched - 1 : 0;
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

    free(saleEntries);
}

void pageHeader()
{
    printf("Transaction Histories Page\n");
    printf("Current Datettime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to go back.\n");
}

void initializeSaleEntries()
{
    FILE *accountsDB = fopen(accountsDatabasePath, "a+");
    rewind(accountsDB);
    char accountInfo[2048];
    fgets(accountInfo, sizeof(accountInfo), accountsDB);
    while (fgets(accountInfo, sizeof(accountInfo), accountsDB))
    {
        accountInfo[strcspn(accountInfo, "\n")] = '\0';

        char *dbUsername = strtok(accountInfo, "|");
        char *dbPassword = strtok(NULL, "|");
        char *dbDisplayName = strtok(NULL, "|");
        char *dbStatus = strtok(NULL, "|");
        char *dbIdentifier = strtok(NULL, "|");

        bool isCashier = strcmp(dbStatus, "user") == 0;

        if (isCashier)
            fillSaleEntries(dbDisplayName, dbIdentifier);
    }
    fclose(accountsDB);
}

void fillSaleEntries(const char *dbDisplayName, const char *dbIdentifier)
{
    char cashierFilePath[2048];
    int numCashierTransactions = 0;
    SaleEntry *cashierSaleEntries = (SaleEntry *)malloc(sizeof(SaleEntry));

    snprintf(cashierFilePath,
             sizeof(cashierFilePath),
             "%s/%s%s",
             transactionsFolderPath,
             dbIdentifier,
             ".csv");

    FILE *cashierFile = fopen(cashierFilePath, "a+");
    if (cashierFile == NULL)
    {
        initializeTransactionsFolder();
    }
    rewind(cashierFile);
    char transactionEntry[2048];
    fgets(transactionEntry, sizeof(transactionEntry), cashierFile);
    while (fgets(transactionEntry, sizeof(transactionEntry), cashierFile))
    {
        transactionEntry[strcspn(transactionEntry, "\n")] = '\0';

        char *dbItemName = strtok(transactionEntry, "|");
        char *dbItemIdentifier = strtok(NULL, "|");
        char *dbNumStocksSold = strtok(NULL, "|");
        char *dbDateTimeRaw = strtok(NULL, "|");

        struct tm tm = {0};
        char month[20];
        int day, year;

        sscanf(dbDateTimeRaw, "%s %d, %d", month, &day, &year);

        tm.tm_mday = day;
        tm.tm_mon = getMonthNumber(month);
        tm.tm_year = year - 1900;

        numCashierTransactions++;
        cashierSaleEntries = realloc(cashierSaleEntries, numCashierTransactions * sizeof(SaleEntry));
        cashierSaleEntries[numCashierTransactions - 1] = (SaleEntry){
            .cashierName = strdup(dbDisplayName),
            .itemName = strdup(dbItemName),
            .itemIdentifier = strdup(dbItemIdentifier),
            .stocksSold = atoi(dbNumStocksSold),
            .dateRaw = strdup(dbDateTimeRaw),
            .timestamp = mktime(&tm),
        };
    }
    fclose(cashierFile);

    qsort(cashierSaleEntries, numCashierTransactions, sizeof(SaleEntry), compareDates);

    for (int i = 0; i < numCashierTransactions; i++)
    {
        SaleEntry *currentEntry = &cashierSaleEntries[i];

        numSaleEntries++;
        saleEntries = realloc(saleEntries, numSaleEntries * sizeof(SaleEntry));
        saleEntries[numSaleEntries - 1] = *currentEntry;
    }
}