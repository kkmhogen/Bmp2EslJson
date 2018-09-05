#include "stdafx.h"
#include "BmpFileParse.h"
#include <stdlib.h>
#include <math.h>
#include "ToolsBytes.h"
#include <iostream>
using namespace std;


BmpFileParse::BmpFileParse(void)
{
	mBitMapFile.imageData = NULL;
	mBitMapFile.bmpColors = NULL;
}


BmpFileParse::~BmpFileParse(void)
{
	clearResource();
}

void BmpFileParse::clearResource()
{
	if (mBitMapFile.imageData != NULL)
	{
		free(mBitMapFile.imageData);
		mBitMapFile.imageData = NULL;
	}

	if (mBitMapFile.bmpColors != NULL)
	{
		free(mBitMapFile.bmpColors);
		mBitMapFile.bmpColors = NULL;
	}
}




/***********************************************************************************************/


/*fuction:BITMAPFILE * loadBitmap(char *path); */
/*load a bitmap*/
/***********************************************************************************************/
int BmpFileParse::loadBitmap(char *path)
{
    FILE *bmfp;

	int nErr = fopen_s(&bmfp, path,"rb");
    if (bmfp == NULL || nErr != 0)
    {
        cerr<<"open "<<path <<" fail!\n";
        return ERR_FILE_OPEN_FAIL;
    }

	//make sure privous file clear 
	clearResource();

    /********************************test the file to judge whether it is a bitmap**************************/
    unsigned short bfType = 0;
    fseek(bmfp, 0L, SEEK_SET);
    fread(&bfType, sizeof(char), 2, bmfp);
    if (0x4D42!= bfType)
    {
		cerr<<"This is not a bmp file!!!\n";
		fclose(bmfp);
		return ERR_FILE_NOT_BMP;
    }

    /*********************************read the Header of the bitmap*****************************************/
    /*read file information header*/
    fseek(bmfp, 0L, SEEK_SET);
	fread(&mBitMapFile.bfHeader, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), bmfp);


    /*read bmp information header*/
    fread(&(mBitMapFile.biHeader), 1 ,sizeof(BITMAPINFOHEADER), bmfp);


    /********************************read image data*******************************************************/
    /*read image data*/
	int nRet = ERR_SUCCESS;
    if(mBitMapFile.biHeader.biBitCount == 1)
    {
		//read file
		if (!parse1BitBmpFile(bmfp))
		{
			nRet = ERR_FILE_PARSE_ERR;
		}
	}
	else if(mBitMapFile.biHeader.biBitCount == 8)
    {
		//read file
		if (!parse8BitBmpFile(bmfp))
		{
			nRet = ERR_FILE_PARSE_ERR;
		}
	}
	else if(mBitMapFile.biHeader.biBitCount == 256)
    {
		//read file
		if (!parse256BitBmpFile(bmfp))
		{
			nRet = ERR_FILE_PARSE_ERR;
		}
	}
	else
	{
		
		nRet = ERR_FILE_NOT_SUPPORT;
	}
    fclose(bmfp);

	return nRet;
}

int BmpFileParse::getBmpLcdBinFile(LcdType nLcdType, uint8_t* pOutputLcdBinFile, uint32_t* nOutputFileLength)
{
	uint8_t* pWrite  = pOutputLcdBinFile;

	if (mBitMapFile.imageData == NULL)
	{
		return ERR_FILE_EMPTY;
	}

	if (nLcdType == LCD_29_TWO_COLOR)
	{
		if (mBitMapFile.width != LCD_29_WIDTH || mBitMapFile.height != LCD_29_HEIGHT)
		{
			cerr<<"file width or height not support"<<endl;
			return ERR_LCD_SIZE_ERR;
		}

		uint8_t pixelColor = 0;
		int nPixelBitLen = 0;
		for (int column = mBitMapFile.width - 1; column >= 0; column--)
		{
			for (int line = 0; line < mBitMapFile.height; line++)
			{
				pixelColor = mBitMapFile.imageData[line * mBitMapFile.width + column];
				tools_setBitOfChar(pWrite, pixelColor, nPixelBitLen);

				nPixelBitLen++;
				if (nPixelBitLen >= 8)
				{
					pWrite++;
					nPixelBitLen = 0;
				}
			}
		}
	}

	*nOutputFileLength = (pWrite - pOutputLcdBinFile);

	return ERR_SUCCESS;
}

bool BmpFileParse::parse1BitBmpFile(FILE* bmfp)
{
	int offset = 0;
	int step = 0;
	int width = mBitMapFile.biHeader.biWidth;
    int height = mBitMapFile.biHeader.biHeight;
	uint32_t channels = 0;

	if (mBitMapFile.biHeader.biBitCount == 1) 
	{
        channels = mBitMapFile.channels = 1;

        //一个字节存8个像素，除以8向上取整得到字节数
        offset = ((int)ceil(width / 8.0)) % 4; 

        //求每行末尾的无意义字节数
        if (offset != 0)
        {
            offset = 4 - offset;
        }

        //存数据的数据块，每个像素用一个字节
        mBitMapFile.imageData = (uint8_t*)malloc(sizeof(uint8_t) * width * height);
		mBitMapFile.width = width;
		mBitMapFile.height = height;
        step = channels * width; //每行宽度，字节为单位

		//读取调色板，二值图像调色板只有两种颜色
		if (mBitMapFile.biHeader.biClrUsed == 2 || mBitMapFile.biHeader.biClrUsed == 0)
		{
			mBitMapFile.bmpColors = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 2); 
			if (!fread(mBitMapFile.bmpColors, sizeof(RGBQUAD), 2, bmfp))
			{
				return false;
			}
		}
        int w = (int)ceil(width / 8.0);//每行占的字节数

        //用于临时存储从每个字节读取出来的像素值
        uint8_t bits[8];
		uint8_t pixVal;
        
		//计数器，每行达到图像宽度（width-1），说明后面的bit均为无效，因为最后一个有效字节也只有部分bits是有效的
        int m = 0;
        for (int i = 0; i < height; i++)
        {
            m = 0;
            for (int j = 0; j < w; j++)
            {
                if (!fread(&pixVal, sizeof(unsigned char), 1, bmfp))
				{
					return false;
				}

                //获取字符中没一位的值，以一个unsigned char[8]的数组存储
                tools_getBitsOfChar(pixVal, bits);
                    
				//把每个字节的8位值解析出来，分别存入8个字节
                for (int k = 0; k < 8; k++) 
				{
                    if (m < width) 
					{
                        //    count[(height - 1 - i)*step + m] = bits[k];
                        if (bits[k] == 1) 
						{ 
							//把值1映射为8位图中的255
                            bits[k] = 255;
                        }

                        // 坐标原点在左下角=》(height - 1 - i)*step+j，通过这个变换坐标原点变为左上角
                        mBitMapFile.imageData[(height - 1 - i)*step + m] = bits[k];
                    }
                    m++;
                }
            }

			//读行末尾无意义的字节
            if (offset != 0)
            {
                for (int j = 0; j < offset; j++)
                {
                    if (!fread(&pixVal, sizeof(unsigned char), 1, bmfp))
					{
						return false;
					}
                }
            }
        }
    }

	return true;
}



bool BmpFileParse::parse8BitBmpFile(FILE* bmfp)
{
	int offset = 0;
	int step = 0;
	int width = mBitMapFile.biHeader.biWidth;
    int height = mBitMapFile.biHeader.biHeight;
	uint32_t channels = 0;

	if (mBitMapFile.biHeader.biBitCount == 8)//256色位图
    {
        channels = 1;
        //每行数据量是4字节整数倍
        offset = (channels*width) % 4;
        //计算每行需要在有效数据后填充的无意义字节数
        if (offset != 0)
        {
            offset = 4 - offset;
        }
        mBitMapFile.channels = 1;
		mBitMapFile.width = width;
		mBitMapFile.height = height;
        mBitMapFile.imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
        step = channels*width; //每行宽度（字节为单位）

		uint32_t nClrColorNum = 0;
		if (mBitMapFile.biHeader.biClrUsed == 0)
		{
			nClrColorNum = 256;
		}
		else
		{
			nClrColorNum = mBitMapFile.biHeader.biClrUsed;
		}

		mBitMapFile.bmpColors = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256); 

		//读取调色板，8位字符调色板包含256种颜色
		if (!fread(mBitMapFile.bmpColors, sizeof(RGBQUAD), 256, bmfp))
		{
			return false;
		}

        //读每个像素的值
		uint8_t pixVal;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (!fread(&pixVal, sizeof(unsigned char), 1, bmfp))
				{
					return false;
				}
				if (pixVal > nClrColorNum)
				{
					return false;
				}

				if (mBitMapFile.bmpColors[pixVal].rgbBlue == 255
					&& mBitMapFile.bmpColors[pixVal].rgbGreen == 255
					&& mBitMapFile.bmpColors[pixVal].rgbRed == 255)
				{
					pixVal = 255;
				}
				else
				{
					pixVal = 0;
				}

                // 坐标原点在左下角=》(height - 1 - i)*step+j，通过这个变换坐标原点变为左上角
                mBitMapFile.imageData[(height - 1 - i)*step + j] = pixVal;
            }

            //读行末尾无意义的字节
            if (offset != 0)
            {
                for (int j = 0; j < offset; j++)
                {
                    if (!fread(&pixVal, sizeof(unsigned char), 1, bmfp))
					{
						return false;
					}
                }
            }
        }
    }

	return true;
}

bool BmpFileParse::parse256BitBmpFile(FILE* bmfp)
{
	int offset = 0;
	int step = 0;
	int width = mBitMapFile.biHeader.biWidth;
    int height = mBitMapFile.biHeader.biHeight;
	uint32_t channels = 0;

	if (mBitMapFile.biHeader.biBitCount == 8)//256色位图
    {
        channels = 1;
        //每行数据量是4字节整数倍
        offset = (channels*width) % 4;
        //计算每行需要在有效数据后填充的无意义字节数
        if (offset != 0)
        {
            offset = 4 - offset;
        }
        mBitMapFile.channels = 1;
		mBitMapFile.width = width;
		mBitMapFile.height = height;
        mBitMapFile.imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
        step = channels*width; //每行宽度（字节为单位）

		//读取调色板，8位字符调色板包含256种颜色
		fread(mBitMapFile.bmpColors, sizeof(RGBQUAD), 256, bmfp);

        //读每个像素的值
		uint8_t pixVal;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (!fread(&pixVal, sizeof(unsigned char), 1, bmfp))
				{
					return false;
				}
				if (mBitMapFile.bmpColors[pixVal].rgbBlue == 255
					&& mBitMapFile.bmpColors[pixVal].rgbGreen == 255
					&& mBitMapFile.bmpColors[pixVal].rgbRed == 255)
				{
					pixVal = 255;
				}
				else
				{
					pixVal = 0;
				}

                // 坐标原点在左下角=》(height - 1 - i)*step+j，通过这个变换坐标原点变为左上角
                mBitMapFile.imageData[(height - 1 - i)*step + j] = pixVal;
            }

            //读行末尾无意义的字节
            if (offset != 0)
            {
                for (int j = 0; j < offset; j++)
                {
                    if (!fread(&pixVal, sizeof(unsigned char), 1, bmfp))
					{
						return false;
					}
                }
            }
        }
    }

	return true;
}