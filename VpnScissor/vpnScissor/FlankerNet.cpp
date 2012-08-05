#include "FlankerNet.h"
tools::tools()
{
	for(int i=10;i<36;i++){
		IntToChar[i]='a'+i-10;
		CharToInt['a'+i-10]=i;
	}
	for(int i=0;i<10;i++)
	{
		IntToChar[i]='0'+i;
		CharToInt['0'+i]=i;
	}
}
u_char tools::convert(int income) const
{
	std::map<int,u_char>::const_iterator it=IntToChar.find(income);//此处不定义为const iterator编译不通过
	if(it!=std::map<int,u_char>::const_iterator(IntToChar.end()))
		return it->second;
}
int tools::convert(char income) const 
{
	char key;
	if(income<='Z'&&income>='A')
		key=income-'A'+'a';
	else
		key=income;
	std::map<u_char,int>::const_iterator it=CharToInt.find(income);
	if(it!=std::map<u_char,int>::const_iterator(CharToInt.end()))//不做const类型转化编译不通过
		return it->second;
}
bool tools::convert(const char* income,u_char* buffer,int len)
{
	const char *ptrin=income;
	u_char *ptrout=buffer;
	int i;
	for(i=0;i<len;i++)
		if(*(ptrin+i)>0)
			*(ptrout+i)=*(ptrin+i);
		else
			break;
	if(i!=len)
	{
		memset(buffer,0,len*sizeof(char));//如果有负数char,则将buffer清零
		return false;
	}
	else
		return true;
}
void tools::ConvertCharStringToPacketFormat(u_char* source,u_char* destination,int len)//将输入的十六进制字符串转化为十六进制，例如"10"转化为一个16写到u_char里
{
	for(int i=0;i<len;i++)
	{
		u_char sum=0;
		u_char Fchar=source[2*i];//这里需要注意'10'两位实际上会变成1位写入destination中
		u_char Schar=source[2*i+1];
		sum+=16*CharToInt[Fchar];
		sum+=CharToInt[Schar];
		destination[i]=sum;
	}
}
void tools::ConvertDecNumStringToHexString(u_char* source,u_char* destion,int len)//将输入的十进制数字字符串转化为十六进制字符串，如16->10
{
	int outcome=0;
	for(int i=0;i<len;i++)
		outcome=(source[i]-'0')+outcome*10;
	*destion=(IntToChar[outcome/16]);
	*(destion+1)=IntToChar[outcome%16];
}
bool tools::ConvertIPV4StringtoHexString(u_char* source,u_char* destination)//将输入的IP转化为十六进制字符串，格式为xxx.xxx.xxx.xxx to xxxxxxxx
{
	int i=0,count=1;
	int dotoffset[5];
	dotoffset[0]=-1;
	int dotcount=0;
	while(source[i]!='\0'){
		if(source[i]=='.')
			dotcount++;
		i++;
	}
	//if(dotcount!=4){
	//	ASSERT("not acceptable ip addr format,need xxx.xxx.xxx.xxx");
	//	return false;
	//}
	while(source[i]!='\0')
	{
		if(source[i]=='.'){
			dotoffset[count]=i;
			count++;
		}
		i++;
	}
	dotoffset[4]=i;
	for(int i=0;i<4;i++){
		ConvertDecNumStringToHexString(source+dotoffset[i]+1,destination+2*i,dotoffset[i+1]-dotoffset[i]-1);//将每两个逗号之间的内容转化
	}
	destination[8]=0;//'\0'结束符
	return true;
}

void tools::u_strcat(u_char* source,u_char* dest)
{
	u_char *sptr=source,*dptr=dest;
	while(*sptr!=0)
		sptr++;
	while(*dptr!=0)
	{
		*sptr=*dptr;
		sptr++;
		dptr++;
	}
	*sptr=0;
}