#pragma once

#include <SOIL2.h>

// based on https://gamedev.stackexchange.com/questions/80940/how-to-create-normal-map-from-bump-map-in-runtime
class HeightmapConverter
{
public:
	HeightmapConverter(int width, int height, unsigned char *data) : width(width), height(height), data(data) {};
	void ComputeNormalMap(unsigned char *normal);
private:
	int width, height;
	unsigned char *data;

	const double EXTRUSION = 2.0;

	double getPixel(int x, int y)
	{
		if(x < 0) x = 0;
		if(y < 0) y = 0;
		if(x >= width) x = width - 1;
		if(y >= height) y = height - 1;
		// invert y because opengl has weird conventions
		y = height - 1 - y;
		int index = x + y * width;
		return data[index] / 256.0;
	}
};


void HeightmapConverter::ComputeNormalMap(unsigned char *normal)
{
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			double center = getPixel(x, y);
			double up = getPixel(x, y - 1);
			double down = getPixel(x, y + 1);
			double left = getPixel(x - 1, y);
			double right = getPixel(x + 1, y);
			double upleft = getPixel(x - 1, y - 1);
			double upright = getPixel(x + 1, y - 1);
			double downleft = getPixel(x - 1, y + 1);
			double downright = getPixel(x + 1, y + 1);

			double vert = (down - up) * 2.0 + downright + downleft - upright - upleft;
			double horiz = (right - left) * 2.0 + upright + downright - upleft - downleft;
			double depth = 1.0 / EXTRUSION;
			double scale = 127.0 / sqrt(vert * vert + horiz * horiz + depth * depth);

			unsigned char r = 128 - horiz * scale;
			unsigned char g = 128 + vert * scale;
			unsigned char b = 128 + depth * scale;

			int index = x + y * width;
			normal[index * 3 + 0] = r;
			normal[index * 3 + 1] = g;
			normal[index * 3 + 2] = b;
		}
	}
}