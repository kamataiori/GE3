#pragma once
#include "Object3dCommon.h"
#include "Model.h"
#include <fstream>
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Transform.h"

//---前方宣言---//
class Object3dCommon;

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
	};

	//平行光源
	struct DirectionalLight {
		Vector4 color;  //!<ライトの色
		Vector3 direction;  //!<ライトの向き
		float intensity;  //!<輝度
	};


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Object3dCommon* object3dCommon);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

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


	//--------Setter--------//
	//ModelのSetter
	void SetModel(Model* model) { this->model_ = model; }

	//--------setter--------//
	void SetScale(const Vector3& scale) { this->transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { this->transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { this->transform.translate = translate; }

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
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;  //TransformMatrix用Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderResource;  //平行光源用
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;
	DirectionalLight* directionalLightData = nullptr;


	//-------Transform--------//

	Transform transform;
	Transform cameraTransform;

};

