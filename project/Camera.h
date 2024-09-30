#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include "MathFunctions.h"

class Camera
{
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	Camera();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private:

	//------ビュー行列関連データ------//
	Transform transform;
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;

	//------プロジェクション行列関連データ------//
	Matrix4x4 projectionMatrix;
	//水平方向視野角
	float horizontalViewingAngle;
	//アスペクト比
	float aspect;
	//ニアクリップ
	float nearClip;
	//ファークリップ距離
	float farClip;

	//合成行列
	Matrix4x4 viewProjectionMatrix;
};

