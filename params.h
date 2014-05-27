#pragma once

class CParams
{
public:
	float		fGridSize;
	float		fHeight;
	float		fFOV;
	int			nSmooth;
	int			nAnchors;
	cv::Size	imgSize;
	cv::Size	imgCnt;
	std::string	strPath;

	CParams();
	void LoadParams(const char *pConfFile);
};
