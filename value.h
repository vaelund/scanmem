/*
    Simple routines for working with the value_t data structure.

    Copyright (C) 2006,2007,2009 Tavis Ormandy <taviso@sdf.lonestar.org>
    Copyright (C) 2009           Eli Dupree <elidupree@charter.net>
    Copyright (C) 2009,2010      WANG Lu <coolwanglu@gmail.com>
    Copyright (C) 2015           Sebastian Parschauer <s.parschauer@gmx.de>
    Copyright (C) 2017           Andrea Stacchiotti <andreastacchiotti(a)gmail.com>

    This file is part of libscanmem.

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef VALUE_H
#define VALUE_H

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

/* some routines for working with value_t structures */

/* this is memory-efficient but DANGEROUS */
/* always keep in mind to not mess up with `length` when scanning for BYTEARRAY or STRING */
typedef union {
    struct __attribute__ ((packed)) {
        unsigned  u8b:1;        /* could be an unsigned  8-bit variable (e.g. unsigned char)      */
        unsigned u16b:1;        /* could be an unsigned 16-bit variable (e.g. unsigned short)     */
        unsigned u32b:1;        /* could be an unsigned 32-bit variable (e.g. unsigned int)       */
        unsigned u64b:1;        /* could be an unsigned 64-bit variable (e.g. unsigned long long) */
        unsigned  s8b:1;        /* could be a    signed  8-bit variable (e.g. signed char)        */
        unsigned s16b:1;        /* could be a    signed 16-bit variable (e.g. short)              */
        unsigned s32b:1;        /* could be a    signed 32-bit variable (e.g. int)                */
        unsigned s64b:1;        /* could be a    signed 64-bit variable (e.g. long long)          */
        unsigned f32b:1;        /* could be a 32-bit floating point variable (i.e. float)         */
        unsigned f64b:1;        /* could be a 64-bit floating point variable (i.e. double)        */
    };

    uint16_t length;       /* used when search for an array of bytes or text, I guess uint16_t is enough */
    uint16_t all_flags;    /* used to access the whole union for bitwise operations */
} match_flags;

/* this struct describes matched values */
typedef struct {
    union {
        int8_t int8_value;
        uint8_t uint8_value;
        int16_t int16_value;
        uint16_t uint16_value;
        int32_t int32_value;
        uint32_t uint32_value;
        int64_t int64_value;  
        uint64_t uint64_value;  
        float float32_value;
        double float64_value;
        uint8_t bytes[sizeof(int64_t)];
        char chars[sizeof(int64_t)];
    };
    
    match_flags flags;
} value_t;

/* This union describes 8 bytes retrieved from target memory.
 * Pointers to this union are the only ones that are allowed to be unaligned:
 * to avoid performance degradation/crashes on arches that don't support unaligned access
 * (e.g. ARM) we access unaligned memory only through the attributes of this packed union.
 * As described in http://www.alfonsobeato.net/arm/how-to-access-safely-unaligned-data/ ,
 * a packed structure forces the compiler to write general access methods to its members
 * that don't depend on alignment.
 * So NEVER EVER dereference a mem64_t*, but use its accessors to obtain the needed type.
 */
typedef union __attribute__((packed)) {
    int8_t int8_value;
    uint8_t uint8_value;
    int16_t int16_value;
    uint16_t uint16_value;
    int32_t int32_value;
    uint32_t uint32_value;
    int64_t int64_value;
    uint64_t uint64_value;
    float float32_value;
    double float64_value;
    uint8_t bytes[sizeof(int64_t)];
    char chars[sizeof(int64_t)];
} mem64_t;

/* bytearray wildcards: they must be uint8_t. They are ANDed with the incoming
 * memory before the comparison, so that '??' wildcards always return true
 * It's possible to extend them to fully granular wildcard-ing, if needed */
typedef enum __attribute__ ((__packed__)) {
    FIXED = 0xffu,
    WILDCARD = 0x00u,
} wildcard_t;

/* this struct describes values provided by users */
typedef struct {
    int8_t int8_value;
    uint8_t uint8_value;
    int16_t int16_value;
    uint16_t uint16_value;
    int32_t int32_value;
    uint32_t uint32_value;
    int64_t int64_value;
    uint64_t uint64_value;
    float float32_value;
    double float64_value;

    const uint8_t *bytearray_value;
    const wildcard_t *wildcard_value;

    const char *string_value;

    match_flags flags;
} uservalue_t;

/* used when outputting values to user */
/* only works for numbers */
void valtostr(const value_t *val, char *str, size_t n);
/* parse bytearray, it will allocate the arrays itself, then needs to be free'd by `free_uservalue()` */
bool parse_uservalue_bytearray(char *const *argv, unsigned argc, uservalue_t *val);
bool parse_uservalue_number(const char *nptr, uservalue_t * val); /* parse int or float */
bool parse_uservalue_int(const char *nptr, uservalue_t * val);
bool parse_uservalue_float(const char *nptr, uservalue_t * val);
void free_uservalue(uservalue_t *uval);
void valcpy(value_t * dst, const value_t * src);
void uservalue2value(value_t * dst, const uservalue_t * src); /* dst.flags must be set beforehand */

#define get_s8b(val) ((val)->int8_value)
#define get_u8b(val) ((val)->uint8_value)
#define get_s16b(val) ((val)->int16_value)
#define get_u16b(val) ((val)->uint16_value)
#define get_s32b(val) ((val)->int32_value)
#define get_u32b(val) ((val)->uint32_value)
#define get_s64b(val) ((val)->int64_value)
#define get_u64b(val) ((val)->uint64_value)
#define get_f32b(val) ((val)->float32_value)
#define get_f64b(val) ((val)->float64_value)

#define set_s8b(val, v) (((val)->int8_value) = v)
#define set_u8b(val, v) (((val)->uint8_value) = v)
#define set_s16b(val, v) (((val)->int16_value) = v)
#define set_u16b(val, v) (((val)->uint16_value) = v)
#define set_s32b(val, v) (((val)->int32_value) = v)
#define set_u32b(val, v) (((val)->uint32_value) = v)
#define set_s64b(val, v) (((val)->int64_value) = v)
#define set_u64b(val, v) (((val)->uint64_value) = v)
#define set_f32b(val, v) (((val)->float32_value) = v)
#define set_f64b(val, v) (((val)->float64_value) = v)

#define DECLARE_GET_BY_SYSTEM_DEPENDENT_TYPE_FUNCTIONS(type, typename) \
    unsigned type get_u##typename (const value_t *val); \
    signed type get_s##typename (const value_t *val);

DECLARE_GET_BY_SYSTEM_DEPENDENT_TYPE_FUNCTIONS(char, char);
DECLARE_GET_BY_SYSTEM_DEPENDENT_TYPE_FUNCTIONS(short, short);
DECLARE_GET_BY_SYSTEM_DEPENDENT_TYPE_FUNCTIONS(int, int);
DECLARE_GET_BY_SYSTEM_DEPENDENT_TYPE_FUNCTIONS(long, long);
DECLARE_GET_BY_SYSTEM_DEPENDENT_TYPE_FUNCTIONS(long long, longlong);

static inline void zero_match_flags(match_flags *flags)
{
    /* It's faster than a 2 bytes memset() */
    flags->all_flags = 0;
}

static inline void zero_value(value_t *val)
{
    /* zero components separately -
       10 bytes memset() is too slow */
    val->int64_value = 0;               /* zero the whole union */
    zero_match_flags(&val->flags);
}

static inline void zero_uservalue(uservalue_t *val)
{
    memset(val, 0, sizeof(*val));
}

static inline void truncval_to_flags(value_t *dst, match_flags flags)
{
    assert(dst != NULL);

    /* Act on all numeric flags in a single go */
    dst->flags.all_flags &= flags.all_flags;
}

static inline void truncval(value_t *dst, const value_t *src)
{
    assert(src != NULL);

    truncval_to_flags(dst, src->flags);
}

#endif /* VALUE_H */
