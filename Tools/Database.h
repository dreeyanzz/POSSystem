#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdarg.h>
#include "Tools.h"

extern const char *databasesPath;
extern char accountsDatabasePath[256];
extern char itemsDatabasePath[256];
extern char isOpenDatabasePath[256];
extern char numCashiersDatabasePath[256];
extern char transactionsFolderPath[256];
extern const char *accountsDatabaseName;
extern const char *itemsDatabaseName;
extern const char *isOpenDatabaseName;
extern const char *numCashiersDatabaseName;
extern const char *transactionsFolderName;

typedef enum
{
    ADMIN,
    USER,
} Status;

typedef struct
{
    char username[100];
    char password[100];
    char name[100];
    char identifier[100];
    Status status;
} User;

extern User userData;

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

typedef enum
{
    CHANGE_ITEM_NAME,
    CHANGE_ITEM_IDENTIFIER,
    CHANGE_ITEM_STOCKS,
    CHANGE_ITEM_PRICE,
    REMOVE_ITEM_ENTRY,
} ItemsEntryOperationType;

typedef enum
{
    CHANGE_ACCOUNT_USERNAME,
    CHANGE_ACCOUNT_PASSWORD,
    CHANGE_ACCOUNT_DISPLAYNAME,
    CHANGE_ACCOUNT_STATUS,
    CHANGE_ACCOUNT_IDENTIFIER,
    REMOVE_ACCOUNT_ENTRY,
} AccountsEntryOperationType;

typedef struct
{
    char *cashierName;
    char *itemName;
    char *itemIdentifier;
    int stocksSold;
    char *dateRaw;
    time_t timestamp;
} SaleEntry;

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
void changeItemPropertyByIdentifier(const char *identifier, ItemsEntryOperationType operationType, ...);
/*
    Removes an entry of account in accounts.csv by identifier
    Checks every line of accounts.csv, if the passed identifier matches, it deletes that line/entry
*/
void removeAccountDatabaseEntryByIdentifier(const char *identifier);
void changeAccountPropertyByIdentifier(const char *identifier, AccountsEntryOperationType operationType, ...);

/* Counts how many lines/entries are there in a passed database */
int countEntries(FILE *db);

void initializeNumCashiersDatabase();
int countNumCashiers();

void initializeTransactionsFolder();

/* Checks the content of isOpen.csv, true returns `true`, false returns `false` */
bool getIsOpenStatus();

#endif