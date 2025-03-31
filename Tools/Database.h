#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>

extern const char *databasesPath;
extern char accountsDatabasePath[256];
extern char itemsDatabasePath[256];
extern char isOpenDatabasePath[256];
extern const char *accountsDatabaseName;
extern const char *itemsDatabaseName;
extern const char *isOpenDatabaseName;

typedef enum
{
    user,
    admin,
} AccountType;

typedef struct
{
    char *itemName;
    char *itemIdentifier;
    int itemPrice;
} ItemsDatabaseEntry;

typedef struct
{
    char *username;
    char *password;
    char *displayName;
    AccountType accountType;
    char *identifier;
} AccountsDatabaseEntry;

void initializeDatabases();

void InitializeAccountsDatabase();
void enrollAccount(const char *username, const char *password, const char *displayName, const char *status, const char *identifier);

void InitializeItemsDatabase();
void AddItemToDatabase(const char *itemName, const char *identifier, const long price);

void removeItemsDatabaseEntryByIdentifier(const char *identifier);
void removeAccountDatabaseEntryByIdentifier(const char *identifier);

int countEntries(FILE *db);

bool getIsOpenStatus();

#endif