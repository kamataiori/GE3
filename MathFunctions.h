#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "math.h"
#include <fstream>
#include <sstream>

//単位行列の作成
Matrix4x4 MakeIdentity4x4();

//1. X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

//2. Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

//3. Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4  Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// アフィン変換行列計算関数
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& matrix);

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);
