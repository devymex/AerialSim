#include "stdafx.h"
#include "flighter.h"
#include "util.h"

CFlighter *g_pWnd = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_pWnd != NULL && message == WM_CREATE && g_pWnd->m_hWnd == NULL)
	{
		g_pWnd->m_hWnd = hWnd;
		return g_pWnd->OnCreate();
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

CFlighter::CFlighter()
	: m_hWnd(NULL)
	, m_hglrc(NULL)
	, m_hdc(NULL)
	, m_pVertBuf(NULL)
	, m_pNormBuf(NULL)
	, m_pIdxBuf(NULL)
	, m_pAncBuf(NULL)
{
	g_pWnd = this;
	memset(&m_fs, 0, sizeof(m_fs));
}

CFlighter::~CFlighter()
{
	Destroy();
}

BOOL CFlighter::Create()
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("AERIALSIM");
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex))
	{
		return FALSE;
	}

	m_hWnd = CreateWindow(
		wcex.lpszClassName,
		wcex.lpszClassName,
		0,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL,
		NULL,
		wcex.hInstance,
		NULL);
	if (m_hWnd == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

void CFlighter::Destroy()
{
	if (m_hglrc != NULL)
	{
		wglDeleteContext(m_hglrc);
		m_hglrc = NULL;
	}
	if (m_hdc != NULL)
	{
		ReleaseDC(m_hWnd, m_hdc);
		m_hdc = NULL;
	}
	if (m_hWnd != NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
	m_pVertBuf = NULL;
	m_pNormBuf = NULL;
	m_pIdxBuf = NULL;
	m_pAncBuf = NULL;
	memset(&m_fs, 0, sizeof(m_fs));
}

void CFlighter::SetTerrain(const VEC_POINT3F &vertBuf,
						   const VEC_POINT3F &normBuf,
						   const VEC_INT &idxBuf,
						   const VEC_INT &ancBuf)
{
	m_pIdxBuf = &idxBuf;
	m_pAncBuf = &ancBuf;
	m_pVertBuf = &vertBuf;
	m_pNormBuf = &normBuf;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, m_pVertBuf->data());
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, m_pNormBuf->data());
}

void CFlighter::SetFlightSchema(const FLIGHTSCHEMA &fs)
{
	m_fs = fs;
	Resize(m_fs.imgSize.width, m_fs.imgSize.height, m_fs.fFOV);
}

void CFlighter::Flight(const char *pOutPath)
{
	std::cout << "Begin flight..." << std::endl;

	cv::Mat image;
	MAP_ANCHOR anchorMap;
	int nImgId = 0;

	for (int c = 0; c < m_fs.imgCnt.width; ++c)
	{
		for (int r = 0; r < m_fs.imgCnt.height; ++r)
		{
			SetModalView(r, c);
			GenerateImage(image);
			SaveImage(image, nImgId, pOutPath);
			SetupAnchors(anchorMap, nImgId++);
		}
	}

	int nCnt = SaveHomoAnchor(pOutPath, anchorMap);
	std::cout << "Flight finished. Captured ";
	std::cout << nImgId << " pictures." << std::endl;
	std::cout << "Set up " << nCnt << " anchors and ";
	std::cout << anchorMap.size() << " homonymies" << std::endl;
}

void CFlighter::SetLightPos(const cv::Point3f &pos)
{
	glLightfv(GL_LIGHT0, GL_POSITION, (const float*)&pos);
}

LRESULT CFlighter::OnCreate()
{
	m_hdc = GetDC(m_hWnd);
	if (m_hdc == NULL)
	{
		return -1;
	}

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;

	int nPixFmtIdx = ChoosePixelFormat(m_hdc, &pfd);
	if (!SetPixelFormat(m_hdc, nPixFmtIdx, &pfd))
	{
		return -1;
	}

	m_hglrc = wglCreateContext(m_hdc);
	if (m_hglrc == NULL)
	{
		return -1;
	}
	if (!wglMakeCurrent(m_hdc, m_hglrc))
	{
		return -1;
	}

	float LightColor[] = {0.3f, 0.3f, 0.3f, 1.0f};
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightColor);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.5f, 0.5f, 0.0f, 0.0f);

	return 0;
}

void CFlighter::SetupAnchors(MAP_ANCHOR &anchorMap, int nImgId)
{
	for (VEC_INT_CITER i = m_pAncBuf->cbegin();
		i != m_pAncBuf->cend(); ++i)
	{
		const cv::Point3f &vert = m_pVertBuf->at(*i);
		double x, y, z;
		gluProject(vert.x, vert.y, vert.z,
			m_mvMat, m_prjMat, m_vpParams, &x, &y, &z);
		if (x >= 0 && x <= m_fs.imgSize.width &&
			y >= 0 && y <= m_fs.imgSize.height)
		{
			cv::Point3i imgPt(nImgId, int(x + 0.5f), int(y + 0.5f));
			anchorMap[*i].push_back(imgPt);
		}
	}
}

void CFlighter::SetModalView(int r, int c)
{
	float fStepX = (m_fs.fEndX - m_fs.fBegX) / (m_fs.imgCnt.width - 1);
	float fStepY = (m_fs.fEndY - m_fs.fBegY) / (m_fs.imgCnt.height - 1);
	cv::Point3f eyePos(0, 0, m_fs.fHeight + m_fs.fBase);
	cv::Point3f center(0, 0, m_fs.fBase);
	cv::Point3f up(0.0f, 1.0f, 0.0f);
	eyePos.x = (c * fStepX) + m_fs.fBegX;
	if (c % 2 == 0)
	{
		eyePos.y = (r * fStepY) + m_fs.fBegY;
	}
	else
	{
		eyePos.y = ((m_fs.imgCnt.height - r -1) * fStepY) + m_fs.fBegY;
		up.y = -1.0f;
	}
	center.x = eyePos.x;
	center.y = eyePos.y;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyePos.x, eyePos.y, eyePos.z,
		center.x, center.y, center.z,
		up.x, up.y, up.z);

	glGetDoublev(GL_MODELVIEW_MATRIX, m_mvMat);
}

void CFlighter::Resize(int nWidth, int nHeight, float fFOV)
{
	SetWindowPos(m_hWnd, NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int nDiffX = nWidth - (rcClient.right - rcClient.left);
	int nDiffY = nHeight - (rcClient.bottom - rcClient.top);

	SetWindowPos(m_hWnd, NULL, 0, 0,
		nWidth + nDiffX, nHeight + nDiffY, SWP_NOZORDER);

	glViewport(0, 0, nWidth, nHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fFOV, (double)nWidth / (double)nHeight, 0.01, 1000.0);

	glGetDoublev(GL_PROJECTION_MATRIX, m_prjMat);
	glGetIntegerv(GL_VIEWPORT, m_vpParams);
}

void CFlighter::GenerateImage(cv::Mat &img)
{
	cv::Size imgSize(m_fs.imgSize.width, m_fs.imgSize.height);
	if (img.size() != imgSize || img.type() != CV_8UC4)
	{
		img = cv::Mat::zeros(imgSize, CV_8UC4);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, m_pIdxBuf->size(),
		GL_UNSIGNED_INT, m_pIdxBuf->data());

	glReadPixels(0, 0, m_fs.imgSize.width, m_fs.imgSize.height,
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.data);

	cv::flip(img, img, 0);
}

void CFlighter::SaveImage(const cv::Mat &img, int nId, const char *pPath)
{
	std::string strFile = std::string(pPath);
	strFile += Int2Str(nId) + std::string(".bmp");
	cv::imwrite(strFile, img);
}

int CFlighter::SaveHomoAnchor(const char *pOutPath,
							   const MAP_ANCHOR &anchorMap)
{
	std::string strFile;

	strFile = std::string(pOutPath) + std::string("anchors");
	std::ofstream ancFile(strFile, std::ios::binary);

	strFile = std::string(pOutPath) + std::string("homos");
	std::ofstream homFile(strFile, std::ios::binary);

	int nCnt = 0;
	for (MAP_ANCHOR_CITER i = anchorMap.begin(); i != anchorMap.end(); ++i)
	{
		const VEC_POINT3I &anchors = i->second;
		homFile.write((char*)&nCnt, sizeof(nCnt));
		for (VEC_POINT3I_CITER j = anchors.cbegin(); j != anchors.cend(); ++j)
		{
			ancFile.write((char*)&*j, sizeof(*j));
			homFile.write((char*)&nCnt, sizeof(nCnt));
			++nCnt;
		}
	}
	return nCnt;
}
