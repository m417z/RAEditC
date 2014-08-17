#pragma once

#include <windows.h>

typedef ULONG_PTR        REG_T;                        // register type, unsigned
typedef LONG_PTR         REG_SIGNED_T;                 // register type, signed

typedef signed char      SBYTE;                        // signed byte
typedef signed short     SWORD;                        // signed word
typedef signed long      SDWORD;                       // signed dword

#define TO_R(x)          ((REG_T)(x))                  // cast to register
#define TO_R_SIGNED(x)   ((REG_SIGNED_T)(x))           // cast to signed register

#define RWORD(x)         (*(WORD *)(&x))               // word register (can be lvalue)
#define RBYTE_LOW(x)     (*(BYTE *)(&x))               // low byte register (can be lvalue)
#define RBYTE_HIGH(x)    (*((BYTE *)(&x)+1))           // high byte register (can be lvalue)
#define R_SIGNED(x)      (*(REG_SIGNED_T *)(&x))       // signed register (can be lvalue)
#define RWORD_SIGNED(x)  (*(SWORD *)(&x))              // signed word register (can be lvalue)
#define RBYTE_SIGNED(x)  (*(SBYTE *)(&x))              // signed byte register (can be lvalue)
