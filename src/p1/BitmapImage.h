#pragma once

#include <windows.h>
#include <string>
#include <cstdio>
#include <atlstr.h>

class BitmapImage
{
public:
	FILE* pFile{};

	BYTE* _buffer;

	BITMAPFILEHEADER _fileHeader;
	BITMAPINFOHEADER _infoHeader;

	BYTE* _bgr;
	long lSize;
	long bgrSize;

	BitmapImage();

	int Load(LPWSTR filePath);
	int Read();
	void Write();
	int Save(LPWSTR filePath);

};
