#pragma once

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

inline f32
SquareRoot(f32 A)
{
    f32 Result = (f32)sqrt(A);
    return(Result);
}

inline f32
Pow(f32 A, f32 B)
{
    f32 Result = (f32)pow(A, B);
    return(Result);
}

inline u32
RoundReal32ToUInt32(f32 F)
{
    u32 Result = (u32)(F + 0.5f);
    return(Result);
}

inline v2
V2i(i32 X, i32 Y)
{
    v2 Result = {(f32)X, (f32)Y};

    return(Result);
}

inline v2
V2i(u32 X, u32 Y)
{
    v2 Result = {(f32)X, (f32)Y};

    return(Result);
}

inline v2
V2(f32 X, f32 Y)
{
    v2 Result;

    Result.x = X;
    Result.y = Y;

    return(Result);
}

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

inline f32
Square(f32 A)
{
    f32 Result = A * A;

    return(Result);
}

inline f32
Triangle01(f32 t)
{
    f32 Result = 2.0f * t;
    if (Result > 1.0f)
    {
        Result = 2.0f - Result;
    }

    return(Result);
}

inline f32
Lerp(f32 A, f32 t, f32 B)
{
    f32 Result = (1.0f - t) * A + t * B;

    return(Result);
}

inline i32
Clamp(i32 Min, i32 Value, i32 Max)
{
    i32 Result = Value;

    if (Result < Min)
    {
        Result = Min;
    }
    else if (Result > Max)
    {
        Result = Max;
    }

    return(Result);
}

inline f32
Clamp(f32 Min, f32 Value, f32 Max)
{
    f32 Result = Value;

    if (Result < Min)
    {
        Result = Min;
    }
    else if (Result > Max)
    {
        Result = Max;
    }

    return(Result);
}

inline f32
Clamp01(f32 Value)
{
    f32 Result = Clamp(0.0f, Value, 1.0f);

    return(Result);
}

inline f32
Clamp01MapToRange(f32 Min, f32 t, f32 Max)
{
    f32 Result = 0.0f;

    f32 Range = Max - Min;
    if (Range != 0.0f)
    {
        Result = Clamp01((t - Min) / Range);
    }

    return(Result);
}

inline f32
ClampAboveZero(f32 Value)
{
    f32 Result = (Value < 0) ? 0.0f : Value;
    return(Result);
}

inline f32
SafeRatioN(f32 Numerator, f32 Divisor, f32 N)
{
    f32 Result = N;

    if (Divisor != 0.0f)
    {
        Result = Numerator / Divisor;
    }

    return(Result);
}

inline f32
SafeRatio0(f32 Numerator, f32 Divisor)
{
    f32 Result = SafeRatioN(Numerator, Divisor, 0.0f);

    return(Result);
}

inline f32
SafeRatio1(f32 Numerator, f32 Divisor)
{
    f32 Result = SafeRatioN(Numerator, Divisor, 1.0f);

    return(Result);
}


inline v2
Perp(v2 A)
{
    v2 Result = {-A.y, A.x};
    return(Result);
}

inline v2
operator*(f32 A, v2 B)
{
    v2 Result;

    Result.x = A * B.x;
    Result.y = A * B.y;

    return(Result);
}

inline v2
operator*(v2 B, f32 A)
{
    v2 Result = A * B;

    return(Result);
}

inline v2 &
operator*=(v2 &B, f32 A)
{
    B = A * B;

    return(B);
}

inline v2
operator-(v2 A)
{
    v2 Result;

    Result.x = -A.x;
    Result.y = -A.y;

    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return(Result);
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;

    return(A);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return(Result);
}

inline v2 &
operator-=(v2 &A, v2 B)
{
    A = A - B;

    return(A);
}

inline v2
Hadamard(v2 A, v2 B)
{
    v2 Result = {A.x * B.x, A.y * B.y};

    return(Result);
}

inline f32
Inner(v2 A, v2 B)
{
    f32 Result = A.x * B.x + A.y * B.y;

    return(Result);
}

inline f32
LengthSq(v2 A)
{
    f32 Result = Inner(A, A);

    return(Result);
}

inline f32
Length(v2 A)
{
    f32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v2
Clamp01(v2 Value)
{
    v2 Result;

    Result.x = Clamp01(Value.x);
    Result.y = Clamp01(Value.y);

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

inline v3
Hadamard(v3 A, v3 B)
{
    v3 Result = {A.x * B.x, A.y * B.y, A.z * B.z};

    return(Result);
}

inline f32
Inner(v3 A, v3 B)
{
    f32 Result = A.x * B.x + A.y * B.y + A.z * B.z;

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

inline f32
LengthSq(v3 A)
{
    f32 Result = Inner(A, A);

    return(Result);
}

inline f32
Length(v3 A)
{
    f32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v3
Normalize(v3 A)
{
    v3 Result = A * (1.0f / Length(A));

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
Clamp01(v3 Value)
{
    v3 Result;

    Result.x = Clamp01(Value.x);
    Result.y = Clamp01(Value.y);
    Result.z = Clamp01(Value.z);

    return(Result);
}

inline v3
Lerp(v3 A, f32 t, v3 B)
{
    v3 Result = (1.0f - t) * A + t * B;

    return(Result);
}


inline v4
operator*(f32 A, v4 B)
{
    v4 Result;

    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    Result.w = A * B.w;

    return(Result);
}

inline v4
operator*(v4 B, f32 A)
{
    v4 Result = A * B;

    return(Result);
}

inline v4 &
operator*=(v4 &B, f32 A)
{
    B = A * B;

    return(B);
}

inline v4
operator-(v4 A)
{
    v4 Result;

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    Result.w = -A.w;

    return(Result);
}

inline v4
operator+(v4 A, v4 B)
{
    v4 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;

    return(Result);
}

inline v4 &
operator+=(v4 &A, v4 B)
{
    A = A + B;

    return(A);
}

inline v4
operator-(v4 A, v4 B)
{
    v4 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    Result.w = A.w - B.w;

    return(Result);
}

inline v4 &
operator-=(v4 &A, v4 B)
{
    A = A - B;

    return(A);
}

inline v4
Hadamard(v4 A, v4 B)
{
    v4 Result = {A.x * B.x, A.y * B.y, A.z * B.z, A.w * B.w};

    return(Result);
}

inline f32
Inner(v4 A, v4 B)
{
    f32 Result = A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;

    return(Result);
}

inline f32
LengthSq(v4 A)
{
    f32 Result = Inner(A, A);

    return(Result);
}

inline f32
Length(v4 A)
{
    f32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v4
Clamp01(v4 Value)
{
    v4 Result;

    Result.x = Clamp01(Value.x);
    Result.y = Clamp01(Value.y);
    Result.z = Clamp01(Value.z);
    Result.w = Clamp01(Value.w);

    return(Result);
}

inline v4
Lerp(v4 A, f32 t, v4 B)
{
    v4 Result = (1.0f - t) * A + t * B;

    return(Result);
}

inline v4
sRGBLinearize(v4 C)
{
    v4 Result;

    Result.r = Square(C.r);
    Result.g = Square(C.g);
    Result.b = Square(C.b);
    Result.a = C.a;

    return(Result);
}

inline v4
LinearTosRGB(v4 C)
{
    v4 Result;

    Result.r = SquareRoot(C.r);
    Result.g = SquareRoot(C.g);
    Result.b = SquareRoot(C.b);
    Result.a = C.a;

    return(Result);
}

inline v4
sRGBLinearize(f32 R, f32 G, f32 B, f32 A)
{
    v4 Input = {R, G, B, A};
    v4 Result = sRGBLinearize(Input);
    return(Result);
}

inline v4
SRGB255ToLinear1(v4 C)
{
    v4 Result;

    f32 Inv255 = 1.0f / 255.0f;

    Result.r = Square(Inv255 * C.r);
    Result.g = Square(Inv255 * C.g);
    Result.b = Square(Inv255 * C.b);
    Result.a = Inv255 * C.a;

    return(Result);
}

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

static v2
RayIntersect2(v2 Pa, v2 ra, v2 Pb, v2 rb)
{
    v2 Result = {};

    f32 d = (rb.x * ra.y - rb.y * ra.x);
    if (d != 0.0f)
    {
        f32 ta = ((Pa.x - Pb.x) * rb.y + (Pb.y - Pa.y) * rb.x) / d;
        f32 tb = ((Pa.x - Pb.x) * ra.y + (Pb.y - Pa.y) * ra.x) / d;

        Result = V2(ta, tb);
    }

    return(Result);
}

inline v4
BGRAUnpack4x8(u32 Packed)
{
    v4 Result = {(f32)((Packed >> 16) & 0xFF),
                 (f32)((Packed >> 8) & 0xFF),
                 (f32)((Packed >> 0) & 0xFF),
                 (f32)((Packed >> 24) & 0xFF)};

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

inline v4
RGBAUnpack4x8(u32 Packed)
{
    v4 Result = {(f32)((Packed >> 0) & 0xFF),
                 (f32)((Packed >> 8) & 0xFF),
                 (f32)((Packed >> 16) & 0xFF),
                 (f32)((Packed >> 24) & 0xFF)};

    return(Result);
}

inline u32
RGBAPack4x8(v4 Unpacked)
{
    u32 Result = ((RoundReal32ToUInt32(Unpacked.a) << 24) |
                  (RoundReal32ToUInt32(Unpacked.b) << 16) |
                  (RoundReal32ToUInt32(Unpacked.g) << 8) |
                  (RoundReal32ToUInt32(Unpacked.r) << 0));

    return(Result);
}

inline i32
IsInRange(f32 Min, f32 Value, f32 Max)
{
    i32 Result = ((Min <= Value) &&
                  (Value <= Max));

    return(Result);
}
