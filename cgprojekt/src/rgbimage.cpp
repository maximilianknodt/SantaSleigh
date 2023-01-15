#include "rgbimage.h"
#include "color.h"
#include "assert.h"
#include "math.h"
#include "string.h"
#include <iostream>
#include "Matrix.h"
#include <cmath>

RGBImage::RGBImage(unsigned int Width, unsigned int Height) : m_Image(new Color[Width * Height]), m_Height(Height), m_Width(Width) {}

RGBImage::~RGBImage() {
	delete[] this->m_Image;
}

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c) {
	// Ueberpruefung, ob x und y Werte innerhalb der Matrix liegen
	if(x < 0 || x > this->width()-1 || y < 0 || y > this->height()-1) return;

	this->m_Image[y * width() + x] = c;
}

const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const {
	if(x < 0 || x > this->width()-1 || y < 0 || y > this->height()-1) throw ("out of bounds");
	return this->m_Image[y * width() + x];
}

unsigned int RGBImage::width() const {
	return this->m_Width;
}

unsigned int RGBImage::height() const {
	return this->m_Height;
}

unsigned char RGBImage::convertColorChannel(float v) {
	if(v < 0.0f) v = 0.0f;
	if(v > 1.0f) v = 1.0f;
	return 255 * v;
}

// P5 Aufgabe 2
RGBImage& RGBImage::SoebelFilter(RGBImage& dst, const RGBImage& src, float factor) {
    
	// Vergleich der Groessen, wenn false -> Fehlermeldung und Abbruch des Programms
    assert(dst.width() == src.width());
    assert(dst.height() == src.height());
    
	// Matrix k wird auf jeden Pixel, der einen Pixel vom Rand entfernt ist, gelegt
    int k [3][3] = {
		{1, 0, -1},
		{2, 0, -2},
		{1, 0, -1}
	} ;

    for(int y = 1; y < src.height() - 1; y++) {
        for(int x = 1; x < src.width() - 1; x++) {
			// Wert bei vertikaler harter Kantenerkennung
			float u = 0.0f;
			// Wert bei horizontaler harter Kantenerkennung (deshalb Matrix transponieren)
			float v = 0.0f;

            for(int j = 0; j <= 2; j++) {
                for(int i = 0; i <= 2; i++) {
                    Color c = src.getPixelColor(x + i - 1, y + j - 1);
                    u += ((c.R + c.G + c.B) / 3)  * k[i][j] * factor;
                    v += ((c.R + c.G + c.B) / 3)  * k[j][i] * factor;
                }
            }
			// Wurzel aus der Summe der quadrierten Werte, um das Vorzeichen "los zu werden"
			// und maximale Werte zu verdeutlichen
            float s = std::sqrt(u * u + v * v);
            dst.setPixelColor(x, y, Color(s, s, s));
        }
    }
    
    return dst;
}

bool RGBImage::saveToDisk(const char* Filename) {
	FILE* bmpFile = fopen(Filename, "wb");
	int paddedRowSize = 4 * ceil(3 * this->width() / 4);
	int imageSize = paddedRowSize * this->height();
	int fileSize = 14 + 40 + imageSize;

	unsigned char* image = (unsigned char*) malloc(imageSize);
	memset(image, '0', imageSize);

	for(int i = 0; i < this->height(); i++){
		for(int j = 0; j < this->width(); j++){
			Color c = getPixelColor(j, this->height() - i - 1);
			int index = i * paddedRowSize + j * 3;
			image[index + 0] = convertColorChannel(c.B);
			image[index + 1] = convertColorChannel(c.G);
			image[index + 2] = convertColorChannel(c.R);
		}
	}

	https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	// --------- BITMAPFILEHEADER 14 Byte --------- 
	bmpfileheader[ 2] = (unsigned char)(fileSize    );
	bmpfileheader[ 3] = (unsigned char)(fileSize>> 8);
	bmpfileheader[ 4] = (unsigned char)(fileSize>>16);
	bmpfileheader[ 5] = (unsigned char)(fileSize>>24);

	// --------- BITMAPINFOHEADER 40 Byte --------- 
	bmpinfoheader[ 4] = (unsigned char)(this->width());
	bmpinfoheader[ 5] = (unsigned char)(this->width()>> 8);
	bmpinfoheader[ 6] = (unsigned char)(this->width()>>16);
	bmpinfoheader[ 7] = (unsigned char)(this->width()>>24);
	bmpinfoheader[ 8] = (unsigned char)(this->height());
	bmpinfoheader[ 9] = (unsigned char)(this->height()>> 8);
	bmpinfoheader[10] = (unsigned char)(this->height()>>16);
	bmpinfoheader[11] = (unsigned char)(this->height()>>24);

	//fwrite(Input, Input-Elementgroesse, Anzahl der Input-Elemente, Stream in den geschrieben wird)
	if((fwrite(bmpfileheader, 1, 14, bmpFile)
		&& fwrite(bmpinfoheader, 1, 40, bmpFile)
		&& fwrite(image, 1, imageSize, bmpFile))){
			fclose(bmpFile);
			free(image);
			return true;
		}

	free(bmpFile);
	return false;
}
