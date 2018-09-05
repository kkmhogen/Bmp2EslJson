#include <stdio.h>
#include <Windows.h>

/*
void ToGray(char* srcFile, char* destFile)
{
	BITMAPFILEHEADER fileHeader;// 位图头文件
	BITMAPINFOHEADER infoHeader;// 位图信息头
	FILE* pFile = fopen(srcFile, "rb");
	if (pFile == NULL)
	{
		printf("打开位图失败\n");
		exit(-1);
	}
	// 读取头信息
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);
	WORD bitCount = infoHeader.biBitCount;//取色深
	if (bitCount == 16)
	{
		exit(-1);
	}
	double byteCount = (double)bitCount / 8;
	int nClr = 0;
	if (bitCount < 16)
	{
		// 读取使用的调色板颜色数量,biClrUsed为0时，代表位数默认的调色板值
		nClr = infoHeader.biClrUsed ? infoHeader.biClrUsed : 1 << bitCount;
		if (nClr>256)
			nClr = 0;
	}
	// 读取调色板
	RGBQUAD *quad = NULL;
	if (nClr > 0)//排除24位和32位没有调色板的
	{
		quad = (RGBQUAD*)malloc(sizeof(RGBQUAD)*nClr);
		fread(quad, sizeof(RGBQUAD)*nClr, 1, pFile);
	}
	LONG srcW = infoHeader.biWidth;
	LONG srcH = infoHeader.biHeight;
	// 算出行字节数 位数*像素/8
	int lineSize = bitCount * srcW >> 3;
	//偏移量, windows系统要求每个扫描行按四字节对齐
	//int alignBytes = (((infoHeader.biWidth * bitCount + 31) & ~31) >> 3) - ((infoHeader.biWidth * bitCount) >> 3);
	int alignBytes = 4 - (infoHeader.biWidth*bitCount >> 3) % 4;
	if (alignBytes == 4)
	{
		alignBytes = 0;
	}
	
	//原图像缓存      
	int srcBufSize = lineSize * srcH;
	BYTE* srcBuf = (BYTE*)malloc(srcBufSize);
	int i, j;
	//读取文件中数据   
	for (i = 0; i < srcH; i++)
	{
		fread(&srcBuf[lineSize * i], lineSize, 1, pFile);
		fseek(pFile, alignBytes, SEEK_CUR);
	}
	//256色位图调色板  
	RGBQUAD *quadDes = NULL;
	quadDes = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
	for (i = 0; i < 256; i++)
	{
		//灰度图的RGB值相等  
		quadDes[i].rgbBlue = quadDes[i].rgbGreen = quadDes[i].rgbRed = i;
	}
	//灰度图每个像素采用8位表示 
//	int desBufSize = (((srcW * 8 + 31) & ~31) >> 3) * srcH;
	int desBufSize = (4 - srcW % 4 + srcW)*srcH;
	if (srcW % 4 == 0)
	{
		desBufSize = srcW*srcH;
	}
	BYTE *desBuf = (BYTE*)malloc(desBufSize);
	//每个扫描行占用字节数  
	int desLineSize = ((srcW * 8 + 31) >> 5) * 4;
//	int desLineSize = desBufSize/srcH;
	for (i = 0; i < srcH; i++)
	{
		for (j = 0; j < srcW; j++)
		{
			//从调色板中读取RGB值   
			if (nClr > 0)
			{
				unsigned int pos = srcBuf[i * lineSize + (int)(j * byteCount)];
				desBuf[i * desLineSize + j] = 0.3 * quad[pos].rgbRed + 0.59 * quad[pos].rgbGreen + 0.11 * quad[pos].rgbBlue;
			}
			else
				desBuf[i * desLineSize + j] = 0.3 * srcBuf[i * lineSize + (int)(j * byteCount)]
				+ 0.59 * srcBuf[i * lineSize + (int)(j * byteCount) + 1]
				+ 0.11 * srcBuf[i * lineSize + (int)(j * byteCount) + 2];
		}
	}
	//创建目标文件   
	FILE* pDfile = fopen(destFile, "wb");
	//文件头信息  
	BITMAPFILEHEADER nbmfHeader;
	nbmfHeader.bfType = 0x4D42;
	nbmfHeader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
		+256 * sizeof(RGBQUAD)+srcW * srcH;
	nbmfHeader.bfReserved1 = 0;
	nbmfHeader.bfReserved2 = 0;
	nbmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256 * sizeof(RGBQUAD);
	//Bitmap头信息  
	BITMAPINFOHEADER   bmi;
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = srcW;
	bmi.biHeight = srcH;
	bmi.biPlanes = 1;
	bmi.biBitCount = 8;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;
	bmi.biXPelsPerMeter = 0;
	bmi.biYPelsPerMeter = 0;
	bmi.biClrUsed = 256;
	bmi.biClrImportant = 0;
	//写入文件头信息  
	fwrite(&nbmfHeader, sizeof(BITMAPFILEHEADER), 1, pDfile);
	//写入Bitmap头信息  
	fwrite(&bmi, sizeof(BITMAPINFOHEADER), 1, pDfile);
	if (quadDes)
	{
		fwrite(quadDes, sizeof(RGBQUAD)* 256, 1, pDfile);
	}
	//写入图像数据  
	fwrite(desBuf, desBufSize, 1, pDfile);
	fclose(pDfile);
	if (quad)
	{
		free(quad);
		quad = NULL;
	}
	if (quadDes)
	{
		free(quadDes);
		quadDes = NULL;
	}
	if (desBuf)
	{
		free(desBuf);
		desBuf = NULL;
	}
}*/

void ToGray(char* srcFile, char* destFile)
{
	BITMAPFILEHEADER fileHeader;// 位图头文件
	BITMAPINFOHEADER infoHeader;// 位图信息头
	FILE* pFile = fopen(srcFile, "rb");
	if (pFile == NULL)
	{
		printf("打开位图失败\n");
		exit(-1);
	}
	// 读取头信息
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);
	WORD bitCount = infoHeader.biBitCount;//取表示色彩的位数
	if (bitCount == 16)
	{
		exit(-1);
	}
	int nClr = 0;
	int i = 0, j = 0;
	RGBQUAD *quad = NULL;
	if (bitCount < 16)
	{
		// 读取使用的调色板颜色数量,biClrUsed为0时，代表位数默认的调色板值
		nClr = infoHeader.biClrUsed ? infoHeader.biClrUsed : 1 << bitCount;
		if (nClr>256)
			nClr = 0;
	}
	// 读取调色板
	if (nClr > 0)//排除24位和32位没有调色板的
	{
		quad = (RGBQUAD*)malloc(sizeof(RGBQUAD)*nClr);
		fread(quad, sizeof(RGBQUAD)*nClr, 1, pFile);
		for (i = 0; i < nClr; i++)
		{
			quad[i].rgbRed = quad[i].rgbBlue = quad[i].rgbGreen =
				(BYTE)(0.3 * quad[i].rgbRed + 0.59 * quad[i].rgbGreen + 0.11 * quad[i].rgbBlue);
		}
	}
	LONG srcW = infoHeader.biWidth;
	LONG srcH = infoHeader.biHeight;
	// 图像数据区域
	int BufSize = (((srcW * bitCount + 31) & ~31)>>3)*srcH;
	BYTE* Buf = (BYTE*)malloc(BufSize);
	int LineSize = (((srcW * bitCount + 31) & ~31) >> 3);
	fread(Buf, BufSize, 1, pFile);
	if (nClr == 0)
	{
		for (i = 0; i < srcH; i++)//行
		{
			if (bitCount == 24)
			{
				for (j = 0; j < srcW*3; j = j + 3)// 列,因为每个像素占3个字节
				{
					int n = i*LineSize + j;
					Buf[n] = Buf[n + 1] = Buf[n + 2] = (BYTE)(0.3*Buf[n] + 0.59*Buf[n + 1] + 0.11*Buf[n + 2]);
				}
			}
			else
			{
				for (j = 0; j < srcW*4; j = j + 4)// 列
				{
					int n = i*LineSize + j;
					Buf[n] = Buf[n + 1] = Buf[n + 2] = 0.11*Buf[n] + 0.59*Buf[n + 1] + 0.3*Buf[n + 2];

				}
			}		
		}
	}
	//创建目标文件   
	FILE* pDfile = fopen(destFile, "wb");
	//写入数据
	// 写入文件头
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pDfile);
	//写入Bitmap头信息  
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pDfile);
	if (quad)
	{
		fwrite(quad, sizeof(RGBQUAD)* nClr, 1, pDfile);
	}
	//写入图像数据  
	fwrite(Buf, BufSize, 1, pDfile);
	fclose(pDfile);
	if (quad)
	{
		free(quad);
		quad = NULL;
	}
	if (Buf)
	{
		free(Buf);
		Buf = NULL;
	}
}
int main(int argc, char* argv[])
{
	ToGray(argv[1], argv[2]); 
//	ToGray(argv[3], argv[4]);//单色
	ToGray(argv[5], argv[6]);//16色
	ToGray(argv[7], argv[8]);// 256色
	ToGray(argv[9], argv[10]);// 24位
	ToGray(argv[11], argv[12]);//32位
	return 0;
}