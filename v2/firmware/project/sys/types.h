#ifndef _TYPES_H
#define _TYPES_H


#define UNUSED_FIELD		unsigned


typedef unsigned char       uint8_t;
typedef signed   char       int8_t;
typedef unsigned int        uint16_t;
typedef signed   int        int16_t;
typedef unsigned long       uint32_t;
typedef signed   long       int32_t;
typedef unsigned long long  uint64_t;
typedef signed   long long  int64_t;

typedef enum { FALSE = 0, TRUE = 1 } BOOL;
typedef enum { LOW   = 0, HIGH = 1 } LOGIC;
typedef enum { ZERO  = 0, ONE  = 1 } DIGITAL;
typedef enum { FALL  = 0, RISE = 1 } EDGE;


#endif