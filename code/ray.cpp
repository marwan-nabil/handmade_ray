#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "ray_math.h"
#include "ray.h"

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

internal v3
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 ResultColor = {};

    f32 MinimumHitDistance = 0.001f;
    f32 ToleranceValue = 0.0001f;

    v3 CurrentRayOrigin = RayOrigin;
    v3 CurrentRayDirection = RayDirection;
    v3 CurrentColorAttenuation = V3(1, 1, 1);

    v3 NextRayOrigin = {};
    v3 NextRayOriginNormal = {};
    v3 NextRayDirection = {};
    v3 NextColorAttenuation = {};

    for (u32 RayReflectionCount = 0; RayReflectionCount < 8; RayReflectionCount++)
    {
        f32 ClosestHitDistance = F32MAX;
        u32 HitMaterialIndex = 0;

        for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
        {
            plane *CurrentPlane = &World->Planes[PlaneIndex];
            f32 IntersectionPointDistanceAlongRay = F32MAX;

            f32 Denominator = InnerProduct(CurrentRayDirection, CurrentPlane->Normal);
            if ((Denominator < -ToleranceValue) || (Denominator > ToleranceValue))
            {
                IntersectionPointDistanceAlongRay = 
                    (-InnerProduct(CurrentRayOrigin, CurrentPlane->Normal) + CurrentPlane->DistanceAlongNormal) /
                    Denominator;

                if ((IntersectionPointDistanceAlongRay > MinimumHitDistance) && (IntersectionPointDistanceAlongRay < ClosestHitDistance))
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
            if (RootTerm > ToleranceValue)
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

                if ((IntersectionPointDistanceAlongRay > MinimumHitDistance) && (IntersectionPointDistanceAlongRay < ClosestHitDistance))
                {
                    ClosestHitDistance = IntersectionPointDistanceAlongRay;
                    HitMaterialIndex = CurrentSphere->MaterialIndex;

                    NextRayOrigin = CurrentRayOrigin + IntersectionPointDistanceAlongRay * CurrentRayDirection;
                    NextRayOriginNormal = NormalizeToZero(NextRayOrigin - CurrentSphere->CenterPoint);
                }
            }
        }

        material *HitMaterial = &World->Materials[HitMaterialIndex];
        ResultColor = ResultColor + HadamardProduct(CurrentColorAttenuation, HitMaterial->EmmissionColor);

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
                Lerp(NextRayDirectionIfRandomBounce, NextRayDirectionIfPureBounce, HitMaterial->Specularity)
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
    }

    return ResultColor;
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

        Materials[0].EmmissionColor = V3(0.3f, 0.4f, 0.5f);
        Materials[0].ReflectionColor = V3(0, 0, 0);
        Materials[0].Specularity = 0;

        Materials[1].EmmissionColor = V3(0.0, 0.0, 0.0);
        Materials[1].ReflectionColor = V3(0.5f, 0.5f, 0.5f);
        Materials[1].Specularity = 0;

        Materials[2].EmmissionColor = V3(0.0, 0.0, 0.0);
        Materials[2].ReflectionColor = V3(0.7f, 0.5f, 0.3f);
        Materials[2].Specularity = 0;

        Materials[3].EmmissionColor = V3(4.0f, 0, 0);
        Materials[3].ReflectionColor = V3(0, 0, 0);
        Materials[3].Specularity = 0;

        Materials[4].EmmissionColor = V3(0, 0, 0);
        Materials[4].ReflectionColor = V3(0.2f, 0.8f, 0.2f);
        Materials[4].Specularity = 0.7;

        Materials[5].EmmissionColor = V3(0, 0, 0);
        Materials[5].ReflectionColor = V3(0.4f, 0.8f, 0.9f);
        Materials[5].Specularity = 0.85;
        
        Materials[6].EmmissionColor = V3(0, 0, 0);
        Materials[6].ReflectionColor = V3(0.95f, 0.95f, 0.95f);
        Materials[6].Specularity = 1;

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

        v3 CameraPosition = V3(0, -10, 1);
        v3 CameraZAxis = NormalizeToZero(CameraPosition);
        v3 CameraXAxis = NormalizeToZero(CrossProduct(V3(0, 0, 1), CameraZAxis));
        v3 CameraYAxis = NormalizeToZero(CrossProduct(CameraZAxis, CameraXAxis));

        u32 *CurrentOutputPixel = OutputImage.Pixels;

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

        u32 RaysPerPixel = 64;
        f32 SingleRayContributionRatio = 1.0f / (f32)RaysPerPixel;

        f32 HalfPixelWidth = 0.5f / OutputImage.Width;
        f32 HalfPixelHeight = 0.5f / OutputImage.Height;

        for (u32 Y = 0; Y < OutputImage.Height; Y++)
        {
            f32 FilmYRatio = -1.0f + 2 * ((f32)Y / (f32)OutputImage.Height);

            for (u32 X = 0; X < OutputImage.Width; X++)
            {
                f32 FilmXRatio = -1.0f + 2 * ((f32)X / (f32)OutputImage.Width);
                
                v3 PixelColor = {};

                for (u32 RayCount = 0; RayCount < RaysPerPixel; RayCount++)
                {
                    f32 OffsetX = FilmXRatio + GenerateRandomBilateral() * HalfPixelWidth;
                    f32 OffsetY = FilmYRatio + GenerateRandomBilateral() * HalfPixelHeight;

                    v3 PositionOnFilm =
                        FilmCenter +
                        OffsetX * HalfFilmWidth * FilmXAxis +
                        OffsetY * HalfFilmHeight * FilmYAxis;

                    v3 RayOrigin = CameraPosition;
                    v3 RayDirection = NormalizeToZero(PositionOnFilm - RayOrigin);

                    v3 CurrentRayColor = RayCast(&World, RayOrigin, RayDirection);
                    PixelColor = PixelColor + SingleRayContributionRatio * CurrentRayColor;
                }

                //v4 BMPColor = LinearColor1ToSRGB255(V4(PixelColor, 255.0f));
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

            if ((Y % 64) == 0)
            {
                printf("\rRaycasting %d%% ...", (100 * Y / OutputImage.Height));
            }
        }

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