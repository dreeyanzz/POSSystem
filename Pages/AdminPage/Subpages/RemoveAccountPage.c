#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header_files/RemoveAccountPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static const int columnWidth = 35;
static const int maxEntriesToShow = 7;

static void pageHeader();
static void fillAccountsDBEntries();
static void adjustEntriesToShow();
static void showAccountsDBEntries();

static void confirmRemoval();

static FILE *accountsDB;
static AccountsDatabaseEntry *accountsDBEntries;
static int numAccountsDBEntries;

static int numItemsToShow;
static int toShowStartIndex = 0;
static int toShowEndIndex;

static int selectedEntryIndex = 0;

void RemoveAccountPage()
{
    accountsDB = fopen(accountsDatabasePath, "a+");

    while (true)
    {
        clearTerminal();

        numAccountsDBEntries = countEntries(accountsDB);
        accountsDBEntries = (AccountsDatabaseEntry *)malloc(numAccountsDBEntries * sizeof(AccountsDatabaseEntry));
        fillAccountsDBEntries();

        numItemsToShow = numAccountsDBEntries >= maxEntriesToShow ? maxEntriesToShow : numAccountsDBEntries;
        toShowEndIndex = toShowStartIndex + numItemsToShow - 1;

        adjustEntriesToShow();

        pageHeader();
        printf("selectedEntryIndex: %d\n", selectedEntryIndex);
        printf("\n");

        showAccountsDBEntries();

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_ESCAPE)
            break;
        else if (key == KEY_ENTER)
        {
            bool isAdmin = strcmp(accountsDBEntries[selectedEntryIndex].identifier, "ehyxccqfugqXgM6Q7QXKPChO3iPffF8o") == 0;

            if (!isAdmin)
                confirmRemoval();
        }
        else
            switch (key)
            {
            case KEY_UP:
                selectedEntryIndex == 0 ? selectedEntryIndex = numAccountsDBEntries - 1 : selectedEntryIndex--;
                break;

            case KEY_TAB:
            case KEY_DOWN:
                selectedEntryIndex == numAccountsDBEntries - 1 ? selectedEntryIndex = 0 : selectedEntryIndex++;
                break;

            default:
                break;
            }

        free(accountsDBEntries);
        refreshDelay();
    }

    fclose(accountsDB);
}

void pageHeader()
{
    printf("Remove Account Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to go back.\n");
}

void fillAccountsDBEntries()
{
    char buffer[2048];
    rewind(accountsDB);
    fgets(buffer, sizeof(buffer), accountsDB); // Skip header line
    int index = 0;
    while (fgets(buffer, sizeof(buffer), accountsDB))
    {

        buffer[strcspn(buffer, "\n")] = '\0';

        char *dbUsername = strtok(buffer, "|");
        char *dbPassword = strtok(NULL, "|");
        char *dbDisplayName = strtok(NULL, "|");
        char *dbStatus = strtok(NULL, "|");
        char *dbIdentifier = strtok(NULL, "|");

        accountsDBEntries[index] = (AccountsDatabaseEntry){
            .username = strdup(dbUsername),
            .password = strdup(dbPassword),
            .displayName = strdup(dbDisplayName),
            .accountType = strcmp(dbStatus, "admin") == 0 ? admin : user,
            .identifier = strdup(dbIdentifier),
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

void showAccountsDBEntries()
{
    printf("  ");
    ansi_colorize_start((ANSI_SGR[]){ANSI_UNDERLINE, ANSI_OVERLINE, ANSI_BOLD}, 3);
    printRow(columnWidth, 5,
             "Username",
             "Password",
             "Display Name",
             "Status",
             "Identifier");
    printf("\n");
    ansi_colorize_end();
    for (int i = toShowStartIndex; i <= toShowEndIndex; i++)
    {
        const AccountsDatabaseEntry entry = accountsDBEntries[i];

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
                 entry.username,
                 entry.password,
                 entry.displayName,
                 entry.accountType == admin ? "admin" : "user",
                 entry.identifier);
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

        showAccountsDBEntries();

        printf("Are you sure you want to remove this account? Please review before proceeding.\n");
        printf("Press [y] for yes, [n] for no.\n");

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_y)
        {
            fclose(accountsDB);

            char *accountIdentifier = strdup(accountsDBEntries[selectedEntryIndex].identifier);

            removeAccountDatabaseEntryByIdentifier(accountIdentifier);
            if (selectedEntryIndex > 0)
                selectedEntryIndex--;
            if (toShowStartIndex > 0)
                toShowStartIndex--;
            toShowEndIndex--;
            accountsDB = fopen(accountsDatabasePath, "a+");

            break;
        }
        else if (key == KEY_n)
            break;

        refreshDelay();
    }
}