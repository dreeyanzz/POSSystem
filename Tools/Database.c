#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "Tools.h"
#include "Database.h"

const char *databasesPath = "Databases";
char accountsDatabasePath[256]; // Databases/accounts.csv
char itemsDatabasePath[256];    // Databases/items.csv
char isOpenDatabasePath[256];   // Databases/isOpen.csv
const char *accountsDatabaseName = "accounts.csv";
const char *itemsDatabaseName = "items.csv";
const char *isOpenDatabaseName = "isOpen.csv";

void initializeDatabases()
{
    snprintf(accountsDatabasePath, sizeof(accountsDatabasePath), "%s/%s", databasesPath, accountsDatabaseName); // Databases/accounts.csv
    snprintf(itemsDatabasePath, sizeof(itemsDatabasePath), "%s/%s", databasesPath, itemsDatabaseName);          // Databases/items.csv
    snprintf(isOpenDatabasePath, sizeof(isOpenDatabasePath), "%s/%s", databasesPath, isOpenDatabaseName);       // Databases/isOpen.csv

    createFolderIfNotExists(databasesPath);
    InitializeAccountsDatabase();
    InitializeItemsDatabase();
}

void InitializeAccountsDatabase()
{
    FILE *file = fopen(accountsDatabasePath, "r");
    if (file == NULL)
    {
        file = fopen(accountsDatabasePath, "w");

        enrollAccount("username", "password", "displayName", "status", "identifier");
        enrollAccount("admin", "admin", "admin", "admin", generateAccountIdentifier());
    }

    fclose(file);
}

void enrollAccount(const char *username, const char *password, const char *displayName, const char *status, const char *identifier)
{
    FILE *accountsDatabase = fopen(accountsDatabasePath, "a");
    if (accountsDatabase == NULL)
    {
        initializeDatabases();
    }

    fprintf(accountsDatabase, "%s|%s|%s|%s|%s\n", username, password, displayName, status, identifier);

    fclose(accountsDatabase);
}

void InitializeItemsDatabase()
{

    FILE *itemsDatabase = fopen(itemsDatabasePath, "r");
    if (itemsDatabase == NULL)
    {
        itemsDatabase = fopen(itemsDatabasePath, "w");

        fprintf(itemsDatabase, "Item Name|Item Identifier|Item Remaining Stocks|Item Price\n");
    }

    fclose(itemsDatabase);
}

void AddItemToDatabase(const char *itemName, const char *identifier, const long numStocks, const long price)
{
    FILE *itemsDatabase = fopen(itemsDatabasePath, "a");
    if (itemsDatabase == NULL)
    {
        initializeDatabases();
    }

    fprintf(itemsDatabase, "%s|%s|%d|%d\n", itemName, identifier, numStocks, price);

    fclose(itemsDatabase);
}

void removeItemsDatabaseEntryByIdentifier(const char *identifier)
{
    const char *tempItemsDatabaseName = "temp.csv";

    size_t allocatedSize = strlen(databasesPath) + 1 /* slash */ + strlen(tempItemsDatabaseName) + 1 /* null terminator */;
    char *newItemsDatabasePath = malloc(allocatedSize);
    snprintf(newItemsDatabasePath, allocatedSize, "%s/%s", databasesPath, tempItemsDatabaseName);

    FILE *itemsDatabase = fopen(itemsDatabasePath, "r");
    FILE *newItemsDatabase = fopen(newItemsDatabasePath, "w");

    fprintf(newItemsDatabase, "Item Name|Item Identifier|Item Remaining Stocks|Item Price\n"); // header

    rewind(itemsDatabase);
    char buffer[1024];
    fgets(buffer, sizeof(buffer), itemsDatabase);
    while (fgets(buffer, sizeof(buffer), itemsDatabase))
    {
        char *dbItemName = strtok(buffer, "|");
        char *dbItemIdentifier = strtok(NULL, "|");
        char *dbItemRemainingStocks = strtok(NULL, "|");
        char *dbItemPrice = strtok(NULL, "|");

        if (strcmp(identifier, dbItemIdentifier) != 0)
            fprintf(newItemsDatabase, "%s|%s|%s|%s", dbItemName, dbItemIdentifier, dbItemRemainingStocks, dbItemPrice);
    }

    fclose(itemsDatabase);
    fclose(newItemsDatabase);

    remove(itemsDatabasePath);
    rename(newItemsDatabasePath, itemsDatabasePath);

    free(newItemsDatabasePath);
}

void changeItemPropertyByIdentifier(const char *identifier, ItemsEntryOperationType operationType, ...)
{
    va_list args;
    va_start(args, operationType);

    const char *tempItemsDatabaseName = "temp.csv";

    size_t allocatedSize = strlen(databasesPath) + 1 /* slash */ + strlen(tempItemsDatabaseName) + 1 /* null terminator */;
    char *newItemsDatabasePath = malloc(allocatedSize);
    snprintf(newItemsDatabasePath, allocatedSize, "%s/%s", databasesPath, tempItemsDatabaseName);

    FILE *itemsDatabase = fopen(itemsDatabasePath, "r");
    FILE *newItemsDatabase = fopen(newItemsDatabasePath, "w");

    fprintf(newItemsDatabase, "Item Name|Item Identifier|Item Remaining Stocks|Item Price\n"); // header

    rewind(itemsDatabase);
    char buffer[1024];
    fgets(buffer, sizeof(buffer), itemsDatabase);
    while (fgets(buffer, sizeof(buffer), itemsDatabase))
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        char *dbItemName = strtok(buffer, "|");
        char *dbItemIdentifier = strtok(NULL, "|");
        char *dbItemRemainingStocks = strtok(NULL, "|");
        char *dbItemPrice = strtok(NULL, "|");

        if (strcmp(identifier, dbItemIdentifier) == 0)
        {
            switch (operationType)
            {
            case CHANGE_ITEM_NAME:
                fprintf(newItemsDatabase, "%s|%s|%s|%s\n", va_arg(args, char *), dbItemIdentifier, dbItemRemainingStocks, dbItemPrice);
                break;

            case CHANGE_ITEM_IDENTIFIER:
                fprintf(newItemsDatabase, "%s|%s|%s|%s\n", dbItemName, va_arg(args, char *), dbItemRemainingStocks, dbItemPrice);
                break;

            case CHANGE_ITEM_STOCKS:
                fprintf(newItemsDatabase, "%s|%s|%d|%s\n", dbItemName, dbItemIdentifier, va_arg(args, int), dbItemPrice);
                break;

            case CHANGE_ITEM_PRICE:
                fprintf(newItemsDatabase, "%s|%s|%s|%d\n", dbItemName, dbItemIdentifier, dbItemRemainingStocks, va_arg(args, int));
                break;
            }

            continue;
        }
        fprintf(newItemsDatabase, "%s|%s|%s|%s\n", dbItemName, dbItemIdentifier, dbItemRemainingStocks, dbItemPrice);
    }

    fclose(itemsDatabase);
    fclose(newItemsDatabase);

    remove(itemsDatabasePath);
    rename(newItemsDatabasePath, itemsDatabasePath);

    free(newItemsDatabasePath);

    va_end(args);
}

void removeAccountDatabaseEntryByIdentifier(const char *identifier)
{

    const char *tempAccountsDatabaseName = "temp.csv";

    size_t allocatedSize = strlen(databasesPath) + 1 /* slash */ + strlen(tempAccountsDatabaseName) + 1 /* null terminator */;
    char *newAccountsDatabasePath = malloc(allocatedSize);
    snprintf(newAccountsDatabasePath, allocatedSize, "%s/%s", databasesPath, tempAccountsDatabaseName);

    FILE *accountsDatabase = fopen(accountsDatabasePath, "r");
    FILE *newAccountsDatabase = fopen(newAccountsDatabasePath, "w");

    fprintf(newAccountsDatabase, "username|password|displayName|status|identifier\n"); // header

    rewind(accountsDatabase);
    char buffer[1024];
    fgets(buffer, sizeof(buffer), accountsDatabase);
    while (fgets(buffer, sizeof(buffer), accountsDatabase))
    {
        char *dbUsername = strtok(buffer, "|");
        char *dbPassword = strtok(NULL, "|");
        char *dbDisplayName = strtok(NULL, "|");
        char *dbStatus = strtok(NULL, "|");
        char *dbAccountItentifier = strtok(NULL, "|");

        dbAccountItentifier[strcspn(dbAccountItentifier, "\n")] = '\0';

        if (strcmp(identifier, dbAccountItentifier) != 0)
            fprintf(newAccountsDatabase, "%s|%s|%s|%s|%s\n", dbUsername, dbPassword, dbDisplayName, dbStatus, dbAccountItentifier);
    }

    fclose(accountsDatabase);
    fclose(newAccountsDatabase);

    remove(accountsDatabasePath);
    rename(newAccountsDatabasePath, accountsDatabasePath);

    free(newAccountsDatabasePath);
}

int countEntries(FILE *db)
{

    int numEntries = 0;

    char buffer[1024];
    rewind(db);                        // put back the pointer to the top
    fgets(buffer, sizeof(buffer), db); // Skip header
    while (fgets(buffer, sizeof(buffer), db))
    {
        char *line = buffer;
        if (*line == '\0' || *line == '\n')
        {
            continue;
        }

        numEntries++;
    }

    rewind(db); // put the pointer again to the top

    return numEntries;
}

bool getIsOpenStatus()
{
    FILE *isOpenDatabase = fopen(isOpenDatabasePath, "r");
    if (isOpenDatabase == NULL)
    {
        isOpenDatabase = fopen(isOpenDatabasePath, "w");
        fprintf(isOpenDatabase, "false");
        fclose(isOpenDatabase);
        return false;
    }

    char buffer[10];
    fgets(buffer, sizeof(buffer), isOpenDatabase);
    bool isOpen = strcmp(buffer, "true") == 0;

    fclose(isOpenDatabase);

    return isOpen;
}