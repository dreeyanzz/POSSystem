#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Tools.h"
#include "Database.h"

const char *databasesPath = "Databases";
char accountsDatabasePath[256];
char itemsDatabasePath[256];
const char *accountsDatabaseName = "accounts.csv";
const char *itemsDatabaseName = "items.csv";

void initializeDatabases()
{
    snprintf(accountsDatabasePath, sizeof(accountsDatabasePath), "%s/%s", databasesPath, accountsDatabaseName);
    snprintf(itemsDatabasePath, sizeof(itemsDatabasePath), "%s/%s", databasesPath, itemsDatabaseName);

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

        fprintf(itemsDatabase, "itemName|identifier|price\n");
    }

    fclose(itemsDatabase);
}

void AddItemToDatabase(const char *itemName, const char *identifier, const long price)
{
    FILE *itemsDatabase = fopen(itemsDatabasePath, "a");
    if (itemsDatabase == NULL)
    {
        initializeDatabases();
    }

    fprintf(itemsDatabase, "%s|%s|%d\n", itemName, identifier, price);

    fclose(itemsDatabase);
}

void removeItemsDatabaseEntryByIdentifier(const char *identifier)
{
    char *tempItemsDatabaseName = "temp.csv";

    size_t allocatedSize = strlen(databasesPath) + 1 /* slash */ + strlen(tempItemsDatabaseName) + 1 /* null terminator */;
    char *newItemsDatabasePath = malloc(allocatedSize);
    snprintf(newItemsDatabasePath, allocatedSize, "%s/%s", databasesPath, tempItemsDatabaseName);

    FILE *itemsDatabase = fopen(itemsDatabasePath, "r");
    FILE *newItemsDatabase = fopen(newItemsDatabasePath, "w");

    fprintf(newItemsDatabase, "itemName|identifier|price\n"); // header

    rewind(itemsDatabase);
    char buffer[1024];
    fgets(buffer, sizeof(buffer), itemsDatabase);
    while (fgets(buffer, sizeof(buffer), itemsDatabase))
    {
        char *dbItemName = strtok(buffer, "|");
        char *dbItemIdentifier = strtok(NULL, "|");
        char *dbItemPrice = strtok(NULL, "|");

        if (strcmp(identifier, dbItemIdentifier) != 0)
            fprintf(newItemsDatabase, "%s|%s|%s", dbItemName, dbItemIdentifier, dbItemPrice);
    }

    fclose(itemsDatabase);
    fclose(newItemsDatabase);

    remove(itemsDatabasePath);
    rename(newItemsDatabasePath, itemsDatabasePath);
}