#ifndef TOOLS_BYTES_H
#define TOOLS_BYTES_H

int tools_byteToHexStrs(const uint8_t* source, char* dest, uint32_t sourceLen);

void tools_getBitsOfChar(uint8_t inputChar, uint8_t* bitsOutput);

//set one bit color (0: black,  1: white)
void tools_setBitOfChar(uint8_t* pOutputChar, uint8_t pixelColor, uint8_t nPixelBitPos);

//set two bit color (00: black,  01: white, 02: red)
void tools_set2BitOfChar(uint8_t* pOutputChar, uint8_t pixelColor, uint8_t nPixelBitPos);

//字节序转换，从大端到小端
uint32_t tools_H2NL(uint32_t hostValue);

//字节序转换
uint16_t tools_H2NS(uint16_t hostValue);

uint8_t tools_char2Bytes(char cValue1, char cValue2);

uint8_t tools_itoa(int32_t val, char* dst, int32_t radix);


#endif