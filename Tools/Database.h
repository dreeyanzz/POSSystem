#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>

/*
    Global variables
*/

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
    long numStocks;
    long itemPrice;
} ItemsDatabaseEntry;

typedef struct
{
    char *username;
    char *password;
    char *displayName;
    AccountType accountType;
    char *identifier;
} AccountsDatabaseEntry;

/*
    Initializes all databases
*/
void initializeDatabases();

/* Initalizes/prepares accounts.csv */
void InitializeAccountsDatabase();
/* Adds an entry into the accounts.csv */
void enrollAccount(const char *username, const char *password, const char *displayName, const char *status, const char *identifier);

/* Initializes/prepares items.csv */
void InitializeItemsDatabase();
/* Adds an entry into the items.csv */
void AddItemToDatabase(const char *itemName, const char *identifier, const long numStocks, const long price);

/*
    Removes an entry of item in items.csv by identifier
    Checks every line of items.csv, if the passed identifier matches, it deletes that line/entry
*/
void removeItemsDatabaseEntryByIdentifier(const char *identifier);
/*
    Removes an entry of account in accounts.csv by identifier
    Checks every line of accounts.csv, if the passed identifier matches, it deletes that line/entry
*/
void removeAccountDatabaseEntryByIdentifier(const char *identifier);

/* Counts how many lines/entries are there in a passed database */
int countEntries(FILE *db);

/* Checks the content of isOpen.csv, true returns `true`, false returns `false` */
bool getIsOpenStatus();

#endif