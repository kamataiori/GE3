#pragma once
#include <memory>
#include <WinApp.h>
#include <Input.h>
#include <DirectXCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <ModelCommon.h>

class Framework
{
public:
	//------メンバ関数------//

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 終了チェック
	/// </summary>
	virtual bool IsEndRequest() { return endRequest_; }

	/// <summary>
	/// 実行
	/// </summary>
	void Run();

public:
	//------メンバ変数------//

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~Framework() = default;

protected:
	// WinAppの初期化
	std::unique_ptr<WinApp> winApp = nullptr;
	// Inputの初期化
	std::unique_ptr<Input> input = nullptr;
	// DxCommonの初期化
	std::unique_ptr<DirectXCommon> dxCommon = nullptr;

	// Sprite共通部の初期化
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	// 3Dオブジェクト共通部の初期化
	std::unique_ptr<Object3dCommon> object3dCommon = nullptr;
	// モデル共通部の初期化
	std::unique_ptr<ModelCommon> modelCommon = nullptr;

private:
	// ゲーム終了フラグ
	bool endRequest_ = false;

};
