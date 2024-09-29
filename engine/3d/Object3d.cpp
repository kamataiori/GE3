#include "Object3d.h"
#include "MathFunctions.h"
#include "TextureManager.h"

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	//引数で受け取ってメンバ変数に記録する
	this->object3dCommon_ = object3dCommon;

	// 座標変換行列データの初期化
	CreateTransformationMatrixData();

	//平行光源の初期化
	CreateDirectionalLightData();

	//Transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

}

void Object3d::Update()
{
	//TransformからworldMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//cameraTransformからcameraMatrixを作る
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	//cameraMatrixからviewMatrixを作る
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	//ProjectionMatrixを作って透視投影行列を書き込む
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

	Matrix4x4 worldviewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->WVP = worldviewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;

	transform.rotate.y += 0.06f;

}

void Object3d::ImGuiUpdate(const std::string& Name)
{
	std::string nodeName = "object3d_" + Name;

	ImGui::Begin("object3d");
	if (ImGui::TreeNode(nodeName.c_str())) {
		ImGui::DragFloat3("translate", &transform.translate.x);
		ImGui::DragFloat3("scale", &transform.scale.x);
		ImGui::DragFloat3("rotate", &transform.rotate.x);
		ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x);
		ImGui::DragFloat3("cameraScale", &cameraTransform.scale.x);
		ImGui::DragFloat3("cameraRotate", &cameraTransform.rotate.x);
		ImGui::TreePop();
	}
	ImGui::End();
}

void Object3d::Draw()
{
	//座標変換行列CBufferの場所を設定
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	//平行光源CBufferの場所を設定
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, shaderResource->GetGPUVirtualAddress());
	
	//3Dモデルが割り当てられていたら描画する
	if (model_) {
		model_->Draw();
	}
}

void Object3d::CreateTransformationMatrixData()
{
	// 座標変換行列用のリソースを作成
	transformationMatrixResource = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	//書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	//単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}

void Object3d::CreateDirectionalLightData()
{
	//平行光源用のリソースを作る
	shaderResource = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));

	//書き込むためのアドレスを取得
	shaderResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
}