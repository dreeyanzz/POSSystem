#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "header_files/EnrollAccountPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static void pageHeader();

void EnrollAccountPage(void)
{
    initializeDatabases();

    FILE *accountsDatabase = fopen(accountsDatabasePath, "a+"); // Open the accounts.csv file in append mode + read mode
    if (!accountsDatabase)
    {
        perror("Error in EnrollAccountPage");
        return;
    }

    char *username = (char *)malloc(sizeof(char));
    char *password = (char *)malloc(sizeof(char));
    char *displayName = (char *)malloc(sizeof(char));
    bool isAdmin = false;
    bool hasExisted = false;

    username[0] = '\0';
    password[0] = '\0';
    displayName[0] = '\0';

    size_t numberOfFields = 4;
    Field fields[] = {
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

    int cursor = 0;

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        char buffer[1024];
        hasExisted = false;
        rewind(accountsDatabase);                               // Reset file pointer
        fgets(buffer, sizeof(buffer), accountsDatabase);        // Skip header
        while (fgets(buffer, sizeof(buffer), accountsDatabase)) //  Scan every line of accounts.csv
        {
            buffer[strcspn(buffer, "\n")] = '\0';

            char *dbUsername = strtok(buffer, "|");
            if (dbUsername && strcmp(username, dbUsername) == 0)
            {
                hasExisted = true;
                break;
            }
        }

        printf("Press [enter] to confirm.\n");
        for (int i = 0; i < numberOfFields; i++)
        {
            (i == cursor) ? printf("%s", coloredCusor()) : printf("  ");

            switch (fields[i].type)
            {
            case BoolField_Type:
            {

                printf("%s: %s", fields[i].data.boolField.displayName,
                       (*fields[i].data.boolField.var) ? fields[i].data.boolField.displayTrue : fields[i].data.boolField.displayFalse);
                break;
            }

            case TextField_Type:
                printf("%s: %s", fields[i].data.textField.displayName,
                       *fields[i].data.textField.var);
                break;

            case PasswordField_Type:
            {

                if (fields[i].data.passwordField.showPassword)
                    printf("%s: %s", fields[i].data.passwordField.displayName, *fields[i].data.passwordField.var);
                else
                {

                    size_t stringLength = strlen(*fields[i].data.passwordField.var);

                    printf("%s: ", fields[i].data.passwordField.displayName);

                    for (size_t i = 0; i < stringLength; i++)
                        printf("*");
                }

                break;
            }

            default:
                break;
            }
            if ((i == cursor) && (fields[cursor].type != BoolField_Type))
                printf("%s", ansi_colorize(" ", (ANSI_SGR[]){ANSI_BG_WHITE}, 1));
            printf("\n");
        }
        printf("\n");

        ansi_colorize_start((ANSI_SGR[]){ANSI_BOLD, ANSI_BG_RED}, 2);
        if (username[0] == '\0')
            printf("Username cannot be empty.\n");
        if (hasExisted)
            printf("This username already exists. Please use another username.\n");
        if (password[0] == '\0')
            printf("Password cannot be empty.\n");
        if (displayName[0] == '\0')
            printf("Display name cannot be empty.\n");
        ansi_colorize_end();

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {
            if ((hasExisted == false) && (password[0] != '\0') && (displayName[0] != '\0') && (username[0] != '\0'))
            {
                while (true)
                {
                    clearTerminal();

                    printf("\n");

                    for (int i = 0; i < numberOfFields; i++)
                    {
                        printColoredCursor();

                        switch (fields[i].type)
                        {
                        case BoolField_Type:
                        {

                            printf("%s: %s", fields[i].data.boolField.displayName,
                                   (*fields[i].data.boolField.var) ? fields[i].data.boolField.displayTrue : fields[i].data.boolField.displayFalse);
                            break;
                        }

                        case TextField_Type:
                        {
                            printf("%s: %s", fields[i].data.textField.displayName,
                                   *fields[i].data.textField.var);
                            break;
                        }

                        case PasswordField_Type:
                        {

                            if (fields[i].data.passwordField.showPassword)
                                printf("%s: %s", fields[i].data.passwordField.displayName, *fields[i].data.passwordField.var);
                            else
                            {
                                size_t stringLength = strlen(*fields[i].data.passwordField.var);

                                printf("%s: ", fields[i].data.passwordField.displayName);

                                for (size_t i = 0; i < stringLength; i++)
                                    printf("*");
                            }

                            break;
                        }

                        default:
                            break;
                        }
                        printf("\n");
                    }
                    printf("\n");

                    printf("Are you sure you want to enroll this account? Please review before proceeding.\n");
                    printf("Press [y] for yes, [n] for no.\n");

                    KeyboardKey key2 = getKeyPressInsensitive();

                    if (key2 == KEY_y)
                    {
                        enrollAccount(username, password, displayName, (isAdmin ? "admin" : "user"), generateAccountIdentifier());
                        username[0] = '\0';
                        password[0] = '\0';
                        displayName[0] = '\0';
                        break;
                    }
                    else if (key2 == KEY_n)
                        break;

                    refreshDelay();
                }
            }
        }
        else if (key == KEY_ESCAPE)
            break;

        switch (key)
        {
        case KEY_EQUALS:
        {
            if (fields[cursor].type == PasswordField_Type)
                fields[cursor].data.passwordField.showPassword = !fields[cursor].data.passwordField.showPassword;

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
                (*fields[cursor].data.boolField.var) = !(*fields[cursor].data.boolField.var);
            break;
        }

        case KEY_BACKSPACE:
        {
            if (fields[cursor].type == PasswordField_Type || fields[cursor].type == TextField_Type)
            {
                if (strlen(*fields[cursor].data.textField.var) > 0)
                {
                    size_t stringLength = strlen(*fields[cursor].data.textField.var);
                    (*fields[cursor].data.textField.var)[stringLength - 1] = '\0';
                    *fields[cursor].data.textField.var = realloc(*fields[cursor].data.textField.var, stringLength);
                }
            }

            break;
        }

        default:
        {
            if (fields[cursor].type == PasswordField_Type || fields[cursor].type == TextField_Type)
            {
                if (temp[0] != '\0')
                {
                    size_t stringLength = strlen(*fields[cursor].data.textField.var) + strlen(temp) + 1;
                    *fields[cursor].data.textField.var = realloc(*fields[cursor].data.textField.var, stringLength);
                    strcat(*fields[cursor].data.textField.var, temp);
                }
            }

            break;
        }
        }

        refreshDelay();
    }

    fclose(accountsDatabase);
}

static void pageHeader()
{
    printf("Enroll Account Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [=] to toggle password visibility.\n");
    printf("Press [esc] to go back.\n");
}