#include "SpriteCommon.h"

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	//引数で受け取ってメンバ変数に記録する
	dxCommon_ = dxCommon;

	//グラフィックスパイプラインの生成
	GraphicsPipelineState();
}

void SpriteCommon::RootSignature()
{

}

void SpriteCommon::GraphicsPipelineState()
{
	//ルートシグネチャの生成
	RootSignature();
}

void SpriteCommon::CommonSetting()
{
	//ルートシグネチャをセットするコマンド


	//グラフィックスパイプラインをセットするコマンド


	//プリミティブトポロジーをセットするコマンド
}
