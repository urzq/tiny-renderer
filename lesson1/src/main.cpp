#include <vector>
#include <array>

#include "tgaimage.h"

TGAImage image(800, 800, TGAImage::RGB);

const TGAColor red = TGAColor(255, 0, 0, 255);

const int CHUNK = 1024;
const int width = 800;
const int height = 800;

struct Vertex {
	float x;
	float y;
	float z;
};

using Face = std::array<int, 3>;

void line(int x0, int y0, int x1, int y1, const TGAColor& color) {
	bool steep = false;

	// if the line is steep, we transpose the image 
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	// Reorder the line so it's left to right
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	int D = 2 * dy - dx;
	int x = x0;
	int y = y0;
	int sy = 1;
	if (y1 < y0) {
		dy = -dy;
		sy = -1;
	}

	if (steep)
		image.set(y, x, color);
	else
		image.set(x, y, color);

	for (int x = x0; x < x1; x++) {
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

int main(int argc, char** argv) {
	FILE* file = fopen("res\\viking.obj", "r");
	if (file == nullptr) {
		return EXIT_FAILURE;
	}

	char buff[CHUNK];

	std::vector<Vertex> vertices;
	std::vector<Face> faces;

	while (fgets(buff, CHUNK, file)) {
		if (buff[0] == 'v') {
			Vertex v;
			int nb = sscanf(buff, "v %f %f %f", &v.x, &v.y, &v.z);
			if (nb == 3)
				vertices.push_back(v);
		}
		else if (buff[0] == 'f') {
			Face face;
			// %*d = scan an integer, but do not extract it into a variable.
			int nb = sscanf(buff, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &face[0], &face[1], &face[2]);
			if (nb == 3)
				faces.push_back(face);
		}
	}

	fclose(file);

	for (const Face& face : faces) {
		for (int i = 0; i < 3; i++) {
			auto& v0 = vertices[face[i] - 1];
			auto& v1 = vertices[face[(i + 1) % 3] - 1];

			auto sx = int((v0.x + 1) * (width - 1) / 2.0);
			auto sy = int((v0.y) * (height - 1) / 2.0);
			auto ex = int((v1.x + 1) * (width - 1) / 2.0);
			auto ey = int((v1.y) * (height - 1) / 2.0);

			line(sx, sy, ex, ey, red);
		}
	}

	// origin at the left bottom corner of the image
	image.flip_vertically();
	image.write_tga_file("res\\output.tga");

	return EXIT_SUCCESS;
}