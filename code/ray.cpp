#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include <Windows.h>

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
    if (!Result.Pixels)
    {
        printf("ERROR: failed to malloc!\n");
    }

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

internal u32
XorShift32(random_series *Series)
{
    u32 X = Series->State;
    X ^= X << 13;
    X ^= X >> 17;
    X ^= X << 5;
    Series->State = X;
    return X;
}

internal f32
RandomUnilateral(random_series *Series)
{
    f32 Result = (f32)XorShift32(Series) / (f32)_UI32_MAX;
    return Result;
}

internal f32
RandomBilateral(random_series *Series)
{
    f32 Result = -1.0f + 2.0f * RandomUnilateral(Series);
    return Result;
}

internal f32
ExactLinearToSRGB(f32 Linear)
{
    f32 sRGB;

    if (Linear < 0.0f)
    {
        Linear = 0.0f;
    }
    else if (Linear > 1.0f)
    {
        Linear = 1.0f;
    }

    sRGB = Linear * 12.92f;
    if (Linear > 0.0031308f)
    {
        sRGB = 1.055f * Pow(Linear, 1.0f / 2.4f) - 0.055f;
    }
    
    return sRGB;
}

internal u32 *
GetPointerToPixel(image_u32 *Image, u32 X, u32 Y)
{
    u32 *Result = Image->Pixels + Y * Image->Width + X;
    return Result;
}

internal u64
LockedAdd_ReturnPrevious(volatile u64 *Value, u64 Increment)
{
    u64 Result = (u64)InterlockedExchangeAdd64((volatile LONG64 *)Value, Increment);
    return Result;
}

internal void
CastPixelRays(cast_state *Input)
{
    u32 BouncesComputed = 0;
    v3 PixelColor = {};

    u32 LaneWidth = 1; // rays per lane
    u32 LaneCount = Input->RaysPerPixel / LaneWidth;
    f32 LaneContributionRatio = 1.0f / (f32)LaneCount;

    lane_f32 Tolerance = 0.00001f;
    lane_f32 MinHitDistance = 0.001f;

    for (u32 LaneIndex = 0; LaneIndex < LaneCount; LaneIndex++)
    {
        lane_v3 SingleLaneColor = {};
        lane_f32 OffsetX = Input->FilmX + RandomBilateral(Input->ThreadEntropy) * Input->HalfPixelWidth;
        lane_f32 OffsetY = Input->FilmY + RandomBilateral(Input->ThreadEntropy) * Input->HalfPixelHeight;
        lane_v3 PositionOnFilm = Input->FilmCenter
            + 0.5f * Input->FilmWidth * OffsetX * Input->CameraX
            + 0.5f * Input->FilmHeight * OffsetY * Input->CameraY;
        lane_v3 RayOrigin = Input->CameraPosition;
        lane_v3 RayDirection = NOZ(PositionOnFilm - RayOrigin);

        lane_v3 Attenuation = V3(1, 1, 1);

        for (u32 BounceCount = 0; BounceCount < Input->MaxBounceCount; BounceCount++)
        {
            BouncesComputed += LaneWidth;
            lane_f32 HitDistance = F32MAX;
            lane_u32 HitMaterialIndex = 0;
            lane_v3 LastHitNormal = {};

            for (u32 PlaneIndex = 0;
                 PlaneIndex < Input->World->PlaneCount;
                 PlaneIndex++)
            {
                plane Plane = Input->World->Planes[PlaneIndex];
                lane_v3 PlaneNormal = Plane.N;
                lane_f32 PlaneDistance = Plane.d;
                lane_u32 PlaneMaterialIndex = Plane.MaterialIndex;

                lane_f32 Denominator = Inner(PlaneNormal, RayDirection);
                lane_u32 DenomCheckMask = (Denominator < -Tolerance || Denominator > Tolerance);

                lane_f32 T = (-PlaneDistance - Inner(PlaneNormal, RayOrigin)) / Denominator;
                lane_u32 TCheckMask = (T > MinHitDistance && T < HitDistance);

                lane_u32 HitMask = TCheckMask & DenomCheckMask;

                ConditionalAssign(&HitDistance, T, HitMask);
                ConditionalAssign(&HitMaterialIndex, PlaneMaterialIndex, HitMask);
                ConditionalAssign(&LastHitNormal, PlaneNormal, HitMask);
            }

            for (u32 SphereIndex = 0;
                 SphereIndex < Input->World->SphereCount;
                 SphereIndex++)
            {
                sphere Sphere = Input->World->Spheres[SphereIndex];
                lane_v3 SpherePosition = Sphere.P;
                lane_f32 SphereRadius = Sphere.r;
                lane_u32 SphereMaterialIndex = Sphere.MaterialIndex;

                lane_v3 SphereRelativeRayOrigin = RayOrigin - SpherePosition;

                lane_f32 a = Inner(RayDirection, RayDirection);
                lane_f32 b = Inner(RayDirection, SphereRelativeRayOrigin) + Inner(SphereRelativeRayOrigin, RayDirection);
                lane_f32 c = Inner(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - SphereRadius * SphereRadius;

                lane_f32 Denominator = 2.0f * a;
                lane_f32 RootTerm = SquareRoot(b * b - 4.0f * a * c);

                lane_u32 RootTermCheckMask = (RootTerm > Tolerance);

                lane_f32 TPos = (-b + RootTerm) / Denominator;
                lane_f32 TNeg = (-b - RootTerm) / Denominator;
                //ConditionalAssign(&TPos, (-b + RootTerm) / Denominator, RootTermCheckMask); // casey has those non-conditional
                //ConditionalAssign(&TNeg, (-b - RootTerm) / Denominator, RootTermCheckMask); // casey has those non-conditional

                lane_f32 T = TPos;
                //ConditionalAssign(&t, TPos); // casey has those non-conditional

                lane_u32 TPickMask = (TNeg > MinHitDistance && TNeg < TPos);
                ConditionalAssign(&T, TNeg, TPickMask);

                lane_u32 TCheckMask = (T > MinHitDistance && T < HitDistance);
                lane_u32 HitMask = RootTermCheckMask & TCheckMask;
                ConditionalAssign(&HitDistance, T, HitMask);
                ConditionalAssign(&HitMaterialIndex, SphereMaterialIndex, HitMask);
                ConditionalAssign(&LastHitNormal, NOZ(HitDistance * RayDirection + SphereRelativeRayOrigin), HitMask);
            }

            if (HitMaterialIndex)
            {
                material HitMaterial = Input->World->Materials[HitMaterialIndex];
                SingleLaneColor += Hadamard(Attenuation, HitMaterial.EmmitColor);

                f32 CosineAttenuation = 1.0f;
                CosineAttenuation = Inner(-RayDirection, LastHitNormal);
                if (CosineAttenuation < 0.0f)
                {
                    CosineAttenuation = 0.0f;
                }
                Attenuation = Hadamard(Attenuation, CosineAttenuation * HitMaterial.ReflectionColor);

                RayOrigin += HitDistance * RayDirection;

                v3 PureBounce = RayDirection - 2.0f * Inner(RayDirection, LastHitNormal) * LastHitNormal;
                v3 RandomVector = V3(RandomBilateral(Input->ThreadEntropy), RandomBilateral(Input->ThreadEntropy), RandomBilateral(Input->ThreadEntropy));
                v3 RandomBounce = NOZ(LastHitNormal + RandomVector);
                RayDirection = NOZ(Lerp(RandomBounce, HitMaterial.Specular, PureBounce));
            }
            else
            {
                material HitMaterial = Input->World->Materials[HitMaterialIndex];
                SingleLaneColor += Hadamard(Attenuation, HitMaterial.EmmitColor);
                break;
            }
        }

        PixelColor += LaneContributionRatio * SingleLaneColor;
    }

    Input->PixelColor = PixelColor;
    Input->BouncesComputed += BouncesComputed;
}

internal b32x
RenderTile(work_queue *Queue)
{
    u64 WorkOrderIndex = LockedAdd_ReturnPrevious(&Queue->NextWorkOrderIndex, 1);
    if (WorkOrderIndex >= Queue->WorkOrderCount)
    {
        return false;
    }
    work_order *WorkOrder = Queue->WorkOrders + WorkOrderIndex;
    image_u32 *Image = WorkOrder->Image;

    cast_state CastState = {};
    CastState.Queue = Queue;
    CastState.World = WorkOrder->World;
    CastState.ThreadEntropy = &WorkOrder->Entropy;

    CastState.MaxBounceCount = Queue->MaxBounceCount;
    CastState.BouncesComputed = 0;

    CastState.RaysPerPixel = Queue->RaysPerPixel;
    CastState.HalfPixelWidth = 0.5f / (f32)Image->Width;
    CastState.HalfPixelHeight = 0.5f / (f32)Image->Height;

    CastState.CameraPosition = V3(6, -10, 4);
    CastState.CameraZ = NOZ(CastState.CameraPosition);
    CastState.CameraX = NOZ(Cross(V3(0, 0, 1), CastState.CameraZ));
    CastState.CameraY = NOZ(Cross(CastState.CameraZ, CastState.CameraX));

    CastState.FilmDistance = 1.0f;
    CastState.FilmCenter = CastState.CameraPosition - CastState.FilmDistance * CastState.CameraZ;
    CastState.FilmWidth = 1.0f;
    CastState.FilmHeight = 1.0f;
    if (Image->Width > Image->Height)
    {
        CastState.FilmHeight = ((f32)Image->Height / (f32)Image->Width);
    }
    else if (Image->Height > Image->Width)
    {
        CastState.FilmWidth = ((f32)Image->Width / (f32)Image->Height);
    }

    for (u32 Y = WorkOrder->MinY; Y < WorkOrder->MaxY; Y++)
    {
        CastState.FilmY = -1.0f + 2.0f * ((f32)Y / Image->Height);

        u32 *Out = GetPointerToPixel(Image, WorkOrder->MinX, Y);

        for (u32 X = WorkOrder->MinX; X < WorkOrder->MaxX; X++)
        {
            CastState.FilmX = -1.0f + 2.0f * ((f32)X / Image->Width);

            CastPixelRays(&CastState);

            v4 BMPColor = {
                255.0f * ExactLinearToSRGB(CastState.PixelColor.r),
                255.0f * ExactLinearToSRGB(CastState.PixelColor.g),
                255.0f * ExactLinearToSRGB(CastState.PixelColor.b),
                255.0f
            };

            *Out++ = BGRAPack4x8(BMPColor);
        }
    }

    LockedAdd_ReturnPrevious(&Queue->TilesDone, 1);
    LockedAdd_ReturnPrevious(&Queue->TotalBouncesComputed, CastState.BouncesComputed);

    return true;
}

internal DWORD WINAPI 
WorkerThread(void *Parameter)
{
    work_queue *Queue = (work_queue *)Parameter;
    while (RenderTile(Queue)) { ; }
    return 0;
}

internal void
CreateWorkerThread(void *Parameter)
{
    DWORD ThreadId;
    HANDLE ThreadHandle = CreateThread(0, 0, WorkerThread, Parameter, 0, &ThreadId);
    CloseHandle(ThreadHandle);
}

internal u32
GetCPUCoreCount()
{
    SYSTEM_INFO SystemInfo = {};
    GetSystemInfo(&SystemInfo);
    return SystemInfo.dwNumberOfProcessors;
}

int
main(int ArgCount, char **Arguments)
{
    printf("Raycasting ...\n");

    material Materials[7] = {};
    // sky
    Materials[0].EmmitColor = V3(0.3f, 0.4f, 0.5f);
    // plane
    Materials[1].ReflectionColor = V3(0.5f, 0.5f, 0.5f);
    // sphere 1
    Materials[2].ReflectionColor = V3(0.95f, 0.95f, 0.95f);
    Materials[2].Specular = 1.0f;
    // sphere 2
    Materials[3].EmmitColor = V3(4.0f, 0.0f, 0.0f);
    // sphere 3
    Materials[4].ReflectionColor = V3(0.2f, 0.8f, 0.2f);
    Materials[4].Specular = 0.7f;
    // sphere 4
    Materials[5].ReflectionColor = V3(0.4f, 0.8f, 0.9f);
    Materials[5].Specular = 0.85f;
    // sphere 5
    Materials[6].ReflectionColor = V3(0.25f, 0.8f, 0.3f);
    Materials[6].Specular = 0.9f;

    plane Planes[1] = {};
    Planes[0].N = V3(0, 0, 1);
    Planes[0].d = 0;
    Planes[0].MaterialIndex = 1;

    sphere Spheres[4] = {};
    // 1
    Spheres[0].P = V3(0, 0, 0);
    Spheres[0].r = 2.0f;
    Spheres[0].MaterialIndex = 2;
    // 2
    Spheres[1].P = V3(5, -2, 1);
    Spheres[1].r = 1.0f;
    Spheres[1].MaterialIndex = 3;
    // 3
    Spheres[2].P = V3(1, -1, 3);
    Spheres[2].r = 1.0f;
    Spheres[2].MaterialIndex = 4;
    // 4
    Spheres[3].P = V3(-3, -4, 0);
    Spheres[3].r = 1.0f;
    Spheres[3].MaterialIndex = 5;

    world World = {};
    World.Materials = Materials;
    World.MaterialCount = ArraySize(Materials);
    World.Planes = Planes;
    World.PlaneCount = ArraySize(Planes);
    World.Spheres = Spheres;
    World.SphereCount = ArraySize(Spheres);

    image_u32 Image = AllocateImage(1280, 720);

    u32 CoreCount = GetCPUCoreCount();
    u32 TileWidth = 64; // (Image.Width + CoreCount - 1) / CoreCount;
    u32 TileHeight = TileWidth;
    u32 TileCountX = (Image.Width + TileWidth - 1) / TileWidth;
    u32 TileCountY = (Image.Height + TileHeight - 1) / TileHeight;
    u32 TotalTileCount = TileCountX * TileCountY;

    work_queue Queue = {};
    Queue.MaxBounceCount = 8;
    Queue.RaysPerPixel = 64;
    Queue.WorkOrders = (work_order *)malloc(TotalTileCount * sizeof(work_order));
    if (!Queue.WorkOrders)
    {
        printf("ERROR: failed to malloc!\n");
    }

    printf("Configuratoin: %d cores rendering %d %dx%d pixel tiles, with %d kbyte/tile.\n", 
           CoreCount, TotalTileCount, TileWidth, TileHeight, TileWidth * TileHeight * 4 / 1024);
    printf("Tracing quality: %d Rays/Pixel, %d Bounces/Ray.\n", Queue.RaysPerPixel, Queue.MaxBounceCount);

    for (u32 TileY = 0; TileY < TileCountY; TileY++)
    {
        for (u32 TileX = 0; TileX < TileCountX; TileX++)
        {
            u32 MinX = TileX * TileWidth;
            u32 MinY = TileY * TileHeight;
            u32 MaxX = (TileX + 1) * TileWidth;
            u32 MaxY = (TileY + 1) * TileHeight;

            if (MaxX > Image.Width)
            {
                MaxX = Image.Width;
            }

            if (MaxY > Image.Height)
            {
                MaxY = Image.Height;
            }

            work_order *WorkOrder = Queue.WorkOrders + Queue.WorkOrderCount++;
            assert(Queue.WorkOrderCount <= TotalTileCount);
            WorkOrder->World = &World;
            WorkOrder->Image = &Image;
            WorkOrder->MinX = MinX;
            WorkOrder->MinY = MinY;
            WorkOrder->MaxX = MaxX;
            WorkOrder->MaxY = MaxY;

            // TODO: replace with another entropy source, this is not random
            random_series Entropy = {TileX * 12875 + TileY * 76534};
            WorkOrder->Entropy = Entropy;
        }
    }

    assert(Queue.WorkOrderCount == TotalTileCount);

    LockedAdd_ReturnPrevious(&Queue.NextWorkOrderIndex, 0); // sync threads

    clock_t StartTime = clock();

    for (u32 CoreIndex = 1; CoreIndex < CoreCount; CoreIndex++)
    {
        CreateWorkerThread(&Queue);
    }

    while (Queue.TilesDone < TotalTileCount)
    {
        if (RenderTile(&Queue))
        {
            printf("\rRaycasting %d%%", 100 * (u32)Queue.TilesDone / TotalTileCount);
            fflush(stdout);
        }
    }

    clock_t EndTime = clock();
    u32 TotalDurationMs = (EndTime - StartTime);
    
    printf("\n\tRaycasting took %dms\n", TotalDurationMs);
    printf("\tBounces Computed %lld\n", Queue.TotalBouncesComputed);
    printf("\tperformance: %f ns/bounce\n", (f32)TotalDurationMs * 1000.0f * 1000.0f / (f32)Queue.TotalBouncesComputed);
    
    WriteImage((char *)"test.bmp", Image);
    printf("Done.\n");
    return 0;
}