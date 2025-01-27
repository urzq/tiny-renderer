#include <limits>
#include <vector>
#include <array>

#include "tgaimage.h"

const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);


const int CHUNK = 1024;
const int width = 300;
const int height = 300;

struct Vec2i {
	Vec2i(int x, int y) :
		x(x),
		y(y) {
	}

	int x;
	int y;
};


struct BoundingBox {
	Vec2i bottomLeft;
	Vec2i topRight;

	BoundingBox(Vec2i bottomLeft, Vec2i topRight) :
		bottomLeft(bottomLeft),
		topRight(topRight) {
	}
};

struct Triangle {
	Vec2i t0;
	Vec2i t1;
	Vec2i t2;
};

void line(Vec2i t0, Vec2i t1, TGAImage& image, const TGAColor& color) {
	bool steep = false;

	// if the line is steep, we transpose the image 
	if (std::abs(t0.x - t1.x) < std::abs(t0.y - t1.y)) {
		std::swap(t0.x, t0.y);
		std::swap(t1.x, t1.y);
		steep = true;
	}

	// Reorder the line so it's left to right
	if (t0.x > t1.x) {
		std::swap(t0.x, t1.x);
		std::swap(t0.y, t1.y);
	}

	int dx = t1.x - t0.x;
	int dy = t1.y - t0.y;

	int D = 2 * dy - dx;
	int x = t0.x;
	int y = t0.y;
	int sy = 1;
	if (t1.y < t0.y) {
		dy = -dy;
		sy = -1;
	}

	if (steep)
		image.set(y, x, color);
	else
		image.set(x, y, color);

	for (int x = t0.x; x < t1.x; x++) {
		if (D < 0) {
			D = D + 2 * dy;
		}
		else {
			y += sy;
			D = D + 2 * (dy - dx);
		}

		if (steep)
			image.set(y, x, color);
		else
			image.set(x, y, color);
	}
}

BoundingBox findBoundingBox(Triangle t) {

	BoundingBox b({ std::numeric_limits<int>::max(),std::numeric_limits<int>::max() }, { std::numeric_limits<int>::min(), std::numeric_limits<int>::min() });
	return b;
}

void _triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	line(t0, t1, image, color);
	line(t1, t2, image, color);
	line(t2, t0, image, color);
}

bool isInside(const Triangle& t, Vec2i p)  {
	return true;
}

void draw(Triangle t, TGAImage& image, TGAColor color) {
	BoundingBox b = findBoundingBox(t);

	for (int x = b.bottomLeft.x; x <= b.topRight.x; x++) {
		for (int y = b.bottomLeft.y; y <= b.topRight.y; y++) {
			if (isInside(t, { x, y })) {
				image.set(x, y, color);
			}
		}
	}
}

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);

	Triangle a = { Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80) };
	Triangle b = { Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180) };
	Triangle c = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	draw(a, image, white);
	draw(b, image, red);
	draw(c, image, green);

	// origin at the left bottom corner of the image
	image.flip_vertically();
	image.write_tga_file("res\\output.tga");

	return EXIT_SUCCESS;
}