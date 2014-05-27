#include "stdafx.h"
#include "params.h"
#include "util.h"

float GetPrivateProfileFloat(LPCTSTR lpAppName,
							LPCTSTR lpKeyName,
							float fDefault,
							LPCTSTR lpFileName)
{
	TCHAR szBuf[20] = {0};
	UINT nr = GetPrivateProfileString(lpAppName, lpKeyName,
		_T(""), szBuf, sizeof(szBuf), lpFileName);
	if (nr == 0)
	{
		return fDefault;
	}
#ifdef _UNICODE
	std::string strNum = W2A(szBuf);
#else
	std::string strNum = szBuf;
#endif
	return float(atof(strNum.c_str()));
}

std::string GetPrivateProfileStr(LPCTSTR lpAppName,
							LPCTSTR lpKeyName,
							LPCTSTR lpDefault,
							LPCTSTR lpFileName)
{
	TCHAR szBuf[MAX_PATH] = {0};
	UINT nr = GetPrivateProfileString(lpAppName, lpKeyName,
		lpDefault, szBuf, MAX_PATH, lpFileName);
	if (nr == 0)
	{
		lstrcpy(szBuf, lpDefault);
	}
#ifdef _UNICODE
	std::string str = W2A(szBuf);
#else
	std::stirng str = szBuf;
#endif
	return str;
}

CParams::CParams()
	: imgSize(640, 480)
	, imgCnt(10, 10)
	, fHeight(100.0f)
	, fGridSize(1)
	, nSmooth(4)
	, strPath(".\\")
	, fFOV(40)
	, nAnchors(0)
{
}

void CParams::LoadParams(const char *pConfFile)
{
#ifdef _UNICODE
	std::wstring confFile = A2W(pConfFile);
#else
	std::string confFile = pConfFile;
#endif

	fGridSize = GetPrivateProfileFloat(_T("DEM"),
		_T("GRID_SIZE"), fGridSize, confFile.c_str());

	nAnchors = GetPrivateProfileInt(_T("DEM"),
		_T("ANCHORS"), nAnchors, confFile.c_str());

	nSmooth = GetPrivateProfileInt(_T("DEM"),
		_T("SMOOTH"), nSmooth, confFile.c_str());

	imgSize.width = GetPrivateProfileInt(_T("IMAGE"),
		_T("WIDTH"), imgSize.width, confFile.c_str());

	imgSize.height = GetPrivateProfileInt(_T("IMAGE"),
		_T("HEIGHT"), imgSize.height, confFile.c_str());

	imgCnt.height = GetPrivateProfileInt(_T("IMAGE"),
		_T("ROWS"), imgCnt.height, confFile.c_str());

	imgCnt.width = GetPrivateProfileInt(_T("IMAGE"),
		_T("COLS"), imgCnt.width, confFile.c_str());

	strPath = GetPrivateProfileStr(_T("IMAGE"),
		_T("OUT_PATH"), _T(".\\"), confFile.c_str());
	if (strPath.back() != '\\')
	{
		strPath.push_back('\\');
	}

	fHeight = GetPrivateProfileFloat(_T("CAMERA"),
		_T("HEIGHT"), fHeight, confFile.c_str());

	fFOV = GetPrivateProfileFloat(_T("CAMERA"),
		_T("FOV"), fFOV, confFile.c_str());
}
