#include "StdAfx.h"
#include "ToolsBytes.h"


// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

//return length, not include 0
uint8_t tools_itoa(int32_t val, char* dst, int32_t radix)
{
	uint8_t length = 0;

    char *_pdst = dst;   
    if (!val) //允许val等于0 
    {
        *_pdst = '0';
        *++_pdst = '\0';
		length = _pdst- dst + 1;
        return length;
    }        

    if(val <0)
    {
        *_pdst++ = '-';
        val = -val;
    }
    char *_first = _pdst;     
    char _cov;           
    uint16_t _rem;   
    while(val > 0)
    {
        _rem = (unsigned int)(val % radix);
        val /= radix;//每次计算一位 ，从低到高
        if  (_rem > 9)//16进制
            *_pdst++ = (char)(_rem - 10 + 'a'); 
        else
            *_pdst++ = (char)(_rem + '0');      
    }      
    *_pdst-- = '\0';
	length = _pdst - dst + 1;
    
	do
	{ 
		//由于数据是地位到高位储存的，需要转换位置
        _cov = *_pdst;
        *_pdst = *_first;
        *_first = _cov;
        _pdst--;
        _first++;        
    }while(_first < _pdst);  

    return length;
}

uint8_t tools_char2Bytes(char cValue1, char cValue2)
{
    char arrayChar[] = "0123456789ABCDEF";
    int lowValue = -1, highValue = -1;

    for (int i = 0; i < sizeof(arrayChar); i++)
    {
        if (arrayChar[i] == cValue1)
        {
            highValue = i;
            if (lowValue != -1)
            {
                break;
            }
        }
        
        if (arrayChar[i] == cValue2)
        {
            lowValue = i;
            if (highValue != -1)
            {
                break;
            }
        }
    }

    uint8_t nValue = (uint8_t)((highValue << 4) + lowValue);

    return nValue;
}

int tools_byteToHexStrs(const uint8_t* source, char* dest, uint32_t sourceLen)
{
    uint32_t i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;

        highByte += 0x30;

        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }

    return sourceLen*2;
}

void tools_getBitsOfChar(uint8_t inputChar, uint8_t* bitsOutput)
{
	for (int i = 0, j = 7; i < 8; i++, j--)
	{
		bitsOutput[i] = (inputChar >> j) & 0x1;
	}
}

void tools_setBitOfChar(uint8_t* pOutputChar, uint8_t pixelColor, uint8_t nPixelBitPos)
{
	if (pixelColor)
	{
		uint8_t nMask = 1;
		nMask = nMask << (7-nPixelBitPos);

		*pOutputChar  = *pOutputChar | nMask;
	}
}

uint32_t tools_H2NL(uint32_t hostValue)
{
	uint32_t networkValue = 0;
	uint8_t* pByte = (uint8_t*)&networkValue;
	pByte[0] = hostValue >> 24; 
	pByte[1] = (uint8_t)((hostValue >> 16) & 0xFF);
	pByte[2] = (uint8_t)((hostValue >> 8) & 0xFF);
	pByte[3] = (uint8_t)(hostValue & 0xFF);

	return networkValue;
}

uint16_t tools_H2NS(uint16_t hostValue)
{
	uint16_t networkValue = 0;
	uint8_t* pByte = (uint8_t*)&networkValue;
	pByte[0] = (uint8_t)(hostValue >> 8);
	pByte[1] = (uint8_t)(hostValue & 0xFF);

	return networkValue;
}