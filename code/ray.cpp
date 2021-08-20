#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include <Windows.h>

#include "base.h"
#include "ray.h"
#include "ray_math.h"
#include "ray_lane.h"


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

internal lane_u32
XorShift32(random_series *Series)
{
    lane_u32 X = Series->State;
    X ^= X << 13;
    X ^= X >> 17;
    X ^= X << 5;
    Series->State = X;
    return X;
}

internal lane_f32
RandomUnilateral(random_series *Series)
{
    lane_f32 Result = LaneF32FromLaneU32(XorShift32(Series) >> 1) / LaneF32FromF32((f32)(_UI32_MAX >> 1));
    return Result;
}

internal lane_f32
RandomBilateral(random_series *Series)
{
    lane_f32 Result = -1.0f + 2.0f * RandomUnilateral(Series);
    return Result;
}

internal void
CastPixelRays(cast_state *Input)
{
    world *World = Input->World;
    random_series EntropyState = Input->ThreadEntropy;
    u32 RaysPerPixel = Input->RaysPerPixel;
    u32 MaxBounceCount = Input->MaxBounceCount;
    lane_f32 HalfPixelWidth = LaneF32FromF32(Input->HalfPixelWidth);
    lane_f32 HalfPixelHeight = LaneF32FromF32(Input->HalfPixelHeight);
    lane_f32 FilmWidth = LaneF32FromF32(Input->FilmWidth);
    lane_f32 FilmHeight = LaneF32FromF32(Input->FilmHeight);
    lane_f32 FilmX = Input->FilmX + HalfPixelWidth;
    lane_f32 FilmY = Input->FilmY + HalfPixelHeight;
    lane_v3 FilmCenter = LaneV3FromV3(Input->FilmCenter);
    lane_v3 CameraX = LaneV3FromV3(Input->CameraX);
    lane_v3 CameraY = LaneV3FromV3(Input->CameraY);
    lane_v3 CameraZ = LaneV3FromV3(Input->CameraZ);
    lane_v3 CameraPosition = LaneV3FromV3(Input->CameraPosition);

    u32 LaneWidth = SIMD_LANE_WIDTH; // parallel operatoins per lane
    u32 LaneCount = RaysPerPixel / LaneWidth;
    f32 LaneContributionRatio = 1.0f / (f32)RaysPerPixel;
    Assert((LaneCount * LaneWidth) == RaysPerPixel);

    lane_f32 Tolerance = LaneF32FromF32(0.00001f);
    lane_f32 MinHitDistance = LaneF32FromF32(0.001f);
    lane_u32 BouncesComputed = LaneU32FromU32(0);
    u64 LoopsComputed = 0;
    lane_v3 PixelColor = {};

    for (u32 LaneIndex = 0; LaneIndex < LaneCount; LaneIndex++)
    {
        lane_v3 SingleLaneColor = {};
        lane_f32 OffsetX = FilmX + RandomBilateral(&EntropyState) * HalfPixelWidth;
        lane_f32 OffsetY = FilmY + RandomBilateral(&EntropyState) * HalfPixelHeight;
        lane_v3 PositionOnFilm = FilmCenter
            + 0.5f * FilmWidth * OffsetX * CameraX
            + 0.5f * FilmHeight * OffsetY * CameraY;
        lane_v3 RayOrigin = CameraPosition;
        lane_v3 RayDirection = NOZ(PositionOnFilm - RayOrigin);
        lane_v3 Attenuation = LaneV3(1, 1, 1);

        lane_u32 LaneMask = LaneU32FromU32(0xffffffff);

        for (u32 BounceCount = 0; BounceCount < MaxBounceCount; BounceCount++)
        {
            lane_v3 LastHitNormal = {};
            lane_u32 LaneIncrement = LaneU32FromU32(1);
            BouncesComputed += (LaneIncrement & LaneMask);
            LoopsComputed += SIMD_LANE_WIDTH;

            lane_f32 HitDistance = LaneF32FromF32(F32MAX);
            lane_u32 HitMaterialIndex = LaneU32FromU32(0);

            for (u32 PlaneIndex = 0;
                 PlaneIndex < World->PlaneCount;
                 PlaneIndex++)
            {
                plane Plane = World->Planes[PlaneIndex];
                lane_v3 PlaneNormal = LaneV3FromV3(Plane.N);
                lane_f32 PlaneDistance = LaneF32FromF32(Plane.d);

                lane_f32 Denominator = Inner(PlaneNormal, RayDirection);
                lane_u32 DenomCheckMask = (Denominator < -Tolerance) | (Denominator > Tolerance);

                if (!MaskIsZeroed(DenomCheckMask))
                {
                    lane_f32 T = (-PlaneDistance - Inner(PlaneNormal, RayOrigin)) / Denominator;
                    lane_u32 TCheckMask = (T > MinHitDistance) & (T < HitDistance);

                    lane_u32 HitMask = TCheckMask & DenomCheckMask;

                    if (!MaskIsZeroed(HitMask))
                    {
                        lane_u32 PlaneMaterialIndex = LaneU32FromU32(Plane.MaterialIndex);
                        ConditionalAssign(&HitDistance, T, HitMask);
                        ConditionalAssign(&HitMaterialIndex, PlaneMaterialIndex, HitMask);
                        ConditionalAssign(&LastHitNormal, PlaneNormal, HitMask);
                    }
                }
            }

            for (u32 SphereIndex = 0;
                 SphereIndex < World->SphereCount;
                 SphereIndex++)
            {
                sphere Sphere = World->Spheres[SphereIndex];
                lane_v3 SpherePosition = LaneV3FromV3(Sphere.P);
                lane_f32 SphereRadius = LaneF32FromF32(Sphere.r);

                lane_v3 SphereRelativeRayOrigin = RayOrigin - SpherePosition;

                lane_f32 a = Inner(RayDirection, RayDirection);
                lane_f32 b = Inner(RayDirection, SphereRelativeRayOrigin) + Inner(SphereRelativeRayOrigin, RayDirection);
                lane_f32 c = Inner(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - SphereRadius * SphereRadius;

                lane_f32 Denominator = 2.0f * a;

                lane_f32 RootTerm = SquareRoot(b * b - 4.0f * a * c);
                lane_u32 RootTermCheckMask = (RootTerm > Tolerance);

                if (!MaskIsZeroed(RootTermCheckMask))
                {
                    lane_f32 TPos = (-b + RootTerm) / Denominator;
                    lane_f32 TNeg = (-b - RootTerm) / Denominator;

                    lane_f32 T = TPos;
                    lane_u32 TPickMask = (TNeg > MinHitDistance) & (TNeg < TPos);
                    ConditionalAssign(&T, TNeg, TPickMask);
                    lane_u32 TCheckMask = (T > MinHitDistance) & (T < HitDistance);

                    if (!MaskIsZeroed(TCheckMask))
                    {
                        lane_u32 SphereMaterialIndex = LaneU32FromU32(Sphere.MaterialIndex);
                        lane_u32 HitMask = RootTermCheckMask & TCheckMask;
                        ConditionalAssign(&HitDistance, T, HitMask);
                        ConditionalAssign(&HitMaterialIndex, SphereMaterialIndex, HitMask);
                        ConditionalAssign(&LastHitNormal, NOZ(HitDistance * RayDirection + SphereRelativeRayOrigin), HitMask);
                    }
                }
            }

            lane_v3 MaterialEmmitColor = LaneMask & GatherV3(World->Materials, HitMaterialIndex, EmmitColor);
            lane_v3 MaterialReflectionColor = GatherV3(World->Materials, HitMaterialIndex, ReflectionColor);
            lane_f32 MaterialSpecular = GatherF32(World->Materials, HitMaterialIndex, Specular);

            SingleLaneColor += Hadamard(Attenuation, MaterialEmmitColor);

            LaneMask &= (HitMaterialIndex != LaneU32FromU32(0));

            if (MaskIsZeroed(LaneMask))
            {
                break;
            }
            else
            {
                lane_f32 CosineAttenuation = Max(Inner(-RayDirection, LastHitNormal), LaneF32FromF32(0.0f));

                Attenuation = Hadamard(Attenuation, CosineAttenuation * MaterialReflectionColor);

                RayOrigin += HitDistance * RayDirection;

                lane_v3 PureBounce = RayDirection - 2.0f * Inner(RayDirection, LastHitNormal) * LastHitNormal;
                lane_v3 RandomVector =
                    LaneV3(RandomBilateral(&EntropyState), RandomBilateral(&EntropyState), RandomBilateral(&EntropyState));
                lane_v3 RandomBounce = NOZ(LastHitNormal + RandomVector);
                RayDirection = NOZ(Lerp(RandomBounce, MaterialSpecular, PureBounce));
            }
        }

        PixelColor += LaneF32FromF32(LaneContributionRatio) * SingleLaneColor;
    }

    Input->BouncesComputed += HorizontalAdd(BouncesComputed);
    Input->LoopsComputed += LoopsComputed;
    Input->PixelColor = HorizontalAdd(PixelColor);
    Input->ThreadEntropy = EntropyState;
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

    lane_v3 CameraPosition = LaneV3(6, -10, 1);
    lane_v3 CameraZ = NOZ(CameraPosition);
    lane_v3 CameraX = NOZ(Cross(LaneV3(0, 0, 1), CameraZ));
    lane_v3 CameraY = NOZ(Cross(CameraZ, CameraX));

    cast_state CastState = {};
    CastState.Queue = Queue;
    CastState.World = WorkOrder->World;
    CastState.ThreadEntropy = WorkOrder->Entropy;

    CastState.MaxBounceCount = Queue->MaxBounceCount;
    CastState.BouncesComputed = 0;
    CastState.LoopsComputed = 0;

    CastState.RaysPerPixel = Queue->RaysPerPixel;
    CastState.HalfPixelWidth = 0.5f / (f32)Image->Width;
    CastState.HalfPixelHeight = 0.5f / (f32)Image->Height;

    CastState.CameraPosition = ExtractFirstLane(CameraPosition);
    CastState.CameraZ = ExtractFirstLane(CameraZ);
    CastState.CameraX = ExtractFirstLane(CameraX);
    CastState.CameraY = ExtractFirstLane(CameraY);

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

            f32 R = 255.0f * ExactLinearToSRGB(CastState.PixelColor.x);
            f32 G = 255.0f * ExactLinearToSRGB(CastState.PixelColor.y);
            f32 B = 255.0f * ExactLinearToSRGB(CastState.PixelColor.z);
            f32 A = 255.0f;

            u32 Result = ((RoundF32ToU32(A) << 24) |
                          (RoundF32ToU32(R) << 16) |
                          (RoundF32ToU32(G) << 8) |
                          (RoundF32ToU32(B) << 0));

            *Out++ = Result;
        }
    }

    LockedAdd_ReturnPrevious(&Queue->TilesDone, 1);
    LockedAdd_ReturnPrevious(&Queue->TotalBouncesComputed, CastState.BouncesComputed);
    LockedAdd_ReturnPrevious(&Queue->TotalLoopsComputed, CastState.LoopsComputed);

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

    material Materials[7] = {
        // sky 
        {0, {}, {0.3f, 0.4f, 0.5f}},
        // plane
        {0, {0.5f, 0.5f, 0.5f}, {}},
        // sphere 1
        {1.0f, {0.95f, 0.95f, 0.95f}, {}},
        // sphere 2
        {0.0f, {}, {4.0f, 0.0f, 0.0f}},
        // sphere 3
        {0.7f, {0.2f, 0.8f, 0.2f}, {}},
        // sphere 4
        {0.85f, {0.4f, 0.8f, 0.9f}, {}},
        // sphere 5
        {0.9f, {0.25f, 0.8f, 0.3f}, {}}
    };

    plane Planes[1] = {
        {{0, 0, 1}, 0, 1}
    };

    sphere Spheres[4] = {
        {{0, 0, 0}, 2.0f, 2},
        {{5, -2, 1}, 1.0f, 3},
        {{1, -1 ,3}, 1.0f, 4},
        {{-3, -4, 0}, 1.0f, 5}
    };

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
    Queue.RaysPerPixel = 32;
    Queue.WorkOrders = (work_order *)malloc(TotalTileCount * sizeof(work_order));
    if (!Queue.WorkOrders)
    {
        printf("ERROR: failed to malloc!\n");
    }

    printf("Configuratoin: %d cores with %d-wide lane operations rendering %d %dx%d pixel tiles, with %d kbyte/tile.\n", 
           CoreCount, SIMD_LANE_WIDTH, TotalTileCount, TileWidth, TileHeight, TileWidth * TileHeight * 4 / 1024);
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
            random_series Entropy = {
                LaneU32FromU32(
                    TileX * 12875 + TileY * 76534 + 484453,
                    TileX * 43367 + TileY * 33498 + 897542,
                    TileX * 39624 + TileY * 61239 + 543653,
                    TileX * 74250 + TileY * 98340 + 856832,
                    TileX * 365127 + TileY * 718444 + 654535,
                    TileX * 905515 + TileY * 109628 + 908235,
                    TileX * 796211 + TileY * 822378 + 124632,
                    TileX * 370718 + TileY * 358820 + 812348
                )
            };
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

    u64 UsedBounces = Queue.TotalBouncesComputed;
    u64 TotalBounces = Queue.TotalLoopsComputed;
    u64 WastedBounces = Queue.TotalLoopsComputed - Queue.TotalBouncesComputed;
    
    printf("\n\tRaycasting took %dms\n", TotalDurationMs);
    printf("\tUsed Bounces %lld\n", UsedBounces);
    printf("\tTotal bounces %lld\n", TotalBounces);
    printf("\tWasted Bounces %lld (%0.2f%%)\n", WastedBounces, (f32)WastedBounces / (f32)TotalBounces * 100.0f);
    printf("\tperformance: %f ns/bounce\n", (f32)TotalDurationMs * 1000.0f * 1000.0f / (f32)Queue.TotalBouncesComputed);
    
    WriteImage((char *)"test.bmp", Image);
    printf("Done.\n");
    return 0;
}