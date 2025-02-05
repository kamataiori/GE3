#pragma once
#include "CharacterBase.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "Hammer.h"

// プレイヤーの行動を管理する列挙型
enum class Behavior {
	kRoot,   // 通常（キー入力時に移動）
	kJump,   // ジャンプ中
	kFloat,   // 何もしていない（浮遊ギミック）
	kAttack,  // 攻撃中
	kattack2,
};

class Player : public CharacterBase
{
public:

	Player(BaseScene* baseScene_) : CharacterBase(baseScene_){ hammer_ = std::make_unique<Hammer>(baseScene_); }

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void SetCamera(Camera* camera);

public:
	// Hammer のゲッター
	Hammer* GetHammer() const { return hammer_.get(); }

	// Hammer のセッター
	void SetHammerTransform(const Transform& transform) { hammer_->SetTransform(transform); }

private:
	/// <summary>
	/// 移動
	/// </summary>
	void UpdateMovement();

	/// <summary>
	/// 浮遊ギミック初期化
	/// </summary>
	void InitializeFloatingGimmick();

	/// <summary>
	/// 浮遊ギミック更新
	/// </summary>
	void UpdateFloatingGimmick();

	/// <summary>
	/// ジャンプ処理
	/// </summary>
	void UpdateJump();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void UpdateAttack();

	void UpdateAttack2();

	Behavior behavior_ = Behavior::kRoot; // 現在の状態

	float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}


private:

	// 浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;

	// 浮遊移動のサイクル<frame>
	uint16_t Period = 0;

	// 移動速度
	const float velocity = 0.4f;
	bool isJumping = false; // ジャンプ中かどうかを判定
	float jumpVelocity = 0.0f; // ジャンプの垂直速度
	const float gravity = -0.1f; // 重力
	const float jumpPower = 1.25f; // ジャンプの初速度
	Vector3 jumpDirection_ = { 0.0f, 0.0f, 0.0f }; // ジャンプ時の移動方向

	// 武器
	std::unique_ptr<Hammer> hammer_;
	Transform hammerTransform;

};

