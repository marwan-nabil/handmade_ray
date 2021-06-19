#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

#include "ray.h"

internal u32
GetTotalPixelSize(image_u32 Image)
{
    u32 Result = Image.Width * Image.Height * sizeof(u32);
    return Result;
}

internal image_u32
AllocateImage(u32 Width, u32 Height)
{
    image_u32 Result = {};
    Result.Width = Width;
    Result.Height = Height;
    Result.Pixels = (u32 *)malloc(GetTotalPixelSize(Result));
    return Result;
}

internal void
WriteImage(char *OutputFileName, image_u32 Image)
{
    u32 OutputPixelsSize = GetTotalPixelSize(Image);

    bitmap_header Header = {};
    Header.FileType = 0x4d42;
    Header.FileSize = sizeof(bitmap_header) + OutputPixelsSize;
    Header.BitmapOffset = sizeof(bitmap_header);
    Header.Size = sizeof(bitmap_header) - 14;
    Header.Width = Image.Width;
    Header.Height = (i32)Image.Height;
    Header.Planes = 1;
    Header.BitsPerPixel = 32;
    Header.Compression = 0;
    Header.SizeOfBitmap = OutputPixelsSize;
    Header.HorizontalResolution = 0;
    Header.VerticalResolution = 0;
    Header.ColorsUsed = 0;
    Header.ColorsImportant = 0;

    FILE *OutFile = fopen(OutputFileName, "wb");
    if (OutFile)
    {
        fwrite(&Header, sizeof(bitmap_header), 1, OutFile);
        fwrite(Image.Pixels, OutputPixelsSize, 1, OutFile);
        fclose(OutFile);
    }
    else
    {
        fprintf(stderr, "[ERROR]: unable to write output file %s.\n", OutputFileName);
    }
}

internal v3 
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 Result = World->Materials[0].Color;

    f32 HitDistance = F32MAX;
    f32 Tolerance = 0.0001f;

    for (u32 PlaneIndex = 0;
         PlaneIndex < World->PlaneCount;
         PlaneIndex++)
    {
        plane Plane = World->Planes[PlaneIndex];

        f32 Denominator = Inner(Plane.N, RayDirection);
        if (Denominator < -Tolerance || Denominator > Tolerance)
        {
            f32 t = (-Plane.d - Inner(Plane.N, RayOrigin)) / Denominator;
            if (t > 0.0f && t < HitDistance)
            {
                HitDistance = t;
                Result = World->Materials[Plane.MaterialIndex].Color;
            }
        }
    }

    for (u32 SphereIndex = 0;
         SphereIndex < World->SphereCount;
         SphereIndex++)
    {
        sphere Sphere = World->Spheres[SphereIndex];
        
        f32 a = Inner(RayDirection, RayDirection);
        f32 b = 2.0f * Inner(RayDirection, RayOrigin);
        f32 c = Inner(RayOrigin, RayOrigin) - Sphere.r * Sphere.r;

        f32 Denominator = 2 * a;
        f32 RootTerm = SquareRoot(b * b - 4.0f * a * c);

        if (RootTerm > Tolerance)
        {
            f32 TPos = (-b + RootTerm) / Denominator;
            f32 TNeg = (-b - RootTerm) / Denominator;

            f32 t = TPos;
            if (TNeg > 0 && TNeg < TPos)
            {
                t = TNeg;
            }

            if (t > 0.0f && t < HitDistance)
            {
                HitDistance = t;
                Result = World->Materials[Sphere.MaterialIndex].Color;
            }
        }

    }

    return Result;
}

int
main(int ArgCount, char **Arguments)
{
    printf("Raycasting ...");
    material Materials[3] = {};
    Materials[0].Color = V3(0, 0, 0);
    Materials[1].Color = V3(1, 0, 0);
    Materials[2].Color = V3(0, 0, 1);

    plane Plane = {};
    Plane.N = V3(0, 0, 1);
    Plane.d = 0;
    Plane.MaterialIndex = 1;

    sphere Sphere = {};
    Sphere.P = V3(0, 0, 0);
    Sphere.r = 1.0f;
    Sphere.MaterialIndex = 2;

    world World = {};
    World.MaterialCount = 3;
    World.Materials = Materials;
    World.PlaneCount = 1;
    World.Planes = &Plane;
    World.SphereCount = 1;
    World.Spheres = &Sphere;

    v3 CameraPosition = V3(0, -10, 1);
    v3 CameraZ = NOZ(CameraPosition);
    v3 CameraX = NOZ(Cross(V3(0, 0, 1), CameraZ));
    v3 CameraY = NOZ(Cross(CameraZ, CameraX));

    image_u32 Image = AllocateImage(1280, 720);

    f32 FilmDistance = 1.0f;
    f32 FilmWidth = 1.0f;
    f32 FilmHeight = 1.0f;
    v3 FilmCenter = CameraPosition - FilmDistance * CameraZ;

    u32 *Out = Image.Pixels;
    for (u32 Y = 0;
         Y < Image.Height;
         Y++)
    {
        f32 FilmY = -1.0f + 2.0f * ((f32)Y / Image.Height);
        for (u32 X = 0;
             X < Image.Width;
             X++)
        {
            f32 FilmX = -1.0f + 2.0f * ((f32)X / Image.Width);
            v3 PositionOnFilm = FilmCenter 
                + 0.5f * FilmWidth * FilmX * CameraX
                + 0.5f * FilmHeight * FilmY * CameraY;

            v3 RayOrigin = CameraPosition;
            v3 RayDirection = NOZ(PositionOnFilm - CameraPosition);
            v3 Color = RayCast(&World, RayOrigin, RayDirection);
            v4 BMPColor = V4(255.0f * Color, 255.0f);
            *Out++ = BGRAPack4x8(BMPColor);
        }
    }
    
    WriteImage((char *)"test.bmp", Image);
    printf(" Done.\n");
    return 0;
}