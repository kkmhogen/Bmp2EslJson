#include "stdafx.h"
#include <stdio.h> 
#include <stdlib.h>
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"

typedef  struct  tagBITMAPFILEHEADER
{ 
	unsigned short int  bfType;       //位图文件的类型，必须为BM 
	unsigned long       bfSize;       //文件大小，以字节为单位
	unsigned short int  bfReserverd1; //位图文件保留字，必须为0 
	unsigned short int  bfReserverd2; //位图文件保留字，必须为0 
	unsigned long       bfbfOffBits;  //位图文件头到数据的偏移量，以字节为单位
}BITMAPFILEHEADER; 
typedef  struct  tagBITMAPINFOHEADER 
{ 
	long biSize;                        //该结构大小，字节为单位
	long  biWidth;                     //图形宽度以象素为单位
	long  biHeight;                     //图形高度以象素为单位
	short int  biPlanes;               //目标设备的级别，必须为1 
	short int  biBitcount;             //颜色深度，每个象素所需要的位数
	short int  biCompression;        //位图的压缩类型
	long  biSizeImage;              //位图的大小，以字节为单位
	long  biXPelsPermeter;       //位图水平分辨率，每米像素数
	long  biYPelsPermeter;       //位图垂直分辨率，每米像素数
	long  biClrUsed;            //位图实际使用的颜色表中的颜色数
	long  biClrImportant;       //位图显示过程中重要的颜色数
}BITMAPINFOHEADER; 
typedef  struct 
{ 
	BITMAPFILEHEADER  file; //文件信息区
	BITMAPINFOHEADER  info; //图象信息区
}bmp;

void writebmpfile() 
{ 
	bmp  m = {0};        //定义一个位图结构
	FILE *fp;

	IplImage * temp = cvLoadImage("BMP_binary.bmp",0);
	
   	fopen_s(&fp,"BMP_binary_bit.bmp","wb+");
    if (NULL == fp)
    {
        printf("can't open the bmp imgae.\n");
        exit(0);
    }

	int widthStep = 0;
	widthStep = ((temp->width + 31) / 32) * 4;

	//写入文件头
	m.file.bfType = 0X4D42;

	m.file.bfSize = widthStep*temp->height + 62;       //文件大小，数据大小+文件头大小
	m.file.bfReserverd1 = 0;
	m.file.bfReserverd2 = 0;
	m.file.bfbfOffBits = 62;

	//写入信息头
	m.info.biSize = 40;
	m.info.biWidth = temp->width;
	m.info.biHeight = temp->height;
	m.info.biPlanes = 1;
	m.info.biBitcount = 1;
	m.info.biCompression = 0;
	m.info.biSizeImage = widthStep*temp->height;
	m.info.biXPelsPermeter = 0;
	m.info.biClrUsed = 0;
	m.info.biClrImportant = 0;

	fseek(fp,0,SEEK_SET);

	fwrite(&(m.file.bfType),sizeof(m.file.bfType),1,fp);
	fseek(fp,2,SEEK_SET);
	fwrite(&(m.file.bfSize),sizeof(m)-2,1,fp);

	//54-62共八个字节表示调色板信息
	uchar Palette[8] = {0,0,0,0,255,255,255,0};//
	fseek(fp,54,SEEK_SET);
	fwrite(Palette,sizeof(uchar),8,fp);


	uchar* data = new uchar[widthStep*temp->height];
	memset(data,0,widthStep*temp->height);

	//单色位图，1表示白色，0表示黑色，结果图背景为白（1），线条为黑（0）
	for(int i = 0; i < temp->height; i++)
	{
		for(int j = 0; j < widthStep; j++)
		{
			uchar temp_data = 0;
			for(int k = 0; k < 8; k++)
			{
				if(j*8 + k < temp->width)
				{
					int temp_value = 0;
					if((uchar)temp->imageData[(temp->height-1-i)*temp->widthStep+j*8+k] == 255)
					{
						temp_value = 1 << (7-k);
						temp_data += temp_value;
					}
				}
			}
			data[i*widthStep+j] = temp_data;
		}
	}
	fseek(fp,62,SEEK_SET);
	fwrite(data,sizeof(uchar),widthStep*temp->height,fp);
	fclose(fp);
}

int main()
{
	writebmpfile();
	return 0;
}