#include "Object3d.h"
#include "MathFunctions.h"
#include "TextureManager.h"

void Object3d::Initialize()
{
	//引数で受け取ってメンバ変数に記録する
	this->object3dCommon_ = Object3dCommon::GetInstance();

	// 座標変換行列データの初期化
	CreateTransformationMatrixData();

	//平行光源の初期化
	CreateDirectionalLightData();

	// 光源のカメラ位置の初期化
	CreateCameraShaderData();

	//Transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.0f,3.14f,0.0f},{0.0f,/*4.0f*/1.0f,-10.0f} };

	//デフォルトカメラをセット
	this->camera = object3dCommon_->GetDefaultCamera();

}

void Object3d::Update()
{
	// model_ から modelData を取得
	const Model::ModelData& modelData = model_->GetModelData();

	//TransformからworldMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//cameraTransformからcameraMatrixを作る
	if (cameraManager_) {
		Camera* camera = cameraManager_->GetCurrentCamera();
		if (camera) {
			Matrix4x4 viewProjectionMatrix = camera->GetViewProjectionMatrix();
			worldviewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		}
	}
	else {
		worldviewProjectionMatrix = worldMatrix;
	}

	transformationMatrixData->WVP = Multiply(modelData.rootNode.localMatrix , worldviewProjectionMatrix);
	transformationMatrixData->World = Multiply(modelData.rootNode.localMatrix , worldMatrix);

	//ライトの位置にカメラの位置をを入れる
	cameraLightData->worldPosition = cameraTransform.translate;

	//transform.rotate.y += 0.06f;
}



void Object3d::ImGuiUpdate(const std::string& Name)
{
	std::string nodeName = "object3d_" + Name;

	ImGui::Begin("object3d");
	if (ImGui::TreeNode(nodeName.c_str())) {
		ImGui::DragFloat3("translate", &transform.translate.x);
		ImGui::DragFloat3("scale", &transform.scale.x);
		ImGui::DragFloat3("rotate", &transform.rotate.x);
		ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 0.01f);
		ImGui::DragFloat3("CameraTransform", &cameraTransform.translate.x, 0.01f);
		//ImGui::Checkbox("usebillboardMatrix", &usebillboardMatrix);
		ImGui::DragFloat3("directionalLight", &directionalLightData->direction.x, 0.01f);
		directionalLightData->direction = Normalize(directionalLightData->direction);
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

	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, CameraShaderResource->GetGPUVirtualAddress());

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

void Object3d::CreateCameraShaderData()
{
	//平行光源用のリソースを作る
	CameraShaderResource = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
	//データを書き込む
	//書き込むためのアドレスを取得
	CameraShaderResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraLightData));
	//デフォルト値はとりあえず以下のようにしておく
	cameraLightData->worldPosition = {};
}

void Object3d::SetCameraManager(CameraManager* cameraManager)
{
	this->cameraManager_ = cameraManager;
}
