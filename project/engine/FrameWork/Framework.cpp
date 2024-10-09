#include "Framework.h"

void Framework::Initialize()
{
	// WindowsAPIの初期化
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();

	// 入力の初期化
	Input::GetInstance()->Initialize(winApp.get());

	// DirectXの初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp.get());

	// Sprite共通部の初期化
	SpriteCommon::GetInstance()->Initialize();

	// テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize();

	// 3Dオブジェクト共通部の初期化
	Object3dCommon::GetInstance()->Initialize();

	// モデルマネージャーの初期化
	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	// モデル共通部の初期化
	modelCommon = std::make_unique<ModelCommon>();
	modelCommon->Initialize();


}

void Framework::Finalize()
{
	// DirectXの解放
	CloseHandle(dxCommon->GetFenceEvent());

	// WinAppの終了処理
	winApp->Finalize();
	// 各クラスの解放
	Input::GetInstance()->Finalize();
	// テクスチャマネージャーの終了処理
	TextureManager::GetInstance()->Finalize();
	// モデルマネージャーの終了処理
	ModelManager::GetInstance()->Finalize();
	// シーンマネージャーの終了処理
	SceneManager::GetInstance()->Finalize();
	delete sceneFactory_;
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

	// シーンマネージャーの更新処理
	SceneManager::GetInstance()->Update();

	// 入力の更新
	Input::GetInstance()->Update();
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
