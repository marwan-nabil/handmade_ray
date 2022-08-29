#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "ray_math.h"
#include "ray.h"
#include <ctime>

internal f32
GenerateRandomUnilateral()
{
    return (f32)rand() / (f32)RAND_MAX;
}

internal f32
GenerateRandomBilateral()
{
    return 2 * GenerateRandomUnilateral() - 1;
}

internal u32 *
GetPixelPointer(image_u32 Image, u32 X, u32 Y)
{
    u32 *Result = Image.Pixels + X + Y * Image.Width;
    return Result;
}

internal void
RenderTile(world *World, image_u32 OutputImage, u32 MinX, u32 OnePastMaxX, u32 MinY, u32 OnePastMaxY)
{
    v3 CameraPosition = V3(0, -10, 1);
    v3 CameraZAxis = NormalizeToZero(CameraPosition);
    v3 CameraXAxis = NormalizeToZero(CrossProduct(V3(0, 0, 1), CameraZAxis));
    v3 CameraYAxis = NormalizeToZero(CrossProduct(CameraZAxis, CameraXAxis));

    f32 FilmWidth = 1.0f;
    f32 FilmHeight = 1.0f;
    if (OutputImage.Width > OutputImage.Height)
    {
        FilmHeight = FilmHeight * (f32)OutputImage.Height / (f32)OutputImage.Width;
    }
    else if (OutputImage.Width < OutputImage.Height)
    {
        FilmWidth = FilmWidth * (f32)OutputImage.Width / (f32)OutputImage.Height;
    }

    f32 HalfFilmWidth = 0.5f * FilmWidth;
    f32 HalfFilmHeight = 0.5f * FilmHeight;
    f32 FilmDistanceFromCamera = 1.0f;
    v3 FilmXAxis = CameraXAxis;
    v3 FilmYAxis = CameraYAxis;

    v3 FilmCenter = CameraPosition - FilmDistanceFromCamera * CameraZAxis;

    u32 ReflectionsPerRay = 8;
    u32 RaysPerPixel = 8;
    f32 SingleRayContributionRatio = 1.0f / (f32)RaysPerPixel;

    f32 HalfPixelWidth = 0.5f / OutputImage.Width;
    f32 HalfPixelHeight = 0.5f / OutputImage.Height;

    f32 MinimumRayHitDistance = 0.001f;
    f32 NearZeroToleranceValue = 0.0001f;

    u32 BouncesComputed = 0;

    for (u32 Y = MinY;  Y < OnePastMaxY; Y++)
    {
        u32 *CurrentOutputPixel = GetPixelPointer(OutputImage, MinX, Y);
        f32 FilmYRatio = -1.0f + 2 * ((f32)Y / (f32)OutputImage.Height);

        for (u32 X = MinX; X < OnePastMaxX; X++)
        {
            v3 PixelColor = {};

            f32 FilmXRatio = -1.0f + 2 * ((f32)X / (f32)OutputImage.Width);

            for (u32 RayCount = 0; RayCount < RaysPerPixel; RayCount++)
            {
                v3 RayColor = {};

                f32 FilmXRatioWithNoise = FilmXRatio + GenerateRandomBilateral() * HalfPixelWidth;
                f32 FilmYRatioWithNoise = FilmYRatio + GenerateRandomBilateral() * HalfPixelHeight;

                v3 PositionOnFilm =
                    FilmCenter +
                    FilmXRatioWithNoise * HalfFilmWidth * FilmXAxis +
                    FilmYRatioWithNoise * HalfFilmHeight * FilmYAxis;

                v3 CurrentRayOrigin = CameraPosition;
                v3 CurrentRayDirection = NormalizeToZero(PositionOnFilm - CameraPosition);
                v3 CurrentColorAttenuation = V3(1, 1, 1);

                v3 NextRayOrigin = {};
                v3 NextRayOriginNormal = {};
                v3 NextRayDirection = {};
                v3 NextColorAttenuation = {};

                for (u32 RayReflectionCount = 0; RayReflectionCount < ReflectionsPerRay; RayReflectionCount++)
                {
                    f32 ClosestHitDistance = F32MAX;
                    u32 HitMaterialIndex = 0;

                    for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
                    {
                        plane *CurrentPlane = &World->Planes[PlaneIndex];
                        f32 IntersectionPointDistanceAlongRay = F32MAX;

                        f32 Denominator = InnerProduct(CurrentRayDirection, CurrentPlane->Normal);
                        if ((Denominator < -NearZeroToleranceValue) || (Denominator > NearZeroToleranceValue))
                        {
                            IntersectionPointDistanceAlongRay = 
                                (-InnerProduct(CurrentRayOrigin, CurrentPlane->Normal) + CurrentPlane->DistanceAlongNormal) /
                                Denominator;

                            if ((IntersectionPointDistanceAlongRay > MinimumRayHitDistance) && (IntersectionPointDistanceAlongRay < ClosestHitDistance))
                            {
                                ClosestHitDistance = IntersectionPointDistanceAlongRay;
                                HitMaterialIndex = CurrentPlane->MaterialIndex;

                                NextRayOrigin = CurrentRayOrigin + IntersectionPointDistanceAlongRay * CurrentRayDirection;
                                NextRayOriginNormal = NormalizeToZero(CurrentPlane->Normal);
                            }
                        }
                    }

                    for (u32 SphereIndex = 0; SphereIndex < World->SpheresCount; SphereIndex++)
                    {
                        sphere *CurrentSphere = &World->Spheres[SphereIndex];
                        f32 IntersectionPointDistanceAlongRay = F32MAX;
        
                        /* solving the ray & sphere equations together we get at the quadratic equation */

                        v3 SphereRelativeRayOrigin = CurrentRayOrigin - CurrentSphere->CenterPoint;
                        f32 A = InnerProduct(CurrentRayDirection, CurrentRayDirection);
                        f32 B = 2 * InnerProduct(CurrentRayDirection, SphereRelativeRayOrigin);
                        f32 C = InnerProduct(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - Square(CurrentSphere->Radius);
                        f32 Denominator = 2 * A;
                        f32 RootTerm = SquareRoot(Square(B) - 4 * A * C);

                        //if ((Denominator < -ToleranceValue) || (Denominator > ToleranceValue))
                        if (RootTerm > NearZeroToleranceValue)
                        {
                            f32 Solution1 = (- B + RootTerm) / Denominator;
                            f32 Solution2 = (- B - RootTerm) / Denominator;

                            if ((Solution2 > 0) && (Solution2 < Solution1))
                            {
                                IntersectionPointDistanceAlongRay = Solution2;
                            }
                            else
                            {
                                IntersectionPointDistanceAlongRay = Solution1;
                            }

                            if ((IntersectionPointDistanceAlongRay > MinimumRayHitDistance) && (IntersectionPointDistanceAlongRay < ClosestHitDistance))
                            {
                                ClosestHitDistance = IntersectionPointDistanceAlongRay;
                                HitMaterialIndex = CurrentSphere->MaterialIndex;

                                NextRayOrigin = CurrentRayOrigin + IntersectionPointDistanceAlongRay * CurrentRayDirection;
                                NextRayOriginNormal = NormalizeToZero(NextRayOrigin - CurrentSphere->CenterPoint);
                            }
                        }
                    }

                    material *HitMaterial = &World->Materials[HitMaterialIndex];
                    RayColor = RayColor + HadamardProduct(CurrentColorAttenuation, HitMaterial->EmmissionColor);

                    if (HitMaterialIndex)
                    {
                        v3 NextRayDirectionIfPureBounce = NormalizeToZero
                        (
                            CurrentRayDirection - 
                            2.0f * InnerProduct(CurrentRayDirection, NextRayOriginNormal) * NextRayOriginNormal
                        );
            
                        v3 NextRayDirectionIfRandomBounce = NormalizeToZero
                        (
                            NextRayOriginNormal + 
                            V3(GenerateRandomBilateral(), GenerateRandomBilateral(), GenerateRandomBilateral())
                        );

                        NextRayDirection = NormalizeToZero
                        (
                            LinearInterpolation(NextRayDirectionIfRandomBounce, NextRayDirectionIfPureBounce, HitMaterial->Specularity)
                        );

#if 1
                        f32 CosineAttenuation = InnerProduct(-CurrentRayDirection, NextRayOriginNormal);
                        if (CosineAttenuation < 0.0f)
                        {
                            CosineAttenuation = 0.0f;
                        }
#else
                        f32 CosineAttenuation = 1.0f;
#endif

                        NextColorAttenuation = HadamardProduct(CurrentColorAttenuation, CosineAttenuation * HitMaterial->ReflectionColor);
                    }
                    else
                    {
                        break;
                    }

                    CurrentRayOrigin = NextRayOrigin;
                    CurrentRayDirection = NextRayDirection;
                    CurrentColorAttenuation = NextColorAttenuation;
                    BouncesComputed++;
                }

                /*------------------------------------------------------------------*/

                PixelColor = PixelColor + SingleRayContributionRatio * RayColor;
            }

            v4 BMPColor = V4
            (
                255.0f * ExactLinearToSRGB(PixelColor.Red),
                255.0f * ExactLinearToSRGB(PixelColor.Green),
                255.0f * ExactLinearToSRGB(PixelColor.Blue),
                255.0f
            );

            *CurrentOutputPixel++ = 
                (RoundF32ToU32(BMPColor.Alpha) << 24) |
                (RoundF32ToU32(BMPColor.Red) << 16) |
                (RoundF32ToU32(BMPColor.Green) << 8) |
                (RoundF32ToU32(BMPColor.Blue) << 0);
        }
    }

    World->TotalRayBouncesComputed += BouncesComputed;
    World->TilesRetiredCount++;
}

int main(int argc, char **argv)
{
    image_u32 OutputImage = {};
    OutputImage.Width = 1280;
    OutputImage.Height = 720;

    u32 OutputPixelsSize = sizeof(u32) * OutputImage.Width * OutputImage.Height;

    OutputImage.Pixels = (u32 *)malloc(OutputPixelsSize);
    if (OutputImage.Pixels)
    {
        material Materials[7] = {};

        Materials[0].Specularity = 0;
        Materials[0].EmmissionColor = V3(0.3f, 0.4f, 0.5f);
        Materials[0].ReflectionColor = V3(0, 0, 0);

        Materials[1].Specularity = 0;
        Materials[1].EmmissionColor = V3(0.0, 0.0, 0.0);
        Materials[1].ReflectionColor = V3(0.5f, 0.5f, 0.5f);

        Materials[2].Specularity = 0;
        Materials[2].EmmissionColor = V3(0.0, 0.0, 0.0);
        Materials[2].ReflectionColor = V3(0.7f, 0.5f, 0.3f);

        Materials[3].Specularity = 0;
        Materials[3].EmmissionColor = V3(4.0f, 0, 0);
        Materials[3].ReflectionColor = V3(0, 0, 0);

        Materials[4].Specularity = 0.7;
        Materials[4].EmmissionColor = V3(0, 0, 0);
        Materials[4].ReflectionColor = V3(0.2f, 0.8f, 0.2f);

        Materials[5].Specularity = 0.85;
        Materials[5].EmmissionColor = V3(0, 0, 0);
        Materials[5].ReflectionColor = V3(0.4f, 0.8f, 0.9f);
        
        Materials[6].Specularity = 1;
        Materials[6].EmmissionColor = V3(0, 0, 0);
        Materials[6].ReflectionColor = V3(0.95f, 0.95f, 0.95f);

        plane Planes[1] = {};

        Planes[0].MaterialIndex = 1;
        Planes[0].Normal = V3(0, 0, 1);
        Planes[0].DistanceAlongNormal = 0.0f;

        sphere Spheres[5] = {};

        Spheres[0].MaterialIndex = 2;
        Spheres[0].CenterPoint = V3(0, 0, 0);
        Spheres[0].Radius = 1.0f;

        Spheres[1].MaterialIndex = 3;
        Spheres[1].CenterPoint = V3(3, -2, 0);
        Spheres[1].Radius = 1.0f;

        Spheres[2].MaterialIndex = 4;
        Spheres[2].CenterPoint = V3(-2, -1, 2);
        Spheres[2].Radius = 1.0f;

        Spheres[3].MaterialIndex = 5;
        Spheres[3].CenterPoint = V3(1, -1, 3);
        Spheres[3].Radius = 1.0f;

        Spheres[4].MaterialIndex = 6;
        Spheres[4].CenterPoint = V3(-2, 3, 0);
        Spheres[4].Radius = 2.0f;

        world World = {};
        World.MaterialsCount = ArraySize(Materials);
        World.Materials = Materials;
        World.PlanesCount = ArraySize(Planes);
        World.Planes = Planes;
        World.SpheresCount = ArraySize(Spheres);
        World.Spheres = Spheres;
        World.TotalRayBouncesComputed = 0;

        u32 CoreCount = 12;

        u32 TileWidth = OutputImage.Width / CoreCount;
        u32 TileHeight = TileWidth;
        u32 TileCountX = (OutputImage.Width + TileWidth - 1) / TileWidth;
        u32 TileCountY = (OutputImage.Height + TileHeight - 1) / TileHeight;
        u32 TotalTilesCount = TileCountX * TileCountY;

        printf
        (
            "\n====================================="
            "\n  Configuration Summary:"
            "\n  CoreCount: %d"
            "\n  TileWidth: %d  TileHeight: %d pixels"
            "\n  TileCountX: %d  TileCountY: %d"
            "\n  Tile Size: %zd kbytes"
            "\n=====================================\n",
            CoreCount, TileWidth, TileHeight, TileCountX, TileCountY,
            TileWidth * TileHeight * sizeof(u32) / 1024
        );

        clock_t StartClock = clock();

        for (u32 TileY = 0; TileY < TileCountY; TileY++)
        {
            u32 MinY = TileY * TileHeight;
            u32 OnePastMaxY = MinY + TileHeight;
            if (OnePastMaxY > OutputImage.Height)
            {
                OnePastMaxY = OutputImage.Height;
            }

            for (u32 TileX = 0; TileX < TileCountX; TileX++)
            {   
                u32 MinX = TileX * TileWidth;
                u32 OnePastMaxX = MinX + TileWidth;
                if (OnePastMaxX > OutputImage.Width)
                {
                    OnePastMaxX = OutputImage.Width;
                }

                RenderTile(&World, OutputImage, MinX, OnePastMaxX, MinY, OnePastMaxY);

                printf("\rRaycasting %d%% ...", (100 * World.TilesRetiredCount / TotalTilesCount));
                fflush(stdout);
            }
        }

        clock_t EndClock = clock();

        u32 TimeElapsedInMilliSeconds = EndClock - StartClock;

        printf
        (
            "\n====================================="
            "\n  Performance Summary:"
            "\n  Raycasting time: %dms"
            "\n  Total number of ray bounces computed: %lld"
            "\n  time per ray bounce: %f ms/bounce"
            "\n=====================================",
            TimeElapsedInMilliSeconds, World.TotalRayBouncesComputed,
            (f32)TimeElapsedInMilliSeconds / (f32)World.TotalRayBouncesComputed
        );

        bitmap_header OutputBitmapHeader = {};
        OutputBitmapHeader.FileType = 0x4d42;
        OutputBitmapHeader.FileSize = sizeof(bitmap_header) + OutputPixelsSize;
        OutputBitmapHeader.BitmapOffset = sizeof(bitmap_header);
        OutputBitmapHeader.Size = sizeof(bitmap_header) - 14;
        OutputBitmapHeader.Width = OutputImage.Width;
        OutputBitmapHeader.Height = (i32)OutputImage.Height;
        OutputBitmapHeader.Planes = 1;
        OutputBitmapHeader.BitsPerPixel = 32;
        OutputBitmapHeader.Compression = 0;
        OutputBitmapHeader.SizeOfBitmap = OutputPixelsSize;
        OutputBitmapHeader.HorizontalResolution = 0;
        OutputBitmapHeader.VerticalResolution = 0;
        OutputBitmapHeader.ColorsUsed = 0;
        OutputBitmapHeader.ColorsImportant = 0;
    
        const char *OutputFileName = "test.bmp";

        FILE *OutputFile = fopen(OutputFileName, "wb");
        if (OutputFile)
        {
            fwrite(&OutputBitmapHeader, sizeof(bitmap_header), 1, OutputFile);
            fwrite(OutputImage.Pixels, OutputPixelsSize, 1, OutputFile);
            fclose(OutputFile);
        }
        else
        {
            fprintf(stderr, "[ERROR]: unable to open/write output file %s.\n", OutputFileName);
        }
    }
    else
    {
        fprintf(stderr, "[ERROR]: unable to malloc the pixels memory.\n");
    }

    printf("\nDone.\n");

    return 0;
}
