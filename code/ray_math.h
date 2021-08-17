#pragma once

#include <float.h>
#include <math.h>
#include "base.h"

/******************************************************************/
//              f32 stuff
/******************************************************************/
inline u32
RoundF32ToU32(f32 F)
{
    u32 Result = (u32)(F + 0.5f);
    return(Result);
}

inline f32
Pow(f32 A, f32 B)
{
    f32 Result = (f32)pow(A, B);
    return(Result);
}

inline f32
Square(f32 A)
{
    f32 Result = A * A;

    return(Result);
}

inline f32
SquareRoot(f32 A)
{
    f32 Result = (f32)sqrt(A);
    return(Result);
}

/******************************************************************/
//              V2 stuff
/******************************************************************/
union v2
{
    struct
    {
        f32 x, y;
    };
    struct
    {
        f32 u, v;
    };
    f32 E[2];
};

inline v2
V2(f32 X, f32 Y)
{
    v2 Result;

    Result.x = X;
    Result.y = Y;

    return(Result);
}

/******************************************************************/
//              v3 stuff
/******************************************************************/

struct v3
{
    f32 x, y, z;
};

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return(Result);
}

inline v3
operator-(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return(Result);
}

internal v3
operator*(v3 A, f32 B)
{
    v3 Result;
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    return Result;
}

internal v3
operator*(f32 A, v3 B)
{
    return B * A;
}





