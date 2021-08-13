#pragma once

#define SIMD_LANE_WIDTH 4

struct v3
{
	f32 x, y, z;
};


#if (SIMD_LANE_WIDTH == 8)

#include <intrin.h>

struct lane_u32
{
	__m256i V;
};

struct lane_f32
{
	__m256 V;
};

#elif (SIMD_LANE_WIDTH == 4)

#include <intrin.h>

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

internal lane_f32
LaneF32FromF32(f32 Value)
{
	lane_f32 Result;
	Result.V = _mm_set1_ps(Value);
	return Result;
}

internal lane_u32
LaneU32FromU32(u32 Value)
{
	lane_u32 Result;
	Result.V = _mm_set1_epi32(Value);
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
	__m128 MaskPS = _mm_cvtepi32_ps(Mask.V);
	Destination->V = 
		_mm_or_ps
		(
			_mm_andnot_ps(MaskPS, Destination->V),
			_mm_and_ps(MaskPS, Source.V)
		);
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

inline lane_f32
Clamp01(lane_f32 Value)
{
	lane_f32 Result = Min(Max(Value, LaneF32FromF32(0.0f)), LaneF32FromF32(1.0f));
	return Result;
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





internal lane_u32
operator+(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	Result.V = _mm_add_epi32(A.V, B.V);
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
operator^(lane_u32 LHS, lane_u32 RHS)
{
	lane_u32 Result;
	Result.V = _mm_xor_si128(LHS.V, RHS.V);
	return Result;
}

internal lane_u32
operator&(lane_u32 LHS, lane_u32 RHS)
{
	lane_u32 Result;
	Result.V = _mm_and_si128(LHS.V, RHS.V);
	return Result;
}

internal lane_u32
AndNot(lane_u32 Mask, lane_u32 Destination)
{
	lane_u32 Result;
	Result.V = _mm_andnot_si128(Mask.V, Destination.V);
	return Result;
}

internal lane_u32
operator|(lane_u32 LHS, lane_u32 RHS)
{
	lane_u32 Result;
	Result.V = _mm_or_si128(LHS.V, RHS.V);
	return Result;
}

internal lane_u32
operator!=(lane_u32 A, lane_u32 B)
{
	lane_u32 Result;
	// Note(Marwan): probably buggy
	Result.V = _mm_xor_si128(_mm_cmpeq_epi32(A.V, B.V), _mm_setzero_si128());
	return Result;
}

#elif (SIMD_LANE_WIDTH == 1)

typedef u32 lane_u32;
typedef f32 lane_f32;
typedef v3 lane_v3;

internal void
ConditionalAssign(lane_u32 *Destination, lane_u32 Source, lane_u32 Mask)
{
	Mask = Mask ? 0xffffffff : 0;
	*Destination = (~Mask & *Destination) | (Mask & Source);
}

internal void
ConditionalAssign(lane_f32 *Destination, lane_f32 Source, lane_u32 Mask)
{
	ConditionalAssign((u32 *)Destination, *(u32 *)&Source, Mask);
}

internal lane_f32
Max(lane_f32 A, lane_f32 B)
{
	if (A > B)
	{
		return A;
	}
	else
	{
		return B;
	}
}

internal b32x
MaskIsZeroed(lane_u32 LaneMask)
{
	b32x Result = (LaneMask == 0);
	return Result;
}

internal u32
HorizontalAdd(lane_u32 Lane)
{
	return Lane;
}

internal f32
HorizontalAdd(lane_f32 Lane)
{
	return Lane;
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

internal lane_f32
LaneF32FromLaneU32(lane_u32 Value)
{
	lane_f32 Result = (lane_f32)Value;
	return Result;
}

#else
#error "you have to set SIMD_LANE_WIDTH"
#endif

#if (SIMD_LANE_WIDTH != 1)

struct lane_v3
{
	lane_f32 x;
	lane_f32 y;
	lane_f32 z;
};





internal lane_f32
operator+(lane_f32 A, f32 B)
{
	lane_f32 Result = A + LaneF32FromF32((u32)B);
	return Result;
}

internal lane_f32
operator+(f32 A, lane_f32 B)
{
	lane_f32 Result = LaneF32FromF32(A) + B;
	return Result;
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
	lane_f32 Result = 0 - B;
	return Result;
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

lane_f32 &
lane_f32::operator=(f32 B)
{
	*this = LaneF32FromF32(B);
	return *this;
}

internal lane_f32
operator+=(lane_f32 &LHS, lane_f32 RHS)
{
	LHS = LHS + RHS;
	return LHS;
}

internal lane_f32
operator-=(lane_f32 &LHS, lane_f32 RHS)
{
	LHS = LHS - RHS;
	return LHS;
}

internal lane_f32
operator*=(lane_f32 &LHS, lane_f32 RHS)
{
	LHS = LHS * RHS;
	return LHS;
}

internal lane_f32
operator/=(lane_f32 &LHS, lane_f32 RHS)
{
	LHS = LHS / RHS;
	return LHS;
}





lane_u32 &
lane_u32::operator=(u32 B)
{
	*this = LaneU32FromU32(B);
	return *this;
}

internal lane_u32
operator^=(lane_u32 &LHS, lane_u32 RHS)
{
	LHS = LHS ^ RHS;
	return LHS;
}

internal lane_u32
operator&=(lane_u32 &LHS, lane_u32 RHS)
{
	LHS = LHS & RHS;
	return LHS;
}

internal lane_u32
operator|=(lane_u32 &LHS, lane_u32 RHS)
{
	LHS = LHS | RHS;
	return LHS;
}

internal lane_u32
operator+=(lane_u32 &LHS, lane_u32 RHS)
{
	LHS = LHS + RHS;
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





internal lane_v3
operator*(lane_v3 A, lane_f32 B)
{
	lane_v3 Result;
	Result.x = A.x * B;
	Result.y = A.y * B;
	Result.z = A.z * B;
	return Result;
}

internal lane_v3
operator*(lane_f32 A, lane_v3 B)
{
	lane_v3 Result = B * A;
	return Result;
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

internal lane_v3
GatherV3_(void *BasePointer, u32 Stride, lane_u32 GatherIndices)
{
	lane_v3 Result;
	Result.x = GatherF32_((f32 *)BasePointer + 0, Stride, GatherIndices);
	Result.y = GatherF32_((f32 *)BasePointer + 1, Stride, GatherIndices);
	Result.z = GatherF32_((f32 *)BasePointer + 2, Stride, GatherIndices);
	return Result;
}

internal lane_v3
operator&(lane_u32 Mask, lane_v3 A)
{
	lane_v3 Result;
	Result.x = Mask & A.x;
	Result.y = Mask & A.y;
	Result.z = Mask & A.z;
	return Result;
}








internal void
ConditionalAssign(lane_u32 *Destination, lane_u32 Source, lane_u32 Mask)
{
	*Destination = AndNot(Mask, *Destination) | (Mask & Source);
}

#endif

#define GatherF32(BasePointer, GatherIndices, Member) GatherF32_(&(BasePointer)->Member, sizeof(*(BasePointer)), GatherIndices)
#define GatherV3(BasePointer, GatherIndices, Member) GatherV3_(&(BasePointer)->Member, sizeof(*(BasePointer)), GatherIndices)


internal void
ConditionalAssign(lane_v3 *Destination, lane_v3 Source, lane_u32 Mask)
{
	ConditionalAssign(&Destination->x, Source.x, Mask);
	ConditionalAssign(&Destination->y, Source.y, Mask);
	ConditionalAssign(&Destination->z, Source.z, Mask);
}

internal b32x
MaskIsZeroed(lane_u32 LaneMask)
{

}

internal u32
HorizontalAdd(lane_u32 A)
{

}

internal f32
HorizontalAdd(lane_f32 A)
{

}

internal v3
HorizontalAdd(lane_v3 A)
{
	v3 Result;
	Result.x = HorizontalAdd(A.x);
	Result.y = HorizontalAdd(A.y);
	Result.z = HorizontalAdd(A.z);
	return Result;
}