#include "Framework.h"

void Framework::Initialize()
{
	// WindowsAPIの初期化
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();

	// 入力の初期化
	input = std::make_unique<Input>();
	input->Initialize(winApp.get());

	// DirectXの初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp.get());

	// Sprite共通部の初期化
	SpriteCommon::GetInstance()->Initialize();

	// 3Dオブジェクト共通部の初期化
	Object3dCommon::GetInstance()->Initialize();

	// モデル共通部の初期化
	modelCommon = std::make_unique<ModelCommon>();
	modelCommon->Initialize();
}

void Framework::Finalize()
{
	// 入力の解放
	input.reset();

	// DirectXの解放
	CloseHandle(dxCommon->GetFenceEvent());

	// WinAppの終了処理
	winApp->Finalize();
	// 各クラスの解放
	spriteCommon.reset();
	spriteCommon->Finalize();
	object3dCommon.reset();
	object3dCommon->Finalize();
	modelCommon.reset();
	dxCommon->Finalize();
}

void Framework::Update()
{
	// Windowsにメッセージが来てたら最優先で処理させる
	if (winApp->ProcessMesage()) {
		endRequest_ = true;
	}

	// 入力の更新
	input->Update();
}

void Framework::Run()
{
	// ゲームの初期化
	Initialize();

	while (true) // ゲームループ
	{
		// 毎フレーム更新
		Update();
		//終了リクエストが来たら抜ける
		if (IsEndRequest()) {
			break;
		}
		//描画
		Draw();
	}
	//ゲームの終了
	Finalize();
}
