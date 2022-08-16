#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "ray_math.h"
#include "ray.h"

internal v3
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 Result = World->Materials[0].Color;

    f32 ClosestHitDistance = F32MAX;
    f32 ToleranceValue = 0.0001f;

    for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
    {
        plane *CurrentPlane = &World->Planes[PlaneIndex];
        f32 IntersectionPointDistanceAlongRay = F32MAX;

        /*
            plane equation: inner_product_of(point_on_the_plane, plane_normal) == plane_distance_from_origin
            ray equation: ray_origin + point_distance_along_ray * ray_direction = point

            assume there is a shared point, a point that satisfies both equations, substituting:
    
            inner_product_of((ray_origin + point_distance_along_ray * ray_direction), plane_normal) == plane_distance_from_origin
            inner_product_of(ray_origin, plane_normal) + inner_product_of((point_distance_along_ray * ray_direction), plane_normal) = plane_distance_from_origin
            point_distance_along_ray * inner_product_of(ray_direction, plane_normal)= -inner_product_of(ray_origin, plane_normal) + plane_distance_from_origin
            point_distance_along_ray = (-inner_product_of(ray_origin, plane_normal) + plane_distance_from_origin) / inner_product_of(ray_direction, plane_normal)
        */

        f32 Denominator = InnerProduct(RayDirection, CurrentPlane->Normal);
        if ((Denominator < -ToleranceValue) || (Denominator > ToleranceValue))
        {
            IntersectionPointDistanceAlongRay = 
                (-InnerProduct(RayOrigin, CurrentPlane->Normal) + CurrentPlane->DistanceAlongNormal) /
                Denominator;

            if ((IntersectionPointDistanceAlongRay > 0.0f) && (IntersectionPointDistanceAlongRay < ClosestHitDistance))
            {
                ClosestHitDistance = IntersectionPointDistanceAlongRay;
                Result = World->Materials[CurrentPlane->MaterialIndex].Color;
            }
        }
    }

    for (u32 SphereIndex = 0; SphereIndex < World->SpheresCount; SphereIndex++)
    {
        sphere *CurrentSphere = &World->Spheres[SphereIndex];
        f32 IntersectionPointDistanceAlongRay = F32MAX;
        
        /* solving the ray & sphere equations together we get at the quadratic equation */

        v3 SphereRelativeRayOrigin = RayOrigin - CurrentSphere->CenterPoint;
        f32 A = InnerProduct(RayDirection, RayDirection);
        f32 B = 2 * InnerProduct(RayDirection, SphereRelativeRayOrigin);
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

            if ((IntersectionPointDistanceAlongRay > 0.0f) && (IntersectionPointDistanceAlongRay < ClosestHitDistance))
            {
                ClosestHitDistance = IntersectionPointDistanceAlongRay;
                Result = World->Materials[CurrentSphere->MaterialIndex].Color;
            }
        }
    }

    return Result;
}

int main(int argc, char **argv)
{
    printf("Raycasting...\n\n");

    image_u32 OutputImage = {};
    OutputImage.Width = 1280;
    OutputImage.Height = 720;

    u32 OutputPixelsSize = sizeof(u32) * OutputImage.Width * OutputImage.Height;

    OutputImage.Pixels = (u32 *)malloc(OutputPixelsSize);
    if (OutputImage.Pixels)
    {
        material Materials[4] = {};
        Materials[0].Color = V3(0.1f, 0.1f, 0.1f);
        Materials[1].Color = V3(1, 0, 0);
        Materials[2].Color = V3(0, 1, 0);
        Materials[3].Color = V3(0, 0, 1);

        plane Planes[2] = {};

        Planes[0].MaterialIndex = 1;
        Planes[0].Normal = V3(0, 0, 1);
        Planes[0].DistanceAlongNormal = 0.0f;

        Planes[1].MaterialIndex = 2;
        Planes[1].Normal = V3(1, 0, 0);
        Planes[1].DistanceAlongNormal = -6.0f;

        sphere Spheres[1] = {};

        Spheres[0].MaterialIndex = 3;
        Spheres[0].CenterPoint = V3(0, 5, 0);
        Spheres[0].Radius = 1.0f;

        world World = {};
        World.MaterialsCount = ArraySize(Materials);
        World.Materials = Materials;
        World.PlanesCount = ArraySize(Planes);
        World.Planes = Planes;
        World.SpheresCount = ArraySize(Spheres);;
        World.Spheres = Spheres;

        v3 CameraPosition = V3(0, -10, 1);
        v3 CameraZAxis = NormalizeToZero(CameraPosition);
        v3 CameraXAxis = NormalizeToZero(CrossProduct(V3(0, 0, 1), CameraZAxis));
        v3 CameraYAxis = NormalizeToZero(CrossProduct(CameraZAxis, CameraXAxis));

        u32 *CurrentOutputPixel = OutputImage.Pixels;

        f32 FilmWidth = 1.0f;
        f32 HalfFilmWidth = 0.5f * FilmWidth;
        f32 FilmHeight = 1.0f * ((f32)OutputImage.Height / (f32)OutputImage.Width);
        f32 HalfFilmHeight = 0.5f * FilmHeight;
        f32 FilmDistanceFromCamera = 1.0f;
        v3 FilmXAxis = CameraXAxis;
        v3 FilmYAxis = CameraYAxis;

        v3 FilmCenter = CameraPosition - FilmDistanceFromCamera * CameraZAxis;

        for (u32 Y = 0; Y < OutputImage.Height; Y++)
        {
            f32 FilmYRatio = -1.0f + 2 * ((f32)Y / (f32)OutputImage.Height);

            for (u32 X = 0; X < OutputImage.Width; X++)
            {
                f32 FilmXRatio = -1.0f + 2 * ((f32)X / (f32)OutputImage.Width);

                v3 PositionOnFilm =
                    FilmCenter +
                    FilmXRatio * FilmXAxis * HalfFilmWidth +
                    FilmYRatio * FilmYAxis * HalfFilmHeight;
                
                v3 RayDirection = PositionOnFilm - CameraPosition;
                
                v3 PixelColor = RayCast(&World, CameraPosition, RayDirection);

                v4 BMPColor = V4(255.0f * PixelColor, 255.0f);

                *CurrentOutputPixel++ = 
                    (RoundF32ToU32(BMPColor.Alpha) << 24) |
                    (RoundF32ToU32(BMPColor.Red) << 16) |
                    (RoundF32ToU32(BMPColor.Green) << 8) |
                    (RoundF32ToU32(BMPColor.Blue) << 0);
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
    {
        fprintf(stderr, "[ERROR]: unable to malloc the pixels memory.\n");
    }

    printf("Done.\n");

    return 0;
}