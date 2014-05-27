#include "stdafx.h"
#include "util.h"

NOSKIPMAP g_noSkipMap;

float String2Float(const char *&pStr)
{
	float fVal = 0.0f;
	bool bNeg = false;

	for (; !g_noSkipMap(*pStr); ++pStr);

	if (*pStr == '-')
	{
		bNeg = true;
		++pStr;
	}

	for (; *pStr >= '0' && *pStr <= '9'; ++pStr)
	{
		fVal = fVal * 10 + *pStr - '0';
	}

	if (*pStr == '.')
	{
		++pStr;
		float fDec = 0.1f;
		for (; *pStr >= '0' && *pStr <= '9'; ++pStr, fDec /= 10.0f)
		{
			fVal += (*pStr - '0') * fDec;
		}
	}

	if (bNeg)
	{
		fVal = -fVal;
	}

	return fVal;
}

std::string W2A(LPCWSTR pStrIn)
{
	int nSize = WideCharToMultiByte(936, 0, pStrIn, -1, NULL, 0, NULL, NULL);
	
	LPSTR pOut = new CHAR[nSize];
	WideCharToMultiByte(936, 0, pStrIn, -1, pOut, nSize, NULL, NULL);

	std::string strOut(pOut);
	delete []pOut;
	return strOut;
}

std::wstring A2W(LPCSTR pStrIn)
{
	int nSize = MultiByteToWideChar(936, 0, pStrIn, -1, NULL, 0);

	LPWSTR pOut = new WCHAR[nSize];
	MultiByteToWideChar(936, 0, pStrIn, -1, pOut, nSize);

	std::wstring strOut(pOut);
	delete []pOut;
	return strOut;
}

size_t GetFileSize(std::ifstream &file)
{
	std::fstream::pos_type nCurPos = file.tellg();

	file.seekg(0, std::ios::end);
	size_t nFileSize = (size_t)file.tellg();
	file.seekg(nCurPos);

	return nFileSize;
}

void CalcNormal(const cv::Point3f *pPoints, const int *pIdx, cv::Point3f &out)
{
	cv::Point3f v1 = pPoints[pIdx[1]] - pPoints[pIdx[0]];
	cv::Point3f v2 = pPoints[pIdx[2]] - pPoints[pIdx[0]];

	out = v1.cross(v2);

	float dLen = (float)cv::norm(out);
	out.x /= dLen;
	out.y /= dLen;
	out.z /= dLen;
}

void TriangleNormal(const VEC_POINT3F &dem, const DEMINFO &demInfo,
					VEC_POINT3F &triNorm, std::vector<int> &indices)
{
	triNorm.resize((demInfo.nRows - 1) * (demInfo.nCols - 1) * 2);
	indices.resize(triNorm.size() * 3);
	for (int r = 0; r < demInfo.nRows - 1; ++r)
	{
		for (int c = 0; c < demInfo.nCols - 1; ++c)
		{
			int nLD = r * demInfo.nCols + c;
			int nRD = nLD + 1;
			int nLU = nLD + demInfo.nCols;
			int nRU = nLU + 1;

			int nIdx = (r * (demInfo.nCols - 1) + c) * 2;
			indices.at(nIdx * 3 + 0) = nLD;
			indices.at(nIdx * 3 + 1) = nRD;
			indices.at(nIdx * 3 + 2) = nRU;
			CalcNormal(dem.data(), &indices.at(nIdx * 3), triNorm.at(nIdx));

			++nIdx;
			indices.at(nIdx * 3 + 0) = nLD;
			indices.at(nIdx * 3 + 1) = nRU;
			indices.at(nIdx * 3 + 2) = nLU;
			CalcNormal(dem.data(), &indices.at(nIdx * 3), triNorm.at(nIdx));
		}
	}
}

void GenerateNormals(const VEC_POINT3F &triNorms,
					 const DEMINFO &demInfo,
					 VEC_POINT3F &normal)
{
	float weights[6] = {0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f};

	normal.resize(demInfo.nRows * demInfo.nCols);
	memset(normal.data(), 0, normal.size() * sizeof(normal[0]));

	for (int r = 0; r < demInfo.nRows; ++r)
	{
		for (int c = 0; c < demInfo.nCols; ++c)
		{
			int nLD = ((r - 1) * (demInfo.nCols - 1) + c - 1) * 2;
			int nLU = nLD + (demInfo.nCols - 1) * 2;

			int triIdx[6] = {nLD, nLD + 1, nLD + 2, nLU + 1, nLU + 2, nLU + 3};
			cv::Point3f &avgNorm = normal.at(r * demInfo.nCols + c);
			float fDiv = 0.0f;
			for (int j = 0; j < 6; ++j)
			{
				if (triIdx[j] >= 0 && triIdx[j] < (int)triNorms.size())
				{
					const cv::Point3f &tri = triNorms.at(triIdx[j]);
					avgNorm.x += tri.x * weights[j];
					avgNorm.y += tri.y * weights[j];
					avgNorm.z += tri.z * weights[j];
					fDiv += weights[j];
				}
			}
			avgNorm.x /= fDiv;
			avgNorm.y /= fDiv;
			avgNorm.z /= fDiv;
		}
	}
}

void CalcLightPos(const cv::Point3f &minPt,
				  const cv::Point3f &maxPt,
				  cv::Point3f &lightPos)
{
	lightPos.x	= (minPt.x - maxPt.x) / 2.0f;
	lightPos.y	= (minPt.y - maxPt.y) / 2.0f;
	float fMidX = (maxPt.x + minPt.x) / 2.0f;
	float fMidY = (maxPt.y + minPt.y) / 2.0f;
	float fOffX = fMidX - minPt.x;
	float fOffY = fMidY - minPt.y;
	lightPos.z	= minPt.z + std::sqrt(fOffX * fOffX + fOffY * fOffY);
}
