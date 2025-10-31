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

/*
    Authenticates the input username and input password,
    if username and password matches,
    puts values in `User user`.
*/
bool authenticate(char *username, char *password, User *user);
/*
    Sets status to the `User user.status` struct,
    depends on the value passed from authenticate().
*/
Status setStatus(char status[]);

// Page functions

/*
    Displays the LogIn Page.
    Displays Username and Password fields.
*/
void LogInPage(User *user);
/*
    Decides whether to show `admin side` or `cashier side` pages
*/
void Decider(Status status);

/*
    ███╗   ███╗ █████╗ ██╗███╗   ██╗
    ████╗ ████║██╔══██╗██║████╗  ██║
    ██╔████╔██║███████║██║██╔██╗ ██║
    ██║╚██╔╝██║██╔══██║██║██║╚██╗██║
    ██║ ╚═╝ ██║██║  ██║██║██║ ╚████║
    ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝
*/

User userData = {
    .username = "",
    .password = "",
    .name = "",
    .identifier = "",
    .status = USER,
};

int main(void)
{

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
    char *username;
    char *password;

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

        while (true)
        {
            clearTerminal();

            printf("Log in\n");
            printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
            printf("Press [enter] to initiate login.\n");
            printf("Press [tab], [up arrow], or [down arrow] to move.\n");
            printf("Press [=] to show, or hide password at the current cursor.\n");
            printf("Press [esc] to exit.\n\n");

            for (int i = 0; i < numberOfFields; i++)
            {
                FieldType currentFieldType = fields[i].type;

                (i == cursor) ? printf("%s", coloredCusor()) : printf("  ");

                switch (currentFieldType)
                {
                case PasswordField_Type:
                {
                    PasswordField currentPasswordField = fields[i].data.passwordField;
                    bool showPassword = currentPasswordField.showPassword;

                    if (showPassword)
                        printf("%s: %s", currentPasswordField.displayName, *currentPasswordField.var);
                    else
                    {
                        size_t stringLength = strlen(*currentPasswordField.var);
                        printf("%s: ", currentPasswordField.displayName);
                        for (size_t i = 0; i < stringLength; i++)
                            printf("*");
                    }
                }
                break;

                case TextField_Type:
                {
                    TextField currentTextField = fields[i].data.textField;

                    printf("%s: %s", currentTextField.displayName, *currentTextField.var);
                }
                break;

                default:
                    break;
                }
                if ((i == cursor) && (currentFieldType != BoolField_Type))
                    printf("%s", ansi_colorize(" ", (ANSI_SGR[]){ANSI_BG_WHITE}, 1));
                printf("\n");
            }

            KeyboardKey key = getKeyPress();

            if (key == KEY_ENTER)
                break;
            else if (key == KEY_ESCAPE)
            {
                clearTerminal();
                printf("Goodbye!\n");
                exit(0);
            }

            switch (key)
            {
            case KEY_EQUALS:
            {
                if (fields[cursor].type == PasswordField_Type)
                    fields[cursor].data.passwordField.showPassword = !fields[cursor].data.passwordField.showPassword;
            }
            break;

            case KEY_TAB:
            case KEY_UP:
                cursor == 0 ? cursor = (numberOfFields - 1) : cursor--;
                break;

            case KEY_DOWN:
                cursor == (numberOfFields - 1) ? cursor = 0 : cursor++;
                break;

            case KEY_BACKSPACE:
            {
                if (fields[cursor].type == PasswordField_Type || fields[cursor].type == TextField_Type)
                {
                    Field selectedField = fields[cursor];
                    FieldType currentFieldType = fields[cursor].type;

                    if (currentFieldType == TextField_Type)
                    {
                        TextField currentTextField = selectedField.data.textField;
                        int currentTextFieldStringLength = strlen(*currentTextField.var);
                        int lastCharIndex = currentTextFieldStringLength - 1;

                        if (currentTextFieldStringLength > 0)
                        {
                            (*currentTextField.var)[lastCharIndex] = '\0';
                            *currentTextField.var = realloc(*currentTextField.var, currentTextFieldStringLength);
                        }
                    }
                    else if (currentFieldType == PasswordField_Type)
                    {
                        PasswordField currentPasswordField = selectedField.data.passwordField;
                        int currentPasswordFieldStringLength = strlen(*currentPasswordField.var);
                        int lastCharIndex = currentPasswordFieldStringLength - 1;

                        if (currentPasswordFieldStringLength > 0)
                        {
                            (*currentPasswordField.var)[lastCharIndex] = '\0';
                            *currentPasswordField.var = realloc(*currentPasswordField.var, currentPasswordFieldStringLength);
                        }
                    }
                }
            }
            break;

            default:
            {
                FieldType selectedFieldType = fields[cursor].type;
                char ch = mappedAlNum(key);
                char temp[2] = {ch, '\0'};

                if (ch != '\0')
                {
                    if (selectedFieldType == PasswordField_Type)
                    {
                        PasswordField passwordField = fields[cursor].data.passwordField;

                        int stringLength = strlen(*passwordField.var) + strlen(temp) + 1;
                        *passwordField.var = realloc(*passwordField.var, stringLength);
                        strcat(*passwordField.var, temp);
                    }
                    else if (selectedFieldType == TextField_Type)
                    {
                        TextField textField = fields[cursor].data.textField;

                        int stringLength = strlen(*textField.var) + strlen(temp) + 1;
                        *textField.var = realloc(*textField.var, stringLength);
                        strcat(*textField.var, temp);
                    }
                }
            }
            break;
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
                printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
                printf("Press [enter] to initiate login.\n");
                printf("Press [tab], [up arrow], or [down arrow] to move.\n");
                printf("Press [=] to show, or hide password at the current cursor.\n");
                printf("Press [esc] to exit.\n\n");

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

    FILE *file = fopen(accountsDatabasePath, "r");
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
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0'; // Remove the newline character from the line

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
        char *identifer = strtok(NULL, "|");

        bool matchUsername = strcmp(username, inputUsername) == 0;
        bool matchPassword = strcmp(password, inputPassword) == 0;

        if (matchUsername && matchPassword)
        {

            strcpy(userData->username, username);
            strcpy(userData->password, password);
            strcpy(userData->name, name);
            userData->status = setStatus(status);
            strcpy(userData->identifier, identifer);

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