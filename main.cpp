#include "stdafx.h"
#include "ctimer.h"
#include "mytypes.h"
#include "kdtree.h"
#include "flighter.h"
#include "params.h"
#include "util.h"

bool LoadXYZFile(const char *pFileName, std::vector<cv::Point3f> &points)
{
	std::cout << "Loading input file \"";
	std::cout << pFileName << '\"' << std::endl;
	std::ifstream xyzFile;
	xyzFile.open(pFileName, std::ios::binary);
	if (!xyzFile.is_open())
	{
		std::cout << "Can't open the input file!" << std::endl;
		return false;
	}

	size_t nFileSize = GetFileSize(xyzFile);
	std::vector<char> fileBuf(nFileSize + 2, 0);
	xyzFile.read(fileBuf.data(), nFileSize);

	points.reserve(nFileSize / 30);
	for (const char *pStr = fileBuf.data(); *pStr != 0;)
	{
		cv::Point3f pt;
		pt.x = String2Float(pStr);
		pt.y = String2Float(pStr);
		pt.z = String2Float(pStr);
		points.push_back(pt);
		for (; !g_noSkipMap(*pStr); ++pStr);
	}
	std::cout << "Load " << points.size() << " points." <<std::endl;
	return true;
}

void AnalyzeData(const VEC_POINT3F &points, cv::Point3f &minPt,
				 cv::Point3f &maxPt, cv::Point3f &avgPt)
{
	std::cout << "Analyzing data..." << std::endl;
	if (!points.empty())
	{
		minPt = points[0];
		maxPt = points[0];
		cv::Point3d avg(0.0, 0.0, 0.0);
		for (VEC_POINT3F_CITER i = points.begin() + 1; i != points.end(); ++i)
		{
			if		(i->x < minPt.x)	minPt.x = i->x;
			else if (i->x > maxPt.x)	maxPt.x = i->x;

			if		(i->y < minPt.y)	minPt.y = i->y;
			else if (i->y > maxPt.y)	maxPt.y = i->y;

			if		(i->z < minPt.z)	minPt.z = i->z;
			else if (i->z > maxPt.z)	maxPt.z = i->z;

			avg.x += i->x;
			avg.y += i->y;
			avg.z += i->z;
		}
		avgPt.x = float(avg.x / points.size());
		avgPt.y = float(avg.y / points.size());
		avgPt.z = float(avg.z / points.size());
	}
	std::cout << "Minimum coordinates: (" << minPt.x << ", "
		<< minPt.y << ", " << minPt.z << ");" << std::endl;
	std::cout << "Maximum coordinates: (" << maxPt.x << ", "
		<< maxPt.y << ", " << maxPt.z << ");" << std::endl;
	std::cout << "Average coordinates: (" << avgPt.x << ", "
		<< avgPt.y << ", " << avgPt.z << ")." << std::endl;
}

void GenerateDEM(const VEC_POINT3F &points, CKDTree &kdTree,
				 const DEMINFO &demInfo, VEC_POINT3F &dem)
{
	std::cout << "Generating DEM (";
	std::cout << demInfo.nRows << " x " << demInfo.nCols << ")..." << std::endl;
	dem.resize(demInfo.nRows * demInfo.nCols);
	memset(dem.data(), 0, dem.size() * sizeof(dem[0]));
	for (int r = 0; r < demInfo.nRows; ++r)
	{
		for (int c = 0; c < demInfo.nCols; ++c)
		{
			cv::Point3f &curPt = dem.at(r * demInfo.nCols + c);
			curPt.x = c * demInfo.fStep + demInfo.fBegX;
			curPt.y = r * demInfo.fStep + demInfo.fBegY;
			int nNbCnt = kdTree.Neighbors(curPt.x, curPt.y);
			float fSumDist = 0.0f;
			for (int j = 0; j < nNbCnt; ++j)
			{
				curPt.z += points.at(kdTree.Index(j)).z / kdTree.Distance(j);
				fSumDist += (1.0f / kdTree.Distance(j));
			}
			curPt.z /= fSumDist;
		}
	}
	std::cout << "DEM generated, with " << demInfo.nRows * demInfo.nCols;
	std::cout << " elements" << std::endl;
}

void GenerateNormalIndex(const VEC_POINT3F &dem,
						 const DEMINFO &demInfo,
						 VEC_POINT3F &normal,
						 std::vector<int> &indices)
{
	std::cout << "Generating nomal vector and indices..." << std::endl;
	VEC_POINT3F triNorm;
	TriangleNormal(dem, demInfo, triNorm, indices);
	GenerateNormals(triNorm, demInfo, normal);
	std::cout << "Completed with " << triNorm.size() << " triangles ";
	std::cout << "and " << indices.size() << " indices." << std::endl;
}

int main(int nArgCnt, const char **ppArgs)
{
	//Variables and Initialization
	VEC_POINT3F		points;
	VEC_POINT3F		dem;
	VEC_POINT3F		normals;
	VEC_INT			indices;
	VEC_INT			anchorIdx;
	cv::Point3f		minPt, maxPt, avgPt;
	cv::Point3f		lightPos;
	FLIGHTSCHEMA	fs;
	DEMINFO			demInfo;
	CTimer			timer;
	double			dSec;
	CParams			params;
	CFlighter		mainWnd;


	//Loading parameters
	if (nArgCnt < 2)
	{
		std::cout << "At least 1 parameters." << std::endl;
		return -1;
	}
	const char *pInputFile = ppArgs[1];
	const char *pConfFile = ".\\config.ini";
	if (nArgCnt > 2)
	{
		pConfFile = ppArgs[2];
	}
	params.LoadParams(pConfFile);
	demInfo.fStep	= params.fGridSize;
	fs.imgSize		= params.imgSize;
	fs.imgCnt		= params.imgCnt;
	fs.fHeight		= params.fHeight;
	fs.fFOV			= params.fFOV;


	//Loading input file
	timer.Reset();
	if (!LoadXYZFile(pInputFile, points))
	{
		return -1;
	}
	dSec = timer.Cur();
	std::cout << "Used " << dSec << " sec." << std::endl << std::endl;
	if ((int)points.size() <= params.nSmooth)
	{
		std::cout << "Too few points, program exited." <<std::endl;
		return -1;
	}


	//Analyzing data
	timer.Reset();
	AnalyzeData(points, minPt, maxPt, avgPt);
	CalcLightPos(minPt, maxPt, lightPos);
	fs.fBase	= minPt.z;
	fs.fBegX	= minPt.x;
	fs.fBegY	= minPt.y;
	fs.fEndX	= maxPt.x;
	fs.fEndY	= maxPt.y;
	dSec = timer.Cur();
	std::cout << "Used " << dSec << " sec." << std::endl << std::endl;


	//Building Index
	timer.Reset();
	std::cout << "Building Search index..." << std::endl;
	CKDTree kdTree(params.nSmooth);
	kdTree.Build(points);
	dSec = timer.Cur();
	std::cout << "Search index build." << std::endl;
	std::cout << "Used " << dSec << " sec." << std::endl << std::endl;


	//Generating DEM
	timer.Reset();
	demInfo.fBegX = minPt.x;
	demInfo.fBegY = minPt.y;
	demInfo.nRows = int(std::ceil((maxPt.y - minPt.y) / demInfo.fStep));
	demInfo.nCols = int(std::ceil((maxPt.x - minPt.x) / demInfo.fStep));
	GenerateDEM(points, kdTree, demInfo, dem);
	kdTree.Destroy();
	points.clear();
	points.shrink_to_fit();
	dSec = timer.Cur();
	std::cout << "Used " << dSec << " sec." << std::endl << std::endl;


	//Generating normal vector and Indices
	timer.Reset();
	GenerateNormalIndex(dem, demInfo, normals, indices);
	anchorIdx.resize(dem.size());
	for (int i = 0; i < (int)anchorIdx.size(); ++i)
	{
		anchorIdx.at(i) = i;
	}
	std::random_shuffle(anchorIdx.begin(), anchorIdx.end());
	anchorIdx.erase(anchorIdx.begin() + params.nAnchors, anchorIdx.end());
	std::sort(anchorIdx.begin(), anchorIdx.end());
#ifdef _DEBUG
	if (anchorIdx.front() != 0)
	{
		anchorIdx.insert(anchorIdx.begin(), 0);
	}
#endif
	dSec = timer.Cur();
	std::cout << "Used " << dSec << " sec." << std::endl << std::endl;


	//Flight scheme
	timer.Reset();
	mainWnd.Create();
	mainWnd.SetTerrain(dem, normals, indices, anchorIdx);
	mainWnd.SetFlightSchema(fs);
	mainWnd.SetLightPos(lightPos);
	mainWnd.Flight(params.strPath.c_str());
	mainWnd.Destroy();
	dSec = timer.Cur();
	std::cout << "Used " << dSec << " sec." << std::endl << std::endl;


	system("pause");
	return 0;
}
