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

internal f32
RandomUnilateral()
{
    f32 Result = (f32)rand() / (f32)RAND_MAX;
    return Result;
}

internal f32
RandomBilateral()
{
    f32 Result = -1.0f + 2.0f * RandomUnilateral();
    return Result;
}

internal v3 
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 Result = {};

    f32 Tolerance = 0.00001f;
    f32 MinHitDistance = 0.001f;

    v3 LastHitNormal = {};

    v3 Attenuation = V3(1, 1, 1);

    for (u32 RayCount = 0; RayCount < 4; RayCount++)
    {
        f32 HitDistance = F32MAX;
        u32 HitMaterialIndex = 0;

        for (u32 PlaneIndex = 0;
             PlaneIndex < World->PlaneCount;
             PlaneIndex++)
        {
            plane Plane = World->Planes[PlaneIndex];

            f32 Denominator = Inner(Plane.N, RayDirection);
            if (Denominator < -Tolerance || Denominator > Tolerance)
            {
                f32 t = (-Plane.d - Inner(Plane.N, RayOrigin)) / Denominator;
                if (t > MinHitDistance && t < HitDistance)
                {
                    HitDistance = t;
                    HitMaterialIndex = Plane.MaterialIndex;
                    LastHitNormal = Plane.N;
                }
            }
        }

        for (u32 SphereIndex = 0;
             SphereIndex < World->SphereCount;
             SphereIndex++)
        {
            sphere Sphere = World->Spheres[SphereIndex];
        
            v3 SphereRelativeRayOrigin = RayOrigin - Sphere.P;

            f32 a = Inner(RayDirection, RayDirection);
            f32 b = Inner(RayDirection, SphereRelativeRayOrigin) + Inner(SphereRelativeRayOrigin, RayDirection);
            f32 c = Inner(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - Sphere.r * Sphere.r;

            f32 Denominator = 2 * a;
            f32 RootTerm = SquareRoot(b * b - 4.0f * a * c);

            if (RootTerm > Tolerance)
            {
                f32 TPos = (-b + RootTerm) / Denominator;
                f32 TNeg = (-b - RootTerm) / Denominator;

                f32 t = TPos;
                if (TNeg > MinHitDistance && TNeg < TPos)
                {
                    t = TNeg;
                }

                if (t > MinHitDistance && t < HitDistance)
                {
                    HitDistance = t;
                    HitMaterialIndex = Sphere.MaterialIndex;
                    LastHitNormal = NOZ(HitDistance * RayDirection + SphereRelativeRayOrigin);
                }
            }
        }

        if (HitMaterialIndex)
        {
            material HitMaterial = World->Materials[HitMaterialIndex];
            Result += Hadamard(Attenuation, HitMaterial.EmmitColor);
#if 0
            f32 CosineAttenuation = Inner(-RayDirection, LastHitNormal);
            if (CosineAttenuation < 0.0f)
            {
                CosineAttenuation = 0.0f;
            }
            Attenuation = Hadamard(Attenuation, CosineAttenuation * HitMaterial.ReflectionColor);
#endif            
            Attenuation = Hadamard(Attenuation, HitMaterial.ReflectionColor);
            RayOrigin += HitDistance * RayDirection;

            v3 PureBounce = RayDirection - 2.0f * Inner(RayDirection, LastHitNormal) * LastHitNormal;
            v3 RandomVector = V3(RandomBilateral(), RandomBilateral(), RandomBilateral());
            v3 RandomBounce = NOZ(LastHitNormal + RandomVector);
            RayDirection = NOZ(Lerp(RandomBounce, HitMaterial.Scatter, PureBounce));
        }
        else
        {
            material HitMaterial = World->Materials[HitMaterialIndex];
            Result += Hadamard(Attenuation, HitMaterial.EmmitColor);
            break;
        }
    }

    return Result;
}

int
main(int ArgCount, char **Arguments)
{
    printf("Raycasting ...\n");
    material Materials[5] = {};
    Materials[0].EmmitColor = V3(74.0f / 255.0f, 101.0f / 255.0f, 129.0f / 255.0f); // sky
    Materials[0].Scatter = 1;
    Materials[1].ReflectionColor = V3(0.5f, 0.5f, 0.5f); // plane
    Materials[1].Scatter = 0.2f;
    Materials[2].ReflectionColor = V3(0.9f, 0.3f, 0.1f); // sphere 1
    Materials[2].Scatter = 0.001f;
    Materials[3].ReflectionColor = V3(0.1f, 0.7f, 0.1f); // sphere 2
    Materials[3].Scatter = 0.7f;
    Materials[4].ReflectionColor = V3(0.9f, 0.9f, 0.8f); // sphere 3
    Materials[4].Scatter = 1.0f;

    plane Planes[1] = {};
    Planes[0].N = V3(0, 0, 1);
    Planes[0].d = 0;
    Planes[0].MaterialIndex = 1;

    sphere Spheres[3] = {};
    Spheres[0].P = V3(0, 0, 0);
    Spheres[0].r = 1.0f;
    Spheres[0].MaterialIndex = 2;
    Spheres[1].P = V3(3, -2, 0);
    Spheres[1].r = 1.0f;
    Spheres[1].MaterialIndex = 3;
    Spheres[2].P = V3(-2, -1, 2);
    Spheres[2].r = 1.0f;
    Spheres[2].MaterialIndex = 4;

    world World = {};
    World.Materials = Materials;
    World.MaterialCount = ArraySize(Materials);
    World.Planes = Planes;
    World.PlaneCount = ArraySize(Planes);
    World.Spheres = Spheres;
    World.SphereCount = ArraySize(Spheres);

    v3 CameraPosition = V3(0, -10, 1);
    v3 CameraZ = NOZ(CameraPosition);
    v3 CameraX = NOZ(Cross(V3(0, 0, 1), CameraZ));
    v3 CameraY = NOZ(Cross(CameraZ, CameraX));

    image_u32 Image = AllocateImage(1280, 720); // 4096, 2048

    f32 FilmDistance = 1.0f;
    f32 FilmWidth = 1.0f;
    f32 FilmHeight = 1.0f;

    if (Image.Width > Image.Height)
    {
        FilmHeight = ((f32)Image.Height / (f32)Image.Width);
    }
    else if (Image.Height > Image.Width)
    {
        FilmWidth = ((f32)Image.Width / (f32)Image.Height);
    }

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

            v3 Color = {};
            u32 RaysPerPixel = 16;
            f32 ContributionRatio = 1.0f / (f32)RaysPerPixel;
            for (u32 RayIndex = 0; RayIndex < RaysPerPixel; RayIndex++)
            {
                Color += ContributionRatio * RayCast(&World, RayOrigin, RayDirection);
            }

            v4 BMPColor = V4(255.0f * Color, 255.0f);
            *Out++ = BGRAPack4x8(BMPColor);
        }

        printf("\rRaycasting %d%%", 100 * (Y + 1) / Image.Height);
        fflush(stdout);
    }
    
    WriteImage((char *)"test.bmp", Image);
    printf(" Done.\n");
    return 0;
}