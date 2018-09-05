#ifndef GENERATE_PICTURE_H
#define GENERATE_PICTURE_H

#include <stdio.h>
#include <tchar.h>
#include "stdafx.h"
#include "BinCompressAlgorithm.h"

#define ENCODE_TYPE_NORMAL 1
#define ENCODE_TYPE_ZIP 2

typedef struct tagPictureNormalCode
{
	uint8_t nEncodeType;
	uint8_t nPictureID[4];
	uint8_t nPictureLen[2];
}PictureNormalCode;

typedef struct tagPictureZipCode
{
	uint8_t nEncodeType;
	uint8_t nPictureID[4];
	uint8_t nPictureLen[2];
	uint8_t nUnZipDictoryLen;   //解压字典个数
	uint8_t nUnZipDictoryCode[MAX_DICT_CHAR_NUM];  //解压字典长度
}PictureZipCode;

class GeneratePicture
{
public:
	int GeneratePicContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString);

	int GeneratePicZipContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString);
};

#endif