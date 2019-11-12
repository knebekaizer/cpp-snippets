
/*
**  SNIPTYPE.H - Include file for SNIPPETS data types and commonly used macros
*/


#ifndef SNIPTYPE__H
#define SNIPTYPE__H


#include <stdlib.h>                             /* For free()           */
#include <string.h>                             /* For NULL & strlen()  */


typedef enum {Error_ = -1, Success_, False_ = 0, True_} Boolean_T;


#ifdef BYTE
 #undef BYTE
#endif
#ifdef WORD
 #undef WORD
#endif
#  ifdef DWORD
 #undef DWORD
#endif
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;


typedef union {
      signed char       c;
      BYTE              b;
} VAR8_;


typedef union {
      VAR8_             v8[2];
      signed short      s;
      WORD              w;
} VAR16_;


typedef union {
      VAR16_            v16[2];
      signed long       l;
      DWORD             dw;
      float             f;
      void              *p;
} VAR32_;


typedef union {
      VAR32_            v32[2];
      double            d;
} VAR64_;


#define NUL '\0'
#define LAST_CHAR(s) (((char *)s)[strlen(s) - 1])
#define TOBOOL(x) (!(!(x)))
#define FREE(p) (free(p),(p)=NULL)


#endif /* SNIPTYPE__H */
