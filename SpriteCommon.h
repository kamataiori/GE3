#pragma once
#include "DirectXCommon.h"
#include "Logger.h"

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
	/// ルートシグネチャの作成
	/// </summary>
	void RootSignature();

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	void GraphicsPipelineState();

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void CommonSetting();


	////======ゲッター関数======////

	DirectXCommon* GetDxCommon() const { return dxCommon_; }




private:

	DirectXCommon* dxCommon_;

};

