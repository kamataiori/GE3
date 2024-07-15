#pragma once
#include "DirectXCommon.h"

class SpriteCommon
{
public:

	///===========================
	// メンバ関数
	///===========================

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void CommonDrawingSettings();


	///===========================
	// ゲッター
	///===========================

	DirectXCommon* GetDxCommon() const { return dxCommon_; }



private:

	DirectXCommon* dxCommon_;

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	void RouteSignature();

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	void GraphicsPipeline();

};

