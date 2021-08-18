#pragma once

#if (SIMD_LANE_WIDTH == 4)

#include <intrin.h>
#include "base.h"

/**********************************************************/
/*                      Lane Types                        */
/**********************************************************/

struct lane_u32
{
	__m128i V;
	lane_u32 &operator=(u32);
};

struct lane_f32
{
	__m128 V;
	lane_f32 &operator=(f32);
};

/**********************************************************/
/*                     Lane f32 Operations                */
/**********************************************************/

internal lane_f32
LaneF32FromF32(f32 Value)
{
	lane_f32 Result;
	Result.V = _mm_set1_ps(Value);
	return Result;
}

internal lane_f32
operator+(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm_add_ps(A.V, B.V);
	return Result;
}

internal lane_f32
operator-(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm_sub_ps(A.V, B.V);
	return Result;
}

internal lane_f32
operator*(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm_mul_ps(A.V, B.V);
	return Result;
}

internal lane_f32
operator/(lane_f32 Value, lane_f32 Divisor)
{
	lane_f32 Result;
	Result.V = _mm_div_ps(Value.V, Divisor.V);
	return Result;
}

internal lane_u32
operator<(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm_castps_si128(_mm_cmplt_ps(A.V, B.V));
	return Result;
}

internal lane_u32
operator>(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm_castps_si128(_mm_cmpgt_ps(A.V, B.V));
	return Result;
}

internal lane_u32
operator==(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm_castps_si128(_mm_cmpeq_ps(A.V, B.V));
	return Result;
}

internal lane_u32
operator!=(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm_castps_si128(_mm_cmpneq_ps(A.V, B.V));
	return Result;
}

internal lane_u32
operator<=(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm_castps_si128(_mm_cmple_ps(A.V, B.V));
	return Result;
}

internal lane_u32
operator>=(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm_castps_si128(_mm_cmpge_ps(A.V, B.V));
	return Result;
}

internal lane_f32
operator&(lane_u32 Mask, lane_f32 A)
{
	lane_f32 Result;
	Result.V = _mm_and_ps(_mm_castsi128_ps(Mask.V), A.V);
	return Result;
}

inline lane_f32
Min(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm_min_ps(A.V, B.V);
	return Result;
}

inline lane_f32
Max(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm_max_ps(A.V, B.V);
	return Result;
}

internal lane_f32
SquareRoot(lane_f32 Value)
{
	lane_f32 Result;
	Result.V = _mm_sqrt_ps(Value.V);
	return Result;
}

internal void
ConditionalAssign(lane_f32 *Destination, lane_f32 Source, lane_u32 Mask)
{
	//__m128 MaskPS = _mm_cvtepi32_ps(Mask.V); // a bug
	__m128 MaskPS = _mm_castsi128_ps(Mask.V);
	Destination->V =
		_mm_or_ps
		(
			_mm_andnot_ps(MaskPS, Destination->V),
			_mm_and_ps(MaskPS, Source.V)
		);
}

internal lane_f32
GatherF32_(void *BasePointer, u32 Stride, lane_u32 GatherIndices)
{
	lane_f32 Result;
	u32 *V = (u32 *)&GatherIndices.V;
	Result.V = _mm_setr_ps
	(
		*(f32 *)((u8 *)BasePointer + V[0] * Stride),
		*(f32 *)((u8 *)BasePointer + V[1] * Stride),
		*(f32 *)((u8 *)BasePointer + V[2] * Stride),
		*(f32 *)((u8 *)BasePointer + V[3] * Stride)
	);
	return Result;
}

/**********************************************************/
/*                     Lane u32 Operations                */
/**********************************************************/


internal lane_u32
LaneU32FromU32(u32 Value)
{
	lane_u32 Result;
	Result.V = _mm_set1_epi32(Value);
	return Result;
}

internal lane_u32
LaneU32FromU32(u32 A, u32 B, u32 C, u32 D)
{
	lane_u32 Result;
	Result.V = _mm_setr_epi32(A, B, C, D);
	return Result;
}

internal lane_u32
operator+(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_add_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator-(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_sub_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator*(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_mul_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator/(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_div_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator<<(lane_u32 Value, u32 ShiftAmount)
{
	lane_u32 Result;
	Result.V = _mm_slli_epi32(Value.V, ShiftAmount);
	return Result;
}

internal lane_u32
operator>>(lane_u32 Value, u32 ShiftAmount)
{
	lane_u32 Result;
	Result.V = _mm_srli_epi32(Value.V, ShiftAmount);
	return Result;
}

internal lane_u32
operator&(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_and_si128(A.V, B.V);
	return Result;
}

internal lane_u32
operator|(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_or_si128(A.V, B.V);
	return Result;
}

internal lane_u32
operator^(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_xor_si128(A.V, B.V);
	return Result;
}

internal lane_u32
operator==(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_cmpeq_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator!=(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	//Result.V = _mm_xor_si128(_mm_cmpeq_epi32(A.V, B.V), _mm_setzero_si128()); // buggy
	Result.V = _mm_xor_si128(_mm_cmpeq_epi32(A.V, B.V), _mm_set1_epi32(0xffffffff));
	return Result;
}

internal lane_u32
AndNot(lane_u32 Mask, lane_u32 Destination)
{
	lane_u32 Result;
	Result.V = _mm_andnot_si128(Mask.V, Destination.V);
	return Result;
}

internal b32x
MaskIsZeroed(lane_u32 LaneMask)
{
	int Mask = _mm_movemask_epi8(LaneMask.V);
	return (Mask == 0);
}

/**********************************************************/
/*                     Mixed Operations                   */
/**********************************************************/

internal lane_f32
LaneF32FromLaneU32(lane_u32 Value)
{
	lane_f32 Result;
	Result.V = _mm_cvtepi32_ps(Value.V);
	return Result;
}

internal lane_u32
LaneU32FromLaneF32(lane_f32 Value)
{
	lane_u32 Result;
	Result.V = _mm_cvtps_epi32(Value.V);
	return Result;
}

#endif