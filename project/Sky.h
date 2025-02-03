#pragma once
#include <memory>
#include <Object3d.h>

class Sky
{
public:
	Sky(BaseScene* baseScene) : baseScene_(baseScene) { object3d_ = std::make_unique<Object3d>(baseScene_); }

	void Initialize();

	void Update();

	void Draw();

	void SetCamera(Camera* camera) { object3d_->SetCamera(camera); }

	// Transform のセッター
	void SetTransform(const Transform& transform) { transform_ = transform; }
	// Transform のゲッター
	const Transform& GetTransform() const { return transform_; }

private:

	BaseScene* baseScene_;

	std::unique_ptr<Object3d> object3d_;

	Transform transform_;
};

