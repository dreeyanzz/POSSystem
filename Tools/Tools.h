#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Enumeration for common keyboard keys.
   Non-letter keys are grouped separately.
   Lower-case letters are assigned starting at 100,
   and upper-case letters starting at 200, to ensure case-sensitivity.
*/
typedef enum
{
    KEY_NONE = 0,

    /* Control & Special Keys */
    KEY_ESCAPE = 1,
    KEY_TAB,
    KEY_CAPSLOCK,
    KEY_LEFTCTRL,
    KEY_LEFTSHIFT,
    KEY_LEFTALT,
    KEY_LEFT_GUI,
    KEY_RIGHTCTRL,
    KEY_RIGHTSHIFT,
    KEY_RIGHTALT,
    KEY_RIGHT_GUI,
    KEY_MENU,

    /* Punctuation and Numbers */
    KEY_GRAVE, // `
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_MINUS,  // -
    KEY_EQUALS, // =
    KEY_BACKSPACE,
    KEY_LEFTBRACKET,  // [
    KEY_RIGHTBRACKET, // ]
    KEY_BACKSLASH,    // \\\/
    KEY_SEMICOLON,    // ;
    KEY_APOSTROPHE,   // '

    /* Letters (Case-Sensitive) */
    /* Lower-case letters starting at 100 */
    KEY_a = 100,
    KEY_b,
    KEY_c,
    KEY_d,
    KEY_e,
    KEY_f,
    KEY_g,
    KEY_h,
    KEY_i,
    KEY_j,
    KEY_k,
    KEY_l,
    KEY_m,
    KEY_n,
    KEY_o,
    KEY_p,
    KEY_q,
    KEY_r,
    KEY_s,
    KEY_t,
    KEY_u,
    KEY_v,
    KEY_w,
    KEY_x,
    KEY_y,
    KEY_z,

    /* Upper-case letters starting at 200 */
    KEY_A = 200,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    /* More Keys */
    KEY_ENTER,
    KEY_SPACE,

    /* Function Keys starting at 300 */
    KEY_F1 = 300,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    /* Navigation Keys starting at 400 */
    KEY_INSERT = 400,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,

    KEY_MAX
} KeyboardKey;

typedef enum ANSI_SGR
{
    ANSI_RESET = 0,
    ANSI_BOLD = 1,
    ANSI_FAINT = 2,
    ANSI_ITALIC = 3,
    ANSI_UNDERLINE = 4,
    ANSI_SLOW_BLINK = 5,
    ANSI_RAPID_BLINK = 6,
    ANSI_REVERSE_VIDEO = 7,
    ANSI_CONCEAL = 8,
    ANSI_CROSSED_OUT = 9,
    ANSI_DOUBLE_UNDERLINE = 21, // May be interpreted as "bold off" in some terminals
    ANSI_NORMAL_INTENSITY = 22,
    ANSI_NOT_ITALIC = 23,
    ANSI_UNDERLINE_OFF = 24,
    ANSI_BLINK_OFF = 25,
    ANSI_REVERSE_VIDEO_OFF = 27,
    ANSI_REVEAL = 28,
    ANSI_NOT_CROSSED_OUT = 29,
    ANSI_FG_BLACK = 30,
    ANSI_FG_RED = 31,
    ANSI_FG_GREEN = 32,
    ANSI_FG_YELLOW = 33,
    ANSI_FG_BLUE = 34,
    ANSI_FG_MAGENTA = 35,
    ANSI_FG_CYAN = 36,
    ANSI_FG_WHITE = 37,
    ANSI_FG_DEFAULT = 39,
    ANSI_BG_BLACK = 40,
    ANSI_BG_RED = 41,
    ANSI_BG_GREEN = 42,
    ANSI_BG_YELLOW = 43,
    ANSI_BG_BLUE = 44,
    ANSI_BG_MAGENTA = 45,
    ANSI_BG_CYAN = 46,
    ANSI_BG_WHITE = 47,
    ANSI_BG_DEFAULT = 49,
    ANSI_BRIGHT_FG_BLACK = 90,
    ANSI_BRIGHT_FG_RED = 91,
    ANSI_BRIGHT_FG_GREEN = 92,
    ANSI_BRIGHT_FG_YELLOW = 93,
    ANSI_BRIGHT_FG_BLUE = 94,
    ANSI_BRIGHT_FG_MAGENTA = 95,
    ANSI_BRIGHT_FG_CYAN = 96,
    ANSI_BRIGHT_FG_WHITE = 97,
    ANSI_BRIGHT_BG_BLACK = 100,
    ANSI_BRIGHT_BG_RED = 101,
    ANSI_BRIGHT_BG_GREEN = 102,
    ANSI_BRIGHT_BG_YELLOW = 103,
    ANSI_BRIGHT_BG_BLUE = 104,
    ANSI_BRIGHT_BG_MAGENTA = 105,
    ANSI_BRIGHT_BG_CYAN = 106,
    ANSI_BRIGHT_BG_WHITE = 107
} ANSI_SGR;

// String manipulations

char *ansi_colorize(const char *string, ANSI_SGR configurations[], size_t numConfigs);
void ansi_colorize_start(ANSI_SGR configurations[], size_t numConfigs);
void ansi_colorize_end();
void printCentered(const char *text, int width);
char *inttoascii(int num);

// --to be named--

char *coloredCusor();
void printColoredCursor();

// capturing key inputs

KeyboardKey getKeyPress(void);
KeyboardKey getKeyPressInsensitive(void);
char mappedAlNum(KeyboardKey key);

// randomizers

char *generateRandomString(int length);
char *generateAccountIdentifier();
int generateRandomNDigit(int n);
int generateItemIdentifier();

// --to be named--

void refreshDelay();
void clearTerminal();

// time functions

time_t getCurrentTime();
char *getFormattedCurrentDateTime();

// --to be named--

void createFolderIfNotExists(const char *folder);

#endif // TOOLS_H
