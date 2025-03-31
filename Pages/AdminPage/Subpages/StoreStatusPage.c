#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "header_files/StoreStatusPage.h"
#include "../../../Tools/Tools.h"
#include "../../../Tools/FieldType.h"
#include "../../../Tools/Database.h"

static void pageHeader();

void StoreStatusPage(void)
{
    FILE *statusDB = fopen(isOpenDatabasePath, "r");
    if (statusDB == NULL)
    {
        statusDB = fopen(isOpenDatabasePath, "w");
        fprintf(statusDB, "false");

        statusDB = fopen(isOpenDatabasePath, "r");
    }

    char status[10];
    bool isOpen;

    while (true)
    {
        clearTerminal();

        pageHeader();
        printf("\n");

        rewind(statusDB);
        fgets(status, sizeof(status), statusDB);
        isOpen = (strcmp(status, "true") == 0) ? true : false;

        printf("Press [left arrow] or [right arrow] or [tab] to toggle the store status.\n");
        printf("Current Status: <- %s ->", isOpen ? "Open" : "Closed");

        KeyboardKey key = getKeyPressInsensitive();
        if (key == KEY_ESCAPE)
            break;

        switch (key)
        {
        case KEY_TAB:
        case KEY_LEFT:
        case KEY_RIGHT:
        {
            isOpen = !isOpen;

            fclose(statusDB);
            statusDB = fopen(isOpenDatabasePath, "w");
            fprintf(statusDB, "%s", isOpen ? "true" : "false");
            fclose(statusDB);
            statusDB = fopen(isOpenDatabasePath, "r");

            break;
        }

        default:
            break;
        }

        refreshDelay();
    }

    fclose(statusDB); // Close the file
}

void pageHeader()
{
    printf("Enroll Account Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [enter] to confirm.\n");
    printf("Press [esc] to go back.\n");
}