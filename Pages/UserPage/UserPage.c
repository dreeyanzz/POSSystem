#include <stdio.h>
#include <stdbool.h>
#include "UserPage.h"
#include "../../Tools/Tools.h"

void UserPage()
{

    while (true)
    {
        clearTerminal();

        printf("User Page\n");
        printf("Current time: %s\n", getFormattedCurrentDateTime());
        printf("Press [esc] to log out.\n");

        KeyboardKey key = getKeyPressInsensitive();

        if (key == KEY_ESCAPE)
        {
            break;
        }

        refreshDelay();
    }
}