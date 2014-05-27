#include "stdafx.h"
#include "kdtree.h"

CKDTree::CKDTree(int nNbCnt)
	: m_nNbCnt(nNbCnt)
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
}

void CKDTree::InitSearch(CSearchResult &sr) const
{
	sr.m_Query = cv::Mat(1, 2, CV_32F);
	sr.m_Indices = cv::Mat(m_nNbCnt, 2, CV_32S);
	sr.m_Dists = cv::Mat(m_nNbCnt, 2, CV_32F);
}

int CKDTree::Search(float x, float y, CSearchResult &sr)
{
	*(float*)(sr.m_Query.data + 0) = x;
	*(float*)(sr.m_Query.data + 4) = y;
	m_kdTree.knnSearch(sr.m_Query, sr.m_Indices, sr.m_Dists, m_nNbCnt);
	return m_nNbCnt;
}
