#pragma once

#include "base.h"

/**********************************************************/
/*                     Lane f32 Operations                */
/**********************************************************/

inline lane_f32
Clamp01(lane_f32 Value)
{
    lane_f32 Result = Min(Max(Value, LaneF32FromF32(0.0f)), LaneF32FromF32(1.0f));
    return Result;
}

internal f32
HorizontalAdd(lane_f32 A)
{
#if (SIMD_LANE_WIDTH == 1)
    f32 Result = A;
#elif (SIMD_LANE_WIDTH == 4)
    f32 *V = (f32 *)&A.V;
    f32 Result = V[0] + V[1] + V[2] + V[3];
#elif (SIMD_LANE_WIDTH == 8)
    f32 *V = (f32 *)&A.V;
    f32 Result = V[0] + V[1] + V[2] + V[3] + V[4] + V[5] + V[6] + V[7];
#endif
    return Result;
}

#define GatherF32(BasePointer, GatherIndices, Member) GatherF32_(&(BasePointer)->Member, sizeof(*(BasePointer)), GatherIndices)

/**********************************************************/
/*                     Lane u32 Operations                */
/**********************************************************/

internal u64
HorizontalAdd(lane_u32 A)
{
#if (SIMD_LANE_WIDTH == 1)
    u64 Result = A;
#elif (SIMD_LANE_WIDTH == 4)
    u32 *V = (u32 *)&A.V;
    u64 Result = (u64)V[0] + (u64)V[1] + (u64)V[2] + (u64)V[3];
#elif (SIMD_LANE_WIDTH == 8)
    u32 *V = (u32 *)&A.V;
    u64 Result = (u64)V[0] + (u64)V[1] + (u64)V[2] + (u64)V[3] + (u64)V[4] + (u64)V[5] + (u64)V[6] + (u64)V[7];
#endif
    return Result;
}

/**********************************************************/
/*                     Lane v3 Operations                 */
/**********************************************************/

inline lane_v3
LaneV3(lane_f32 X, lane_f32 Y, lane_f32 Z)
{
    lane_v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

inline lane_v3
LaneV3(v2 XY, f32 Z)
{
    lane_v3 Result;

    Result.x = XY.x;
    Result.y = XY.y;
    Result.z = Z;

    return(Result);
}

inline lane_v3 &
operator+=(lane_v3 &A, lane_v3 B)
{
    A = A + B;
    return(A);
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

inline lane_v3 &
operator-=(lane_v3 &A, lane_v3 B)
{
    A = A - B;
    return(A);
}

inline lane_v3
operator/(lane_v3 B, lane_f32 A)
{
    lane_v3 Result = B * (1.0f / A);
    return(Result);
}

inline lane_v3 &
operator/=(lane_v3 &B, lane_f32 A)
{
    B = B / A;
    return(B);
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

internal void
ConditionalAssign(lane_v3 *Destination, lane_v3 Source, lane_u32 Mask)
{
    ConditionalAssign(&Destination->x, Source.x, Mask);
    ConditionalAssign(&Destination->y, Source.y, Mask);
    ConditionalAssign(&Destination->z, Source.z, Mask);
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
Lerp(lane_v3 A, lane_f32 t, lane_v3 B)
{
    lane_v3 Result = (1.0f - t) * A + t * B;
    return(Result);
}

internal lane_v3
operator&(lane_u32 Mask, lane_v3 A)
{
    lane_v3 Result;
#if (SIMD_LANE_WIDTH == 1)
    Mask = Mask ? 0xffffffff : 0;
    u32 X = (Mask & *(u32 *)&A.x);
    u32 Y = (Mask & *(u32 *)&A.y);
    u32 Z = (Mask & *(u32 *)&A.z);
    Result.x = *(f32 *)&X;
    Result.y = *(f32 *)&Y;
    Result.z = *(f32 *)&Z;
#elif (SIMD_LANE_WIDTH != 1)
    Result.x = Mask & A.x;
    Result.y = Mask & A.y;
    Result.z = Mask & A.z;
#endif
    return Result;
}

internal v3
HorizontalAdd(lane_v3 Lane)
{
    v3 Result = {
        HorizontalAdd(Lane.x),
        HorizontalAdd(Lane.y),
        HorizontalAdd(Lane.z)
    };
    return Result;
}

#define GatherV3(BasePointer, GatherIndices, Member) GatherV3_(&((BasePointer)->Member), sizeof(*(BasePointer)), GatherIndices)

internal lane_v3
GatherV3_(void *GatherBase, u32 Stride, lane_u32 GatherIndices)
{
    lane_v3 Result;
    Result.x = GatherF32_((f32 *)GatherBase + 0, Stride, GatherIndices);
    Result.y = GatherF32_((f32 *)GatherBase + 1, Stride, GatherIndices);
    Result.z = GatherF32_((f32 *)GatherBase + 2, Stride, GatherIndices);
    return Result;
}
