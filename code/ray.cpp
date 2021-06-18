#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
};
#pragma pack(pop)

int
main(int ArgCount, char **Arguments)
{
    printf("hello world\n");

    u32 OutputWidth = 1280;
    u32 OutputHeight = 720;

    u32 OutputPixelsSize = sizeof(u32) * OutputWidth * OutputHeight;
    u32 *OutputPixels = (u32 *)malloc(OutputPixelsSize);

    u32 *Out = OutputPixels;
    for (u32 Y = 0;
         Y < OutputHeight;
         Y++)
    {
        for (u32 X = 0;
             X < OutputWidth;
             X++)
        {
            *Out = 0xff0000ff; // AA RR GG BB
        }
    }

    bitmap_header Header = {};
    Header.FileType = 0x424d;
    Header.FileSize = sizeof(bitmap_header) + OutputPixelsSize;
    Header.BitmapOffset = sizeof(bitmap_header);
    Header.Size = sizeof(bitmap_header) - 14;
    Header.Width = OutputWidth;
    Header.Height = OutputHeight;
    Header.Planes = 1;
    Header.BitsPerPixel = 32;
    Header.Compression = 3;
    Header.SizeOfBitmap = 0;
    Header.HorizontalResolution = 4096;
    Header.VerticalResolution = 4096;
    Header.ColorsUsed = 0;
    Header.ColorsImportant = 0;
    Header.RedMask =   0x00ff0000;
    Header.GreenMask = 0x0000ff00;
    Header.BlueMask =  0x000000ff;

    const char *OutputFileName = "test.bmp";
    FILE *OutFile = fopen(OutputFileName, "wb");
    if (OutFile)
    {
        fwrite(&Header, sizeof(bitmap_header), 1, OutFile);
        fwrite(OutputPixels, OutputPixelsSize, 1, OutFile);
        fclose(OutFile);
    }
    else
    {
        fprintf(stderr, "[ERROR]: unable to write output file %s.\n", OutputFileName);
    }

    return 0;
}