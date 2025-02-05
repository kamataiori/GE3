#pragma once
#include "CharacterBase.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "OBBCollider.h"

class Enemy : public CharacterBase, public SphereCollider
{
public:

	Enemy(BaseScene* baseScene_) : CharacterBase(baseScene_), SphereCollider(sphere) {}

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void OnCollision() override;

	// HP のゲッター・セッター
	int GetHP() const { return hp_; }
	void SetHP(int hp) { hp_ = hp; }

	// 位置のゲッター
	Vector3 GetPosition() const { return object3d_->GetTranslate(); }

private:
	int hp_; // HP (初期値 100)
};

