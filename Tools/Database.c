#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Tools.h"
#include "Database.h"

const char *databasesPath = "Databases";
char accountsDatabasePath[256];
char itemsDatabasePath[256];

void initializeDatabases()
{
    snprintf(accountsDatabasePath, sizeof(accountsDatabasePath), "%s/accounts.csv", databasesPath);
    snprintf(itemsDatabasePath, sizeof(itemsDatabasePath), "%s/items.csv", databasesPath);

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