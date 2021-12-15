
#pragma once


#ifdef UNICODE
#ifndef _T
	#define _T(x) L ## x
#endif
#define CC (int)(wchar_t)	// Cast caracter.
#else
#ifndef _T
	#define _T(x) x
#endif
#define CC (int)(char)	// Cast caracter.
#endif

#ifdef UNICODE
#define clenT(x) (int)wcslen(x)
#define fgetsT fgetws
#define fopenT _wfopen_s
#define fstreamT wfstream
#define ifstreamT wifstream
#define itoT _itow_s
#define ofstreamT wofstream
#define ostreamT wostream
#define strcatT wcscat_s
#define stringT wstring	// std::wstring
#define Tostringstream wostringstream
#define Ttoi _wtoi

#else
#define clenT(x) (int)strlen(x)
#define fgetsT fgets
#define fopenT fopen_s
#define fstreamT fstream
#define ifstreamT ifstream
#define itoT _itoa_s
#define ofstreamT ofstream
#define ostreamT ostream
#define strcatT strcat_s
#define stringT string	// std::string
#define Tostringstream ostringstream
#define Ttoi atoi
#endif

// Reverse character-set type.
#ifdef UNICODE
typedef const char* LPCRSTR;
typedef char RCHAR, *LPRSTR;
#define stringR string	// std::string
#else
typedef const wchar_t* LPCRSTR;
typedef wchar_t RCHAR, *LPRSTR;
#define stringR wstring	// std::wstring
#endif


using std::string;
using std::wstring;




char* SwitchStrType(const wchar_t*);

wchar_t* SwitchStrType(const char*);

string SwitchStrType(wstring);

wstring SwitchStrType(string);

bool WriteFileU(HANDLE, LPCVOID, DWORD,	LPDWORD, LPOVERLAPPED, LPCTSTR);

template<class T> void zxc(T);




char* SwitchStrType(const wchar_t* argStr)
{
	unsigned int i_BufSize = sizeof(wchar_t) * (int)wcslen(argStr) + 1;
	char* return_c = new char[i_BufSize];

	WideCharToMultiByte(CP_ACP, 0, argStr, -1, return_c, i_BufSize, nullptr, nullptr);


	return return_c;
}


wchar_t* SwitchStrType(const char* argStr)
{
	unsigned int i_BufSize = sizeof(wchar_t) * (int)strlen(argStr) + 1;
	wchar_t* return_wc = new wchar_t[i_BufSize];

	MultiByteToWideChar(CP_ACP, 0, argStr, -1, return_wc, i_BufSize);


	return return_wc;
}


string SwitchStrType(wstring argStr)
{
	unsigned int i_BufSize = sizeof(wchar_t) * (int)argStr.length() + 1;
	char* c_Buf = new char[i_BufSize];

	WideCharToMultiByte(CP_ACP, 0, argStr.c_str(), -1, c_Buf, i_BufSize, nullptr, nullptr);
	string return_s = c_Buf;


	delete[] c_Buf;

	return return_s;
}


wstring SwitchStrType(string argStr)
{
	unsigned int i_BufSize = sizeof(wchar_t) * (int)argStr.length() + 1;
	wchar_t* wc_Buf = new wchar_t[i_BufSize];

	MultiByteToWideChar(CP_ACP, 0, argStr.c_str(), -1, wc_Buf, i_BufSize);
	wstring return_ws = wc_Buf;


	delete[] wc_Buf;

	return return_ws;
}


// <WriteFile +Unicode Byte Order Mark>
bool WriteFileU(
	HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped,
	LPCTSTR ucName)
{
	// The writing to memory is used little-endian. So BOM is reversed.
	unsigned int
		utf8[2] = { 0xbfbbef, 3 },	// UTF-8 BOM.
		utf16be[2] = { 0xfffe, 2 },	// UTF-16BE BOM.
		utf16le[2] = { 0xfeff, 2 },	// UTF-16LE BOM.
		utf32be[2] = { 0xfffe0000, 4 },	// UTF-32BE BOM.
		utf32le[2] = { 0x0000feff, 4 },	// UTF-32LE BOM.
		utf1[2] = { 0x4c64f7, 3 },	// UTF-1 BOM.
		utf7[2] = { 0x762f2b, 3 },	// UTF-7 BOM. But this is a little wrong.
		utfeb[2] = { 0x736673dd, 4 },	// UTF-EBCDIC BOM.
		scsu[2] = { 0xfffe0e, 3 },	// SCSU BOM.
		bocu1[2] = { 0x39eefb, 3 },	// BOCU-1 BOM.
		gbchn[2] = { 0x33953184, 4 };	// GB-18030 BOM.

	unsigned int* BOM;
	if (ucName == _T("UTF-8")) { BOM = utf8; }
	else if (ucName == _T("UTF-16BE")) { BOM = utf16be; }
	else if (ucName == _T("UTF-16LE")) { BOM = utf16le; }
	else if (ucName == _T("UTF-32BE")) { BOM = utf32be; }
	else if (ucName == _T("UTF-32LE")) { BOM = utf32le; }
	else if (ucName == _T("UTF-1")) { BOM = utf1; }
	else if (ucName == _T("UTF-7")) { BOM = utf7; }
	else if (ucName == _T("UTF-EBCDIC")) { BOM = utfeb; }
	else if (ucName == _T("SCSU")) { BOM = scsu; }
	else if (ucName == _T("BOCU-1")) { BOM = bocu1; }
	else if (ucName == _T("GB-18030")) { BOM = gbchn; }
	else { MessageBox(nullptr, _T("UNICODE name is wrong or non-existent."), _T(""), MB_OK); return false; }

	WriteFile(hFile, &BOM[0], BOM[1], lpNumberOfBytesWritten, lpOverlapped);
	return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}


// <MessageBox User Define>
// How to call : mbu(arg)	// (arg = ANY TYPS.)
template<class T> void zxc(T t_arg)
{
	std::Tostringstream toss;	// define buffer instance.
	toss << t_arg;	// t_arg into buffer.
	std::stringT t_ts = toss.str();	// t_arg into type of string.
	LPTSTR t_ptc = (LPTSTR)t_ts.c_str();	// t_arg to type of LPCSTR.
	MessageBox(nullptr, t_ptc, nullptr, MB_OK);
}
