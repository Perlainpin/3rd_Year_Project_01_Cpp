#include "pch.h"

#include "BitmapImage.h"
using namespace std;

BitmapImage::BitmapImage() {

};

int BitmapImage::Load(LPWSTR WfilePath) {

	char filePath[500];
	size_t i;
	wcstombs_s(&i, filePath, WfilePath, 500);

	fopen_s(&pFile, filePath, "r+b"); // Ouvrir le fichier BMP en mode binaire
	if (pFile == NULL) {
		return 1;
	}
	return 0;
};

int BitmapImage::Read() {

	size_t result;

	// Taille du fichier
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// Allocation de la mémoire pour lire le fichier
	_buffer = new BYTE[lSize];
	if (_buffer == NULL) {
		fclose(pFile);
		return 1;
	}

	// Lire le fichier dans le buffer
	result = fread(_buffer, 1, lSize, pFile);
	if (result != lSize) {
		fclose(pFile);
		delete[] _buffer;
		return 2;
	}



	// Appliquer les infos du buffer dans les headers
	memcpy(&_fileHeader, _buffer, sizeof(BITMAPFILEHEADER));
	memcpy(&_infoHeader, _buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	// Récupérer les données des pixels (BGR)
	_bgr = _buffer + _fileHeader.bfOffBits;
	bgrSize = lSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
	fclose(pFile);
	return 0;
};

void BitmapImage::Write() {

	fwrite(&_fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);
	fwrite(&_infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

	fwrite(_bgr, 1, _infoHeader.biSizeImage, pFile);
};

int BitmapImage::Save(LPWSTR WfilePath) {

	wstring ws(WfilePath);
	string filePath = string(ws.begin(), ws.end());

	FILE* pNewFile;

	string fileName = filePath.substr(0, filePath.size() - 4);

	string modifiedFilename = fileName + ".bmp";
	const char* newFilename = modifiedFilename.c_str();

	fopen_s(&pNewFile, newFilename, "wb");
	if (pNewFile == NULL)
	{
		return 1;
		fclose(pNewFile);
	}

	Write();

	fclose(pNewFile);
	return 0;
};