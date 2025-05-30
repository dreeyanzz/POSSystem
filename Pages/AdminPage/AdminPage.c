#include <stdio.h>
#include <stdbool.h>
#include "AdminPage.h"
#include "Subpages/header_files/EnrollAccountPage.h"
#include "Subpages/header_files/AddItemPage.h"
#include "Subpages/header_files/ItemOperationsPage.h"
#include "Subpages/header_files/AccountOperationsPage.h"
#include "Subpages/header_files/StoreStatusPage.h"
#include "Subpages/header_files/TransactionHistoriesPage.h"
#include "../../Tools/Tools.h"

static const PageEntry pageEntries[] = {
    {
        "Add Account",
        &EnrollAccountPage,
    },
    {
        "Add Item",
        &AddItemPage,
    },
    {
        "Item Operations Page",
        &ItemOperationsPage,
    },
    {
        "Account Operations Page",
        &AccountOperationsPage,
    },
    {
        "Transaction Histories",
        &TransactionHistoriesPage,
    },
    {
        "Store Status",
        &StoreStatusPage,
    },
};
static const int numberOfPages = sizeof(pageEntries) / sizeof(pageEntries[0]);

static void pageHeader();

void AdminPage(void)
{
    int cursor = 0;

    while (true)
    {
        clearTerminal();

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
    printf("Admin Page\n");
    printf("Current Datetime: %s\n", getFormattedCurrentDateTime());
    printf("Press [esc] to log out.\n");
}