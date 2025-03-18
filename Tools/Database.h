#ifndef DATABASE_H
#define DATABASE_H

extern const char *databasesPath;
extern char accountsDatabasePath[256];
extern char itemsDatabasePath[256];

void initializeDatabases();

void InitializeAccountsDatabase();
void enrollAccount(const char *username, const char *password, const char *displayName, const char *status, const char *identifier);

void InitializeItemsDatabase();
void AddItemToDatabase(const char *itemName, const char *identifier, const long price);

#endif