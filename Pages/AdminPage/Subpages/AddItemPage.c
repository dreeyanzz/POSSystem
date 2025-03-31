#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "header_files/AddItemPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static void pageHeader();

void AddItemPage(void)
{
    initializeDatabases();

    FILE *itemsDatabase = fopen(itemsDatabasePath, "a+"); // Open the items.csv file in append mode + read mode

    char *itemName = (char *)malloc(sizeof(char));
    char *identifier = (char *)malloc(sizeof(char));
    long price = 0;
    bool hasExisted = false;

    itemName[0] = '\0';
    identifier[0] = '\0';

    size_t numberOfFields = 3;
    Field fields[] = {
        {
            .type = TextField_Type,
            .data.textField = {
                .var = &itemName,
                .displayName = "Item name",
            },
        },
        {
            .type = TextField_Type,
            .data.textField = {
                .var = &identifier,
                .displayName = "Item identifier",
            },
        },
        {
            .type = NumberField_Type,
            .data.numberField = {
                .var = &price,
                .displayName = "Price",
            },
        },
    };

    int cursor = 0;

    while (true)
    {
        clearTerminal();

        printf("Add Item Page\n");
        printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
        printf("Press [esc] to go back.\n");
        printf("\n");

        char buffer[1024];
        hasExisted = false;
        rewind(itemsDatabase);                               // Reset file pointer
        fgets(buffer, sizeof(buffer), itemsDatabase);        // Skip header
        while (fgets(buffer, sizeof(buffer), itemsDatabase)) // Scan every line of items.csv
        {
            buffer[strcspn(buffer, "\n")] = '\0';

            char *dbItemName = strtok(buffer, "|");
            char *dbItemIdentifer = strtok(NULL, "|");
            if (dbItemIdentifer && strcmp(dbItemIdentifer, identifier) == 0)
            {
                hasExisted = true;
                break;
            }
        }

        printf("Press [enter] to confirm.\n");
        for (int i = 0; i < numberOfFields; i++)
        {
            (i == cursor) ? printColoredCursor() : printf("  ");

            switch (fields[i].type)
            {

            case TextField_Type:
            {
                printf("%s: %s", fields[i].data.textField.displayName,
                       *fields[i].data.textField.var);
                break;
            }

            case NumberField_Type:
            {
                printf("%s: %d", fields[i].data.textField.displayName,
                       *fields[i].data.numberField.var);
                break;
            }

            default:
                break;
            }
            if ((i == cursor) && (fields[cursor].type != BoolField_Type))
                printf("%s", ansi_colorize(" ", (ANSI_SGR[]){ANSI_BG_WHITE}, 1));
            printf("\n");
        }

        ansi_colorize_start((ANSI_SGR[]){ANSI_BOLD, ANSI_BG_RED}, 2);
        printf("\n");
        if (hasExisted)
            printf("This identifer already exists. Please use another identifier.\n");
        if (itemName[0] == '\0')
            printf("Item name cannot be empty.\n");
        if (identifier[0] == '\0')
            printf("Item identifer cannot be empty.\n");
        ansi_colorize_end();

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {

            if (itemName[0] != '\0' && identifier[0] != '\0' && hasExisted == false)
            {
                while (true)
                {

                    clearTerminal();

                    printf("Add Item Page\n");
                    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
                    printf("Press [esc] to go back.\n");
                    printf("\n");

                    printf("Press [enter] to confirm.\n");
                    for (int i = 0; i < numberOfFields; i++)
                    {
                        printColoredCursor();

                        switch (fields[i].type)
                        {

                        case TextField_Type:
                        {
                            printf("%s: %s", fields[i].data.textField.displayName,
                                   *fields[i].data.textField.var);
                            break;
                        }

                        case NumberField_Type:
                        {
                            printf("%s: %d", fields[i].data.textField.displayName,
                                   *fields[i].data.numberField.var);
                            break;
                        }

                        default:
                            break;
                        }
                        printf("\n");
                    }
                    printf("\n");

                    printf("Are you sure you want to register this item? Please review before proceeding.\n");
                    printf("Press [y] for yes, [n] for no.\n");

                    KeyboardKey key2 = getKeyPressInsensitive();

                    if (key2 == KEY_y)
                    {
                        AddItemToDatabase(itemName, identifier, price);
                        itemName[0] = '\0';
                        identifier[0] = '\0';
                        price = 0;

                        break;
                    }
                    else if (key2 == KEY_n)
                        break;

                    refreshDelay();
                }
            }
        }
        else if (key == KEY_ESCAPE)
        {
            break;
        }

        switch (key)
        {
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

        case KEY_BACKSPACE:
        {
            switch (fields[cursor].type)
            {
            case NumberField_Type:
            {

                *fields[cursor].data.numberField.var /= 10;

                break;
            }

            case TextField_Type:
            {
                if (strlen(*fields[cursor].data.textField.var) > 0)
                {
                    size_t stringLength = strlen(*fields[cursor].data.textField.var);
                    (*fields[cursor].data.textField.var)[stringLength - 1] = '\0';
                    (*fields[cursor].data.textField.var) = realloc((*fields[cursor].data.textField.var), stringLength);
                }
                break;
            }

            default:
                break;
            }

            break;
        }

        default:
        {

            switch (fields[cursor].type)
            {
            case NumberField_Type:
            {
                if (c >= '0' && c <= '9')
                    *fields[cursor].data.numberField.var = ((*fields[cursor].data.numberField.var) * 10) + (c - '0');

                break;
            }

            case TextField_Type:
            {
                if (temp[0] != '\0')
                {
                    size_t stringLength = strlen(*fields[cursor].data.textField.var) + strlen(temp) + 1;
                    *fields[cursor].data.textField.var = realloc(*fields[cursor].data.textField.var, stringLength);
                    strcat(*fields[cursor].data.textField.var, temp);
                }
                break;
            }

            default:
                break;
            }

            break;
        }
        }

        refreshDelay();
    }

    fclose(itemsDatabase);
}

void pageHeader()
{
    printf("Enroll Account Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [enter] to confirm.\n");
    printf("Press [esc] to go back.\n");
}