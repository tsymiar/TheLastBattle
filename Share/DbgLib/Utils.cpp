#include "stdafx.h"
#include "Utils.h"

namespace DbgLib
{

namespace Internal
{

//////////////////////////////////////////////////////////////////////////
// code is from http://www.codeproject.com/string/wildcmp.asp
bool WildMatch(const tchar* wild, const tchar* string)
{
	// Written by Jack Handy - jakkhandy@hotmail.com
	const tchar *cp = NULL, *mp = NULL;

	while((*string) && (*wild != _T('*')))
	{
		if((*wild != *string) && (*wild != _T('?')))
			return 0;
		wild++;
		string++;
	}

	while(*string)
	{
		if(*wild == _T('*'))
		{
			if(!*++wild)
				return 1;
			mp = wild;
			cp = string + 1;
		}
		else if((*wild == *string) || (*wild == _T('?')))
		{
			wild++;
			string++;
		}
		else
		{
			wild = mp;
			string = cp++;
		}
	}

	while(*wild == _T('*'))
		wild++;

	return !(*wild);
}


//////////////////////////////////////////////////////////////////////////
tostream &operator<<(tostream &p_Str, const Hex &p_Hex)
{
	p_Str.unsetf(tios::dec);
	p_Str.setf(tios::hex | tios::uppercase);
	p_Str << std::setfill(_T('0')) << std::setw(p_Hex.m_Size);
	return p_Str;
}

//////////////////////////////////////////////////////////////////////////
CTokenizer::CTokenizer(const tstring& p_Str, const tstring& p_Delimiters)
{
	const tstring::size_type len = p_Str.length();
	tstring::size_type i = 0;

	while(i < len)
	{
		// eat leading whitespace
		i = p_Str.find_first_not_of(p_Delimiters, i);
		if(i == tstring::npos)
			return;   // nothing left but white space

		// find the end of the token
		tstring::size_type j = p_Str.find_first_of(p_Delimiters, i);

		// push token
		if(j == tstring::npos) 
		{
			m_Tokens.push_back(p_Str.substr(i));
			return;
		} else
			m_Tokens.push_back(p_Str.substr(i, j - i));

		// set up for next loop
		i = j + 1;
	}
}

//////////////////////////////////////////////////////////////////////////
ux CTokenizer::GetNumTokens() const
{
	return static_cast<ux>(m_Tokens.size());
}

//////////////////////////////////////////////////////////////////////////
const tstring CTokenizer::GetToken(ux p_Index) const
{
	return m_Tokens[p_Index];
}

#if defined(PLATFORM_WINDOWS)
//////////////////////////////////////////////////////////////////////////
tstring CWin32Utils::GetApplicationDirectory()
{
	TCHAR moduleName[MAX_PATH];
	if(GetModuleFileName(NULL, moduleName, MAX_PATH))
		return moduleName;
	return _T("unknown");
}

//////////////////////////////////////////////////////////////////////////
tstring CWin32Utils::GetWorkingDirectory()
{
	TCHAR dir[MAX_PATH];
	if(GetCurrentDirectory(MAX_PATH, dir))
		return dir;
	return(_T("unknown"));
}

//////////////////////////////////////////////////////////////////////////
tstring CWin32Utils::GetSystemRootDirectory()
{
	TCHAR dir[MAX_PATH];
	if(GetSystemWindowsDirectory(dir, MAX_PATH))
		return dir;
	return _T("unknown");
}

//////////////////////////////////////////////////////////////////////////
tstring CWin32Utils::GetSystemDirectory()
{
	TCHAR dir[MAX_PATH];
	if(::GetSystemDirectory(dir, MAX_PATH))
		return dir;
	return _T("unknown");
}

#endif // PLATFORM_WINDOWS

} // namespace Internal

} // namespace DbgLib
