#pragma once

struct DEMINFO
{
	float	fBegX;
	float	fBegY;
	float	fStep;
	int		nRows;
	int		nCols;
};

struct FLIGHTSCHEMA
{
	int			nDir;
	float		fBegX;
	float		fBegY;
	float		fEndX;
	float		fEndY;
	float		fHeight;
	float		fBase;
	float		fFOV;
	cv::Size	imgCnt;
	cv::Size	imgSize;
};

typedef std::vector<cv::Point3f>		VEC_POINT3F;
typedef VEC_POINT3F::iterator			VEC_POINT3F_ITER;
typedef VEC_POINT3F::const_iterator		VEC_POINT3F_CITER;

typedef std::vector<int>				VEC_INT;
typedef VEC_INT::iterator				VEC_INT_ITER;
typedef VEC_INT::const_iterator			VEC_INT_CITER;

typedef std::vector<cv::Point3i>		VEC_POINT3I;
typedef VEC_POINT3I::iterator			VEC_POINT3I_ITER;
typedef VEC_POINT3I::const_iterator		VEC_POINT3I_CITER;

typedef std::map<int, VEC_POINT3I>		MAP_ANCHOR;
typedef MAP_ANCHOR::iterator			MAP_ANCHOR_ITER;
typedef MAP_ANCHOR::const_iterator		MAP_ANCHOR_CITER;
