#pragma once

struct Matrix4x4
{
	float m00, m10, m20, m30;
	float m01, m11, m21, m31;
	float m02, m12, m22, m32;
	float m03, m13, m23, m33;

	Matrix4x4();
	Matrix4x4(float m0[16]);
	Matrix4x4(float f);
	Matrix4x4(float* v0, float* v1, float* v2, float* v3);
	~Matrix4x4();

	Matrix4x4 operator* (Matrix4x4& _m);
	Matrix4x4 transposed();
	Matrix4x4 inverted();
	float getDeterminant();
	float* getAsArray();
};

Matrix4x4::Matrix4x4()
{
	m00 = m01 = m02 = m03 = 0.f;
	m10 = m11 = m12 = m13 = 0.f;
	m20 = m21 = m22 = m23 = 0.f;
	m30 = m31 = m32 = m33 = 0.f;
}

Matrix4x4::Matrix4x4(float m0[16])
{
	m00 = m0[0]; m01 = m0[4]; m02 = m0[8]; m03 = m0[12];
	m10 = m0[1]; m11 = m0[5]; m12 = m0[9]; m13 = m0[13];
	m20 = m0[2]; m21 = m0[6]; m22 = m0[10]; m23 = m0[14];
	m30 = m0[3]; m31 = m0[7]; m32 = m0[11]; m33 = m0[15];
}

inline Matrix4x4::Matrix4x4(float f)
{
	m00 = f; m01 = 0; m02 = 0; m03 = 0;
	m10 = 0; m11 = f; m12 = 0; m13 = 0;
	m20 = 0; m21 = 0; m22 = f; m23 = 0;
	m30 = 0; m31 = 0; m32 = 0; m33 = f;
}

Matrix4x4::Matrix4x4(float* v0, float* v1, float* v2, float* v3)
{
	m00 = v0[0]; m01 = v0[1]; m02 = v0[2]; m03 = v0[3];
	m10 = v1[0]; m11 = v1[1]; m12 = v1[2]; m13 = v1[3];
	m20 = v2[0]; m21 = v2[1]; m22 = v2[2]; m23 = v2[3];
	m30 = v3[0]; m31 = v3[1]; m32 = v3[2]; m33 = v3[3];
}

Matrix4x4::~Matrix4x4()
{
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4& _m)
{
	Matrix4x4 r;
	
	r.m00 = m00 * _m.m00 + m10 * _m.m01 + m20 * _m.m02 + m30 * _m.m03;
	r.m10 = m00 * _m.m10 + m10 * _m.m11 + m20 * _m.m12 + m30 * _m.m13;
	r.m20 = m00 * _m.m20 + m10 * _m.m21 + m20 * _m.m22 + m30 * _m.m23;
	r.m30 = m00 * _m.m30 + m10 * _m.m31 + m20 * _m.m32 + m30 * _m.m33;

	r.m01 = m01 * _m.m00 + m11 * _m.m01 + m21 * _m.m02 + m31 * _m.m03;
	r.m11 = m01 * _m.m10 + m11 * _m.m11 + m21 * _m.m12 + m31 * _m.m13;
	r.m22 = m01 * _m.m20 + m11 * _m.m21 + m21 * _m.m22 + m31 * _m.m23;
	r.m33 = m01 * _m.m30 + m11 * _m.m31 + m21 * _m.m32 + m31 * _m.m33;

	r.m02 = m02 * _m.m00 + m12 * _m.m01 + m22 * _m.m02 + m32 * _m.m03;
	r.m12 = m02 * _m.m10 + m12 * _m.m11 + m22 * _m.m12 + m32 * _m.m13;
	r.m22 = m02 * _m.m20 + m12 * _m.m21 + m22 * _m.m22 + m32 * _m.m23;
	r.m32 = m02 * _m.m30 + m12 * _m.m31 + m22 * _m.m32 + m32 * _m.m33;

	r.m03 = m03 * _m.m00 + m13 * _m.m01 + m23 * _m.m02 + m33 * _m.m03;
	r.m13 = m03 * _m.m10 + m13 * _m.m11 + m23 * _m.m12 + m33 * _m.m13;
	r.m23 = m03 * _m.m20 + m13 * _m.m21 + m23 * _m.m22 + m33 * _m.m23;
	r.m33 = m03 * _m.m30 + m13 * _m.m31 + m23 * _m.m32 + m33 * _m.m33;

	return r;
}

inline Matrix4x4 Matrix4x4::transposed()
{
	Matrix4x4 r;
	r.m00 = m00; r.m10 = m01; r.m20 = m02; r.m30 = m03;
	r.m01 = m10; r.m11 = m11; r.m21 = m12; r.m31 = m13;
	r.m02 = m20; r.m12 = m21; r.m22 = m22; r.m32 = m23;
	r.m03 = m30; r.m31 = m31; r.m23 = m32; r.m33 = m33;
	return r;
}

inline Matrix4x4 Matrix4x4::inverted()
{
    Matrix4x4 res;
    float det = this->getDeterminant();
    if (det == 0) return res;

	float inv_det = 1 / det;

	float* m = this->getAsArray();
	float inv[16];

	inv[0] = 
        m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

    inv[4] = 
        -m[4] * m[10] * m[15] +
        m[4] * m[11] * m[14] +
        m[8] * m[6] * m[15] -
        m[8] * m[7] * m[14] -
        m[12] * m[6] * m[11] +
        m[12] * m[7] * m[10];

    inv[8] = 
        m[4] * m[9] * m[15] -
        m[4] * m[11] * m[13] -
        m[8] * m[5] * m[15] +
        m[8] * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];

    inv[12] = 
        -m[4] * m[9] * m[14] +
        m[4] * m[10] * m[13] +
        m[8] * m[5] * m[14] -
        m[8] * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];

    inv[1] = 
        -m[1] * m[10] * m[15] +
        m[1] * m[11] * m[14] +
        m[9] * m[2] * m[15] -
        m[9] * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];

    inv[5] = 
        m[0] * m[10] * m[15] -
        m[0] * m[11] * m[14] -
        m[8] * m[2] * m[15] +
        m[8] * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];

    inv[9] = 
        -m[0] * m[9] * m[15] +
        m[0] * m[11] * m[13] +
        m[8] * m[1] * m[15] -
        m[8] * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];

    inv[13] = 
        m[0] * m[9] * m[14] -
        m[0] * m[10] * m[13] -
        m[8] * m[1] * m[14] +
        m[8] * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];

    inv[2] = 
        m[1] * m[6] * m[15] -
        m[1] * m[7] * m[14] -
        m[5] * m[2] * m[15] +
        m[5] * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];

    inv[6] = 
        -m[0] * m[6] * m[15] +
        m[0] * m[7] * m[14] +
        m[4] * m[2] * m[15] -
        m[4] * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];

    inv[10] = 
        m[0] * m[5] * m[15] -
        m[0] * m[7] * m[13] -
        m[4] * m[1] * m[15] +
        m[4] * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];

    inv[14] = 
        -m[0] * m[5] * m[14] +
        m[0] * m[6] * m[13] +
        m[4] * m[1] * m[14] -
        m[4] * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];

    inv[3] = 
        -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];

    inv[7] = 
        m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];

    inv[11] = 
        -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];

    inv[15] = 
        m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];

    res = {inv};

	return res;
}

inline float Matrix4x4::getDeterminant()
{
	return
		m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 -
		m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
		m02 * m11 * m23 * m30 - m01 * m12 - m23 - m30 -
		m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
		m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 -
		m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
		m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 -
		m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
		m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 -
		m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
		m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 -
		m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
}

inline float* Matrix4x4::getAsArray()
{
	float* _out = new float[16];

	_out[0] = m00; _out[4] = m01; _out[8] = m02; _out[12] = m03;
	_out[1] = m10; _out[5] = m11; _out[9] = m12; _out[13] = m13;
	_out[2] = m20; _out[6] = m21; _out[10] = m22; _out[14] = m23;
	_out[3] = m30; _out[7] = m31; _out[11] = m32; _out[15] = m33;

	return _out;
}
