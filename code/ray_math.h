#pragma once

/******************************************************************/
//              real number stuff
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
//              V3 stuff
/******************************************************************/

inline lane_v3
V3(f32 X, f32 Y, f32 Z)
{
    lane_v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

inline lane_v3
V3(v2 XY, f32 Z)
{
    lane_v3 Result;

    Result.x = XY.x;
    Result.y = XY.y;
    Result.z = Z;

    return(Result);
}

inline lane_v3
operator*(f32 A, lane_v3 B)
{
    lane_v3 Result;
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    return(Result);
}

inline lane_v3
operator*(lane_v3 B, f32 A)
{
    lane_v3 Result = A * B;
    return(Result);
}

inline lane_v3 &
operator*=(lane_v3 &B, f32 A)
{
    B = A * B;
    return(B);
}

inline lane_v3
operator/(lane_v3 B, f32 A)
{
    lane_v3 Result = (1.0f / A) * B;
    return(Result);
}

inline lane_v3 &
operator/=(lane_v3 &B, f32 A)
{
    B = B / A;
    return(B);
}

inline lane_v3
operator-(lane_v3 A)
{
    lane_v3 Result;
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    return(Result);
}

inline lane_v3
operator+(lane_v3 A, lane_v3 B)
{
    lane_v3 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return(Result);
}

inline lane_v3 &
operator+=(lane_v3 &A, lane_v3 B)
{
    A = A + B;
    return(A);
}

inline lane_v3
operator-(lane_v3 A, lane_v3 B)
{
    lane_v3 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return(Result);
}

inline lane_v3 &
operator-=(lane_v3 &A, lane_v3 B)
{
    A = A - B;
    return(A);
}

inline lane_f32
Inner(lane_v3 A, lane_v3 B)
{
    lane_f32 Result = A.x * B.x + A.y * B.y + A.z * B.z;
    return(Result);
}

inline lane_f32
LengthSq(lane_v3 A)
{
    lane_f32 Result = Inner(A, A);
    return(Result);
}

inline lane_v3
NOZ(lane_v3 A)
{
    lane_v3 Result = {};

    lane_f32 LenSq = LengthSq(A);
    lane_u32 Mask = LenSq > Square(0.0001f);
    
    ConditionalAssign(&Result, A * (1.0f / SquareRoot(LenSq)), Mask);

    return Result;
}

inline lane_v3
Hadamard(lane_v3 A, lane_v3 B)
{
    lane_v3 Result = {A.x * B.x, A.y * B.y, A.z * B.z};

    return(Result);
}

inline lane_v3
Cross(lane_v3 A, lane_v3 B)
{
    lane_v3 Result;

    Result.x = A.y * B.z - A.z * B.y;
    Result.y = A.z * B.x - A.x * B.z;
    Result.z = A.x * B.y - A.y * B.x;

    return(Result);
}

inline lane_v3
Lerp(lane_v3 A, f32 t, lane_v3 B)
{
    lane_v3 Result = (1.0f - t) * A + t * B;
    return(Result);
}

/******************************************************************/
//              Color stuff
/******************************************************************/

inline lane_v3
LaneV3(lane_f32 X, lane_f32 Y, lane_f32 Z)
{
    lane_v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

//inline lane_v3
//Lerp(lane_v3 A, lane_f32 t, lane_v3 B)
//{
//    lane_v3 Result = (1.0f - t) * A + t * B;
//    return(Result);
//}