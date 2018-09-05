#ifndef JSON_ENCODE_H
#define JSON_ENCODE_H

#define MAX_FIELD_NAME_LEN 200
#define MAX_FIELD_VALUE_LEN 1000

class JSONEncode
{
	public:
		uint16_t startJsonMsg(char* buffer,  char* fieldName, char* fieldValue);

		uint16_t middleStrField(char* buffer, uint16_t nAddFieldPos, char* fieldName, char* fieldValue);

		uint16_t middleIntField(char* buffer, uint16_t nAddFieldPos, char* fieldName, int32_t nValue);

		uint16_t endJsonMsg(char* buffer, uint16_t nAddFieldPos);
};

#endif