#include "mytypes.h"

class CFlighter
{
public:
	HWND m_hWnd;

	CFlighter();

	~CFlighter();

	BOOL Create();

	void Destroy();

	void SetTerrain(const VEC_POINT3F &vertBuf, const VEC_POINT3F &normBuf,
					const VEC_INT &idxBuf, const VEC_INT &ancBuf);

	void SetFlightSchema(const FLIGHTSCHEMA &fs,
						 const char *pOutPath);

	void SetLightPos(const cv::Point3f &pos);

	void Flight();

	LRESULT OnCreate();

private:

	void Resize(int nWidth, int nHeight, float fFOV);

	void SetupAnchors(MAP_ANCHOR &anchorMap, int nImgId);

	void SetModalView(int r, int c);

	void GenerateImage();
	
	bool SaveImage();

	int SaveHomoAnchor(const MAP_ANCHOR &anchorMap);


	double				m_prjMat[16];
	double				m_mvMat[16];
	int					m_vpParams[4];
	int					m_nImgCnt;
	HDC					m_hdc;
	HGLRC				m_hglrc;
	HANDLE				m_hDrawMutex;
	HANDLE				m_hSaveMutex;
	const VEC_POINT3F	*m_pVertBuf;
	const VEC_POINT3F	*m_pNormBuf;
	const VEC_INT		*m_pIdxBuf;
	const VEC_INT		*m_pAncBuf;
	FLIGHTSCHEMA		m_fs;
	std::string			m_strOutPath;
	cv::Mat				m_img;

	friend DWORD CALLBACK SaveImageThread(LPVOID lpParam);
};
