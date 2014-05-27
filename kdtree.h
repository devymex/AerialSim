#include "mytypes.h"

class CKDTree
{
public:
	CKDTree(int nNbCnt);

	void Build(const VEC_POINT3F points);

	void Destroy();

	int Neighbors(float x, float y);

	inline int Index(int nId)
	{
		return ((int*)m_Indices.data)[nId];
	}

	inline float Distance(int nId)
	{
		return ((float*)m_Dists.data)[nId];
	}

private:
	CKDTree& operator = (const CKDTree&)
	{
	}

	const int m_nNbCnt;
	cv::Mat m_Points;
	cv::Mat m_Query;
	cv::Mat m_Indices;
	cv::Mat m_Dists;
	cv::flann::Index m_kdTree;
};