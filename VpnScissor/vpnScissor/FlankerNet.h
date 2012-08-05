#pragma once
#include <map>
//#include <assert.h>
typedef unsigned char u_char;
class tools
{
public:
	tools();
	int convert(char) const;
	u_char convert(int) const;
	bool convert(const char*,u_char*,int);
	void ConvertCharStringToPacketFormat(u_char* source,u_char* dst,int len);
	bool ConvertIPV4StringtoHexString(u_char*,u_char*);
	void ConvertDecNumStringToHexString(u_char*,u_char*,int);
	void u_strcat(u_char*,u_char*);
private:
	std::map<int,u_char> IntToChar;
	std::map<u_char,int> CharToInt;
};