// stdafx.cpp : source file that includes just the standard includes
// TestZip.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "BinCompressAlgorithm.h"
#include "ToolsBytes.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


#define MIN_NUM_SYMBLE  'a'
#define MAX_NUM_SYMBLE  'z'

#define STATE_NO_ZIP 0
#define STATE_ZIP 1

#define FF_VALUE_SYMBLE 'G'
#define ZERO_VALUE_SYMBLE 'H'

//inputByteValue转换为十六进制字符，如果inputByteValue为压缩字符，则进行压缩
//返回值为输出字符长度，1或者2
int BinCompressAlgorithm::byte2HexString(const ZipStru* zipDictory, uint16_t nDicNum, uint8_t inputByteValue, char* pOutputHexStrVaue)
{
    unsigned char highByte, lowByte;

	if (zipDictory[inputByteValue].nSymbleTag != -1)
	{
		*pOutputHexStrVaue = zipDictory[inputByteValue].nSymbleTag;
        return 1;
	}

    highByte = inputByteValue >> 4;
    lowByte = inputByteValue & 0x0f ;

    highByte += 0x30;

    if (highByte > 0x39)
        pOutputHexStrVaue[0] = highByte + 0x07;
    else
        pOutputHexStrVaue[0] = highByte;

    lowByte += 0x30;

    if (lowByte > 0x39)
        pOutputHexStrVaue[1] = lowByte + 0x07;
    else
        pOutputHexStrVaue[1] = lowByte;

	return 2;
}




void BinCompressAlgorithm::Swap(ZipStru A[], int i, int j)
{
    ZipStru temp = A[i];

    A[i] = A[j];
    A[j] = temp;
}

void BinCompressAlgorithm::BubbleSort(ZipStru A[], int n)
{
    for (int j = 0; j < n - 1; j++)         // 每次最大元素就像气泡一样"浮"到数组的最后
    {
        for (int i = 0; i < n - 1 - j; i++) // 依次比较相邻的两个元素,使较大的那个向后移
        {
			if (A[i].nTotalNum < A[i + 1].nTotalNum)            
            {
                Swap(A, i + 1, i);
            }
        }
    }
}

int BinCompressAlgorithm::GenerateZipDictory(const uint8_t* pSourceValueCode, uint16_t sourceValueLen, ZipStru* outputZipDictory, uint8_t* outputUnzipDictory)
{
	int nZipDictoryLen = 0;
	ZipStru tempStruArray[MAX_ARRAY_SIZE];

	for (int i = 0; i < MAX_ARRAY_SIZE; i++)
	{
		outputZipDictory[i].nCharIndex = i;
		tempStruArray[i].nCharIndex = i;
		tempStruArray[i].nTotalNum = 0;
	}

	for (int i = 0; i < sourceValueLen; i++)
	{
		tempStruArray[pSourceValueCode[i]].nTotalNum ++;
	}

	BubbleSort(tempStruArray, MAX_ARRAY_SIZE);

	for (int i = 0; i < MAX_ARRAY_SIZE; i++)
	{
		if (tempStruArray[i].nTotalNum > MIN_SYMBLE_REPEAT_NUM 
			&& i < MAX_DICT_CHAR_NUM)
		{
			tempStruArray[i].nSymbleTag = ZIP_BEGIN_CODE + i;
			outputUnzipDictory[i] = tempStruArray[i].nCharIndex;
			nZipDictoryLen++;
		}
		else
		{
			tempStruArray[i].nSymbleTag = -1;
		}
	}

	for (int i = 0; i < MAX_ARRAY_SIZE; i++)
	{
		outputZipDictory[tempStruArray[i].nCharIndex].nSymbleTag = tempStruArray[i].nSymbleTag;
		outputZipDictory[tempStruArray[i].nCharIndex].nTotalNum = tempStruArray[i].nTotalNum;
	}

	return nZipDictoryLen;
}


int BinCompressAlgorithm::ZipBytes2String(const ZipStru* zipDictory, uint16_t nDicNum, const uint8_t* pSource, uint16_t sourceValueLen, char* pOutputZipString)
{
    int maxValueIndex = sourceValueLen - 1;
    int nLastZipCharLen;   //上一个压缩字符长度，1或者2
    uint8_t lastZipValue;
    int nState = STATE_NO_ZIP;
    char* pWriteLen = NULL;
    char* pWriteValue = pOutputZipString;

    for (int i = 0; i < sourceValueLen; i++)
    {
        if (nState == STATE_NO_ZIP)
        {
            if (pSource[i] == pSource[i+1] && i < maxValueIndex)
            {
				//write zip char length
                pWriteLen = pWriteValue;
                *pWriteLen = MIN_NUM_SYMBLE-1;
                pWriteValue++;
                
				//write zip char
                nLastZipCharLen = byte2HexString(zipDictory, nDicNum, pSource[i], pWriteValue);
                pWriteValue += nLastZipCharLen;
                
                lastZipValue = pSource[i];
                nState = STATE_ZIP;
            }
            else
            {
                nLastZipCharLen = byte2HexString(zipDictory, nDicNum, pSource[i], pWriteValue);
                pWriteValue += nLastZipCharLen;
            }
        }
        else if (nState == STATE_ZIP)
        { 
            if ((pSource[i] == lastZipValue) && (*pWriteLen < MAX_NUM_SYMBLE))
            {
                *pWriteLen = *pWriteLen + 1;
            }
            else
            {                
                i--;
                nState = STATE_NO_ZIP;
            }
        }
    }

    int nDestLen = pWriteValue - pOutputZipString;
	return nDestLen;
}



int BinCompressAlgorithm::UnZipString2Bytes(const uint8_t* pUnzipCodeArray, uint16_t nDicNum, const char* pSourceZipString, uint16_t sourceValueLen, uint8_t* pOutputUnzipBytes)
{
   uint8_t nDecodeSymble = 0;
   uint8_t gapZipState = STATE_PARSE_NZIP_HEAD;
   int nMaxSourceValueIdx = sourceValueLen - 1;
   int gapHasRcvLcdDataLen = 0;
   int gapZipValueLen = 0;
   uint8_t gapHeighLcdValue = 0;
   uint8_t* pTmpOutputPtr = pOutputUnzipBytes;
    
    for (int i = 0; i < sourceValueLen; i++)
    {
        switch (gapZipState)
        {
            case STATE_PARSE_NZIP_HEAD:
                {
                    if (pSourceZipString[i] >= ZIP_BEGIN_CODE && pSourceZipString[i] < (ZIP_BEGIN_CODE+nDicNum))
                    {
						int nZipCodeIdx = pSourceZipString[i] - ZIP_BEGIN_CODE;
                        nDecodeSymble = pUnzipCodeArray[nZipCodeIdx];
						*pTmpOutputPtr++ = nDecodeSymble;
                        gapHasRcvLcdDataLen++;
						gapZipState = STATE_PARSE_NZIP_HEAD;
                    }
                    else if (pSourceZipString[i] >= MIN_NUM_SYMBLE && pSourceZipString[i] <= MAX_NUM_SYMBLE)
                    {                        
                        gapZipValueLen = pSourceZipString[i] - MIN_NUM_SYMBLE + 2;
						gapZipState = STATE_PARSE_ZIP_HEAD;    
                    }
                    else
                    {
						//has next low char value
                        gapHeighLcdValue = pSourceZipString[i];
                        gapZipState = STATE_PARSE_NZIP_ENDVALUE;
                    }
                }
                break;
                
            case STATE_PARSE_NZIP_ENDVALUE:
                {
                    nDecodeSymble = tools_char2Bytes(gapHeighLcdValue, pSourceZipString[i]);

					*pTmpOutputPtr++ = nDecodeSymble;
                    gapHasRcvLcdDataLen++;
					gapZipState = STATE_PARSE_NZIP_HEAD;
                }
                break;
                
            case STATE_PARSE_ZIP_HEAD:
                {
                    if (pSourceZipString[i] >= ZIP_BEGIN_CODE && pSourceZipString[i] < (ZIP_BEGIN_CODE+nDicNum))
                    {
						int nZipCodeIdx = pSourceZipString[i] - ZIP_BEGIN_CODE;
                        nDecodeSymble = pUnzipCodeArray[nZipCodeIdx];
                    }
                    else
                    {
						//has next low char value
                        gapHeighLcdValue = pSourceZipString[i];
                        gapZipState = STATE_PARSE_ZIP_ENDVALUE;
                        continue;
                    }

                    for (int j = 0; j < gapZipValueLen; j++)
                    {
						*pTmpOutputPtr++ = nDecodeSymble;
                        gapHasRcvLcdDataLen++;
                    }

                    gapZipState = STATE_PARSE_NZIP_HEAD;
                }
                break;

           case STATE_PARSE_ZIP_ENDVALUE:
                {
                    nDecodeSymble = tools_char2Bytes(gapHeighLcdValue, pSourceZipString[i]);
                    for (int j = 0; j < gapZipValueLen; j++)
                    {
						*pTmpOutputPtr++ = nDecodeSymble;
                        gapHasRcvLcdDataLen++;
                    }
                    gapZipState = STATE_PARSE_NZIP_HEAD;
                }
                break;

            default:
                break;
       }
    }

    return gapHasRcvLcdDataLen;
}

