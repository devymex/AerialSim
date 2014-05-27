
#pragma once
#include "windows.h"

class CTimer
{
public:
	inline CTimer()
	{
		QueryPerformanceFrequency((PLARGE_INTEGER)&m_nFreq);
		QueryPerformanceCounter((PLARGE_INTEGER)&m_nStart);
	}

	inline double Cur()
	{
		__int64 nCur;
		QueryPerformanceCounter((PLARGE_INTEGER)&nCur);
		return double(nCur - m_nStart) / double(m_nFreq);
	}

	inline double Reset()
	{
		__int64 nCur;
		double dCur;

		QueryPerformanceCounter((PLARGE_INTEGER)&nCur);
		dCur = double(nCur - m_nStart) / double(m_nFreq);
		m_nStart = nCur;

		return dCur;
	}
private:
	__int64 m_nFreq;

	__int64 m_nStart;
};
