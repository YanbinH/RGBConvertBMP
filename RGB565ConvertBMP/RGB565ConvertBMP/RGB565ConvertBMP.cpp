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
	WORD bfType;//λͼ�ļ������ͣ���Windows�У����ֶε�ֵ��Ϊ��BM��(1-2�ֽڣ�
	DWORD bfSize;//λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ��3-6�ֽڣ���λ��ǰ��
	WORD bfReserved1;//λͼ�ļ������֣�����Ϊ0(7-8�ֽڣ�
	WORD bfReserved2;//λͼ�ļ������֣�����Ϊ0(9-10�ֽڣ�
	DWORD bfOffBits;//λͼ���ݵ���ʼλ�ã��������λͼ��11-14�ֽڣ���λ��ǰ��
	//�ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ
}BitMapFileHeader;	//BITMAPFILEHEADER;

#pragma pack(1)
typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;//���ṹ��ռ���ֽ�����15-18�ֽڣ�
	LONG biWidth;//λͼ�Ŀ�ȣ�������Ϊ��λ��19-22�ֽڣ�
	LONG biHeight;//λͼ�ĸ߶ȣ�������Ϊ��λ��23-26�ֽڣ�
	WORD biPlanes;//Ŀ���豸�ļ��𣬱���Ϊ1(27-28�ֽڣ�
	WORD biBitCount;//ÿ�����������λ����������1��˫ɫ������29-30�ֽڣ�
	//4(16ɫ����8(256ɫ��16(�߲�ɫ)��24�����ɫ��֮һ
	DWORD biCompression;//λͼѹ�����ͣ�������0����ѹ��������31-34�ֽڣ�
	//1(BI_RLE8ѹ�����ͣ���2(BI_RLE4ѹ�����ͣ�֮һ
	DWORD biSizeImage;//λͼ�Ĵ�С(���а�����Ϊ�˲���������4�ı�������ӵĿ��ֽ�)�����ֽ�Ϊ��λ��35-38�ֽڣ�
	LONG biXPelsPerMeter;//λͼˮƽ�ֱ��ʣ���������39-42�ֽڣ�
	LONG biYPelsPerMeter;//λͼ��ֱ�ֱ��ʣ���������43-46�ֽ�)
	DWORD biClrUsed;//λͼʵ��ʹ�õ���ɫ���е���ɫ����47-50�ֽڣ�
	DWORD biClrImportant;//λͼ��ʾ��������Ҫ����ɫ����51-54�ֽڣ�
}BitMapInfoHeader;	//BITMAPINFOHEADER;

#pragma pack(1)
typedef struct tagRGBQUAD {
	BYTE rgbBlue;//��ɫ�����ȣ�ֵ��ΧΪ0-255)
	BYTE rgbGreen;//��ɫ�����ȣ�ֵ��ΧΪ0-255)
	BYTE rgbRed;//��ɫ�����ȣ�ֵ��ΧΪ0-255)
	BYTE rgbReserved;//����������Ϊ0
}RgbQuad;	//RGBQUAD;


uint8_t rgbimg[IMG_W * IMG_H * 3];


int Rgb565ConvertBmp(unsigned short* buf, int width, int height, const char* filename)
{
	FILE* fp;

	BitMapFileHeader bmfHdr; //�����ļ�ͷ
	BitMapInfoHeader bmiHdr; //������Ϣͷ
	RgbQuad bmiClr[3]; //�����ɫ��

	bmiHdr.biSize = sizeof(BitMapInfoHeader);
	bmiHdr.biWidth = width;//ָ��ͼ��Ŀ�ȣ���λ������
	bmiHdr.biHeight = height;//ָ��ͼ��ĸ߶ȣ���λ������
	bmiHdr.biPlanes = 1;//Ŀ���豸�ļ��𣬱�����1
	bmiHdr.biBitCount = 16;//��ʾ�õ���ɫʱ�õ���λ�� 16λ��ʾ�߲�ɫͼ
	bmiHdr.biCompression = BI_BITFIELDS;//BI_RGB����RGB555��ʽ
	bmiHdr.biSizeImage = (width * height * 2);//ָ��ʵ��λͼ��ռ�ֽ���
	bmiHdr.biXPelsPerMeter = 0;//ˮƽ�ֱ��ʣ���λ�����ڵ�������
	bmiHdr.biYPelsPerMeter = 0;//��ֱ�ֱ��ʣ���λ�����ڵ�������
	bmiHdr.biClrUsed = 0;//λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ���
	bmiHdr.biClrImportant = 0;//˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ��0��ʾ������ɫ����Ҫ

	//RGB565��ʽ����
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


	bmfHdr.bfType = (WORD)0x4D42;//�ļ����ͣ�0x4D42Ҳ�����ַ�'BM'
	bmfHdr.bfSize = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3 + bmiHdr.biSizeImage);//�ļ���С
	bmfHdr.bfReserved1 = 0;//����������Ϊ0
	bmfHdr.bfReserved2 = 0;//����������Ϊ0
	bmfHdr.bfOffBits = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3);//ʵ��ͼ������ƫ����

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

	BitMapFileHeader bmfHdr; //�����ļ�ͷ
	BitMapInfoHeader bmiHdr; //������Ϣͷ
	RgbQuad bmiClr[3]; //�����ɫ��

	bmiHdr.biSize = sizeof(BitMapInfoHeader);
	bmiHdr.biWidth = width;//ָ��ͼ��Ŀ�ȣ���λ������
	bmiHdr.biHeight = height;//ָ��ͼ��ĸ߶ȣ���λ������
	bmiHdr.biPlanes = 1;//Ŀ���豸�ļ��𣬱�����1
	bmiHdr.biBitCount = 24;//��ʾ�õ���ɫʱ�õ���λ�� 16λ��ʾ�߲�ɫͼ
	bmiHdr.biCompression = BI_BITFIELDS;//BI_RGB����RGB555��ʽ
	bmiHdr.biSizeImage = (width * height * 3);//ָ��ʵ��λͼ��ռ�ֽ���
	bmiHdr.biXPelsPerMeter = 0;//ˮƽ�ֱ��ʣ���λ�����ڵ�������
	bmiHdr.biYPelsPerMeter = 0;//��ֱ�ֱ��ʣ���λ�����ڵ�������
	bmiHdr.biClrUsed = 0;//λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ���
	bmiHdr.biClrImportant = 0;//˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ��0��ʾ������ɫ����Ҫ

	//RGB888��ʽ����  B-G-R
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


	bmfHdr.bfType = (WORD)0x4D42;//�ļ����ͣ�0x4D42Ҳ�����ַ�'BM'
	bmfHdr.bfSize = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3 + bmiHdr.biSizeImage);//�ļ���С
	bmfHdr.bfReserved1 = 0;//����������Ϊ0
	bmfHdr.bfReserved2 = 0;//����������Ϊ0
	bmfHdr.bfOffBits = (DWORD)(sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader) + sizeof(RgbQuad) * 3);//ʵ��ͼ������ƫ����

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

	imgfile.close();   //�ر��ļ�
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
