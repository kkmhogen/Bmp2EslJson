// BMP2BIN.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "BmpFileParse.h"
#include <iostream>
#include "JsonEncode.h"
#include "GeneratePicture.h"
using namespace std;

#define ERR_PARSE_FILE_FAIL 30
#define ERR_TRANSLATE_FILE_FAIL 31
#define ERR_PASSWORD_INVALID 32
#define ERR_MAC_ADDRESS_INVALID 33
#define ERR_ESL_NOT_SUPPORT 34
#define ERR_OPEN_WRITE_FILE 35
#define ERR_INTPUT_PARA_INVALID 36
#define ERR_FILE_TYPE_NOT_SUPPORT 37


#define MAX_FILE_NAME_LEN 100
#define MAX_BIN_FILE_LENGTH 60000
#define MAX_JSON_OUTPUT_FILE_LENGTH 100000

#define APP_PARA_NUMBER 5

uint8_t binFileOutputChar[MAX_BIN_FILE_LENGTH] = {0};
char jsonFileOutputChar[MAX_JSON_OUTPUT_FILE_LENGTH] = {0};
char JSON_TAIL_STR[] = ",\"data\":\"";

void PintInputError();

int bmpFile2Json(char* fileName, 
				 int nEslType, 
				 char* cPassword, 
				 char*cMacAddress, 
				 uint32_t nPictureID, 
				 uint32_t nMsgSeq, 
				 bool bUsingZip);

int main(int argc, char* argv[])
{
	int nEslType = LCD_29_TWO_COLOR;
	char cPassword[10] = "00000000";
	char cEslType[15] = "";
	char cMacAddress[20] = "A1A2A3A4A5A6";
	uint16_t nMessageSeqID = 0;
	uint32_t nPictureID = 0;
	bool bUsingZip = false;
	char bmpFileName[MAX_FILE_NAME_LEN] = {0};

	if (argc == 1)
	{
		cout<<"Input *.bmp format picture file name:";
		cin>>bmpFileName;

		if (NULL == strstr(bmpFileName, "bmp")
			&& NULL == strstr(bmpFileName, "BMP"))
		{
			cerr<<"Not supported picture file type"<<endl;
			return ERR_FILE_TYPE_NOT_SUPPORT;
		}

		cout<<"Please esl type(e21,e211,e29,e291,e42,e421):";
		cin>>cEslType;
		if (strcmp(cEslType, "e29") == 0)
		{
			nEslType = LCD_29_TWO_COLOR;
		}
		else if (strcmp(cEslType, "e21") == 0)
		{
			nEslType = LCD_21_TWO_COLOR;
		}
		else if (strcmp(cEslType, "e211") == 0)
		{
			nEslType = LCD_21_THREE_COLOR;
		}
		else if (strcmp(cEslType, "e291") == 0)
		{
			nEslType = LCD_29_THREE_COLOR;
		}
		else if (strcmp(cEslType, "e42") == 0)
		{
			nEslType = LCD_42_TWO_COLOR;
		}
		else if (strcmp(cEslType, "e421") == 0)
		{
			nEslType = LCD_42_THREE_COLOR;
		}
		else
		{
			return ERR_ESL_NOT_SUPPORT;
			cerr<<"esl type unknown"<<endl;
		}

		cout<<"Please input password:";
		cin>>cPassword;
		if (strlen(cPassword) != 8)
		{
			cerr<<"password must be 8 characteristic"<<endl;
			return ERR_PASSWORD_INVALID;
		}


		cout<<"Please input device mac address:";
		cin>>cMacAddress;
		if (strlen(cMacAddress) != 12)
		{
			cerr<<"mac address length must be 12 characteristic"<<endl;
			return ERR_MAC_ADDRESS_INVALID;
		}

		cout<<"Please input picture number:";
		cin>>nPictureID;

		cout<<"Please message sequence number:";
		cin>>nMessageSeqID;

		char nChar;
		bool bUsingZip = false;
		cout<<"Using Zip format? Y or N:";
		cin>>nChar;
		if (nChar == 'Y' || nChar == 'y')
		{
			bUsingZip = true;
		}

		if (ERR_SUCCESS == bmpFile2Json(bmpFileName, nEslType, cPassword, cMacAddress, nPictureID, nMessageSeqID, bUsingZip))
		{
			cout<<"Write json file success"<<endl;
		}

		char pause;
		cin>>pause;
	}
	else if (argc % 2 != 1 || argc < 3)
	{
		PintInputError();
		return ERR_INTPUT_PARA_INVALID;
	}
	else
	{
		for (int i = 1; i < argc; i+=2)
		{
			if (strcmp(argv[i], "-t") == 0)
			{
				if (strcmp(argv[i+1], "e29") == 0)
				{
					nEslType = LCD_29_TWO_COLOR;
				}
				else if (strcmp(argv[i+1], "e21") == 0)
				{
					nEslType = LCD_21_TWO_COLOR;
				}
				else if (strcmp(argv[i+1], "e211") == 0)
				{
					nEslType = LCD_21_THREE_COLOR;
				}
				else if (strcmp(argv[i+1], "e291") == 0)
				{
					nEslType = LCD_29_THREE_COLOR;
				}
				else if (strcmp(argv[i+1], "e42") == 0)
				{
					nEslType = LCD_42_TWO_COLOR;
				}
				else if (strcmp(argv[i+1], "e421") == 0)
				{
					nEslType = LCD_42_THREE_COLOR;
				}
				else
				{
					cerr<<"Unknown esl type"<<endl;
					return ERR_ESL_NOT_SUPPORT;
				}
			}
			else if (strcmp(argv[i], "-p") == 0)
			{
				if (strlen(argv[i+1]) != 8)
				{
					cerr<<"password length error"<<endl;
					return ERR_PASSWORD_INVALID;
				}
				else
				{
					strcpy_s(cPassword, 10, argv[i+1]);
				}
			}
			else if (strcmp(argv[i], "-m") == 0)
			{
				if (strlen(argv[i+1]) != 12)
				{
					cerr<<"mac address length error"<<endl;
					return ERR_MAC_ADDRESS_INVALID;
				}
				else
				{
					strcpy_s(cMacAddress, 20, argv[i+1]);
				}
			}
			else if (strcmp(argv[i], "-id") == 0)
			{
				nPictureID = atoi(argv[i+1]);
			}
			else if (strcmp(argv[i], "-s") == 0)
			{
				nMessageSeqID = atoi(argv[i+1]);
			}
			else if (strcmp(argv[i], "-z") == 0)
			{
				if ((strcmp(argv[i+1], "y") == 0) || (strcmp(argv[i+1], "Y") == 0))
				{
					bUsingZip = true;
				}
				else
				{
					bUsingZip = false;
				}
			}
			else if (strcmp(argv[i], "-f") == 0)
			{
				if (NULL == strstr(argv[i+1], "bmp")
					&& NULL == strstr(argv[i+1], "BMP"))
				{
					cerr<<"Not supported picture file type"<<endl;
					return ERR_FILE_TYPE_NOT_SUPPORT;
				}

				strcpy_s(bmpFileName, MAX_FILE_NAME_LEN, argv[i+1]);
			}
			else
			{
				cerr<<"Unknown para:"<<argv[i]<<endl;
			}
		}
		
		if (strlen(bmpFileName) < 3)
		{
			PintInputError();
			return ERR_INTPUT_PARA_INVALID;
		}

		int nErrorCode = bmpFile2Json(bmpFileName, nEslType, cPassword, cMacAddress, nPictureID, nMessageSeqID, bUsingZip);
		if (nErrorCode != ERR_SUCCESS)
		{
			cerr<<"bmp file to json error:"<<nErrorCode<<endl;
			return nErrorCode;
		}
	}

	return 0;
}

void PintInputError()
{
	cerr<<"command format error, bmp2esljson.exe -f BmpFileName -t EslType -p Password -m MacAddress -id PictureID -s MessageSeq -z UsingZip"<<endl;
	cerr<<"Example: bmp2esljson.exe -f goods.bmp -t e29 -p 00000000 -m A1A2A3A4A5A6 -id 1478 -s 142 -z y"<<endl;
}

int bmpFile2Json(char* fileName, 
				 int nEslType, 
				 char* cPassword, 
				 char*cMacAddress, 
				 uint32_t nPictureID, 
				 uint32_t nMsgSeq, 
				 bool bUsingZip)
{
	if (nEslType != LCD_29_TWO_COLOR 
		&& nEslType != LCD_42_TWO_COLOR 
		&& nEslType != LCD_42_THREE_COLOR 
		&& nEslType != LCD_29_THREE_COLOR 
		&& nEslType != LCD_21_TWO_COLOR
		&& nEslType != LCD_21_THREE_COLOR)
	{
		cerr<<"Esl not supported"<<endl;
		return ERR_ESL_NOT_SUPPORT;
	}

	if (strlen(cPassword) != 8)
	{
		cerr<<"password length error"<<endl;
		return ERR_PASSWORD_INVALID;
	}

	if (strlen(cMacAddress) != 12)
	{
		cerr<<"mac address length must be 12 characteristic"<<endl;
		return ERR_MAC_ADDRESS_INVALID;
	}

	BmpFileParse bmpFileParse;
	int nRet = bmpFileParse.loadBitmap(fileName);
	if (ERR_SUCCESS != nRet)
	{
		cerr<<"load bmp file failed, err:"<<nRet<<endl;
		return ERR_PARSE_FILE_FAIL;
	}

	uint32_t nBinFileLength = 0;
	memset(binFileOutputChar, 0, MAX_BIN_FILE_LENGTH);
	if (ERR_SUCCESS != bmpFileParse.getBmpLcdBinFile((LcdType)nEslType, binFileOutputChar, &nBinFileLength))
	{
		cerr<<"get bmp lcd file failed"<<endl;
		return ERR_TRANSLATE_FILE_FAIL;
	}
	if (nEslType == LCD_21_TWO_COLOR)
	{
		if (nBinFileLength != LCD21_CHAR_LEN)
		{
			cerr<<"File size not fit for 2.1inch 2 color ESL: "<<nBinFileLength<<endl;
			return ERR_TRANSLATE_FILE_FAIL;
		}
	}
	else if (nEslType == LCD_21_THREE_COLOR)
	{
		if (nBinFileLength != LCD211_CHAR_LEN)
		{
			cerr<<"File size not fit for 2.1inch 3 color ESL: "<<nBinFileLength<<endl;
			return ERR_TRANSLATE_FILE_FAIL;
		}
	}
	else if (nEslType == LCD_21_THREE_COLOR)
	{
		if (nBinFileLength != LCD211_CHAR_LEN)
		{
			cerr<<"File size not fit for 2.1inch 3 color ESL:"<<nBinFileLength<<endl;
			return ERR_TRANSLATE_FILE_FAIL;
		}
	}
	else if (nEslType == LCD_29_TWO_COLOR)
	{
		if (nBinFileLength != LCD29_CHAR_LEN)
		{
			cerr<<"File size not fit for 2.1 two color inch ESL:"<<nBinFileLength<<endl;
			return ERR_TRANSLATE_FILE_FAIL;
		}
	}
	else if (nEslType == LCD_29_THREE_COLOR)
	{
		if (nBinFileLength != LCD291_CHAR_LEN)
		{
			cerr<<"File size not fit for 2.9 three color inch ESL :"<<nBinFileLength<<endl;
			return ERR_TRANSLATE_FILE_FAIL;
		}
	}
	else if (nEslType == LCD_42_THREE_COLOR)
	{
		if (nBinFileLength != LCD421_CHAR_LEN)
		{
			cerr<<"File size not fit for 4.2 three color inch ESL :"<<nBinFileLength<<endl;
			return ERR_TRANSLATE_FILE_FAIL;
		}
	}
	else if (nEslType == LCD_42_TWO_COLOR)
	{
		if (nBinFileLength != LCD42_CHAR_LEN)
		{
			cerr<<"File size not fit for 4.2inch ESL:"<<nBinFileLength<<endl;
			return ERR_TRANSLATE_FILE_FAIL;
		}
	}
	else
	{
		cerr<<"picture file size not support"<<endl;
		return ERR_TRANSLATE_FILE_FAIL;
	}

	//encode message
	uint16_t nBufferIdx = 0;
	JSONEncode jsonEncode;
	nBufferIdx = jsonEncode.startJsonMsg(jsonFileOutputChar, "msg", "dData");
	nBufferIdx = jsonEncode.middleStrField(jsonFileOutputChar, nBufferIdx, "mac", cMacAddress);
	nBufferIdx = jsonEncode.middleIntField(jsonFileOutputChar, nBufferIdx, "seq", nMsgSeq);
	nBufferIdx = jsonEncode.middleStrField(jsonFileOutputChar, nBufferIdx, "auth1", cPassword);
	if (bUsingZip)
	{
		nBufferIdx = jsonEncode.middleStrField(jsonFileOutputChar, nBufferIdx, "dType", "ascii");
	}
	else
	{
		nBufferIdx = jsonEncode.middleStrField(jsonFileOutputChar, nBufferIdx, "dType", "hex");
	}
	memcpy(&jsonFileOutputChar[nBufferIdx], JSON_TAIL_STR,  sizeof(JSON_TAIL_STR)-1);
	nBufferIdx+= sizeof(JSON_TAIL_STR)-1;

	//encode picture file
	GeneratePicture picTools;
	if (bUsingZip)
	{
		int nEncodeLen = picTools.GeneratePicZipContent(binFileOutputChar,
			nBinFileLength, nPictureID, &jsonFileOutputChar[nBufferIdx]);
		nBufferIdx += nEncodeLen;
	}
	else
	{
		int nEncodeLen = picTools.GeneratePicContent(binFileOutputChar, 
			nBinFileLength, nPictureID, &jsonFileOutputChar[nBufferIdx]);
		nBufferIdx += nEncodeLen;
	}

	//add json tail
	memcpy(&jsonFileOutputChar[nBufferIdx], "\"}", 2);
	nBufferIdx += 2;

	//write json file to output file
	char outputFileName[MAX_FILE_NAME_LEN] = "";
	strcpy_s(outputFileName, MAX_FILE_NAME_LEN, fileName);
	if (bUsingZip)
	{
		strcat_s(outputFileName, MAX_FILE_NAME_LEN, "z.json");
	}
	else
	{
		strcat_s(outputFileName, MAX_FILE_NAME_LEN, ".json");
	}
	FILE* jsonOutputFile;
	int nErr = fopen_s(&jsonOutputFile, outputFileName,"wb");
	if (jsonOutputFile == NULL || nErr != 0)
	{
		cerr<<"Open  output file failed"<<endl;
		fclose(jsonOutputFile);
		return ERR_OPEN_WRITE_FILE;
	}

	if (1 != fwrite(jsonFileOutputChar, nBufferIdx, 1, jsonOutputFile))
	{
		cerr<<"write JSON file failed"<<endl;
		return ERR_OPEN_WRITE_FILE;
	}

	fclose(jsonOutputFile);
	return ERR_SUCCESS;
}

