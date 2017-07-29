//	---------------------------------------------------------------------------
//		https://github.com/lys861205/qrCode
//	---------------------------------------------------------------------------


#include "QRcode.h"

#include <string.h>
#include <errno.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

#include "qrencode.h"
//	-------------------------------------------------------

typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef signed long		LONG;

#define BI_RGB			0L

#pragma pack(push, 2)

typedef struct
{
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)
//	-------------------------------------------------------


//	-------------------------------------------------------
//	Main
//	-------------------------------------------------------

// 输出BMP数据，以及bmp长宽：width
// 注意，传入的txt，得是ANSIC码，不能有中文。
int QRGenerator(const char* txt, TArray<uint8>& raw, unsigned int& unWidth)
{
	//TArray<uint8> raw;

#define QRCODE_TEXT "https://github.com/badforlabor/ue4_qrcode"
	//#define QRCODE_TEXT					"http://www.ultramundum.org/index.htm";		// Text to encode into QRCode

// 调试，输出到文件
#define DEBUG_FILE false
#define OUT_FILE					"d:/test.bmp"								// Output file name

	// 二维码填充
#define OUT_FILE_PIXEL_PRESCALER	4											// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)

	// 二维码颜色
#define PIXEL_COLOR_R				0										// Color of bmp pixels
#define PIXEL_COLOR_G				0
#define PIXEL_COLOR_B				0


	const char*			szSourceSring = txt;// QRCODE_TEXT;
	unsigned int	x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*	pRGBData, *pSourceData, *pDestData;
	QRcode*			pQRC;

	 // Compute QRCode
	pQRC = QRcode_encodeString(szSourceSring, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
	if (pQRC != NULL)
	{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * OUT_FILE_PIXEL_PRESCALER * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * OUT_FILE_PIXEL_PRESCALER;

		// Allocate pixels buffer
		(pRGBData = (unsigned char*)malloc(unDataBytes));
		if (pRGBData == NULL)
		{
			printf("Out of memory");
			return 0;
		}

		// Preset to white

		memset(pRGBData, 0xff, unDataBytes);


		// Prepare bmp headers

		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) +
			unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER);

		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth * OUT_FILE_PIXEL_PRESCALER;
		kInfoHeader.biHeight = -((int)unWidth * OUT_FILE_PIXEL_PRESCALER);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;


		// Convert QrCode bits to bmp pixels

		pSourceData = pQRC->data;
		for (y = 0; y < unWidth; y++)
		{
			pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
			for (x = 0; x < unWidth; x++)
			{
				if (*pSourceData & 1)
				{
					for (l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
					{
						for (n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
						{
							*(pDestData + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_B;
							*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_G;
							*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_R;
						}
					}
				}
				pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
				pSourceData++;
			}
		}


		// Output the bmp file

		int HeaderSize = sizeof(BITMAPFILEHEADER);
		int InfoSize = sizeof(BITMAPINFOHEADER);

		raw.Empty(HeaderSize + InfoSize + unDataBytes);
		raw.AddZeroed(HeaderSize + InfoSize + unDataBytes);

		uint8* ptr = raw.GetData();
		FMemory::Memcpy(ptr, &kFileHeader, HeaderSize);
		ptr += HeaderSize;
		FMemory::Memcpy(ptr, &kInfoHeader, InfoSize);
		ptr += InfoSize;
		FMemory::Memcpy(ptr, pRGBData, unDataBytes);

		// 调试
		bool debugfile = DEBUG_FILE;
		if (debugfile)
		{
			FILE*			f;
			if (!(fopen_s(&f, OUT_FILE, "wb")))
			{


				// 				fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
				// 				fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
				// 				fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);
				fwrite(raw.GetData(), sizeof(unsigned char), raw.Num(), f);
				fclose(f);
			}
			else
			{
				printf("Unable to open file");
			}
		}

		// Free data

		free(pRGBData);
		QRcode_free(pQRC);
	}
	else
	{
		printf("NULL returned");
	}

	return 0;
}

