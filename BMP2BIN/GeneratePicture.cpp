#include "stdafx.h"
#include "GeneratePicture.h"
#include "JsonEncode.h"
#include "BinCompressAlgorithm.h"
#include "ToolsBytes.h"


int GeneratePicture::GeneratePicContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString)
{
	//write tag
	PictureNormalCode picContentHead;
	int nDecodeLen = 0;

	picContentHead.nEncodeType = ENCODE_TYPE_NORMAL; //hex picture type

	*(uint32_t*)picContentHead.nPictureID = tools_H2NL(nPictureID);  //picture id

	//picture id
	*(uint16_t*)picContentHead.nPictureLen = tools_H2NS(nPicContentLen);

	//head to hex string
	tools_byteToHexStrs((uint8_t*)&picContentHead, pOutputString, sizeof(picContentHead));
	nDecodeLen += sizeof(picContentHead) * 2;
	pOutputString += nDecodeLen;

	// to hex string
	int nJsonDataLen = nPicContentLen * 2;
	tools_byteToHexStrs(pPicContent, pOutputString, nPicContentLen);
	nDecodeLen += nJsonDataLen;

	return nDecodeLen;
}

int GeneratePicture::GeneratePicZipContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString)
{
	BinCompressAlgorithm zipAlgorithm;
	ZipStru zipDictory[MAX_ARRAY_SIZE];   //Ñ¹Ëõ×Öµä

	//write tag
	PictureZipCode picContentHead;
	int nDecodeLen = 0;

	picContentHead.nEncodeType = ENCODE_TYPE_ZIP; //hex picture type

	//picture id
	*(uint32_t*)picContentHead.nPictureID = tools_H2NL(nPictureID);  //picture id

	//picture len
	*(uint16_t*)picContentHead.nPictureLen = tools_H2NS(nPicContentLen);


	//Éú³ÉÑ¹ËõºÍ½âÑ¹×Öµä
	int nZipDictoryLen = zipAlgorithm.GenerateZipDictory(pPicContent, nPicContentLen, zipDictory, picContentHead.nUnZipDictoryCode);
	picContentHead.nUnZipDictoryLen = nZipDictoryLen;
	int nValidHeadLen = sizeof(picContentHead) - (MAX_DICT_CHAR_NUM - picContentHead.nUnZipDictoryLen);

	//head to hex string
	tools_byteToHexStrs((uint8_t*)&picContentHead, pOutputString, nValidHeadLen);
	nDecodeLen += nValidHeadLen * 2;
	pOutputString += nDecodeLen;

	//Ñ¹ËõÎÄ¼þ
	uint16_t nZipFileLen = 0;
	nZipFileLen = zipAlgorithm.ZipBytes2String(zipDictory, nZipDictoryLen, pPicContent, nPicContentLen, pOutputString);
	nDecodeLen += nZipFileLen;

	return nDecodeLen;
}