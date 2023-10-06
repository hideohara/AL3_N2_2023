#include "MathUtilityForText.h"
#include <cassert>
#include <cmath>

float Matrix4Determinant(const Matrix4x4& m) {

	float det = +m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
	            m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
	            m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

	            - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	            m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
	            m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

	            - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
	            m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	            m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

	            + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
	            m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
	            m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

	            + m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
	            m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
	            m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

	            - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	            m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
	            m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

	            - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
	            m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	            m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

	            + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
	            m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
	            m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
	return det;
}

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

Vector3& operator-=(Vector3& lhv, const Vector3& rhv) {
	lhv.x -= rhv.x;
	lhv.y -= rhv.y;
	lhv.z -= rhv.z;
	return lhv;
}

Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

Vector3& operator/=(Vector3& v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}

// 2項演算子オーバーロード
const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s) {
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v) { return v * s; }

const Vector3 operator/(const Vector3& v, float s) {
	Vector3 temp(v);
	return temp /= s;
}

float Length(const Vector3& v) { return (float)std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	Vector3 result = v;
	if (len != 0) {
		result /= len;
	}
	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	// 行列式を得る
	float det = Matrix4Determinant(m);

	assert(std::abs(det) > 1.0e-10 && "Determinant is nearly equal to zero");

	float divDet = 1.0f / det;

	Matrix4x4 result;

	result.m[0][0] =
	    divDet * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
	              m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
	              m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);

	result.m[0][1] =
	    divDet * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
	              m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
	              m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);

	result.m[0][2] =
	    divDet * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
	              m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
	              m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);

	result.m[0][3] =
	    divDet * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
	              m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
	              m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	result.m[1][0] =
	    divDet * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
	              m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
	              m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);

	result.m[1][1] =
	    divDet * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
	              m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
	              m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);

	result.m[1][2] =
	    divDet * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
	              m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
	              m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);

	result.m[1][3] =
	    divDet * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
	              m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
	              m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

	result.m[2][0] =
	    divDet * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
	              m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
	              m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);

	result.m[2][1] =
	    divDet * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
	              m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
	              m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);

	result.m[2][2] =
	    divDet * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
	              m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
	              m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);

	result.m[2][3] =
	    divDet * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
	              m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
	              m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	result.m[3][0] =
	    divDet * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
	              m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
	              m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);

	result.m[3][1] =
	    divDet * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
	              m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
	              m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);

	result.m[3][2] =
	    divDet * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
	              m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
	              m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);

	result.m[3][3] =
	    divDet * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
	              m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
	              m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return result;
}

Matrix4x4 MakeIdentityMatrix() {
	static const Matrix4x4 result{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	                              0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	Matrix4x4 result{scale.x, 0.0f, 0.0f,    0.0f, 0.0f, scale.y, 0.0f, 0.0f,
	                 0.0f,    0.0f, scale.z, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeRotateXMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos,  sin,  0.0f,
	                 0.0f, -sin, cos,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeRotateYMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result{cos, 0.0f, -sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	                 sin, 0.0f, cos,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeRotateZMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result{cos,  sin,  0.0f, 0.0f, -sin, cos,  0.0f, 0.0f,
	                 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{1.0f, 0.0f, 0.0f, 0.0f, 0.0f,        1.0f,        0.0f,        0.0f,
	                 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f};

	return result;
}

#include <DirectXMath.h>

Matrix4x4
    MakeViewportMatrix(float left, float top, float width, float height, float nearZ, float farZ) {
	Matrix4x4 matViewport = MakeIdentityMatrix();
	matViewport.m[0][0] = +width / 2.0f;
	matViewport.m[1][1] = -height / 2.0f;
	matViewport.m[2][2] = farZ - nearZ;
	matViewport.m[3][0] = left + width / 2.0f;
	matViewport.m[3][1] = top + height / 2.0f;
	matViewport.m[3][2] = nearZ;

	return matViewport;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {

	// スケーリング行列の作成
	Matrix4x4 matScale = MakeScaleMatrix(scale);

	Matrix4x4 matRotX = MakeRotateXMatrix(rot.x);
	Matrix4x4 matRotY = MakeRotateYMatrix(rot.y);
	Matrix4x4 matRotZ = MakeRotateZMatrix(rot.z);
	// 回転行列の合成
	Matrix4x4 matRot = matRotZ * matRotX * matRotY;

	// 平行移動行列の作成
	Matrix4x4 matTrans = MakeTranslateMatrix(translate);

	// スケーリング、回転、平行移動の合成
	Matrix4x4 matTransform = matScale * matRot * matTrans;

	return matTransform;
}

Matrix4x4& operator*=(Matrix4x4& lhm, const Matrix4x4& rhm) {
	Matrix4x4 result{};

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			for (size_t k = 0; k < 4; k++) {
				result.m[i][j] += lhm.m[i][k] * rhm.m[k][j];
			}
		}
	}
	lhm = result;
	return lhm;
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = m1;

	return result *= m2;
}

Vector3 Transform(const Vector3& v, const Matrix4x4& m) {
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result{
	    (v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
	    (v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
	    (v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w};

	return result;
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {

	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};

	return result;
}
