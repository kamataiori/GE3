#pragma once
#include "Light.h"

// 前方宣言
class SceneManager;

class BaseScene
{
public:
	//------メンバ関数------

	//仮想デストラクタ
	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// シーンマネージャーをシーンに貸し出すためのSetter
	/// </summary>
	/// <param name="sceneManager"></param>
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

	/// <summary>
	/// Lightのゲッター
	/// </summary>
	Light* GetLight() const {
		return light.get();  // unique_ptr から Light ポインタを返す
	}

private:
	// シーンマネージャー (借りてくる)
	SceneManager* sceneManager_ = nullptr;

	// ライトの初期化
	std::unique_ptr<Light> light = std::make_unique<Light>();

};

