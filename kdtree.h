#include "mytypes.h"

class CSearchResult
{
public:
	inline int Index(int nId)
	{
		return ((int*)m_Indices.data)[nId];
	}

	inline float Distance(int nId)
	{
		return ((float*)m_Dists.data)[nId];
	}
	cv::Mat m_Query;
	cv::Mat m_Indices;
	cv::Mat m_Dists;
};

class CKDTree
{
public:
	CKDTree(int nNbCnt);

	void Build(const VEC_POINT3F points);

	void Destroy();

	void InitSearch(CSearchResult &sr) const;

	int Search(float x, float y, CSearchResult &sr);

private:
	CKDTree& operator = (const CKDTree&)
	{
	}

	const int m_nNbCnt;
	cv::Mat m_Points;
	cv::flann::Index m_kdTree;
};