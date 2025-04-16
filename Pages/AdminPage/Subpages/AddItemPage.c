#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "header_files/AddItemPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static void pageHeader();
static bool checkDuplicateItemIdentifier();
static void showFields();
static void confirmAddItem();

static FILE *itemsDatabase;
static char *itemName;
static char *identifier;
static long initialStocks;
static long price;
static bool hasExisted;
bool emptyItemName;
bool emptyIdentifier;

static size_t numberOfFields = 4;
static Field fields[] = {
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
            .var = &initialStocks,
            .displayName = "Initial number of stocks",
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

int cursor;

void AddItemPage(void)
{
    initializeDatabases();

    itemsDatabase = fopen(itemsDatabasePath, "a+"); // Open the items.csv file in append mode + read mode

    itemName = (char *)malloc(sizeof(char));
    identifier = (char *)malloc(sizeof(char));

    itemName[0] = '\0';
    identifier[0] = '\0';
    price = 0;
    hasExisted = false;

    cursor = 0;

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        emptyItemName = (itemName[0] == '\0');
        emptyIdentifier = (identifier[0] == '\0');
        hasExisted = checkDuplicateItemIdentifier();

        printf("Press [enter] to confirm.\n");
        showFields();
        printf("\n");

        ansi_colorize_start((ANSI_SGR[]){ANSI_BOLD, ANSI_BG_RED}, 2);
        if (hasExisted)
            printf("This identifer already exists. Please use another identifier.\n");
        if (emptyItemName)
            printf("Item name cannot be empty.\n");
        if (emptyIdentifier)
            printf("Item identifer cannot be empty.\n");
        ansi_colorize_end();

        KeyboardKey key = getKeyPress();
        char c = mappedAlNum(key);
        char temp[2] = {c, '\0'};

        if (key == KEY_ENTER)
        {
            if (!emptyItemName && !emptyIdentifier && !hasExisted)
                confirmAddItem();
        }
        else if (key == KEY_ESCAPE)
            break;
        else
            switch (key)
            {
            case KEY_UP:
                cursor == 0 ? cursor = (numberOfFields - 1) : cursor--;
                break;

            case KEY_TAB:
            case KEY_DOWN:
                cursor == (numberOfFields - 1) ? cursor = 0 : cursor++;
                break;

            case KEY_BACKSPACE:
            {
                switch (fields[cursor].type)
                {
                case NumberField_Type:
                {

                    NumberField selectedNumberField = fields[cursor].data.numberField;

                    *selectedNumberField.var /= 10;
                }
                break;

                case TextField_Type:
                {
                    TextField selectedTextField = fields[cursor].data.textField;

                    if (strlen(*selectedTextField.var) > 0)
                    {
                        size_t stringLength = strlen(*selectedTextField.var);
                        (*selectedTextField.var)[stringLength - 1] = '\0';
                        (*selectedTextField.var) = realloc((*selectedTextField.var), stringLength);
                    }
                }
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
                    NumberField selectedNumberField = fields[cursor].data.numberField;
                    long *var = selectedNumberField.var;

                    if (c >= '0' && c <= '9')
                        *var = ((*var) * 10) + (c - '0');
                }
                break;

                case TextField_Type:
                {

                    bool emptyTemp = temp[0] == '\0';

                    if (!emptyTemp)
                    {
                        TextField selectedTextField = fields[cursor].data.textField;

                        size_t stringLength = strlen(*selectedTextField.var) + strlen(temp) + 1;
                        *selectedTextField.var = realloc(*selectedTextField.var, stringLength);
                        strcat(*selectedTextField.var, temp);
                    }
                }
                break;
                }
            }
            break;
            }

        refreshDelay();
    }

    free(itemName);
    free(identifier);
    fclose(itemsDatabase);
}

void pageHeader()
{
    printf("Add Item Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to go back.\n");
}

bool checkDuplicateItemIdentifier()
{
    char buffer[1024];
    rewind(itemsDatabase);                               // Reset file pointer
    fgets(buffer, sizeof(buffer), itemsDatabase);        // Skip header
    while (fgets(buffer, sizeof(buffer), itemsDatabase)) // Scan every line of items.csv
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        char *dbItemName = strtok(buffer, "|");
        char *dbItemIdentifer = strtok(NULL, "|");
        if (dbItemIdentifer && strcmp(dbItemIdentifer, identifier) == 0)
            return true;
    }

    return false;
}

void showFields()
{
    for (int i = 0; i < numberOfFields; i++)
    {
        FieldType currentFieldType = fields[i].type;

        (i == cursor) ? printColoredCursor() : printf("  ");

        switch (currentFieldType)
        {

        case TextField_Type:
        {
            TextField currentTextField = fields[i].data.textField;
            char **var = currentTextField.var;
            char *displayName = currentTextField.displayName;

            printf("%s: %s",
                   displayName,
                   *var);
        }
        break;

        case NumberField_Type:
        {
            NumberField currentNumberField = fields[i].data.numberField;
            long *var = currentNumberField.var;
            char *displayName = currentNumberField.displayName;

            printf("%s: %d",
                   displayName,
                   *var);
        }
        break;
        }
        if ((i == cursor) && (fields[cursor].type != BoolField_Type))
            printf("%s", ansi_colorize(" ", (ANSI_SGR[]){ANSI_BG_WHITE}, 1));
        printf("\n");
    }
}

void confirmAddItem()
{
    while (true)
    {

        clearTerminal();

        pageHeader();
        printf("\n");

        printf("Press [enter] to confirm.\n");
        showFields();
        printf("\n");

        printf("Are you sure you want to register this item? Please review before proceeding.\n");
        printf("Press [y] for yes, [n] for no.\n");

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_y)
        {
            AddItemToDatabase(itemName, identifier, initialStocks, price);
            itemName[0] = '\0';
            identifier[0] = '\0';
            initialStocks = 0;
            price = 0;

            break;
        }
        else if (key == KEY_n)
            break;

        refreshDelay();
    }
}