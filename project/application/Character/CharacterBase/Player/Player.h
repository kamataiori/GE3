#pragma once
#include "CharacterBase.h"
#include "Collider.h"
#include "SphereCollider.h"

class Player : public CharacterBase, public SphereCollider
{
public:

	Player(BaseScene* baseScene_) : CharacterBase(baseScene_),SphereCollider(sphere){}

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:
	/// <summary>
	/// 移動
	/// </summary>
	void UpdateMovement();

private:

	// 移動速度
	const float velocity = 0.4f;
	bool isJumping = false; // ジャンプ中かどうかを判定
	float jumpVelocity = 0.0f; // ジャンプの垂直速度
	const float gravity = -0.1f; // 重力
	const float jumpPower = 1.25f; // ジャンプの初速度

};

