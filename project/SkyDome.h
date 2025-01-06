#pragma once
#include "Object3d.h"

class SkyDome
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 背景描画
	/// </summary>
	void BackGroundDraw();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 前景描画
	/// </summary>
	void ForeGroundDraw();

private:

	std::unique_ptr<Object3d> object_; // ポインタで管理
	std::unique_ptr<Camera> camera_;   // ポインタで管理

};

