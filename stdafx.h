#define _WIN32_WINNT	0x0600 
#define _WIN32_WINDOWS	0x0600
#define _WIN32_IE		0x0700
#define WINVER			0x0600

#define _SECURE_ATL		1
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define VC_EXTRALEAN

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <tchar.h>
#include <windows.h>

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <gl/gl.h>
#include <gl/glu.h>
