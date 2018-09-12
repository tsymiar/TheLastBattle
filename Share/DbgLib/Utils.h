/*******************************************************************\
| Win32Utils.h
|--------------------------------------------------------------------
| CREATED:		2007/7/23
| AUTHOR:		Martin Fleisz
\********************************************************************/
#ifndef DBGLIB_UTILS_H
#define DBGLIB_UTILS_H

namespace DbgLib
{

namespace Internal
{

//*****************************************************************************
// class overview:
//   function object to convert an unicode string to a std::string using std::transform
//-----------------------------------------------------------------------------
struct WideToChar
{
	char operator()(wchar_t p_Char)
	{
		// Narrow with the global default locale
		return std::use_facet<std::ctype<wchar_t> >(std::locale()).narrow(p_Char, _AT('?'));
	}
};

//*****************************************************************************
// class overview:
//   function object to convert a std::string to an unicode string using std::transform
//-----------------------------------------------------------------------------
struct CharToWide
{
	wchar_t operator()(char p_Char)
	{
		// Widen with the global default locale
		return std::use_facet<std::ctype<wchar_t> >(std::locale()).widen(p_Char);
	}
};

//////////////////////////////////////////////////////////////////////////
// Function to compare strings containing wild characters
bool WildMatch(const tchar* wild, const tchar* string);

//////////////////////////////////////////////////////////////////////////
// hex output operator for tostreams
struct Hex
{	
	Hex(ux p_Size) : m_Size(p_Size) { }
	ux m_Size;
};

tostream &operator<<(tostream& p_Str, const Hex& p_Hex);

//*****************************************************************************
// class overview:
//   Helper class to extracts tokens from a string
//-----------------------------------------------------------------------------
class CTokenizer
{
	public:
		CTokenizer(const tstring& p_Str, const tstring& p_Delimiters);

	public:

		// returns the number of extracted tokens
		ux GetNumTokens() const;

		// returns the i'th token
		const tstring GetToken(ux p_Index) const;

	private:
		// vector containing our tokens
		std::vector<tstring> m_Tokens;
};

#if defined(PLATFORM_WINDOWS)
//*****************************************************************************
// class overview:
//   Offers various utility functions available on Win32 platforms.
//-----------------------------------------------------------------------------
class CWin32Utils
{
	public:
		// get directory of the current application (.exe)
		static tstring GetApplicationDirectory();

		// get the working directory of the current application
		static tstring GetWorkingDirectory();

		// get system root directory (windows directory)
		static tstring GetSystemRootDirectory();

		// get system directory
		static tstring GetSystemDirectory();
};

#endif // PLATFORM_WINDOWS

} // namespace Internal

} // namespace DbgLib

#endif // DBGLIB_UTILS_H
