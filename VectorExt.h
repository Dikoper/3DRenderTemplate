#pragma once
#include "MatrixExt.h"
struct Vec4
{
	float x, y, z, w;

	Vec4();
	Vec4(float x, float y, float z);
	Vec4(float x, float y, float z, float w);
	~Vec4();

	Vec4 operator* (float* m);//matrix as 16 elements array
	Vec4 operator* (float s);
	Vec4 operator* (Matrix4x4& m);
	Vec4 operator+ (Vec4 _v);
	Vec4 operator- (Vec4 _v);
	bool operator==(Vec4 _v);
	bool operator!=(Vec4 _v);
	float& operator[](int i);
	float* getAsArray();
	static Vec4 cross(Vec4 v0, Vec4 v1);
	static float dot(Vec4 v0, Vec4 v1);
	static Vec4 normalized(Vec4 v);
};

Vec4::Vec4()
{
	x = y = z = w = 0.f;
}

Vec4::Vec4(float _x, float _y, float _z)
{
	x = _x; y = _y; z = _z; w = 1.0f;
}

Vec4::Vec4(float _x, float _y, float _z, float _w)
{
	x = _x; y = _y; z = _z; w = _w;
}

Vec4::~Vec4()
{
}

Vec4 Vec4::normalized(Vec4 v)
{
	float m = dot(v, v);
	return Vec4((v.x * v.x) / m, (v.y * v.y) / m, (v.z * v.z) / m);
}

float Vec4::dot(Vec4 v0, Vec4 v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

Vec4 Vec4::cross(Vec4 v0, Vec4 v1)
{
	return Vec4(
		v0.y * v1.z - v0.z * v1.y,
		v0.z * v1.x - v0.x * v1.z,
		v0.x * v1.y - v0.y * v1.x
	);
}

inline Vec4 Vec4::operator*(float* m)
{
	Vec4 vOut;

	vOut.x = x * m[0] + y * m[4] + z * m[8] + w * m[12];
	vOut.y = x * m[1] + y * m[5] + z * m[9] + w * m[13];
	vOut.z = x * m[2] + y * m[6] + z * m[10] + w * m[14];
	vOut.w = x * m[3] + y * m[7] + z * m[11] + w * m[15];

	return vOut;
}

inline Vec4 Vec4::operator*(float s)
{
	return Vec4(x * s,y * s,z * s,w);
}

inline Vec4 Vec4::operator*(Matrix4x4& m)
{
	Vec4 vOut;

	vOut.x = x * m.m00 + y * m.m01 + z * m.m02 + w * m.m03;
	vOut.y = x * m.m10 + y * m.m11 + z * m.m12 + w * m.m13;
	vOut.z = x * m.m20 + y * m.m21 + z * m.m22 + w * m.m23;
	vOut.w = x * m.m30 + y * m.m31 + z * m.m32 + w * m.m33;

	return vOut;
}

inline Vec4 Vec4::operator+(Vec4 _v)
{
	return Vec4(_v.x + x, _v.y + y, _v.z + z, w);
}

inline Vec4 Vec4::operator-(Vec4 _v)
{
	return Vec4(x-_v.x, y - _v.y, z - _v.z, w);
}

inline bool Vec4::operator==(Vec4 _v)
{  
	return _v.x == x && _v.y == y && _v.z == z && _v.w == w;
}

inline bool Vec4::operator!=(Vec4 _v)
{
	bool r = *this == _v;
	return !r;
}

inline float& Vec4::operator[](int i)
{
	switch (i) 
	{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw new _exception;
			break;
	}
}

float* Vec4::getAsArray()
{
	float* _out = new float[4];

	_out[0] = x;
	_out[1] = y;
	_out[2] = z;
	_out[3] = w;
	return _out;
}
