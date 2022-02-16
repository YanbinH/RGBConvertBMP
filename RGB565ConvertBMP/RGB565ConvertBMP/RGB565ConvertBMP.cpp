// RGB565ConvertBMP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>


#include "img.h"
#include "color.h"

using namespace std;

#define IMG_W 480
#define IMG_H 272

#define BI_BITFIELDS 0x3

typedef char BYTE;
typedef short WORD;
typedef int DWORD;
typedef int LONG;



#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {
	WORD bfType;//位图文件的类型，在Windows中，此字段的值总为‘BM’(1-2字节）
	DWORD bfSize;//位图文件的大小，以字节为单位（3-6字节，低位在前）
	WORD bfReserved1;//位图文件保留字，必须为0(7-8字节）
	WORD bfReserved2;//位图文件保留字，必须为0(9-10字节）
	DWORD bfOffBits;//位图数据的起始位置，以相对于位图（11-14字节，低位在前）
	//文件头的偏移量表示，以字节为单位
}BitMapFileHeader;	//BITMAPFILEHEADER;

#pragma pack(1)
typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;//本结构所占用字节数（15-18字节）
	LONG biWidth;//位图的宽度，以像素为单位（19-22字节）
	LONG biHeight;//位图的高度，以像素为单位（23-26字节）
	WORD biPlanes;//目标设备的级别，必须为1(27-28字节）
	WORD biBitCount;//每个像素所需的位数，必须是1（双色），（29-30字节）
	//4(16色），8(256色）16(高彩色)或24（真彩色）之一
	DWORD biCompression;//位图压缩类型，必须是0（不压缩），（31-34字节）
	//1(BI_RLE8压缩类型）或2(BI_RLE4压缩类型）之一
	DWORD biSizeImage;//位图的大小(其中包含了为了补齐行数是4的倍数而添加的空字节)，以字节为单位（35-38字节）
	LONG biXPelsPerMeter;//位图水平分辨率，像素数（39-42字节）
	LONG biYPelsPerMeter;//位图垂直分辨率，像素数（43-46字节)
	DWORD biClrUsed;//位图实际使用的颜色表中的颜色数（47-50字节）
	DWORD biClrImportant;//位图显示过程中重要的颜色数（51-54字节）
}BitMapInfoHeader;	//BITMAPINFOHEADER;

#pragma pack(1)
typedef struct tagRGBQUAD {
	BYTE rgbBlue;//蓝色的亮度（值范围为0-255)
	BYTE rgbGreen;//绿色的亮度（值范围为0-255)
	BYTE rgbRed;//红色的亮度（值范围为0-255)
	BYTE rgbReserved;//保留，必须为0
}RgbQuad;	//RGBQUAD;


uint8_t rgbimg[IMG_W * IMG_H * 3];


int Rgb565ConvertBmp(unsigned short* buf, int width, int height, const char* filename)
{
	FILE* fp;

	BitMapFileHeader bmfHdr; //定义文件头
	BitMapInfoHeader bmiHdr; //定义信息头
	RgbQuad bmiClr[3]; //定义调色板

	bmiHdr.biSize = sizeof(BitMapInfoHeader);
	bmiHdr.biWidth = width;//指定图像的宽度，单位是像素
	bmiHdr.biHeight = height;//指定图像的高度，单位是像素
	bmiHdr.biPlanes = 1;//目标设备的级别，必须是1
	bmiHdr.biBitCount = 16;//表示用到颜色时用到的位数 16位表示高彩色图
	bmiHdr.biCompression = BI_BITFIELDS;//BI_RGB仅有RGB555格式
	bmiHdr.biSizeImage = (width * height * 2);//指定实际位图所占字节数
	bmiHdr.biXPelsPerMeter = 0;//水平分辨率，单位长度内的像素数
	bmiHdr.biYPelsPerMeter = 0;//垂直分辨率，单位长度内的像素数
	bmiHdr.biClrUsed = 0;//位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
	bmiHdr.biClrImportant = 0;//说明对图象显示有重要影响的颜色索引的数目，0表示所有颜色都重要

	//RGB565格式掩码
	//0x0000F800
	bmiClr[0].rgbBlue = 0;
	bmiClr[0].rgbGreen = 0xF8;
	bmiClr[0].rgbRed = 0;
	bmiClr[0].rgbReserved = 0;
	//0x000007E0
	bmiClr[1].rgbBlue = 0xE0;
	bmiClr[1].rgbGreen = 0x07;
	bmiClr[1].rgbRed = 0;
	bmiClr[1].rgbReserved = 0;
	//0x0000001F
	bmiClr[2].rgbBlue = 0x1F;
	bmiClr[2].rgbGreen = 0;
	bmiClr[2].rgbRed = 0;
	bmiClr[2].rgbReserved = 0;


	bmfHdr.bfType = (WORD)0x4D42;//文件类型，0x4D42也就是字符'BM'
	bmfHdr.bfSize = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3 + bmiHdr.biSizeImage);//文件大小
	bmfHdr.bfReserved1 = 0;//保留，必须为0
	bmfHdr.bfReserved2 = 0;//保留，必须为0
	bmfHdr.bfOffBits = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3);//实际图像数据偏移量

	if (!(fp = fopen(filename, "wb"))) {
		return -1;
	}
	else {
		printf("file %s open success\n", filename);
	}

	fwrite(&bmfHdr, 1, sizeof(BitMapFileHeader), fp);
	fwrite(&bmiHdr, 1, sizeof(BitMapInfoHeader), fp);
	fwrite(&bmiClr, 1, 3 * sizeof(RgbQuad), fp);

	fwrite(buf, 1, bmiHdr.biSizeImage, fp);	//mirror
	//for (int i = 0; i < height; i++) {
	//	fwrite(buf + (width * (height - i - 1) * 2), 2, width, fp);
	//}

	printf("Image size=%d, file size=%d, width=%d, height=%d\n", bmiHdr.biSizeImage, bmfHdr.bfSize, width, height);
	printf("%s over\n", __FUNCTION__);
	fclose(fp);

	return 0;
}


int Rgb888ConvertBmp(uint8_t* buf, int width, int height, const char* filename)
{
	FILE* fp;

	BitMapFileHeader bmfHdr; //定义文件头
	BitMapInfoHeader bmiHdr; //定义信息头
	RgbQuad bmiClr[3]; //定义调色板

	bmiHdr.biSize = sizeof(BitMapInfoHeader);
	bmiHdr.biWidth = width;//指定图像的宽度，单位是像素
	bmiHdr.biHeight = height;//指定图像的高度，单位是像素
	bmiHdr.biPlanes = 1;//目标设备的级别，必须是1
	bmiHdr.biBitCount = 24;//表示用到颜色时用到的位数 16位表示高彩色图
	bmiHdr.biCompression = BI_BITFIELDS;//BI_RGB仅有RGB555格式
	bmiHdr.biSizeImage = (width * height * 3);//指定实际位图所占字节数
	bmiHdr.biXPelsPerMeter = 0;//水平分辨率，单位长度内的像素数
	bmiHdr.biYPelsPerMeter = 0;//垂直分辨率，单位长度内的像素数
	bmiHdr.biClrUsed = 0;//位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
	bmiHdr.biClrImportant = 0;//说明对图象显示有重要影响的颜色索引的数目，0表示所有颜色都重要

	//RGB888格式掩码  B-G-R
	//0x00FF0000
	bmiClr[0].rgbBlue = 0;
	bmiClr[0].rgbGreen = 0;
	bmiClr[0].rgbRed = 0xFF;
	bmiClr[0].rgbReserved = 0;
	//0x0000FF00
	bmiClr[1].rgbBlue = 0;
	bmiClr[1].rgbGreen = 0xff;
	bmiClr[1].rgbRed = 0;
	bmiClr[1].rgbReserved = 0;
	//0x000000FF
	bmiClr[2].rgbBlue = 0xff;
	bmiClr[2].rgbGreen = 0;
	bmiClr[2].rgbRed = 0;
	bmiClr[2].rgbReserved = 0;


	bmfHdr.bfType = (WORD)0x4D42;//文件类型，0x4D42也就是字符'BM'
	bmfHdr.bfSize = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3 + bmiHdr.biSizeImage);//文件大小
	bmfHdr.bfReserved1 = 0;//保留，必须为0
	bmfHdr.bfReserved2 = 0;//保留，必须为0
	bmfHdr.bfOffBits = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3);//实际图像数据偏移量

	if (!(fp = fopen(filename, "wb"))) {
		return -1;
	}
	else {
		printf("file %s open success\n", filename);
	}

	fwrite(&bmfHdr, 1, sizeof(BitMapFileHeader), fp);
	fwrite(&bmiHdr, 1, sizeof(BitMapInfoHeader), fp);
	fwrite(&bmiClr, 1, 3 * sizeof(RgbQuad), fp);
	//BGR
	fwrite(buf, 1, bmiHdr.biSizeImage, fp);	//mirror
	//for (int i = 0; i < height; i++) {
	//	fwrite(buf + (width * (height - i - 1) * 2), 2, width, fp);
	//}

	printf("Image size=%d, file size=%d, width=%d, height=%d\n", bmiHdr.biSizeImage, bmfHdr.bfSize, width, height);
	printf("%s over\n", __FUNCTION__);
	fclose(fp);

	return 0;
}





void YUV422ToRGB565(uint16_t* inbuf, uint8_t* outbuf)
{
	uint8_t Y, U, Y1, V;
	uint8_t R, G, B;

	for (unsigned int i = 0; i < IMG_H * IMG_W/2; i++)
	{
		
		Y = *inbuf >> 8 & 0x00ff;
		U = *inbuf & 0x00ff;
		inbuf++;
		Y1 = *inbuf >> 8 & 0x00ff;
		V = *inbuf & 0x00ff;
		inbuf++;

		R = (1.164 * (Y - 16) + 2.018 * (U - 128));
		G = (1.164 * (Y - 16) - 0.392 * (U - 128) - 0.813 * (V - 128));
		B = (1.164 * (Y - 16) + 1.159 * (V - 128));


		*outbuf++ = B;
		*outbuf++ = G;
		*outbuf++ = R;

		R = (1.164 * (Y1 - 16) + 2.018 * (U - 128));
		G = (1.164 * (Y1 - 16) - 0.392 * (U - 128) - 0.813 * (V - 128));
		B = (1.164 * (Y1 - 16) + 1.159 * (V - 128));


		*outbuf++ = B;
		*outbuf++ = G;
		*outbuf++ = R;

	}

}



int CreateRGB565Headerfile(void)
{
	ifstream imgfile;
	ofstream imghfile;
	char p1,p2,p3,p4;

	imgfile.open("yuv_image.txt", ios::in);
	imghfile.open("img.h");


	imghfile << "#ifndef _IMG_H_" << endl;
	imghfile << "#define _IMG_H_" << endl<<endl;
	imghfile << "uint16_t imgRGB565[] = {" << endl;


	for (int i = 0; i < IMG_W * IMG_H * 4; i+=4)
	{
		imgfile >> p1 >> p2 >> p3 >> p4;
		imghfile << "0x";
		imghfile << p3 << p4 << p1 << p2;
		imghfile << ",";
	}

	imghfile << endl << "};" << endl;
	imghfile << "#endif"<<endl;

	imgfile.close();   //关闭文件
	imghfile.close();

	return 0;
}



int main()
{
	CreateRGB565Headerfile();
	YUV422ToRGB565(imgRGB565, rgbimg);
	Rgb888ConvertBmp(rgbimg, IMG_W, IMG_H, "RGB888.bmp");

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
