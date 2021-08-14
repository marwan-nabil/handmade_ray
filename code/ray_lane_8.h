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
};

struct lane_f32
{
	__m256 V;
};

#endif