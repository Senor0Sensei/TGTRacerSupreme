#include <stdlib.h>
#include <FEHLCD.h>
#include <fstream>
#include <iostream>

#ifndef FEHIMAGES_H
#define FEHIMAGES_H

class FEHImage
{
	public:
		FEHImage();
		void Open(const char *);
		void Draw(int x, int y, int a);
		void DrawAtlas(int x, int y, int px, int py, int width, int height, int a);
		void DrawLine(int x, int y, int i, int a);
		void Close();
	private:
		int rows,cols;
		int *saved_image;
};

#endif
