// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef BIN_ZIP_ALG_H
#define BIN_ZIP_ALG_H

#include <stdio.h>
#include <tchar.h>
#include "stdafx.h"

typedef struct tagZipStru
{
	uint8_t nCharIndex;
	int nSymbleTag;
	uint16_t nTotalNum;
}ZipStru;


#define MAX_ARRAY_SIZE 256
#define MAX_DICT_CHAR_NUM 20
#define MIN_SYMBLE_REPEAT_NUM 10
#define ZIP_BEGIN_CODE 'G'

typedef enum
{
    STATE_PARSE_NZIP_HEAD =0,
    STATE_PARSE_NZIP_ENDVALUE = 1,
    STATE_PARSE_ZIP_HEAD = 2,
    STATE_PARSE_ZIP_ENDVALUE = 3,
}eZipState;

class BinCompressAlgorithm
{
	public:
	//zip byte to string
	int ZipBytes2String(const ZipStru* zipDictory, uint16_t nDicNum, const uint8_t* pSource, uint16_t sourceValueLen, char* pOutputZipCode);

	//unzip string to bytes
	int UnZipString2Bytes(const uint8_t* pUnzipCodeArray, uint16_t nDicNum, const char* pSourceZipString, uint16_t sourceValueLen, uint8_t* pOutputUnzipBytes);

	//generate zip dictory
	int GenerateZipDictory(const uint8_t* pSourceValueCode, uint16_t sourceValueLen, ZipStru* outputZipDictory, uint8_t* outputUnzipDictory);

private:
	int byte2HexString(const ZipStru* zipDictory, uint16_t nDicNum, uint8_t inputByteValue, char* pOutputHexStrVaue);

	void Swap(ZipStru A[], int i, int j);

	void BubbleSort(ZipStru A[], int n);
};

#endif