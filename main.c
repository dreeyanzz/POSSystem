#include <stdio.h>
#include <windows.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Tools/Tools.h"
#include "Pages/AdminPage/AdminPage.h"
#include "Pages/UserPage/UserPage.h"
#include "Tools/FieldType.h"
#include "Tools/Database.h"

/*
    ██████╗ ███████╗ ██████╗██╗      █████╗ ██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗███████╗
    ██╔══██╗██╔════╝██╔════╝██║     ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
    ██║  ██║█████╗  ██║     ██║     ███████║██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║███████╗
    ██║  ██║██╔══╝  ██║     ██║     ██╔══██║██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║╚════██║
    ██████╔╝███████╗╚██████╗███████╗██║  ██║██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║███████║
    ╚═════╝ ╚══════╝ ╚═════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
*/

typedef enum
{
    ADMIN,
    USER
} Status;

typedef struct
{
    char username[100];
    char password[100];
    char name[100];
    Status status;
} User;

bool authenticate(char *username, char *password, User *user);
Status setStatus(char status[]);

// Page functions

void LogInPage(User *user);
void Decider(Status status);

/*
    ███╗   ███╗ █████╗ ██╗███╗   ██╗
    ████╗ ████║██╔══██╗██║████╗  ██║
    ██╔████╔██║███████║██║██╔██╗ ██║
    ██║╚██╔╝██║██╔══██║██║██║╚██╗██║
    ██║ ╚═╝ ██║██║  ██║██║██║ ╚████║
    ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝
*/
int main(void)
{

    User userData;

    srand(time(NULL)); // Seed the randomizers

    while (true)
    {
        clearTerminal();

        initializeDatabases();

        LogInPage(&userData);

        Decider(userData.status);
    }

    return 0;
}

/*
    ██████╗ ███████╗███████╗██╗███╗   ██╗██╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
    ██╔══██╗██╔════╝██╔════╝██║████╗  ██║██║╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
    ██║  ██║█████╗  █████╗  ██║██╔██╗ ██║██║   ██║   ██║██║   ██║██╔██╗ ██║███████╗
    ██║  ██║██╔══╝  ██╔══╝  ██║██║╚██╗██║██║   ██║   ██║██║   ██║██║╚██╗██║╚════██║
    ██████╔╝███████╗██║     ██║██║ ╚████║██║   ██║   ██║╚██████╔╝██║ ╚████║███████║
    ╚═════╝ ╚══════╝╚═╝     ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
*/

void LogInPage(User *userData)
{
    char *username = (char *)malloc(sizeof(char));
    char *password = (char *)malloc(sizeof(char));

    username[0] = '\0';
    password[0] = '\0';

    size_t numberOfFields = 2;
    Field fields[] = {
        {
            TextField_Type,
            .data.textField = {
                .var = &username,
                .displayName = "Enter username",
            },
        },
        {
            PasswordField_Type,
            .data.passwordField = {
                .var = &password,
                .displayName = "Enter password",
                .showPassword = false,
            },
        },
    };

    int cursor = 0;

    while (true)
    {
        clearTerminal();

        username = malloc(1);
        password = malloc(1);

        username[0] = '\0';
        password[0] = '\0';

        cursor = 0;

        // Input loop: continues until the user presses ENTER.
        while (true)
        {
            clearTerminal();

            printf("Log in\n");
            printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
            printf("Press %s to initiate login.\n", ansi_colorize("[enter]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));
            printf("Press %s, %s, or %s to move.\n", ansi_colorize("[tab]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3),
                   ansi_colorize("[up arrow]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3),
                   ansi_colorize("[down arrow]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));
            printf("Press %s to show, or hide password at the current cursor.\n", ansi_colorize("[=]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));
            printf("Press %s to exit.\n\n", ansi_colorize("[esc]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));

            for (int i = 0; i < numberOfFields; i++)
            {
                (i == cursor) ? printf("%s", coloredCusor()) : printf("  ");

                switch (fields[i].type)
                {
                case PasswordField_Type:
                {

                    if (fields[i].data.passwordField.showPassword)
                    {
                        printf("%s: %s", fields[i].data.passwordField.displayName, *fields[i].data.passwordField.var);
                    }
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
                {
                    printf("%s: %s", fields[i].data.textField.displayName,
                           *fields[i].data.textField.var);

                    break;
                }
                }
                if ((i == cursor) && (fields[cursor].type != BoolField_Type))
                    printf("%s", ansi_colorize(" ", (ANSI_SGR[]){ANSI_BG_WHITE}, 1));
                printf("\n");
            }

            KeyboardKey key = getKeyPress();
            char c = mappedAlNum(key);
            char temp[2] = {c, '\0'};

            if (key == KEY_ENTER)
            {
                break;
            }
            else if (key == KEY_ESCAPE)
            {
                clearTerminal();
                printf("Goodbye!");
                exit(0);
            }

            switch (key)
            {
            case KEY_EQUALS:
            {
                if (fields[cursor].type == PasswordField_Type)
                {
                    fields[cursor].data.passwordField.showPassword = !fields[cursor].data.passwordField.showPassword;
                }

                break;
            }

            case KEY_TAB:
            case KEY_UP:
            {
                cursor == 0 ? cursor = (numberOfFields - 1) : cursor--;
                break;
            }

            case KEY_DOWN:
            {
                cursor == (numberOfFields - 1) ? cursor = 0 : cursor++;
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

        if (authenticate(username, password, userData))
            break;
        else
        {

            while (true)
            {
                clearTerminal();

                printf("Log in\n");
                printf("%s\n", getFormattedCurrentDateTime());
                printf("Press %s to initiate login.\n", ansi_colorize("[enter]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));
                printf("Press %s, %s, or %s to move.\n", ansi_colorize("[tab]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3),
                       ansi_colorize("[up arrow]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3),
                       ansi_colorize("[down arrow]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));
                printf("Press %s to show, or hide password.\n", ansi_colorize("[=]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));
                printf("Press %s to exit.\n\n", ansi_colorize("[esc]", (ANSI_SGR[]){ANSI_BG_WHITE, ANSI_FG_GREEN, ANSI_BOLD}, 3));

                KeyboardKey choice = getKeyPressInsensitive();

                for (int i = 0; i < numberOfFields; i++)
                    printf("%s%s: %s\n", coloredCusor(), fields[i].data.textField.displayName,
                           *fields[i].data.textField.var);

                printf("\nWrong username or password! Would you like to try again?\n");
                printf("Press [y] for yes, [n] for no.\n");

                if (choice == KEY_n)
                {
                    clearTerminal();
                    printf("Goodbye!\n");
                    exit(0);
                }
                else if (choice == KEY_y)
                    break;

                refreshDelay();
            }
        }
    }
}

/*
    Decides which tab to put.
    `Admin` or `User`.
*/
void Decider(Status status)
{
    switch (status)
    {
    case ADMIN:
        AdminPage();
        break;

    case USER:
        UserPage();
        break;

    default:
        break;
    }
}
bool authenticate(char *inputUsername, char *inputPassword, User *userData)
{

    FILE *file = fopen(accountsDatabasePath, "r"); // Open the accounts.csv file in read mode
    if (file == NULL)
    {
        initializeDatabases();
        return authenticate(inputUsername, inputPassword, userData);
    }

    /*
        Remove trailing whitespaces from the username
    */
    int usernameLength = strlen(inputUsername);
    while ((usernameLength > 0) && (inputUsername[usernameLength - 1] == ' '))
    {
        inputUsername[usernameLength - 1] = '\0';
        usernameLength--;
    }

    char line[1024];
    bool isAuthenticated = false;
    /*
        Scan through the file(accounts.csv) line by line
    */
    fgets(line, sizeof(line), file); // Skip the header since we don't need it

    while (fgets(line, sizeof(line), file))
    {
        // Remove the newline character from the line
        line[strcspn(line, "\n")] = '\0';

        /*
            Split the line into tokens using the vertical bar(|) as the separator.
            The tokens are:
            - username
            - password
            - name
            - status
        */
        char *username = strtok(line, "|");
        char *password = strtok(NULL, "|");
        char *name = strtok(NULL, "|");
        char *status = strtok(NULL, "|");

        if ((strcmp(username, inputUsername) == 0) && (strcmp(password, inputPassword) == 0))
        {

            /*
                The user is authenticated if the username and password match the records in the accounts.csv file.
                The user's information is stored in the `user` struct.
                The `user` struct contains the following fields:
                - username
                - password
                - name
                - status
            */

            strcpy(userData->username, username);
            strcpy(userData->password, password);
            strcpy(userData->name, name);
            userData->status = setStatus(status);

            isAuthenticated = true;
            break;
        }
    }

    fclose(file);
    return isAuthenticated;
}

/*
    Checks the status of the user and then returns either `ADMIN` or `USER`.
*/
Status setStatus(char status[])
{

    if (strcmp(status, "admin") == 0)
        return ADMIN;
    return USER;
}