#pragma once

#define internal static
#define global static

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef i32 b32;
typedef i32 b32x;

#define F32MAX FLT_MAX
#define F32MIN -FLT_MAX
#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}

#include "ray_lane.h"
#include "ray_math.h"

#define ArraySize(arr) (sizeof(arr) / sizeof((arr)[0]))

#pragma pack(push, 1)
struct bitmap_header
{
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
    u32 Size;
    i32 Width;
    i32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 SizeOfBitmap;
    i32 HorizontalResolution;
    i32 VerticalResolution;
    u32 ColorsUsed;
    u32 ColorsImportant;
};
#pragma pack(pop)

struct image_u32
{
    u32 Width;
    u32 Height;
    u32 *Pixels;
};

struct material
{
    f32 Specular; // 0 pure diffuse/chalky, 1 is pure specular/mirrory
    v3 ReflectionColor;
    v3 EmmitColor;
};

struct plane
{
    v3 N;
    f32 d;
    u32 MaterialIndex;
};

struct sphere
{
    v3 P;
    f32 r;
    u32 MaterialIndex;
};

struct world
{
    u32 MaterialCount;
    material *Materials;

    u32 PlaneCount;
    plane *Planes;

    u32 SphereCount;
    sphere *Spheres;
};

struct random_series
{
    lane_u32 State;
};

struct work_order
{
    world *World;
    image_u32 *Image;
    u32 MinX;
    u32 MinY;
    u32 MaxX;
    u32 MaxY;
    random_series Entropy;
};

struct work_queue
{
    volatile u64 TotalBouncesComputed;
    volatile u64 TilesDone;
    volatile u64 NextWorkOrderIndex;

    u32 RaysPerPixel;
    u32 MaxBounceCount;
    u32 WorkOrderCount;
    work_order *WorkOrders;
};

struct cast_state
{
    // in
    world *World;
    work_queue *Queue;
    random_series ThreadEntropy;
    f32 FilmX, FilmY, FilmWidth, FilmHeight, FilmDistance;
    v3 FilmCenter;
    v3 CameraX, CameraY, CameraZ, CameraPosition;
    u32 RaysPerPixel;
    u32 MaxBounceCount;
    f32 HalfPixelWidth, HalfPixelHeight;

    // out
    u64 BouncesComputed;
    v3 PixelColor;
};
