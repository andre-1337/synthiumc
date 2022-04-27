#ifndef SYNTHIUMC_TYTYPES_H
#define SYNTHIUMC_TYTYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TY_UNKNOWN,
    TY_PTR,
    TY_I32,
    TY_STRING,
    TY_FUNC,
    TY_MOD,
    TY_STRUCT
} TyTypes;

#endif
