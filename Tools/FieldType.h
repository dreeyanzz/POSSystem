#ifndef FIELDTYPE_H
#define FIELDTYPE_H

#include <stdbool.h>

typedef struct
{
    char **var;
    char *displayName;
} TextField;

typedef struct
{
    long *var;
    char *displayName;
} NumberField;

typedef struct
{
    bool *var;
    char *displayName;
    char *displayFalse;
    char *displayTrue;
} BoolField;

typedef struct
{
    char **var;
    char *displayName;
    bool showPassword;
} PasswordField;

typedef enum
{
    TextField_Type,
    NumberField_Type,
    BoolField_Type,
    PasswordField_Type,
} FieldType;

typedef struct
{
    FieldType type;
    union
    {
        TextField textField;
        BoolField boolField;
        PasswordField passwordField;
        NumberField numberField;
    } data;
} Field;
#endif