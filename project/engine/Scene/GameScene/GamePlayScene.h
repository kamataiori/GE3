#pragma once
#include <Vector2.h>
#include <vector>
#include <memory>
#include <Sprite.h>
#include <Object3d.h>
#include "BaseScene.h"
#include "Audio.h"
#include "Light.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

class GamePlayScene : public BaseScene
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

	std::unique_ptr<Sprite> monsterBall = std::make_unique<Sprite>();

	// 3Dオブジェクトの初期化
	/*std::unique_ptr<Object3d> plane = nullptr;
	std::unique_ptr<Object3d> axis = nullptr;*/
	std::unique_ptr<Object3d> skyDome = nullptr;
	std::unique_ptr<Object3d> ground = nullptr;
	std::unique_ptr<Object3d> playerBase = nullptr;
	std::unique_ptr<Object3d> playerWeapon = nullptr;

	////3Dカメラの初期化
	// カメラを追加
	Camera mainCamera_;
	Camera topCamera_;
	Camera diagonalCamera_;
	Camera* currentCamera_ = nullptr; // 現在のカメラ

	Vector2 MonsterPosition;

private:
	bool isJumping = false; // ジャンプ中かどうかを判定
	float jumpVelocity = 0.0f; // ジャンプの垂直速度
	const float gravity = -0.2f; // 重力
	const float jumpPower = 2.5f; // ジャンプの初速度

	bool isAttacking = false; // 攻撃中かどうか
	float attackTimer = 0.0f; // 攻撃状態のタイマー
	const float attackDuration = 0.5f; // 攻撃状態の持続時間

private:
	void UpdatePlayerMovement(); // プレイヤー移動処理
	void UpdateCamera();         // カメラ追従処理

};

