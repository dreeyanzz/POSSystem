#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include "Tools.h"

char *ansi_colorize(const char *string, ANSI_SGR configurations[], size_t numConfigs)
{
    char prefix[64];
    size_t offset = 0;

    // Build the prefix escape sequence.
    offset += sprintf(prefix + offset, "\033[");
    for (size_t i = 0; i < numConfigs; i++)
    {
        if (i > 0)
        {
            offset += sprintf(prefix + offset, ";");
        }
        offset += sprintf(prefix + offset, "%d", configurations[i]);
    }
    sprintf(prefix + offset, "m");

    const char *reset = "\033[0m";

    // Calculate total length: prefix + original string + reset + null terminator.
    size_t total_length = strlen(prefix) + strlen(string) + strlen(reset) + 1;
    char *configuredString = malloc(total_length);
    if (!configuredString)
    {
        return NULL; // Memory allocation failed.
    }

    // Construct the final string.
    strcpy(configuredString, prefix);
    strcat(configuredString, string);
    strcat(configuredString, reset);

    return configuredString;
}

void ansi_colorize_start(ANSI_SGR configurations[], size_t numConfigs)
{
    char prefix[64];
    size_t offset = 0;

    // Build the prefix escape sequence.
    offset += sprintf(prefix + offset, "\033[");
    for (size_t i = 0; i < numConfigs; i++)
    {
        if (i > 0)
        {
            offset += sprintf(prefix + offset, ";");
        }
        offset += sprintf(prefix + offset, "%d", configurations[i]);
    }
    sprintf(prefix + offset, "m");

    printf("%s", prefix);
}

void ansi_colorize_end()
{
    const char *reset = "\033[0m";

    printf("%s", reset);
}

void printCentered(const char *text, int width)
{
    int len = strlen(text);

    // If the text is too long, either truncate or just print it.
    if (len >= width)
    {
        printf("%.*s", width, text);
        return;
    }

    // Calculate how many spaces to pad on the left and right.
    int leftPadding = (width - len) / 2;
    int rightPadding = width - len - leftPadding;

    // Print the left padding, then the text, then the right padding.
    printf("%*s%s%*s", leftPadding, "", text, rightPadding, "");
}

char *inttoascii(int num)
{
    char *str = malloc(12); // Enough for 32-bit int (-2147483648 to 2147483647)
    if (str)
    {
        sprintf(str, "%d", num); // Convert int to string
    }
    return str;
}

char *coloredCusor()
{
    return ansi_colorize("->", (ANSI_SGR[]){ANSI_FG_GREEN}, 1);
}

void printColoredCursor()
{
    printf("%s", coloredCusor());
}

KeyboardKey getKeyPress(void)
{
    if (!kbhit())
        return KEY_NONE;

    int key = getch();

    /* Handle extended keys (arrow keys, etc.) */
    if (key == 0 || key == 224)
    {
        int ext = getch();
        switch (ext)
        {
        case 72:
            return KEY_UP;
        case 80:
            return KEY_DOWN;
        case 75:
            return KEY_LEFT;
        case 77:
            return KEY_RIGHT;
        case 71:
            return KEY_HOME;
        case 79:
            return KEY_END;
        case 73:
            return KEY_PAGEUP;
        case 81:
            return KEY_PAGEDOWN;
        case 82:
            return KEY_INSERT;
        case 83:
            return KEY_DELETE;
        default:
            return KEY_NONE;
        }
    }
    else
    {
        /* Map control and punctuation keys */
        if (key == 27)
            return KEY_ESCAPE;
        if (key == 13)
            return KEY_ENTER;
        if (key == 8)
            return KEY_BACKSPACE;
        if (key == '\t')
            return KEY_TAB;
        if (key == ' ')
            return KEY_SPACE;

        switch (key)
        {
        case '`':
            return KEY_GRAVE;
        case '1':
            return KEY_1;
        case '2':
            return KEY_2;
        case '3':
            return KEY_3;
        case '4':
            return KEY_4;
        case '5':
            return KEY_5;
        case '6':
            return KEY_6;
        case '7':
            return KEY_7;
        case '8':
            return KEY_8;
        case '9':
            return KEY_9;
        case '0':
            return KEY_0;
        case '-':
            return KEY_MINUS;
        case '=':
            return KEY_EQUALS;
        case '[':
            return KEY_LEFTBRACKET;
        case ']':
            return KEY_RIGHTBRACKET;
        case '\\':
            return KEY_BACKSLASH;
        case ';':
            return KEY_SEMICOLON;
        case '\'':
            return KEY_APOSTROPHE;
        }

        /* Case-sensitive mapping for letters */
        if (key >= 'a' && key <= 'z')
            return (KeyboardKey)(KEY_a + (key - 'a'));
        if (key >= 'A' && key <= 'Z')
            return (KeyboardKey)(KEY_A + (key - 'A'));

        return KEY_NONE;
    }
}

KeyboardKey getKeyPressInsensitive(void)
{
    KeyboardKey key = getKeyPress();
    // If key is an upper-case letter, convert it to lower-case equivalent.
    if (key >= KEY_A && key <= KEY_Z)
    {
        return (KeyboardKey)(KEY_a + (key - KEY_A));
    }
    return key;
}

char mappedAlNum(KeyboardKey key)
{
    switch (key)
    {
    /* Map lower-case letters */
    case KEY_a:
    case KEY_b:
    case KEY_c:
    case KEY_d:
    case KEY_e:
    case KEY_f:
    case KEY_g:
    case KEY_h:
    case KEY_i:
    case KEY_j:
    case KEY_k:
    case KEY_l:
    case KEY_m:
    case KEY_n:
    case KEY_o:
    case KEY_p:
    case KEY_q:
    case KEY_r:
    case KEY_s:
    case KEY_t:
    case KEY_u:
    case KEY_v:
    case KEY_w:
    case KEY_x:
    case KEY_y:
    case KEY_z:
        return 'a' + (key - KEY_a);

    /* Map upper-case letters */
    case KEY_A:
    case KEY_B:
    case KEY_C:
    case KEY_D:
    case KEY_E:
    case KEY_F:
    case KEY_G:
    case KEY_H:
    case KEY_I:
    case KEY_J:
    case KEY_K:
    case KEY_L:
    case KEY_M:
    case KEY_N:
    case KEY_O:
    case KEY_P:
    case KEY_Q:
    case KEY_R:
    case KEY_S:
    case KEY_T:
    case KEY_U:
    case KEY_V:
    case KEY_W:
    case KEY_X:
    case KEY_Y:
    case KEY_Z:
        return 'A' + (key - KEY_A);

    /* Map numeric keys */
    case KEY_1:
    case KEY_2:
    case KEY_3:
    case KEY_4:
    case KEY_5:
    case KEY_6:
    case KEY_7:
    case KEY_8:
    case KEY_9:
        return '1' + (key - KEY_1);
    case KEY_0:
        return '0';

    case KEY_SPACE:
        return ' ';

    /* For any non-alphanumeric key, return null character */
    default:
        return '\0';
    }
}

char *generateRandomString(int length)
{
    if (length <= 0)
        return NULL; // Handle invalid length

    char *str = malloc(length + 1); // Allocate memory dynamically
    if (!str)
        return NULL; // Handle allocation failure

    char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int charsetSize = sizeof(charset) - 1;

    for (int i = 0; i < length; i++)
    {
        str[i] = charset[rand() % charsetSize];
    }
    str[length] = '\0'; // Null-terminate the string

    return str;
}

char *generateAccountIdentifier()
{
    return generateRandomString(32);
}

int generateRandomNDigit(int n)
{
    if (n <= 0)
        return -1; // Handle invalid input

    int lower = (int)pow(10, n - 1); // Smallest n-digit number
    int upper = (int)pow(10, n) - 1; // Largest n-digit number

    return lower + rand() % (upper - lower + 1); // Generate random number in range
}

int generateItemIdentifier()
{
    return generateRandomNDigit(10);
}

void refreshDelay()
{
    Sleep(1000 / 50);
}

void clearTerminal()
{
    system("cls");
}

time_t getCurrentTime()
{
    return time(NULL);
}

char *getFormattedCurrentDateTime()
{

    time_t now = getCurrentTime();
    struct tm *local = localtime(&now);
    static char buffer[80];

    // Format the time as "MONTH DD, YYYY HH:MM:SS AM/PM"
    strftime(buffer, sizeof(buffer), "%B %d, %Y | %I:%M:%S %p", local);

    return buffer;
}

void createFolderIfNotExists(const char *folder)

{
    _mkdir(folder);
}

void printRow(int columnWidth, int count, ...)
{
    va_list args;
    va_start(args, count);

    printf("|");

    for (int i = 0; i < count; i++)
    {
        char *currentString = va_arg(args, char *);
        printCentered(currentString, columnWidth);
        printf("|");
    }

    va_end(args);
}