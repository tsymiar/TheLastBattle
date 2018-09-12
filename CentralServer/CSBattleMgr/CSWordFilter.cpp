#include "stdafx.h"

#include "CSWordFilter.h"
#ifndef _WINDOWS_
#include "iconv.h"
#endif



void CChineseCode::UTF_8ToUnicode(wchar_t* pOut,char *pText){
		char* uchar = (char *)pOut;
		uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
		uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
		return;
}

void CChineseCode::UnicodeToUTF_8(char* pOut,wchar_t* pText){
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;
	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
	return;
}

void CChineseCode::UnicodeToGB2312(char* pOut, wchar_t uData){
#ifdef _WINDOWS_
	WideCharToMultiByte(CP_ACP,NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL);
#else
	//iconv_t cd = iconv_open("gb2312", "unicode");
	//size_t pLen = 1;
	//size_t pOutLen = 0;
	//iconv(cd, &uData, &pLen, ,pOut, &pOutLen);
	//iconv_close(cd);
#endif
	return;
}     

void CChineseCode::Gb2312ToUnicode(wchar_t* pOut, char *gbBuffer){
#ifdef _WINDOWS_
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
#else
	//iconv_t cd = iconv_open("unicode","gb2312");
	//size_t pLen = 2;
	//size_t pOutLen = 1;
	//iconv(cd, &gbBuffer, &pLen, pOut, &pOutLen);
	//iconv_close(cd);
#endif
	return ;
}

void CChineseCode::GB2312ToUTF_8(string& pOut,char *pText, int pLen){
	char buf[4];
	int nLength = pLen * 3;
	char* rst = new char[nLength];

	memset(buf,0,4);
	memset(rst,0,nLength);

	int i = 0;
	int j = 0;      
	while(i < pLen){
		if( *(pText + i) >= 0){
			rst[j++] = pText[i++]; 
		}else{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer, pText+i);
			UnicodeToUTF_8(buf, &pbuffer);
			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];    
			j += 3;
			i += 2;
		}
	}

	rst[j] = '/0';
	pOut = rst;             
	delete []rst;     
	return;
}

void CChineseCode::UTF_8ToGB2312(string &pOut, char *pText, int pLen){
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);
	int i =0;
	int j = 0;

	while(i < pLen){

		if(pText > 0){
			newBuf[j++] = pText[i++];                       
		}else{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);
			UnicodeToGB2312(Ctemp, Wtemp);
			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}

	newBuf[j] = '/0';
	pOut = newBuf;
	delete []newBuf;
	return; 
}
