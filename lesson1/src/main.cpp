#include <vector>
#include <array>

#include "tgaimage.h"

TGAImage image(800, 800, TGAImage::RGB);

const TGAColor white = TGAColor(255, 255, 255, 255);
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

void line(int sx, int sy, int ex, int ey, TGAColor color) {
	auto dx = ex - sx;
	auto dy = ey - sy;

	if (abs(dx) > abs(dy)) {
		if (sx > ex) {
			std::swap(sx, ex);
			std::swap(sy, ey);
		}

		for (int x = sx; x < ex; x++) {
			float step = (x - sx) / (float)dx;
			float y = sy + dy * step;
			image.set(x, round(y), color);
		}
	}
	else {
		if (sy > ey) {
			std::swap(sx, ex);
			std::swap(sy, ey);
		}

		for (int y = sy; y < ey; y++) {
			float step = (y - sy) / (float)dy;
			float x = sx + dx * step;
			image.set(round(x), y, color);
		}
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

			if (sx < width && sy < height && ex < width && ey < height) {
				line(sx, sy, ex, ey, red);
			}
		}
	}

	image.flip_vertically();
	image.write_tga_file("res\\output.tga");

	return EXIT_SUCCESS;
}