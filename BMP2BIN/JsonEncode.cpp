#include "StdAfx.h"
#include "JsonEncode.h"
#include "ToolsBytes.h"
#include <string.h>

uint16_t JSONEncode::startJsonMsg(char* buffer,  char* fieldName, char* fieldValue)
{
    uint16_t nAddFieldPos = 0;
    uint16_t i = 0;

    buffer[nAddFieldPos++] = '{';
    buffer[nAddFieldPos++] = '"';

    //add field name
    for ( i = 0; i < MAX_FIELD_NAME_LEN; i++)
    {
        if (fieldName[i] != 0)
        {
            buffer[nAddFieldPos] = fieldName[i];
            nAddFieldPos++;
        }
        else
        {
            break;
        }
    }

    buffer[nAddFieldPos++] = '"';
    buffer[nAddFieldPos++] = ':';
    buffer[nAddFieldPos++] = '"';

    //add field value
    for (i = 0; i < MAX_FIELD_VALUE_LEN; i++)
    {
        if (fieldValue[i] != 0)
        {
            buffer[nAddFieldPos] = fieldValue[i];
            nAddFieldPos++;
        }
        else
        {
            break;
        }
    }

    buffer[nAddFieldPos++] = '"';

    return nAddFieldPos;
    
}


uint16_t JSONEncode::middleStrField(char* buffer, uint16_t nAddFieldPos, char* fieldName, char* fieldValue)
{
    uint16_t i = 0;

    buffer[nAddFieldPos++] = ',';
    buffer[nAddFieldPos++] = '"';
    
    //add field name
    for ( i = 0; i < MAX_FIELD_NAME_LEN; i++)
    {
       if (fieldName[i] != 0)
       {
           buffer[nAddFieldPos] = fieldName[i];
           nAddFieldPos++;
       }
       else
       {
           break;
       }
    }

    buffer[nAddFieldPos++] = '"';
    buffer[nAddFieldPos++] = ':';
    buffer[nAddFieldPos++] = '"';

    //add field value
    for (i = 0; i < MAX_FIELD_VALUE_LEN; i++)
    {
       if (fieldValue[i] != 0)
       {
           buffer[nAddFieldPos] = fieldValue[i];
           nAddFieldPos++;
       }
       else
       {
           break;
       }
    }

    buffer[nAddFieldPos++] = '"';

    return nAddFieldPos;

}

uint16_t JSONEncode::middleIntField(char* buffer, uint16_t nAddFieldPos, char* fieldName, int32_t nValue)
{
	uint16_t i = 0;

    buffer[nAddFieldPos++] = ',';
    buffer[nAddFieldPos++] = '"';
    
    //add field name
    for ( i = 0; i < MAX_FIELD_NAME_LEN; i++)
    {
       if (fieldName[i] != 0)
       {
           buffer[nAddFieldPos] = fieldName[i];
           nAddFieldPos++;
       }
       else
       {
           break;
       }
    }

    buffer[nAddFieldPos++] = '"';
    buffer[nAddFieldPos++] = ':';

    //add field value
    char tempBuffer[12] = {0};
    tools_itoa(nValue, tempBuffer, 10);
    uint8_t nDataLen = strlen(tempBuffer);
    memcpy(&buffer[nAddFieldPos], tempBuffer, nDataLen);
    nAddFieldPos += nDataLen;

    return nAddFieldPos;
}


uint16_t JSONEncode::endJsonMsg(char* buffer, uint16_t nAddFieldPos)
{
    buffer[nAddFieldPos++] = '}';
    buffer[nAddFieldPos++] = '\n';    
    buffer[nAddFieldPos] = 0;
    return nAddFieldPos;
}

