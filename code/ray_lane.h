#pragma once

#define SIMD_LANE_WIDTH 1

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

struct lane_v3
{
	__m256 x;
	__m256 y;
	__m256 z;
};

#elif (SIMD_LANE_WIDTH == 4)

#include <intrin.h>

struct lane_u32
{
	__m128i V;
};

struct lane_f32
{
	__m128 V;
};

struct lane_v3
{
	__m128 x;
	__m128 y;
	__m128 z;
};

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

	//u32 Result = (~Mask & *(u32 *)Destination) | (Mask & *(u32 *)&Source);
	//*Destination = *(f32 *)&Result;
}

internal void
ConditionalAssign(lane_v3 *Destination, lane_v3 Source, lane_u32 Mask)
{
	ConditionalAssign(&Destination->x, Source.x, Mask);
	ConditionalAssign(&Destination->y, Source.y, Mask);
	ConditionalAssign(&Destination->z, Source.z, Mask);
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