#include "stdafx.h"
#include "kdtree.h"

CKDTree::CKDTree(int nNbCnt)
	: m_nNbCnt(nNbCnt)
	, m_Query(1, 2, CV_32F)
	, m_Indices(m_nNbCnt, 1, CV_32S)
	, m_Dists(m_nNbCnt, 1, CV_32F)
{
}

void CKDTree::Build(const VEC_POINT3F points)
{
	m_Points = cv::Mat::zeros(points.size(), 2, CV_32F);
	float *pMatBuf = (float*)m_Points.data;
	int nIdx = 0;
	for (VEC_POINT3F_CITER i = points.begin(); i != points.end(); ++i)
	{
		pMatBuf[nIdx++] = i->x;
		pMatBuf[nIdx++] = i->y;
	}
	m_kdTree.build(
		m_Points,
		cv::flann::KDTreeIndexParams(m_nNbCnt),
		cvflann::FLANN_DIST_EUCLIDEAN);
}

void CKDTree::Destroy()
{
	m_kdTree.release();
	m_Query.release();
	m_Indices.release();
	m_Dists.release();
}

int CKDTree::Neighbors(float x, float y)
{
	*(float*)(m_Query.data + 0) = x;
	*(float*)(m_Query.data + 4) = y;
	m_kdTree.knnSearch(m_Query, m_Indices, m_Dists, m_nNbCnt);
	return m_nNbCnt;
}
