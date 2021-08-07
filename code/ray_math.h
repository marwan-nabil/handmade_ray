#pragma once

/******************************************************************/
//              real number stuff
/******************************************************************/
inline u32
RoundReal32ToUInt32(f32 F)
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
//              V3 stuff
/******************************************************************/
union v3
{
    struct
    {
        f32 x, y, z;
    };
    struct
    {
        f32 u, v, __;
    };
    struct
    {
        f32 r, g, b;
    };
    struct
    {
        v2 xy;
        f32 Ignored0_;
    };
    struct
    {
        f32 Ignored1_;
        v2 yz;
    };
    struct
    {
        v2 uv;
        f32 Ignored2_;
    };
    struct
    {
        f32 Ignored3_;
        v2 v__;
    };
    f32 E[3];
};

inline v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

inline v3
V3(v2 XY, f32 Z)
{
    v3 Result;

    Result.x = XY.x;
    Result.y = XY.y;
    Result.z = Z;

    return(Result);
}

inline v3
operator*(f32 A, v3 B)
{
    v3 Result;
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    return(Result);
}

inline v3
operator*(v3 B, f32 A)
{
    v3 Result = A * B;
    return(Result);
}

inline v3 &
operator*=(v3 &B, f32 A)
{
    B = A * B;
    return(B);
}

inline v3
operator/(v3 B, f32 A)
{
    v3 Result = (1.0f / A) * B;
    return(Result);
}

inline v3 &
operator/=(v3 &B, f32 A)
{
    B = B / A;
    return(B);
}

inline v3
operator-(v3 A)
{
    v3 Result;
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    return(Result);
}

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return(Result);
}

inline v3 &
operator+=(v3 &A, v3 B)
{
    A = A + B;
    return(A);
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

inline v3 &
operator-=(v3 &A, v3 B)
{
    A = A - B;
    return(A);
}

inline f32
Inner(v3 A, v3 B)
{
    f32 Result = A.x * B.x + A.y * B.y + A.z * B.z;
    return(Result);
}

inline f32
LengthSq(v3 A)
{
    f32 Result = Inner(A, A);
    return(Result);
}

inline v3
NOZ(v3 A)
{
    v3 Result = {};

    f32 LenSq = LengthSq(A);
    if (LenSq > Square(0.0001f))
    {
        Result = A * (1.0f / SquareRoot(LenSq));
    }

    return Result;
}

inline v3
Hadamard(v3 A, v3 B)
{
    v3 Result = {A.x * B.x, A.y * B.y, A.z * B.z};

    return(Result);
}

inline v3
Cross(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.y * B.z - A.z * B.y;
    Result.y = A.z * B.x - A.x * B.z;
    Result.z = A.x * B.y - A.y * B.x;

    return(Result);
}

inline v3
Lerp(v3 A, f32 t, v3 B)
{
    v3 Result = (1.0f - t) * A + t * B;
    return(Result);
}

/******************************************************************/
//              V4 stuff
/******************************************************************/
union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                f32 x, y, z;
            };
        };

        f32 w;
    };
    struct
    {
        union
        {
            v3 rgb;
            struct
            {
                f32 r, g, b;
            };
        };

        f32 a;
    };
    struct
    {
        v2 xy;
        f32 Ignored0_;
        f32 Ignored1_;
    };
    struct
    {
        f32 Ignored2_;
        v2 yz;
        f32 Ignored3_;
    };
    struct
    {
        f32 Ignored4_;
        f32 Ignored5_;
        v2 zw;
    };
    f32 E[4];
};

inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;

    return(Result);
}

inline v4
V4(v3 XYZ, f32 W)
{
    v4 Result;

    Result.xyz = XYZ;
    Result.w = W;

    return(Result);
}

/******************************************************************/
//              Color stuff
/******************************************************************/

inline v4
Linear1ToSRGB255(v4 C)
{
    v4 Result;

    f32 One255 = 255.0f;

    Result.r = One255 * SquareRoot(C.r);
    Result.g = One255 * SquareRoot(C.g);
    Result.b = One255 * SquareRoot(C.b);
    Result.a = One255 * C.a;

    return(Result);
}

inline u32
BGRAPack4x8(v4 Unpacked)
{
    u32 Result = ((RoundReal32ToUInt32(Unpacked.a) << 24) |
                  (RoundReal32ToUInt32(Unpacked.r) << 16) |
                  (RoundReal32ToUInt32(Unpacked.g) << 8) |
                  (RoundReal32ToUInt32(Unpacked.b) << 0));

    return(Result);
}

