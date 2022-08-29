#pragma once

#include <math.h>
#include <float.h>

#define F32MAX FLT_MAX
#define F32MIN -FLT_MAX
#define Pi32 3.14159265359f

/*********************************************************************/
/*                                Scalars                            */
/*********************************************************************/

inline f32
Min(f32 A, f32 B)
{
    f32 Result;
    if (A < B)
    {
        Result = A;
    }
    else
    {
        Result = B;
    }
    return Result;
}

inline f32
Max(f32 A, f32 B)
{
    f32 Result;
    if (A > B)
    {
        Result = A;
    }
    else
    {
        Result = B;
    }
    return Result;
}

inline u32
RoundF32ToU32(f32 F)
{
    u32 Result = (u32)(F + 0.5f);
    return(Result);
}

inline f32
Power(f32 A, f32 B)
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

inline f32
Clamp01(f32 Value)
{
    f32 Result = Min(Max(Value, 0.0f), 1.0f);
    return Result;
}

internal f32
ExactLinearToSRGB(f32 LinearColor)
{
    f32 sRGB;

    if (LinearColor < 0.0f)
    {
        LinearColor = 0.0f;
    }
    else if (LinearColor > 1.0f)
    {
        LinearColor = 1.0f;
    }

    if (LinearColor > 0.0031308f)
    {
        sRGB = 1.055f * Power(LinearColor, 1.0f / 2.4f) - 0.055f;
    }
    else
    {
        sRGB = LinearColor * 12.92f;
    }
    
    return sRGB;
}

/*********************************************************************/
/*                                V2                                 */
/*********************************************************************/

union v2
{
    struct
    {
        f32 X, Y;
    };
    struct
    {
        f32 U, V;
    };
    f32 E[2];
};

inline v2
V2(f32 X, f32 Y)
{
    v2 Result;

    Result.X = X;
    Result.Y = Y;

    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return(Result);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return(Result);
}

inline v2
operator-(v2 A)
{
    v2 Result;

    Result.X = -A.X;
    Result.Y = -A.Y;

    return(Result);
}

inline v2
operator*(v2 A, f32 B)
{
    v2 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    return Result;
}

inline v2
operator*(f32 A, v2 B)
{
    return B * A;
}

inline f32
InnerProduct(v2 A, v2 B)
{
    f32 Result = (A.X * B.X) + (A.Y * B.Y);
    return(Result);
}

inline v2
HadamardProduct(v2 A, v2 B)
{
    v2 Result = V2(A.X * B.X, A.Y * B.Y);
    return(Result);
}

inline f32
LengthSquare(v2 A)
{
    f32 Result = InnerProduct(A, A);
    return(Result);
}

inline v2
NormalizeToZero(v2 A)
{
    v2 Result = {};

    f32 LengthSquared = LengthSquare(A);
    if (LengthSquared > Square(0.0001f))
    {
        Result = A * (1.0f / SquareRoot(LengthSquared));
    }

    return Result;
}

inline v2
LinearInterpolation(v2 A, v2 B, f32 T)
{
    v2 Result = (1.0f - T) * A + T * B;
    return(Result);
}

/*********************************************************************/
/*                                V3                                 */
/*********************************************************************/

union v3
{
    struct
    {
        f32 X, Y, Z;
    };
    struct
    {
        f32 Red, Green, Blue;
    };
};

inline v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;

    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;

    return(Result);
}

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return(Result);
}

inline v3
operator-(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return(Result);
}

inline v3
operator-(v3 A)
{
    v3 Result;

    Result.X = -A.X;
    Result.Y = -A.Y;
    Result.Z = -A.Z;

    return(Result);
}

inline v3
operator*(v3 A, f32 B)
{
    v3 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;
    return Result;
}

inline v3
operator*(f32 A, v3 B)
{
    return B * A;
}

inline f32
InnerProduct(v3 A, v3 B)
{
    f32 Result = (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);
    return(Result);
}

inline v3
HadamardProduct(v3 A, v3 B)
{
    v3 Result = V3(A.X * B.X, A.Y * B.Y, A.Z * B.Z);
    return(Result);
}

inline v3
CrossProduct(v3 A, v3 B)
{
    v3 Result;

    Result.X = A.Y * B.Z - A.Z * B.Y;
    Result.Y = A.Z * B.X - A.X * B.Z;
    Result.Z = A.X * B.Y - A.Y * B.X;

    return(Result);
}

inline f32
LengthSquare(v3 A)
{
    f32 Result = InnerProduct(A, A);
    return(Result);
}

inline v3
NormalizeToZero(v3 A)
{
    v3 Result = {};

    f32 LengthSquared = LengthSquare(A);
    if (LengthSquared > Square(0.0001f))
    {
        Result = A * (1.0f / SquareRoot(LengthSquared));
    }

    return Result;
}

inline v3
LinearInterpolation(v3 A, v3 B, f32 T)
{
    v3 Result = (1.0f - T) * A + T * B;
    return(Result);
}

/*********************************************************************/
/*                                V4                                 */
/*********************************************************************/

union v4
{
    struct
    {
        f32 X, Y, Z, W;
    };
    struct
    {
        f32 Red, Green, Blue, Alpha;
    };
    f32 E[4];
};

inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;

    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    Result.W = W;

    return Result;
}

inline v4
V4(v3 XYZ, f32 W)
{
    v4 Result;

    Result.X = XYZ.X;
    Result.Y = XYZ.Y;
    Result.Z = XYZ.Z;
    Result.W = W;

    return Result;
}

inline v4
LinearColor1ToSRGB255(v4 LinearColor)
{
    v4 Result;
    Result.Red = 255.0f * SquareRoot(LinearColor.Red);
    Result.Green = 255.0f * SquareRoot(LinearColor.Green);
    Result.Blue = 255.0f * SquareRoot(LinearColor.Blue);
    Result.Alpha = 255.0f * LinearColor.Alpha;
    return Result;
}