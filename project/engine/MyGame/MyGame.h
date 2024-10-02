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
#include "Camera.h"
#include "CameraManager.h"
#include "Transform.h"
#include "MathFunctions.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

class MyGame
{
public:
	//------メンバ関数------//

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//------getter------//

	//終了フラグのチェック
	bool IsEndRequest() { return endRequest_; }

private:
	//------メンバ変数------//
     //WindowsAPIの初期化
    std::unique_ptr<WinApp> winApp = nullptr;
    //入力の初期化
    std::unique_ptr<Input> input = nullptr;
    //DirectXの初期化
    std::unique_ptr<DirectXCommon> dxCommon = nullptr;
    //Sprite共通部の初期化
    std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
    //3Dオブジェクト共通部の初期化
    std::unique_ptr<Object3dCommon> object3dCommon = nullptr;
    //モデル共通部の初期化
    std::unique_ptr<ModelCommon> modelCommon = nullptr;
    //3Dカメラの初期化
    std::unique_ptr<CameraManager> cameraManager = nullptr;

    // ゲーム終了フラグ
    bool endRequest_ = false;

    //------ゲーム関係------//

    float offsetX = 100.0f;  //各スプライトのX座標をずらすオフセット値
    float initialX = 100.0f; //初期X座標

    // Spriteの初期化
    // 小さく描画するための初期スケールを設定
    Vector2 initialSize = { 80.0f,80.0f };
    std::vector<std::unique_ptr<Sprite>> sprites;

    // 3Dオブジェクトの初期化
    std::unique_ptr<Object3d> plane = std::make_unique<Object3d>();
    std::unique_ptr<Object3d> axis = std::make_unique<Object3d>();

    std::unique_ptr<Camera> camera1 = std::make_unique<Camera>();
    std::unique_ptr<Camera> camera2 = std::make_unique<Camera>();
    bool cameraFlag = false;  //ImGuiで制御するカメラの切り替えフラグ

};
