#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <cassert>
#define _USE_MATH_DEFINES
#include "math.h"
#include <fstream>
#include <sstream>
#include "wrl.h"
#include "ResourceObject.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "Transform.h"
#include "MathFunctions.h"
#include "externals/DirectXTex/DirectXTex.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//ImguiのWindowProcの改造
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


//BlendMode
enum BlendMode {
	//!< ブレンドなし
	kBlendModeNone,
	//!< 通常αブレンド。デフォルト。Src * SrcA + Dest * (1 - SrcA)
	kBlendModeNormal,
	//!< 加算。Src * SrcA + Dest * 1
	kBlendModeAdd,
	//!< 減算。Dest * 1 - Src * SrcA
	kBlendModeSubtract,
	//!< 乗算。Src * 0 + Dest * Src
	kBlendModeMultiply,
	//!< スクリーン。Src * (1 - Dest) + Dest * 1
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

BlendMode mode = kBlendModeNormal;

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;


	//////=========COMの初期化=========////

	CoInitializeEx(0, COINITBASE_MULTITHREADED);

	//ポインタ
	WinApp* winApp = nullptr;

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	////ウィンドウを表示する
	//ShowWindow(hwnd, SW_SHOW);

	//出力ウィンドウへの文字出力
	//OutputDebugStringA("Hello,DirectX!\n");


	/*MSG msg{};*/

//ポインタ
	DirectXCommon* dxCommon = nullptr;

	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//Sprite共通部の初期化
	SpriteCommon* spriteCommon = nullptr;
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

	//Spriteの初期化
	/*Sprite* sprite = nullptr;
	sprite = new Sprite();
	sprite->Initialize(spriteCommon);*/

	////-----Spriteの更新処理と描画処理-----////
	float offsetX = 100.0f;  // 各スプライトのX座標をずらすオフセット値
	float initialX = 100.0f; // 初期X座標
	// 小さく描画するための初期スケールを設定
	Vector2 initialSize(80.0f, 80.0f);  // 例として50x50のサイズに設定
	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 6; ++i)
	{
		Sprite* sprite = new Sprite();
		if (i % 2 == 0) {
			// 0, 2, 4 は "Resources/uvChecker.png"
			sprite->Initialize(spriteCommon, "Resources/uvChecker.png");
		}
		else {
			// 1, 3, 5 は "Resources/monsterBall.png"
			sprite->Initialize(spriteCommon, "Resources/monsterBall.png");
		}
		sprites.push_back(sprite);
	}

	//3Dオブジェクト共通部の初期化
	Object3dCommon* object3dCommon = nullptr;
	object3dCommon = new Object3dCommon();
	object3dCommon->Initialize(dxCommon);

	//3Dオブジェクトの初期化
	Object3d* plane = new Object3d();
	plane->Initialize(object3dCommon);

	Object3d* axis = new Object3d();
	axis->Initialize(object3dCommon);

	//モデル共通部の初期化
	ModelCommon* modelCommon = nullptr;
	modelCommon = new ModelCommon();
	modelCommon->Initialize(dxCommon);

	//モデル読み込み
	ModelManager::GetInstance()->Initialize(dxCommon);
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	plane->SetModel("plane.obj");
	axis->SetModel("axis.obj");

	//モデルにSRTを設定
	plane->SetScale({ 1.0f, 1.0f, 1.0f });
	plane->SetRotate({ 0.0f,(45.0f), 0.0f });
	plane->SetTranslate({ (0.0f), 0.0f, 0.0f });

	axis->SetScale({ 1.0f, 1.0f, 1.0f });
	axis->SetRotate({ 45.0f,(0.0f), 0.0f });
	axis->SetTranslate({ (3.0f), 0.0f, 0.0f });




	//////=========入力の初期化=========////

	//ポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(winApp);


	bool useMonsterBall = true;
	Vector4 color = { 1,1,1,1 };




	//メインループ
	//ウィンドウの×ボタンが押されるまでループ
	while (true) {

		//Windowsにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMesage())
		{
			//ゲームループを抜ける
			break;
		}

		////=========Imguiに、ここからフレームが始まる旨を告げる=========////

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		////=========入力の更新=========////

		input->Update();

		//===================================
		//ここからゲームの処理
		//===================================


		////-----各3DObjectの更新処理-----////

		plane->Update();
		axis->Update();

		plane->ImGuiUpdate("plane");
		axis->ImGuiUpdate("axis");


		////-----各Spriteの更新処理-----////
		for (size_t i = 0; i < sprites.size(); ++i) {
			Sprite* sprite = sprites[i];

			// 現在の座標を変数で受け取る
			Vector2 position = sprite->GetPosition();
			position.x = initialX + i * offsetX;  // X座標をずらす

			//// 座標を変更
			//position.x += 1.1f;
			//position.y += 1.1f;
			position.y = 100.0f;

			//// 変更を反映する
			sprite->SetPosition(position);


			////角度を変化させる
			float rotation = sprite->GetRotation();
			rotation += 0.08f;
			sprite->SetRotation(rotation);

			////色を変化させる
			Vector4 color = sprite->GetColor();
			color.x += 0.01f;
			if (color.x > 1.0f)
			{
				color.x -= 1.0f;
			}
			sprite->SetColor(color);

			//スケールを変化させる
			/*Vector2 size = sprite->GetSize();
			size.x -= 0.6f;
			size.y -= 0.6f;
			sprite->SetSize(size);*/
			//各スプライトを初期サイズに設定
			sprite->SetSize(initialSize);

			//アンカーポイント
			Vector2 currentAnchor = sprite->GetAnchorPoint();
			sprite->SetAnchorPoint({ 0.5f, 0.5f });

			//フリップ
			bool flipX = sprite->GetFlipX();
			sprite->SetFlipX(false);
			bool flipY = sprite->GetFlipY();
			sprite->SetFlipY(false);

			//テクスチャ範囲指定
			Vector2 leftTop = sprite->GetTextureLeftTop();
			Vector2 size = sprite->GetTextureSize();
			if (i % 2 == 0)
			{
				sprite->SetTextureLeftTop({ 0.0f, 0.0f });
				sprite->SetTextureSize({ 64.0f, 64.0f });
			}
			else
			{
				sprite->SetTextureLeftTop({ 600.0f, 280.0f });
				sprite->SetTextureSize({ 200.0f, 200.0f });
			}

			// 各スプライトを更新
			sprite->Update();

		}



		//ImGuiの内部コマンドを生成する
		ImGui::Render();


		//DirectXの描画前処理。全ての描画に共通のグラフィックスコマンドを積む
		dxCommon->PreDraw();


		//3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
		object3dCommon->CommonSetting();

		//================================
		//ここから3Dオブジェクト個々の描画
		//================================

		  // 各オブジェクトの更新と描画
		//for (auto object3d : object3dList) {
		//	object3d->Draw();    // 描画
		//}
		plane->Draw();
		axis->Draw();

		//================================
		//ここまで3Dオブジェクト個々の描画
		//================================


		//Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
		spriteCommon->CommonSetting();

		//================================
		//ここからSprite個々の描画
		//================================



		////=========コマンドを積む=========////

		//一般的には2dは最後。3dを描画した後、ImGuiの前に描画する


		//sprite->Draw();
		for (Sprite* sprite : sprites) {

			// スプライトを描画する
			sprite->Draw();
		}



		//================================
		//ここまでSprite個々の描画
		//================================


		////////=========実際commandListのImGuiの描画コマンドを積む=========////

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

		//描画後処理
		dxCommon->PostDraw();



	}

	////=========解放処理=========////

	//解放処理

	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	delete input;

	CloseHandle(dxCommon->GetFenceEvent());


	////=========COMの終了処理=========////

	//WinAppの終了処理
	winApp->Finalize();
	//テクスチャマネージャーの終了処理
	TextureManager::GetInstance()->Finalize();

	// 3D オブジェクト解放
	delete plane;
	delete axis;

	// Sprite の解放
	for (Sprite* sprite : sprites) {
		delete sprite;
	}

	// 各クラスの解放
	delete spriteCommon;
	delete object3dCommon;
	ModelManager::GetInstance()->Finalize();
	delete modelCommon;
	delete dxCommon;
	delete winApp;

	return 0;

}