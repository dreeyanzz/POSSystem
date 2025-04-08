#include <stdio.h>
#include <stdbool.h>
#include "UserPage.h"
#include "../../Tools/Tools.h"
#include "../../Tools/Database.h"

static void pageHeader();

static const PageEntry pageEntries[] = {
    {
        "User Page",
        &UserPage,
    },
    {
        "User Page",
        &UserPage,
    },
    {
        "User Page",
        &UserPage,
    },
    {
        "User Page",
        &UserPage,
    },
};
static const int numberOfPages = sizeof(pageEntries) / sizeof(pageEntries[0]);

void UserPage()
{
    int cursor = 0;
    bool isOpen = getIsOpenStatus();

    while (true)
    {
        clearTerminal();

        isOpen = getIsOpenStatus();
        while (!isOpen)
        {
            clearTerminal();

            pageHeader();
            printf("\n");

            isOpen = getIsOpenStatus();
            if (isOpen)
                break;

            printf("The Store is currently closed.\n");

            KeyboardKey key = getKeyPressInsensitive();
            if (key == KEY_ESCAPE)
                break;

            refreshDelay();
        }
        if (!isOpen)
            break;

        pageHeader();
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

static void pageHeader()
{
    printf("User Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to log out.\n");
}