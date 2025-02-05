#pragma once
#include "SphereCollider.h"
#include <Object3d.h>

class Hammer : public SphereCollider
{
public:

	Hammer(BaseScene* baseScene) : baseScene_(baseScene), SphereCollider(sphere) { object3d_ = std::make_unique<Object3d>(baseScene_); }

	void Initialize();

	void Update();

	void Draw();

	void OnCollision() override;

	void SetCamera(Camera* camera) { object3d_->SetCamera(camera); }

	// Transform のセッター
	void SetTransform(const Transform& transform) { transform_ = transform; }
	// Transform のゲッター
	const Transform& GetTransform() const { return transform_; }

	Object3d* GetObject3d() { return object3d_.get(); }

	// コライダーサイズを変更
	void SetColliderSize(float radius) { sphere.radius = radius; }

	void SetColliderCenter(Vector3 center) { sphere.center = center; }


private:

	BaseScene* baseScene_;

	std::unique_ptr<Object3d> object3d_;

	Transform transform_;

	Transform CollisionTransform_;
};

