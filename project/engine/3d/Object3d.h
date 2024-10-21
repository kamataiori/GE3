#pragma once
#include "Object3dCommon.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "CameraManager.h"
#include <fstream>
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include <numbers>

//---前方宣言---//
class Object3dCommon;
class Camera;

class Object3d
{
public:

	static const int kWindowWidth = 1280;
	static const int kWindowHeight = 720;

	//--------構造体--------//

	//TransformationMatrixを拡張する
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
	};

	//平行光源
	struct DirectionalLight {
		Vector4 color;  //!<ライトの色
		Vector3 direction;  //!<ライトの向き
		float intensity;  //!<輝度
	};

	struct CameraForGPU {
		Vector3 worldPosition;
	};

	//ポイントライト
	struct PointLight {
		Vector4 color;   //!<ライトの色
		Vector3 position;  //!ライトの位置
		float intensity;  //!<輝度
		float radius;  //! ライトの届く最大距離 
		float decay;  //!減衰率
	};

	//スポットライト
	struct SpotLight {
		Vector4 color;  //!<ライトの色
		Vector3 position;  //!<ライトの位置
		float intensity;  //!<輝度
		Vector3 direction;  //!<スポットライトの方向
		float distance;  //!<ライトの届く最大距離
		float decay;  //!減衰率
		float cosAngle;  //!<スポットライトの余弦
		float padding[2];
	};

	~Object3d() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ImGuiの更新
	/// </summary>
	void ImGuiUpdate(const std::string& Name);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 座標変換行列データの初期化
	/// </summary>
	void CreateTransformationMatrixData();

	/// <summary>
	/// 平行光源の初期化
	/// </summary>
	void CreateDirectionalLightData();

	/// <summary>
	/// 光源のカメラ位置の初期化
	/// </summary>
	void CreateCameraShaderData();

	/// <summary>
	/// ポイントライトの初期化
	/// </summary>
	void CreatePointLightData();

	/// <summary>
	/// スポットライトの初期化
	/// </summary>
	void CreateSpotLightData();

	//--------Setter--------//
	//ModelのSetter
	//void SetModel(Model* model) { this->model_ = model; }
	void SetModel(const std::string& filePath) { model_ = ModelManager::GetInstance()->FindModel(filePath); }

	//--------setter--------//
	void SetScale(const Vector3& scale) { this->transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { this->transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { this->transform.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }
	// カメラの設定
	void SetCameraManager(CameraManager* cameraManager);

	//--------getter--------//
	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }


private:
	//Object3dCommonの初期化
	Object3dCommon* object3dCommon_ = nullptr;
	//Modelの初期化
	Model* model_ = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;  // TransformMatrix用Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderResource;  // 平行光源用
	Microsoft::WRL::ComPtr<ID3D12Resource> CameraShaderResource;  // シェーダーのカメラ位置
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;  // ポイントライト用
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource;  // スポットライト用
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;
	DirectionalLight* directionalLightData = nullptr;
	CameraForGPU* cameraLightData = nullptr;
	PointLight* pointLightData = nullptr;
	SpotLight* spotLightData = nullptr;

	//Cameraの初期化
	Camera* camera = nullptr;
	 CameraManager* cameraManager_ = nullptr;

	Matrix4x4 worldviewProjectionMatrix;


	////-------Transform--------//

	Transform transform;
	Transform cameraTransform;

};

