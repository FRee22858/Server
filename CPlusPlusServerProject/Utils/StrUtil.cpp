#pragma warning(disable:4996)

#include <math.h>
#include "StrUtil.h"
#if defined(WIN32) || defined(__WIN32__)
#include <Windows.h>
#pragma comment(lib,"kernel32.lib")
#endif


string StrUtil::RoundFloat(double fOriginal,int nSaveBits)
{
    static const int DECIMAL_BASE = 10;    
    unsigned int n,scale;
    double fEnd;
    char sEnd[100] = {0};
    string retValue;

    n = 0;
    scale = 1;
    double fOriginal_f = fabs(fOriginal);
    for (int i=0;i<nSaveBits;i++)  
        scale *= DECIMAL_BASE;
    scale *= 2;
    n = (int)(fOriginal_f * scale); 

    n += (n % 2);
    fEnd = ((double)n /scale);
    if(fOriginal < 0)
        sprintf(sEnd,"-%0.2f",fEnd);
    else
        sprintf(sEnd,"%0.2f",fEnd);
    retValue = string(sEnd);
    return retValue;
}

int StrUtil::Str2Int(string Str)
{
    if (!Str.empty())
        return atoi(Str.c_str());
    else return 0;
}

string StrUtil::Int2Str(int integer)
{
    char ret[64] = {0};
    ::sprintf(ret,"%d",integer);
    return ret;
}

long StrUtil::Str2Long(string Str)
{
    if (!Str.empty())
        return atol(Str.c_str());
    else return 0;
}

string StrUtil::Long2Str(long value)
{
    char ret[64] = {0};
    	::sprintf(ret,"%d",value);
    return ret;
}

long StrUtil::CiFang(long baseInt,long numCF)
{  
    if (0 ==numCF) return 1;
    long result = baseInt;
    for ( int i = 2 ; i <= numCF; i ++)
        result = result * baseInt;
    return result;
}

float StrUtil::Str2Float(string str)
{
    if (str.empty() || str.size() == 0) 
		str="0";
    return (float)atof(str.c_str());
}

string StrUtil::Float2Str(float floater)
{
    string ret = RoundFloat((double)floater,2);

    return ret;
}
//std::string StrUtil::Float2Str1( float floater )
//{
//	char result[100];
//	double dOrginaData=(double)floater*10;
//	int iOrginalData=(int)dOrginaData;
//	double subRes=dOrginaData-(double)iOrginalData;
//	int iLeft=(int)(subRes*2.0);
//
//	int iEnd=dOrginaData;
//	iEnd+=iLeft;
//	double dResult=(double)iEnd/10;
//
//	sprintf(result,"%0.1f",dResult);
//
//	string strResult=string(result);
//	return strResult;
//}

int StrUtil::Str2IntForQam(string str)
{
	if(!strcmp(str.c_str(),"QAM16"))
	{
		return 0;
	}
	else if(!strcmp(str.c_str(),"QAM32"))
	{
		return 1;
	}
	else if(!strcmp(str.c_str(),"QAM64"))
	{
		return 2;
	}
	else if(!strcmp(str.c_str(),"QAM128"))
	{
		return 3;
	}
	else if(!strcmp(str.c_str(),"QAM256"))
	{
		return 4;
	}
	else if(!strcmp(str.c_str(),"QAM4"))
	{
		return 5;
	}
	else
	{
		return Str2Int(str);
	}
}

void StrUtil::ConvertUtf8ToGBK( const char* strUtf8,std::string& strGBK )
{
#if defined(WIN32) || defined(__WIN32__)
	WCHAR *strSrc;
	TCHAR *szRes;
	//UTF8转化为Unicode
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
	strSrc = new WCHAR[len+1];
	MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, strSrc, len);

	//Unicode转化为GBK
	len = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[len+1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, len, NULL, NULL);
	strGBK = szRes;
	//改变xml的encoding属性为GB2312
	size_t pos=strGBK.find("encoding=");
	size_t next_pos1=strGBK.find("\"",pos)+1;
	size_t next_pos2=strGBK.find("\"",next_pos1);
	strGBK.erase(next_pos1,next_pos2-next_pos1);
	strGBK.insert(next_pos1,"GB2312");

	delete []strSrc;
	delete []szRes;
#endif
}

void StrUtil::ConvertGBKToUtf8( const char* strGBK,std::string& strUtf8 )
{
#if defined(WIN32) || defined(__WIN32__)
	WCHAR *strSrc;
	TCHAR *szRes;
	//GBK转化为Unicode
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	strSrc = new WCHAR[len+1];
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, strSrc, len);

	//Unicode转化为Utf8
	len = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[len+1];
	WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, len, NULL, NULL);

	strUtf8 = szRes;
	//改变xml的encoding属性为UTF-8
	size_t pos=strUtf8.find("encoding=");
	size_t next_pos1=strUtf8.find("\"",pos)+1;
	size_t next_pos2=strUtf8.find("\"",next_pos1);
	strUtf8.erase(next_pos1,next_pos2-next_pos1);
	strUtf8.insert(next_pos1,"UTF-8");
	
	delete []strSrc;
	delete []szRes;
#endif
}

vector<string> StrUtil::StrSlipt(std::string srcStr, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	srcStr += pattern;//扩展字符串以方便操作
	//int size = srcStr.size();
	size_t size = srcStr.size();

	for (size_t i = 0; i<size; i++)
	{
		pos = srcStr.find(pattern, i);
		if (pos<size)
		{
			std::string s = srcStr.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

string& StrUtil::Replace(std::string &str, std::string oldstr, std::string newstr)
{
	for (string::size_type pos(0); pos != string::npos; pos += newstr.length()) 
	{
		if ((pos = str.find(oldstr, pos)) != string::npos)
		{
			str.replace(pos, oldstr.length(), newstr);
		}
		else
		{
			break;
		}
	}
	return str;
}




