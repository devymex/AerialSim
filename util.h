#include "mytypes.h"

struct NOSKIPMAP
{
	NOSKIPMAP() : charMap(256, 0)
	{
		charMap[0] = 1;
		charMap['-'] = 1;
		charMap['.'] = 1;
		for (int i = 0; i < 10; ++i)
		{
			charMap[i + '0'] = 1;
		}
	}
	bool operator()(char c)
	{
		return charMap[c] != 0;
	}
	std::vector<char> charMap;
};

float String2Float(const char *&pStr);

std::string W2A(LPCWSTR pStrIn);

std::wstring A2W(LPCSTR pStrIn);

size_t GetFileSize(std::ifstream &file);

void CalcNormal(const cv::Point3f *pPoints, const int *pIdx, cv::Point3f &out);

void TriangleNormal(const VEC_POINT3F &dem, const DEMINFO &demInfo,
					VEC_POINT3F &triNorm, std::vector<int> &indices);

void GenerateNormals(const VEC_POINT3F &triNorms, const DEMINFO &demInfo,
					 VEC_POINT3F &normal);

void CalcLightPos(const cv::Point3f &minPt,
				  const cv::Point3f &maxPt,
				  cv::Point3f &lightPos);

extern NOSKIPMAP g_noSkipMap;
