#include <stdio.h>
#include <stdbool.h>
#include "AdminPage.h"
#include "Subpages/header_files/EnrollAccountPage.h"
#include "Subpages/header_files/AddItemPage.h"
#include "Subpages/header_files/RemoveItemPage.h"
#include "Subpages/header_files/RemoveUserPage.h"
#include "../../Tools/Tools.h"

typedef struct
{
    char displayName[100];
    void *pageFunc;

} PageEntry;

const PageEntry pageEntries[] = {
    {
        "Add Account",
        &EnrollAccountPage,
    },
    {
        "Add Item",
        &AddItemPage,
    },
    {
        "Remove Item",
        &RemoveItemPage,
    },
};

const int numberOfPages = sizeof(pageEntries) / sizeof(pageEntries[0]);

void AdminPage(void)
{
    int cursor = 0;

    while (true)
    {
        clearTerminal();

        printf("Admin Page\n");
        printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
        printf("Press [esc] to log out.\n");
        printf("\n");

        printf("Press [enter] to enter the page.\n");
        for (int i = 0; i < numberOfPages; i++)
        {

            i == cursor ? printColoredCursor() : printf("  ");

            printf(" %s\n", pageEntries[i].displayName);
        }

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_ESCAPE)
            break;

        switch (key)
        {

        case KEY_ENTER:
        {
            ((void (*)(void))pageEntries[cursor].pageFunc)();
            break;
        }

        case KEY_UP:
        {
            cursor == 0 ? (cursor = (numberOfPages - 1)) : cursor--;
            break;
        }

        case KEY_TAB:
        case KEY_DOWN:
        {

            cursor == (numberOfPages - 1) ? cursor = 0 : cursor++;
            break;
        }

        default:
            break;
        }

        refreshDelay();
    }
}
