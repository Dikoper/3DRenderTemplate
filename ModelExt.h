#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "VectorExt.h"
#include "ImageExt.h"

struct Model3D 
{
	std::vector<Vec4> verticies;
	std::vector<Vec4> normals;
	std::vector<int> faces;
	std::vector<int> face_normals;
	std::vector<int> face_tx;
	std::vector<Vec4> uv;
	TGAImage diffuse_tx;
	TGAImage normalmap_tx;
	TGAImage specularmap_tx;

	bool loadFromFile(const char* filepath) 
	{
		std::ifstream ifs;
		ifs.open(filepath);
		if (ifs.fail()) return false;
		std::istringstream sline;
		std::string line,prefix;

		while (std::getline(ifs, line))
		{
			std::istringstream sline(line);
			sline >> prefix;
			if (prefix == "v")
			{
				Vec4 _v;
				sline >> _v.x >> _v.y >> _v.z;
				_v.w = 1;
				verticies.push_back(_v);
				continue;
			}
			if (prefix == "vn")
			{
				Vec4 _n;
				sline >> _n.x >> _n.y >> _n.z;
				_n.w = 0;
				normals.push_back(_n);
				continue;
			}
			if (prefix == "vt")
			{
				Vec4 _uv;
				sline >> _uv.x >> _uv.y >> _uv.z;
				_uv.w = 0;
				uv.push_back(_uv);
				continue;
			}
			if (prefix == "f")
			{
				char _sep;
				float _f0, _f1, _f2;
				for (auto i = 0; i < 3; i++)
				{
					if (sline >> _f0 >> _sep >> _f1 >> _sep >> _f2); //works only for faces data
					faces.push_back(--_f0); //dec for right indexing
					face_tx.push_back(--_f1);
					face_normals.push_back(--_f2);
				}
				continue;
			}
		}
		ifs.close();
		return true;
	};

	bool loadTexture(const char* filepath, TGAImage &target) 
	{
		if (target.read_tga_file(filepath))
		{
			target.flip_vertically();
			return true;
		}

		return false;
	}

	uint32_t getColorByUV(float x, float y) 
	{
		uint32_t res = tgaColToUInt(diffuse_tx.get(x * diffuse_tx.width(), y * diffuse_tx.height()));
		return res;
	}

	Vec4 getNormalByUV(float x, float y)
	{
		TGAColor tC = normalmap_tx.get(x * normalmap_tx.width(), y * normalmap_tx.height());
		Vec4 res = Vec4(tC.bgra[2], tC.bgra[1], tC.bgra[0]) * 2.f * (1.f/255.f) - Vec4(1,1,1);
		return res;
	}
};