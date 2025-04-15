#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "header_files/EnrollAccountPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static void pageHeader();
static bool checkDuplicateUsername();
static void showFields();
static void confirmEnrollAccount();

static FILE *accountsDatabase;
static char *username;
static char *password;
static char *displayName;
static bool isAdmin;
static bool isDuplicateUsername;
static bool emptyUsername;
static bool emptyPassword;
static bool emptyDisplayName;

static const int numberOfFields = 4;
static const Field fields[] = {
    {
        .type = TextField_Type,
        .data.textField = {
            .var = &username,
            .displayName = "Enter username",
        },
    },
    {
        .type = PasswordField_Type,
        .data.passwordField = {
            .var = &password,
            .displayName = "Enter password",
            .showPassword = false,
        },
    },
    {
        .type = TextField_Type,
        .data.textField = {
            .var = &displayName,
            .displayName = "Enter display name",
        },
    },
    {
        .type = BoolField_Type,
        .data.boolField = {
            .var = &isAdmin,
            .displayName = "Status",
            .displayFalse = "User",
            .displayTrue = "Admin",
        },
    },
};

static int cursor;

void EnrollAccountPage(void)
{
    initializeDatabases();

    accountsDatabase = fopen(accountsDatabasePath, "a+"); // Open the accounts.csv file in append mode + read mode
    if (!accountsDatabase)
    {
        perror("Error in EnrollAccountPage");
        return;
    }

    username = (char *)malloc(sizeof(char));
    password = (char *)malloc(sizeof(char));
    displayName = (char *)malloc(sizeof(char));

    username[0] = '\0';
    password[0] = '\0';
    displayName[0] = '\0';
    isAdmin = false;
    isDuplicateUsername = false;

    cursor = 0;

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        isDuplicateUsername = checkDuplicateUsername();
        emptyUsername = username[0] == '\0';
        emptyPassword = password[0] == '\0';
        emptyDisplayName = displayName[0] == '\0';

        printf("Press [enter] to confirm.\n");
        showFields();
        printf("\n");

        ansi_colorize_start((ANSI_SGR[]){ANSI_BOLD, ANSI_BG_RED}, 2);
        if (emptyUsername)
            printf("Username cannot be empty.\n");
        if (isDuplicateUsername)
            printf("This username already exists. Please use another username.\n");
        if (emptyPassword)
            printf("Password cannot be empty.\n");
        if (emptyDisplayName)
            printf("Display name cannot be empty.\n");
        ansi_colorize_end();

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {
            if ((isDuplicateUsername == false) && (password[0] != '\0') && (displayName[0] != '\0') && (username[0] != '\0'))
                confirmEnrollAccount();
        }
        else if (key == KEY_ESCAPE)
            break;

        switch (key)
        {
        case KEY_EQUALS:
        {
            if (fields[cursor].type == PasswordField_Type)
            {
                PasswordField selectedPasswordField = fields[cursor].data.passwordField;

                selectedPasswordField.showPassword = !selectedPasswordField.showPassword;
            }

            break;
        }

        case KEY_UP:
        {
            cursor == 0 ? cursor = (numberOfFields - 1) : cursor--;
            break;
        }

        case KEY_TAB:
        case KEY_DOWN:
        {
            cursor == (numberOfFields - 1) ? cursor = 0 : cursor++;
            break;
        }

        case KEY_LEFT:
        case KEY_RIGHT:
        {
            if (fields[cursor].type == BoolField_Type)
            {
                BoolField selectedBoolField = fields[cursor].data.boolField;

                (*selectedBoolField.var) = !(*selectedBoolField.var);
            }
            break;
        }

        case KEY_BACKSPACE:
        {
            FieldType selectedFieldType = fields[cursor].type;

            switch (selectedFieldType)
            {
            case PasswordField_Type:
            {
                PasswordField selectedPasswordField = fields[cursor].data.passwordField;
                char **var = selectedPasswordField.var;
                int stringLength = strlen(*var);

                if (stringLength > 0)
                {
                    (*var)[stringLength - 1] = '\0';
                    *var = realloc(*var, stringLength);
                }
            }
            break;

            case TextField_Type:
            {
                TextField selectedTextField = fields[cursor].data.textField;
                char **var = selectedTextField.var;
                int stringLength = strlen(*var);

                if (stringLength > 0)
                {
                    (*var)[stringLength - 1] = '\0';
                    *var = realloc(*var, stringLength);
                }
            }
            break;
            }

            break;
        }

        default:
        {

            FieldType selectedFieldType = fields[cursor].type;

            switch (selectedFieldType)
            {
            case PasswordField_Type:
            {
                PasswordField selectedPasswordField = fields[cursor].data.passwordField;
                char **var = selectedPasswordField.var;
                int stringLength = strlen(*var) + strlen(temp) + 1;

                *var = realloc(*var, stringLength);
                strcat(*var, temp);
            }
            break;

            case TextField_Type:
            {
                TextField selectedTextField = fields[cursor].data.textField;
                char **var = selectedTextField.var;
                int stringLength = strlen(*var) + strlen(temp) + 1;

                *var = realloc(*var, stringLength);
                strcat(*var, temp);
            }
            break;
            }
        }
        break;
        }

        refreshDelay();
    }

    free(username);
    free(password);
    free(displayName);
    fclose(accountsDatabase);
}

static void pageHeader()
{
    printf("Enroll Account Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [=] to toggle password visibility.\n");
    printf("Press [esc] to go back.\n");
}
bool checkDuplicateUsername()
{
    char buffer[1024];
    rewind(accountsDatabase);                               // Reset file pointer
    fgets(buffer, sizeof(buffer), accountsDatabase);        // Skip header
    while (fgets(buffer, sizeof(buffer), accountsDatabase)) //  Scan every line of accounts.csv
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        char *dbUsername = strtok(buffer, "|");
        if (dbUsername && strcmp(username, dbUsername) == 0)
            return true;
    }

    return false;
};

void showFields()
{
    for (int i = 0; i < numberOfFields; i++)
    {
        FieldType currentFieldType = fields[i].type;

        (i == cursor) ? printf("%s", coloredCusor()) : printf("  ");

        switch (currentFieldType)
        {
        case BoolField_Type:
        {
            BoolField currentBoolField = fields[i].data.boolField;
            char *displayName = strdup(currentBoolField.displayName);
            bool showPassword = *currentBoolField.var;
            char *displayTrue = strdup(currentBoolField.displayTrue);
            char *displayFalse = strdup(currentBoolField.displayFalse);

            printf("%s: %s",
                   displayName,
                   showPassword ? displayTrue : displayFalse);

            free(displayName);
            free(displayTrue);
            free(displayFalse);
        }
        break;

        case TextField_Type:
        {
            TextField currentTextField = fields[i].data.textField;
            char *var = strdup(*currentTextField.var);
            char *displayName = strdup(currentTextField.displayName);

            printf("%s: %s",
                   displayName,
                   var);

            free(var);
            free(displayName);
        }
        break;

        case PasswordField_Type:
        {

            PasswordField currentPasswordField = fields[i].data.passwordField;
            char *var = *currentPasswordField.var;
            char *displayName = strdup(currentPasswordField.displayName);
            bool showPassword = currentPasswordField.showPassword;

            if (showPassword)
                printf("%s: %s",
                       displayName,
                       *currentPasswordField.var);
            else
            {

                size_t stringLength = strlen(var);

                printf("%s: ", displayName);

                for (size_t i = 0; i < stringLength; i++)
                    printf("*");
            }

            free(displayName);

            break;
        }

        default:
            break;
        }
        if ((i == cursor) && (fields[cursor].type != BoolField_Type))
            printf("%s", ansi_colorize(" ", (ANSI_SGR[]){ANSI_BG_WHITE}, 1));
        printf("\n");
    }
}

void confirmEnrollAccount()
{
    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");
        printf("\n");

        showFields();
        printf("\n");

        printf("Are you sure you want to enroll this account? Please review before proceeding.\n");
        printf("Press [y] for yes, [n] for no.\n");

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_y)
        {
            enrollAccount(username, password,
                          displayName,
                          (isAdmin ? "admin" : "user"),
                          generateAccountIdentifier());
            username[0] = '\0';
            password[0] = '\0';
            displayName[0] = '\0';
            break;
        }
        else if (key == KEY_n)
            break;

        refreshDelay();
    }
}