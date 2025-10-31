#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header_files/AccountOperationsPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

typedef struct
{
    char *operationName;
    AccountsEntryOperationType operationType;
    void *operationFunc;
} AccountOperationsFieldEntry;

static const int columnWidth = 35;
static const int maxEntriesToShow = 7;

static void pageHeader();
static void fillAccountsDBEntries();
static void adjustEntriesToShow();
static void showAccountsDBEntries();

static int selectedOperationIndex;
static void showOperations();

static void removeAccountEntry();
static void changeAccountUsername();
static void changeAccountPassword();
static void changeAccountDisplayName();
static void changeAccountStatus();

static FILE *accountsDB;
static AccountsDatabaseEntry *accountsDBEntries;
static int numAccountsDBEntries;

static AccountsDatabaseEntry *itemsToShow;
static int numItemsToShow;
static int toShowStartIndex;
static int toShowEndIndex;

static char *textToSearch;
static int numSearched;

static int selectedEntryIndex = 0;

static const int numOperations = 5;
static const AccountOperationsFieldEntry operations[] = {
    {
        "Remove Account",
        REMOVE_ITEM_ENTRY,
        &removeAccountEntry,
    },
    {
        "Change Account Username",
        CHANGE_ACCOUNT_USERNAME,
        &changeAccountUsername,
    },
    {
        "Change Account Password",
        CHANGE_ACCOUNT_PASSWORD,
        &changeAccountPassword,
    },
    {
        "Change Account Display Name",
        CHANGE_ACCOUNT_DISPLAYNAME,
        &changeAccountDisplayName,
    },
    {
        "Change Account Status",
        CHANGE_ACCOUNT_STATUS,
        changeAccountStatus,
    },
};

void AccountOperationsPage()
{
    accountsDB = fopen(accountsDatabasePath, "a+");

    numAccountsDBEntries = 0;

    itemsToShow = (AccountsDatabaseEntry *)malloc(sizeof(AccountsDatabaseEntry));
    numItemsToShow = 0;
    toShowStartIndex = 0;

    textToSearch = (char *)malloc(sizeof(char));
    textToSearch[0] = '\0';
    numSearched = 0;

    selectedEntryIndex = 0;

    while (true)
    {
        clearTerminal();

        numAccountsDBEntries = countEntries(accountsDB);
        accountsDBEntries = (AccountsDatabaseEntry *)malloc(numAccountsDBEntries * sizeof(AccountsDatabaseEntry));
        fillAccountsDBEntries();

        pageHeader();
        printf("\n");
        if (numAccountsDBEntries == 0)
        {
            printf("No accounts yet. Add some accounts to remove.\n");
            goto input;
        }

        numSearched = 0;
        for (int i = 0; i < numAccountsDBEntries; i++)
        {
            AccountsDatabaseEntry *currentEntry = &accountsDBEntries[i];
            char *accountUsername = strdup(currentEntry->username);
            char *accountDisplayName = strdup(currentEntry->displayName);

            bool matchItemName = strstr(toLowercase(accountUsername), toLowercase(textToSearch)) != NULL;
            bool matchItemIdentifier = strstr(toLowercase(accountDisplayName), toLowercase(textToSearch)) != NULL;

            if (matchItemName || matchItemIdentifier)
            {
                numSearched++;

                itemsToShow = realloc(itemsToShow, numSearched * sizeof(AccountsDatabaseEntry));

                itemsToShow[numSearched - 1] = *currentEntry;
            }

            free(accountUsername);
            free(accountDisplayName);
        }

        printf("Total number of accounts: %d\n", numAccountsDBEntries);
        printf("Number of results: %d\n", numSearched);
        printf("\n");

        printf("Search account username / account display name: %s", textToSearch);
        printWhiteHighlight();
        printf("\n");

        if (numSearched == 0)
        {
            printf("No accounts matched.\n");
            goto input;
        }

        adjustEntriesToShow();
        showAccountsDBEntries();

    input:
        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ESCAPE)
            break;
        else if (key == KEY_ENTER)
        {
            bool isAdmin = strcmp(accountsDBEntries[selectedEntryIndex].identifier, "ehyxccqfugqXgM6Q7QXKPChO3iPffF8o") == 0;

            if (!isAdmin)
                showOperations();
        }
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

        for (int i = 0; i < numAccountsDBEntries; i++)
        {
            free(accountsDBEntries[i].username);
            free(accountsDBEntries[i].password);
            free(accountsDBEntries[i].displayName);
            free(accountsDBEntries[i].identifier);
        }
        free(accountsDBEntries);

        refreshDelay();
    }

    for (int i = 0; i < numAccountsDBEntries; i++)
    {
        free(accountsDBEntries[i].username);
        free(accountsDBEntries[i].password);
        free(accountsDBEntries[i].displayName);
        free(accountsDBEntries[i].identifier);
    }
    free(accountsDBEntries);
    fclose(accountsDB);
}

void pageHeader()
{
    printf("Account Operations Page\n");
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
        AccountsDatabaseEntry *entry = &itemsToShow[i];

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
                 entry->username,
                 entry->password,
                 entry->displayName,
                 entry->accountType == admin ? "admin" : "user",
                 entry->identifier);
        ansi_colorize_end();
        printf("\n");
    }
    printf("\n");
}

void showOperations()
{
    selectedOperationIndex = 0;

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        showAccountsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        printf("Press [enter] to proceed.\n");
        for (int i = 0; i < numOperations; i++)
        {
            AccountOperationsFieldEntry currentOperation = operations[i];

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

void removeAccountEntry()
{
    fclose(accountsDB);

    char *accountIdentifier = strdup(itemsToShow[selectedEntryIndex].identifier);

    removeAccountDatabaseEntryByIdentifier(accountIdentifier);
    if (selectedEntryIndex > 0)
        selectedEntryIndex--;
    if (toShowStartIndex > 0)
        toShowStartIndex--;
    toShowEndIndex--;
    accountsDB = fopen(accountsDatabasePath, "a+");
}

void changeAccountUsername()
{
    char *newAccountUsername = (char *)malloc(sizeof(char));
    newAccountUsername[0] = '\0';

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        showAccountsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        printf("Press [enter] to proceed.\n");
        for (int i = 0; i < numOperations; i++)
        {
            AccountOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }
        printf("\n");

        printf("Enter new account username: %s", newAccountUsername);
        printWhiteHighlight();
        printf("\n");

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {
            fclose(accountsDB);
            char *accountIdentifier = strdup(itemsToShow[selectedEntryIndex].identifier);
            changeAccountPropertyByIdentifier(accountIdentifier, CHANGE_ACCOUNT_USERNAME, newAccountUsername);
            accountsDB = fopen(accountsDatabasePath, "a+");

            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_BACKSPACE:
                if (strlen(newAccountUsername) > 0)
                {
                    size_t stringLength = strlen(newAccountUsername);
                    newAccountUsername[stringLength - 1] = '\0';
                    newAccountUsername = realloc(newAccountUsername, stringLength);
                }
                break;

            default:
            {
                bool emptyTemp = temp[0] == '\0';

                if (!emptyTemp)
                {
                    int stringLength = strlen(newAccountUsername) + strlen(temp) + 1;
                    newAccountUsername = realloc(newAccountUsername, stringLength);
                    strcat(newAccountUsername, temp);
                }
            }
            break;
            }

        refreshDelay();
    }

    free(newAccountUsername);
}

void changeAccountPassword()
{
    char *newAccountPassword = (char *)malloc(sizeof(char));
    newAccountPassword[0] = '\0';

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        showAccountsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        printf("Press [enter] to proceed.\n");
        for (int i = 0; i < numOperations; i++)
        {
            AccountOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }
        printf("\n");

        printf("Enter new account password: %s", newAccountPassword);
        printWhiteHighlight();
        printf("\n");

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {
            fclose(accountsDB);
            char *accountIdentifier = strdup(itemsToShow[selectedEntryIndex].identifier);
            changeAccountPropertyByIdentifier(accountIdentifier, CHANGE_ACCOUNT_PASSWORD, newAccountPassword);
            accountsDB = fopen(accountsDatabasePath, "a+");

            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_BACKSPACE:
                if (strlen(newAccountPassword) > 0)
                {
                    size_t stringLength = strlen(newAccountPassword);
                    newAccountPassword[stringLength - 1] = '\0';
                    newAccountPassword = realloc(newAccountPassword, stringLength);
                }
                break;

            default:
            {
                bool emptyTemp = temp[0] == '\0';

                if (!emptyTemp)
                {
                    int stringLength = strlen(newAccountPassword) + strlen(temp) + 1;
                    newAccountPassword = realloc(newAccountPassword, stringLength);
                    strcat(newAccountPassword, temp);
                }
            }
            break;
            }

        refreshDelay();
    }

    free(newAccountPassword);
}

void changeAccountDisplayName()
{
    char *newAccountDisplayName = (char *)malloc(sizeof(char));
    newAccountDisplayName[0] = '\0';

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        showAccountsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        printf("Press [enter] to proceed.\n");
        for (int i = 0; i < numOperations; i++)
        {
            AccountOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }
        printf("\n");

        printf("Enter new account display name: %s", newAccountDisplayName);
        printWhiteHighlight();
        printf("\n");

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {
            fclose(accountsDB);
            char *accountIdentifier = strdup(itemsToShow[selectedEntryIndex].identifier);
            changeAccountPropertyByIdentifier(accountIdentifier, CHANGE_ACCOUNT_DISPLAYNAME, newAccountDisplayName);
            accountsDB = fopen(accountsDatabasePath, "a+");

            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_BACKSPACE:
                if (strlen(newAccountDisplayName) > 0)
                {
                    size_t stringLength = strlen(newAccountDisplayName);
                    newAccountDisplayName[stringLength - 1] = '\0';
                    newAccountDisplayName = realloc(newAccountDisplayName, stringLength);
                }
                break;

            default:
            {
                bool emptyTemp = temp[0] == '\0';

                if (!emptyTemp)
                {
                    int stringLength = strlen(newAccountDisplayName) + strlen(temp) + 1;
                    newAccountDisplayName = realloc(newAccountDisplayName, stringLength);
                    strcat(newAccountDisplayName, temp);
                }
            }
            break;
            }

        refreshDelay();
    }

    free(newAccountDisplayName);
}

void changeAccountStatus()
{
    bool isAdmin = false;

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        showAccountsDBEntries();
        printf("\n");

        printf("Select an operation\n");
        printf("Press [enter] to proceed.\n");
        for (int i = 0; i < numOperations; i++)
        {
            AccountOperationsFieldEntry currentOperation = operations[i];

            i == selectedOperationIndex ? printColoredCursor() : printf("  ");

            printf("%s\n", currentOperation.operationName);
        }
        printf("\n");

        printf("New account status: <-%s->\n", isAdmin ? "Admin" : "User");

        KeyboardKey key = getKeyPress();
        if (key == KEY_ENTER)
        {
            fclose(accountsDB);
            char *accountIdentifier = strdup(itemsToShow[selectedEntryIndex].identifier);
            changeAccountPropertyByIdentifier(accountIdentifier, CHANGE_ACCOUNT_STATUS, isAdmin ? ADMIN : USER);
            accountsDB = fopen(accountsDatabasePath, "a+");

            break;
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_RIGHT:
            case KEY_LEFT:
                isAdmin = !isAdmin;
                break;
            }

        refreshDelay();
    }
}