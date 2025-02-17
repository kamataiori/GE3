#pragma once
#include "BaseScene.h"
#include "Object3d.h"
#include "MapEditor.h"

class MapScene : public BaseScene
{
public:

	//------メンバ関数------

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 背景描画
	/// </summary>
	void BackGroundDraw() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 前景描画
	/// </summary>
	void ForeGroundDraw() override;

private:

	MapEditor mapEditor;

	//3Dカメラの初期化
	std::unique_ptr<Camera> camera_;

};

