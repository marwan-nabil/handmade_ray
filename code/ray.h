#pragma once

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
    f32 Specularity;
    v3 EmmissionColor;
    v3 ReflectionColor;
};

struct plane
{
    u32 MaterialIndex;
    v3 Normal;
    f32 DistanceAlongNormal;
};

struct sphere
{
    u32 MaterialIndex;
    v3 CenterPoint;
    f32 Radius;
};

struct world
{
    u32 MaterialsCount;
    material *Materials;

    u32 PlanesCount;
    plane *Planes;

    u32 SpheresCount;
    sphere *Spheres;

    u64 TotalRayBouncesComputed;
    u32 TilesRetiredCount;
};
