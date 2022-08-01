#pragma once
#include <math.h>
#include <thread>
#include "DebugHeader.h"
#include "MatrixExt.h"
#include "VectorExt.h"
#include "ModelExt.h"
#include "ImageExt.h"

//__ColorsConst__// ARGB
#define CLR_BLACK	0x00000000
#define CLR_RED		0xffff0000
#define CLR_GREEN	0xff00ff00
#define CLR_BLUE	0xff0000ff
#define CLR_WHITE	0xffffffff

#define CLR_RANDOM	(255 << 24) + (rand()%255 << 16) + (rand()%255 << 8) + rand()%255
//__//
#define barIntrp(i,j,k) {i * b.x + j * b.y + k * b.z}

Model3D* model = NULL;
Matrix4x4 m_Model, m_View, m_Projection, m_Device;
Vec4 light_pos = Vec4(1, 1, 1);
Vec4 cam_pos = Vec4(0, 0, 1); // move to own class

extern int z_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
int z_depth = 255;

template <typename T>
inline T fLerp(T v0, T v1, float s)
{
	if (s < 0) s = 0;
	return v0 + (v1 - v0) * s;
}

struct Shader
{
	virtual ~Shader() {};
	virtual Vec4 processVertex(int,int) = NULL;
	virtual uint32_t processPixel(Vec4&, uint32_t) = NULL;
};

struct Wireframe : public Shader 
{
	virtual Vec4 processVertex(int vert, int face) 
	{
		Vec4 _v = model->verticies[model->faces[face + vert]];
		return _v * m_Model * m_View * m_Projection * m_Device;
	}

	virtual uint32_t processPixel(Vec4& bar, uint32_t color)
	{
		return color;
	}
} wireframe_sh;

struct Filled : public Shader
{
	Vec4 intensity;
	Vec4 Diffuse_x;
	Vec4 Diffuse_y;

	virtual Vec4 processVertex(int vert, int face)
	{
		Vec4 _v = model->verticies[model->faces[face + vert]];
		Vec4 _t = model->uv[model->face_tx[face + vert]];
		Diffuse_x[vert] = _t.x; Diffuse_y[vert] = _t.y;
		Vec4 _n = model->normals[model->face_normals[face + vert]] * m_Model;
		intensity[vert] = std::max(0.0f, Vec4::dot(_n, Vec4::normalized(light_pos)));
		
		return _v * m_Model * m_View * m_Projection * m_Device;
	}

	virtual uint32_t processPixel(Vec4& bar, uint32_t color)
	{
		float xTxtr = Vec4::dot(Diffuse_x, bar);
		float yTxtr = Vec4::dot(Diffuse_y, bar);
		float scale = Vec4::dot(intensity, bar);
		uint32_t d_Clr = model->getColorByUV(xTxtr, yTxtr);

		return multByScalar(d_Clr, scale);
	}
} shader_sh;

struct Shaded : public Shader
{
	Vec4 intensity;
	Vec4 Diffuse_x;
	Vec4 Diffuse_y;

	virtual Vec4 processVertex(int vert, int face)
	{
		Vec4 _v = model->verticies[model->faces[face + vert]];
		Vec4 _t = model->uv[model->face_tx[face + vert]];
		Diffuse_x[vert] = _t.x; Diffuse_y[vert] = _t.y;
		Matrix4x4 invMVP = (m_Model * m_View * m_Projection).inverted().transposed();
		Vec4 _n = model->normals[model->face_normals[face + vert]] * invMVP;

		return _v * m_Model * m_View * m_Projection * m_Device;
	}

	virtual uint32_t processPixel(Vec4& bar, uint32_t color)
	{
		float xTxtr = Vec4::dot(Diffuse_x, bar);
		float yTxtr = Vec4::dot(Diffuse_y, bar);
		Vec4 n = model->getNormalByUV(xTxtr, yTxtr);
		Vec4 l = Vec4::normalized(light_pos);
		Vec4 r = Vec4::normalized(n * (Vec4::dot(n, l) * 2.f) - l);
		float spec = r.z * 0.75f;
		float scale = std::max(0.0f, Vec4::dot(n, l)); //Vec4::dot(intensity, bar);
		uint32_t d_Clr = model->getColorByUV(xTxtr, yTxtr);

		return multByScalar(d_Clr, scale + spec);
	}
} bumped_sh;

void calculateViewport()
{
	m_Projection = { new float[16]{
		0.3,0,0,0,
		0,0.5,0,0,
		0,0,1,-0.1,
		0,0,0,1}
	};

	m_Device = { new float[16]{
	  SCREEN_WIDTH / 2,0,0,0,
	  0,SCREEN_HEIGHT / 2,0,0,
	  0,0,z_depth * 0.5f,0,
	  SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, z_depth * 0.5f,1} //screen centre
	};
}

Matrix4x4 getLookAt(Vec4 pos, Vec4 up, Vec4 target)
{
	Vec4 t = target * -1.f;
	Vec4 z = Vec4::normalized(target - pos);
	Vec4 x = Vec4::normalized(Vec4::cross(up, z));
	Vec4 y = Vec4::normalized(Vec4::cross(z, x));

	Matrix4x4 mt = { new float[16]{
		x.x,y.x,z.x,0,
		x.y,y.y,z.y,0,
		x.z,y.z,z.z,0,
		0,0,0,1}
	};

	Matrix4x4 tr = { new float[16]{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		t.x,t.y,t.z,1}
	};

	Matrix4x4 res = mt * tr;
	return res;
}

//draw flipped image (origin at left bottom)
inline void drawPoint(int x, int y, uint32_t color)
{
	int _x = x;
	int _y = SCREEN_HEIGHT - y;

	//bounds check
	if (SCREEN_WIDTH > _x && _x > 0 && SCREEN_HEIGHT > _y && _y > 0)
		buffer[_y][_x] = color;
}

inline void drawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
	int dy = (y1 - y0);
	int dx = (x1 - x0);

	int length = abs(dy) > abs(dx) ? abs(dy) : abs(dx);

	float ex = dx / (float)length;
	float ey = dy / (float)length;

	float x = x0;
	float y = y0;

	drawPoint(x0, y0, color);

	for (auto i = 0; i < length; i++)
	{
		x += ex;
		y += ey;

		drawPoint(x, y, color);
	}
}

inline void drawLine(Vec4& points, uint32_t color)
{
	drawLine(points.x, points.y, points.z, points.w, color);
}

inline void drawTriangle(Vec4 v0, Vec4 v1, Vec4 v2, uint32_t color)
{
	v0.x /= v0.w; v0.y /= v0.w;
	v1.x /= v1.w; v1.y /= v1.w;
	v2.x /= v2.w; v2.y /= v2.w;

	drawLine(v0.x, v0.y, v1.x, v1.y, color);
	drawLine(v1.x, v1.y, v2.x, v2.y, color);
	drawLine(v2.x, v2.y, v0.x, v0.y, color);
}

inline bool putZBuff(int x, int y, int color)
{
	int _x = x;
	int _y = SCREEN_HEIGHT - y;

	//bounds check
	if (SCREEN_WIDTH > _x && _x > 0 && SCREEN_HEIGHT > _y && _y > 0 && color > z_buffer[_y][_x])
	{
		z_buffer[_y][_x] = color;
		return true;
	}
	return false;
}

void drawZBuff()
{
	for (size_t i = 0; i < SCREEN_WIDTH; i++)
	{
		for (size_t j = 0; j < SCREEN_HEIGHT; j++)
		{
			int _c = z_buffer[j][i];

			if (_c != 0) {
				uint32_t clr = (255 << 24) + ((_c % 255) << 16) + ((_c % 255) << 8) + (_c % 255); // color;
				buffer[j][i] = clr;
			}
		}
	}
}

inline Vec4& calcNormal(Vec4& _p0, Vec4& _p1, Vec4& _p2, Vec4 _origin)
{
	Vec4 cr = Vec4::cross(_p2 - _p0, _p1 - _p0); //its wrong?
	Vec4 normal = Vec4::normalized(cr);
	//check
	Vec4 _rvertex = _origin - (_p0 + normal);
	if (Vec4::dot(normal, _rvertex) > 0)
		normal = normal * -1;
	return normal;
}

inline Vec4& getBoundingBox(Vec4 v0, Vec4 v1, Vec4 v2)
{
	Vec4 bb; //x_min, x_max, y_min, y_max;
	
	bb[0] = v0.x; bb[1] = v0.x;
	bb[2] = v0.y; bb[3] = v0.y;

	if (bb[0] > v1.x) bb[0] = v1.x;
	if (bb[0] > v2.x) bb[0] = v2.x;

	if (bb[1] < v1.x) bb[1] = v1.x;
	if (bb[1] < v2.x) bb[1] = v2.x;

	if (bb[2] > v1.y) bb[2] = v1.y;
	if (bb[2] > v2.y) bb[2] = v2.y;

	if (bb[3] < v1.y) bb[3] = v1.y;
	if (bb[3] < v2.y) bb[3] = v2.y;

	return bb;
}

inline Vec4& getBaracentric(Vec4 v0, Vec4 v1, Vec4 v2, Vec4 p)
{
	Vec4 bar;
	Vec4 u = Vec4::cross(Vec4( v2.x - v0.x, v1.x - v0.x, v0.x - p.x ), Vec4(v2.y - v0.y, v1.y - v0.y, v0.y - p.y)); //get more info about it

	if (std::abs(u.z) >= 1)
		bar = { 1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z };
	else
		bar = { -1, -1, -1 };

	return bar;
}

inline void rasterize(Shader &shader, Vec4 v0, Vec4 v1, Vec4 v2, uint32_t color, Model3D& m)
{
	//projection
	v0.x /= v0.w; v0.y /= v0.w;
	v1.x /= v1.w; v1.y /= v1.w;
	v2.x /= v2.w; v2.y /= v2.w;

	Vec4 bb = getBoundingBox(v0, v1, v2);

#pragma omp parallel for
	for (size_t j = bb[2]; j < bb[3]; j++) //y
	{
		for (size_t i = bb[0]; i < bb[1]; i++) //x
		{
			Vec4 b = getBaracentric(v0, v1, v2, Vec4(i, j, 0));
			if (b.x < 0 || b.y < 0 || b.z < 0)
				continue;

			b = { b.x / v0.w, b.y / v1.w, b.z / v2.w }; //perspective correction
			b = b * (1 / (b.x + b.y + b.z));

			uint32_t pClr = shader.processPixel(b, color);

			float pZ = v0.z * b.x + v1.z * b.y + v2.z * b.z;
			if (putZBuff(i, j, pZ))
			{
				drawPoint(i, j, pClr);
			}
		}
	}
}

void drawWireframeModel(Model3D& m, uint32_t color) // move that type of code to the shader system
{
	model = &m;
	for (auto i = 0; i < m.faces.size(); i += 3)
	{
		Vec4 _p0 = wireframe_sh.processVertex(0, i);
		Vec4 _p1 = wireframe_sh.processVertex(1, i);
		Vec4 _p2 = wireframe_sh.processVertex(2, i);

		drawTriangle(_p0, _p1, _p2, color);
	}
}

void drawFilledModel(Model3D& m, Shader &sh, Vec4 cam_pos, Vec4 light_pos, uint32_t color)
{
	model = &m;
	//do it multithreaded
	for (auto i = 0; i < m.faces.size(); i += 3)
	{
		Vec4 _p0 = sh.processVertex(0, i);
		Vec4 _p1 = sh.processVertex(1, i);
		Vec4 _p2 = sh.processVertex(2, i);

		//Vec4 _n0 = m.normals[m.face_normals[i]];
		//Vec4 _n1 = m.normals[m.face_normals[i+1]];
		//Vec4 _n2 = m.normals[m.face_normals[i+2]];
		//Vec4 _t0 = m.uv[m.face_tx[i]];
		//Vec4 _t1 = m.uv[m.face_tx[i + 1]];
		//Vec4 _t2 = m.uv[m.face_tx[i + 2]];
		//_n0 = _n0 * m_Model;
		//_n1 = _n1 * m_Model;
		//_n2 = _n2 * m_Model;
		//float _i0 = Vec4::dot(_n0, Vec4::normalized(light_pos)); //z forward is positive
		//float _i1 = Vec4::dot(_n1, Vec4::normalized(light_pos)); 
		//float _i2 = Vec4::dot(_n2, Vec4::normalized(light_pos));
		////looks like sheeeeet!!!!
		//_i0 = _i0 < 0 ? 0 : _i0;
		//_i1 = _i1 < 0 ? 0 : _i1;
		//_i2 = _i2 < 0 ? 0 : _i2;
		//Vec4 sNorm = Vec4::normalized(_n0 + _n1 + _n2);//calcNormal(_p0, _p1, _p2, Vec4());normal; dont work properly // 

		//if (Vec4::dot(sNorm, Vec4::normalized(cam_pos)) >= 0) //backface culling -- works wrong(need glu look at?)
		{
			//std::thread t(rasterize, sh, _p0, _p1, _p2, color, m);
			rasterize(sh, _p0, _p1, _p2, color, m);

			//if (debug_draw)
			//{ //normals
			//	_p0.x /= _p0.w; _p0.y /= _p0.w;
			//	_p1.x /= _p1.w; _p1.y /= _p1.w;
			//	_p2.x /= _p2.w; _p2.y /= _p2.w;
			//	Vec4 _bm = fLerp(_p1, _p2, 0.5f);
			//	Vec4 _tm = fLerp(_bm ,_p0, 0.5f);
			//	drawLine(_tm.x, _tm.y, _tm.x + sNorm.x * 50, _tm.y + sNorm.y * 50, 0xFFFF00FF);
			//}
		}
		/*else
		{
			drawTriangle(_p0, _p1, _p2, CLR_RED);
		}*/
	}
}

