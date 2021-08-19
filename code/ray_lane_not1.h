#pragma once

#if (SIMD_LANE_WIDTH != 1)

#include "base.h"

/**********************************************************/
/*                      Lane Types                        */
/**********************************************************/

struct lane_v3
{
	lane_f32 x;
	lane_f32 y;
	lane_f32 z;
};

/**********************************************************/
/*                     Lane f32 Operations                */
/**********************************************************/

lane_f32 &
lane_f32::operator=(f32 B)
{
    *this = LaneF32FromF32(B);
    return *this;
}

internal lane_f32
operator+(lane_f32 A, f32 B)
{
    lane_f32 Result = A + LaneF32FromF32(B);
    return Result;
}

internal lane_f32
operator+(f32 A, lane_f32 B)
{
    lane_f32 Result = LaneF32FromF32(A) + B;
    return Result;
}

internal lane_f32 &
operator+=(lane_f32 &LHS, lane_f32 RHS)
{
    LHS = LHS + RHS;
    return LHS;
}

internal lane_f32
operator-(lane_f32 A, f32 B)
{
    lane_f32 Result = A - LaneF32FromF32(B);
    return Result;
}

internal lane_f32
operator-(f32 A, lane_f32 B)
{
    lane_f32 Result = LaneF32FromF32(A) - B;
    return Result;
}

internal lane_f32
operator-(lane_f32 B)
{
    lane_f32 Result = 0.0f - B;
    return Result;
}

internal lane_f32 &
operator-=(lane_f32 &LHS, lane_f32 RHS)
{
    LHS = LHS - RHS;
    return LHS;
}

internal lane_f32
operator*(lane_f32 A, f32 B)
{
    lane_f32 Result = A * LaneF32FromF32(B);
    return Result;
}

internal lane_f32
operator*(f32 A, lane_f32 B)
{
    lane_f32 Result = LaneF32FromF32(A) * B;
    return Result;
}

internal lane_f32 &
operator*=(lane_f32 &LHS, lane_f32 RHS)
{
    LHS = LHS * RHS;
    return LHS;
}

internal lane_f32
operator/(lane_f32 A, f32 B)
{
    lane_f32 Result = A / LaneF32FromF32(B);
    return Result;
}

internal lane_f32
operator/(f32 A, lane_f32 B)
{
    lane_f32 Result = LaneF32FromF32(A) / B;
    return Result;
}

internal lane_f32 &
operator/=(lane_f32 &LHS, lane_f32 RHS)
{
    LHS = LHS / RHS;
    return LHS;
}

internal lane_u32
operator>(lane_f32 A, f32 B)
{
    lane_u32 Result = A > LaneF32FromF32(B);
    return Result;
}

internal lane_u32
operator>(f32 A, lane_f32 B)
{
    lane_u32 Result = LaneF32FromF32(A) > B;
    return Result;
}

internal lane_u32
operator<(lane_f32 A, f32 B)
{
    lane_u32 Result = A < LaneF32FromF32(B);
    return Result;
}

internal lane_u32
operator<(f32 A, lane_f32 B)
{
    lane_u32 Result = LaneF32FromF32(A) < B;
    return Result;
}

/**********************************************************/
/*                     Lane u32 Operations                */
/**********************************************************/

lane_u32 &
lane_u32::operator=(u32 B)
{
    *this = LaneU32FromU32(B);
    return *this;
}

internal lane_u32 &
operator^=(lane_u32 &LHS, lane_u32 RHS)
{
    LHS = LHS ^ RHS;
    return LHS;
}

internal lane_u32 &
operator&=(lane_u32 &LHS, lane_u32 RHS)
{
    LHS = LHS & RHS;
    return LHS;
}

internal lane_u32 &
operator|=(lane_u32 &LHS, lane_u32 RHS)
{
    LHS = LHS | RHS;
    return LHS;
}

internal lane_u32 &
operator+=(lane_u32 &LHS, lane_u32 RHS)
{
    LHS = LHS + RHS;
    return LHS;
}

internal void
ConditionalAssign(lane_u32 *Destination, lane_u32 Source, lane_u32 Mask)
{
	*Destination = AndNot(Mask, *Destination) | (Mask & Source);
}

/**********************************************************/
/*                     Lane v3 Operations                 */
/**********************************************************/

inline lane_v3
LaneV3(f32 X, f32 Y, f32 Z)
{
    lane_v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

internal lane_v3
LaneV3FromV3(v3 A)
{
    lane_v3 Result;
    Result.x = LaneF32FromF32(A.x);
    Result.y = LaneF32FromF32(A.y);
    Result.z = LaneF32FromF32(A.z);
    return Result;
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

inline lane_v3
operator-(lane_v3 A, lane_v3 B)
{
    lane_v3 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return(Result);
}

inline lane_v3
operator*(lane_f32 A, lane_v3 B)
{
    lane_v3 Result;
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    return(Result);
}

inline lane_v3
operator*(lane_v3 A, lane_f32 B)
{
    lane_v3 Result;
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    return Result;
}

internal v3
ExtractFirstLane(lane_v3 A)
{
    v3 Result;
    Result.x = *(f32 *)&A.x;
    Result.y = *(f32 *)&A.y;
    Result.z = *(f32 *)&A.z;
    return Result;
}

#endif