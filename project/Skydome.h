#pragma once
#include "Object3d.h"

class Skydome
{
public:  // publicメンバ関数

	// コンストラクタに BaseScene* を受け取る
	Skydome(BaseScene* baseScene) : baseScene_(baseScene) {}

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

private:  // privateメンバ変数

	// BaseScene のポインタを保持
	BaseScene* baseScene_;

	// 3Dオブジェクトの初期化
	std::unique_ptr<Object3d> object_ = std::make_unique<Object3d>(baseScene_);
};

