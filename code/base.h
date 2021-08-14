#pragma once

#include <stdint.h>

#define internal static
#define global static

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef i32 b32;
typedef i32 b32x;

#define F32MAX FLT_MAX
#define F32MIN -FLT_MAX
#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define ArraySize(arr) (sizeof(arr) / sizeof((arr)[0]))