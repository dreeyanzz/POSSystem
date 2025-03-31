#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "header_files/RemoveAccountPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static void pageHeader();

const static int columnWidth = 35;

void RemoveAccountPage()
{
    initializeDatabases();

    FILE *accountsDatabase = fopen(accountsDatabasePath, "a+");
    if (!accountsDatabase)
    {
        perror("Error in RemoveAccountPage()");
        return;
    }

    AccountsDatabaseEntry *accountsDBEntries = (AccountsDatabaseEntry *)malloc(sizeof(AccountsDatabaseEntry));

    int selectedEntryIndex = 0;
    const int maxEntriesToShow = 5;

    int numEntries = countEntries(accountsDatabase);
    int numItemsToShow = (numEntries >= maxEntriesToShow ? maxEntriesToShow : numEntries);

    int toShowStartIndex = 0;
    int toShowEndIndex = (toShowStartIndex + numItemsToShow);

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        int numEntries = countEntries(accountsDatabase);
        int numItemsToShow = (numEntries >= maxEntriesToShow ? maxEntriesToShow : numEntries);
        accountsDBEntries = realloc(accountsDBEntries, numEntries * sizeof(AccountsDatabaseEntry));
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
        rewind(accountsDatabase);
        fgets(buffer, sizeof(buffer), accountsDatabase);
        int index = 0;
        while (fgets(buffer, sizeof(buffer), accountsDatabase))
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

        printf("Press [enter] to remove item.\n");
        printf("  ");
        printf("|");
        printCentered("Username", columnWidth);
        printf("|");
        printCentered("Password", columnWidth);
        printf("|");
        printCentered("Display Name", columnWidth);
        printf("|");
        printCentered("Account Type", columnWidth);
        printf("|");
        printCentered("Account Identifier", columnWidth);
        printf("|");
        for (int i = toShowStartIndex; i < toShowEndIndex; i++)
        {

            const AccountsDatabaseEntry currentEntry = accountsDBEntries[i];

            printf("\n");

            if (i == selectedEntryIndex)
            {
                printColoredCursor();
                ansi_colorize_start((ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_BLACK, ANSI_BOLD}, 3);
            }
            else
                printf("  ");
            printf("|");
            printCentered(currentEntry.username, columnWidth);
            printf("|");
            printCentered(currentEntry.password, columnWidth);
            printf("|");
            printCentered(currentEntry.displayName, columnWidth);
            printf("|");
            printCentered(currentEntry.accountType == admin ? "admin" : "user", columnWidth);
            printf("|");
            printCentered(currentEntry.identifier, columnWidth);
            printf("|");

            ansi_colorize_end();
        }
        printf("\n");

        KeyboardKey key = getKeyPressInsensitive();
        if (key == KEY_ESCAPE)
            break;
        if (key == KEY_ENTER && (strcmp(accountsDBEntries[selectedEntryIndex].identifier, "ehyxccqfugqXgM6Q7QXKPChO3iPffF8o") != 0))
        {
            while (true)
            {
                clearTerminal();

                pageHeader();
                printf("\n");

                printf("Press [enter] to remove item.\n");
                printf("  ");
                printf("|");
                printCentered("Username", columnWidth);
                printf("|");
                printCentered("Password", columnWidth);
                printf("|");
                printCentered("Display Name", columnWidth);
                printf("|");
                printCentered("Account Type", columnWidth);
                printf("|");
                printCentered("Account Identifier", columnWidth);
                printf("|");
                for (int i = toShowStartIndex; i < toShowEndIndex; i++)
                {

                    const AccountsDatabaseEntry currentEntry = accountsDBEntries[i];

                    printf("\n");

                    if (i == selectedEntryIndex)
                    {
                        printColoredCursor();
                        ansi_colorize_start((ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_BLACK, ANSI_BOLD}, 3);
                    }
                    else
                        printf("  ");

                    printf("|");
                    printCentered(currentEntry.username, columnWidth);
                    printf("|");
                    printCentered(currentEntry.password, columnWidth);
                    printf("|");
                    printCentered(currentEntry.displayName, columnWidth);
                    printf("|");
                    printCentered(currentEntry.accountType == admin ? "admin" : "user", columnWidth);
                    printf("|");
                    printCentered(currentEntry.identifier, columnWidth);
                    printf("|");

                    ansi_colorize_end();
                }
                printf("\n");

                printf("\nAre you sure you want to remove this item? Please review before proceeding.\n");
                printf("Press [y] for yes, [n] for no.\n");

                KeyboardKey key2 = getKeyPressInsensitive();
                if (key2 == KEY_y)
                {
                    fclose(accountsDatabase);
                    removeAccountDatabaseEntryByIdentifier(strdup(accountsDBEntries[selectedEntryIndex].identifier));
                    accountsDatabase = fopen(accountsDatabasePath, "a+");
                    break;
                }
                else if (key2 == KEY_n)
                    break;

                refreshDelay();
            }
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

    free(accountsDBEntries);
    fclose(accountsDatabase);
}

static void pageHeader()
{
    printf("Remove Account Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to go back.\n");
}