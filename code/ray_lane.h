#pragma once

#ifndef SIMD_LANE_WIDTH
#define SIMD_LANE_WIDTH 4
#endif

#if (SIMD_LANE_WIDTH != 1 && SIMD_LANE_WIDTH != 4 && SIMD_LANE_WIDTH != 8)
#error "you have to set SIMD_LANE_WIDTH to either 1, 4 or 8!"
#endif

#include "base.h"
#include "ray.h"
#include "ray_math.h"


#include "ray_lane_8.h"
#include "ray_lane_4.h"
#include "ray_lane_1.h"
#include "ray_lane_not1.h"
#include "ray_lane_shared.h"
