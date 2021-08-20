#pragma once

#if (SIMD_LANE_WIDTH == 1)

#include "base.h"

/**********************************************************/
/*                      Lane Types                        */
/**********************************************************/

typedef u32 lane_u32;
typedef f32 lane_f32;
typedef v3 lane_v3;

//struct lane_v3
//{
//	lane_f32 x;
//	lane_f32 y;
//	lane_f32 z;
//};

/**********************************************************/
/*                      Forward Declarations              */
/**********************************************************/

internal void ConditionalAssign(lane_u32 *Destination, lane_u32 Source, lane_u32 Mask);


/**********************************************************/
/*                     Lane f32 Operations                */
/**********************************************************/

internal lane_f32
LaneF32FromF32(f32 Value)
{
	return Value;
}

internal lane_f32
LaneF32FromLaneU32(lane_u32 Value)
{
	lane_f32 Result = (lane_f32)Value;
	return Result;
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

internal lane_f32
Min(lane_f32 A, lane_f32 B)
{
	if (A < B)
	{
		return A;
	}
	else
	{
		return B;
	}
}

internal lane_f32
GatherF32_(void *BasePointer, u32 Stride, lane_u32 GatherIndex)
{
	return *(f32 *)((u8 *)BasePointer + GatherIndex * Stride);
}

/**********************************************************/
/*                     Lane u32 Operations                */
/**********************************************************/


internal void
ConditionalAssign(lane_u32 *Destination, lane_u32 Source, lane_u32 Mask)
{
	Mask = Mask ? 0xffffffff : 0;
	*Destination = (~Mask & *Destination) | (Mask & Source);
}

internal b32x
MaskIsZeroed(lane_u32 LaneMask)
{
	b32x Result = (LaneMask == 0);
	return Result;
}

internal lane_u32
LaneU32FromU32(u32 A)
{
	return A;
}

internal lane_u32
LaneU32FromU32(u32 A, u32 B, u32 C, u32 D, u32 E, u32 F, u32 G, u32 H)
{
	return A;
}

/**********************************************************/
/*                     Lane v3 Operations                 */
/**********************************************************/

internal lane_v3
LaneV3FromV3(v3 A)
{
	return A;
}


internal v3
ExtractFirstLane(lane_v3 A)
{
	return A;
}


#endif