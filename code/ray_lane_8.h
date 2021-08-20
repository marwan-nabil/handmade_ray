#pragma once
#if (SIMD_LANE_WIDTH == 8)

#include <intrin.h>
#include "base.h"

/**********************************************************/
/*                      Lane Types                        */
/**********************************************************/

struct lane_u32
{
	__m256i V;
	lane_u32 &operator=(u32);
};

struct lane_f32
{
	__m256 V;
	lane_f32 &operator=(f32);
};


/**********************************************************/
/*                     Lane f32 Operations                */
/**********************************************************/

internal lane_f32
LaneF32FromF32(f32 Value)
{
	lane_f32 Result;
	Result.V = _mm256_set1_ps(Value);
	return Result;
}

internal lane_f32
operator+(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm256_add_ps(A.V, B.V);
	return Result;
}

internal lane_f32
operator-(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm256_sub_ps(A.V, B.V);
	return Result;
}

internal lane_f32
operator*(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm256_mul_ps(A.V, B.V);
	return Result;
}

internal lane_f32
operator/(lane_f32 Value, lane_f32 Divisor)
{
	lane_f32 Result;
	Result.V = _mm256_div_ps(Value.V, Divisor.V);
	return Result;
}

internal lane_u32
operator<(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm256_castps_si256(_mm256_cmp_ps(A.V, B.V, _CMP_LT_OQ));
	return Result;
}

internal lane_u32
operator>(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm256_castps_si256(_mm256_cmp_ps(A.V, B.V, _CMP_GT_OQ));
	return Result;
}

internal lane_u32
operator==(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm256_castps_si256(_mm256_cmp_ps(A.V, B.V, _CMP_EQ_OQ));
	return Result;
}

internal lane_u32
operator!=(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm256_castps_si256(_mm256_cmp_ps(A.V, B.V, _CMP_NEQ_OQ));
	return Result;
}

internal lane_u32
operator<=(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm256_castps_si256(_mm256_cmp_ps(A.V, B.V, _CMP_LE_OQ));
	return Result;
}

internal lane_u32
operator>=(lane_f32 A, lane_f32 B)
{
	lane_u32 Result;
	Result.V = _mm256_castps_si256(_mm256_cmp_ps(A.V, B.V, _CMP_GE_OQ));
	return Result;
}

internal lane_f32
operator&(lane_u32 Mask, lane_f32 A)
{
	lane_f32 Result;
	Result.V = _mm256_and_ps(_mm256_castsi256_ps(Mask.V), A.V);
	return Result;
}

inline lane_f32
Min(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm256_min_ps(A.V, B.V);
	return Result;
}

inline lane_f32
Max(lane_f32 A, lane_f32 B)
{
	lane_f32 Result;
	Result.V = _mm256_max_ps(A.V, B.V);
	return Result;
}

internal lane_f32
SquareRoot(lane_f32 Value)
{
	lane_f32 Result;
	Result.V = _mm256_sqrt_ps(Value.V);
	return Result;
}

internal void
ConditionalAssign(lane_f32 *Destination, lane_f32 Source, lane_u32 Mask)
{
	//__m128 MaskPS = _mm_cvtepi32_ps(Mask.V); // a bug
	__m256 MaskPS = _mm256_castsi256_ps(Mask.V);
	Destination->V =
		_mm256_or_ps
		(
			_mm256_andnot_ps(MaskPS, Destination->V),
			_mm256_and_ps(MaskPS, Source.V)
		);
}

internal lane_f32
GatherF32_(void *BasePointer, u32 Stride, lane_u32 GatherIndices)
{
	lane_f32 Result;
	u32 *V = (u32 *)&GatherIndices.V;
	Result.V = _mm256_setr_ps
	(
		*(f32 *)((u8 *)BasePointer + V[0] * Stride),
		*(f32 *)((u8 *)BasePointer + V[1] * Stride),
		*(f32 *)((u8 *)BasePointer + V[2] * Stride),
		*(f32 *)((u8 *)BasePointer + V[3] * Stride),
		*(f32 *)((u8 *)BasePointer + V[4] * Stride),
		*(f32 *)((u8 *)BasePointer + V[5] * Stride),
		*(f32 *)((u8 *)BasePointer + V[6] * Stride),
		*(f32 *)((u8 *)BasePointer + V[7] * Stride)
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
	Result.V = _mm256_set1_epi32(Value);
	return Result;
}

internal lane_u32
LaneU32FromU32(u32 A, u32 B, u32 C, u32 D, u32 E, u32 F, u32 G, u32 H)
{
	lane_u32 Result;
	Result.V = _mm256_setr_epi32(A, B, C, D, E, F, G, H);
	return Result;
}

internal lane_u32
operator+(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_add_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator-(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_sub_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator*(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_mul_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator/(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_div_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator<<(lane_u32 Value, u32 ShiftAmount)
{
	lane_u32 Result;
	Result.V = _mm256_slli_epi32(Value.V, ShiftAmount);
	return Result;
}

internal lane_u32
operator>>(lane_u32 Value, u32 ShiftAmount)
{
	lane_u32 Result;
	Result.V = _mm256_srli_epi32(Value.V, ShiftAmount);
	return Result;
}

internal lane_u32
operator&(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_and_si256(A.V, B.V);
	return Result;
}

internal lane_u32
operator|(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_or_si256(A.V, B.V);
	return Result;
}

internal lane_u32
operator^(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_xor_si256(A.V, B.V);
	return Result;
}

internal lane_u32
operator==(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_cmpeq_epi32(A.V, B.V);
	return Result;
}

internal lane_u32
operator!=(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm256_xor_si256(_mm256_cmpeq_epi32(A.V, B.V), _mm256_set1_epi32(0xffffffff));
	return Result;
}

internal lane_u32
AndNot(lane_u32 Mask, lane_u32 Destination)
{
	lane_u32 Result;
	Result.V = _mm256_andnot_si256(Mask.V, Destination.V);
	return Result;
}

internal b32x
MaskIsZeroed(lane_u32 LaneMask)
{
	int Mask = _mm256_movemask_epi8(LaneMask.V);
	return (Mask == 0);
}

/**********************************************************/
/*                     Mixed Operations                   */
/**********************************************************/

internal lane_f32
LaneF32FromLaneU32(lane_u32 Value)
{
	lane_f32 Result;
	Result.V = _mm256_cvtepi32_ps(Value.V);
	return Result;
}

internal lane_u32
LaneU32FromLaneF32(lane_f32 Value)
{
	lane_u32 Result;
	Result.V = _mm256_cvtps_epi32(Value.V);
	return Result;
}

#endif