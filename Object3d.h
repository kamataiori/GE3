#pragma once
#include "Object3dCommon.h"
#include <fstream>
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"

//---前方宣言---//
class Object3dCommon;

class Object3d
{
public:
	//--------構造体--------//

	//頂点データの拡張
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	//マテリアルを拡張する
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	//MaterialData構造体
	struct MaterialData {
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	//ModelData構造体
	struct ModelData {
		std::vector<VertexData>vertices;
		MaterialData material;
	};

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
	/// 頂点データを作成
	/// </summary>
	void CreateVertexData();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void  CreateMaterialData();

	/// <summary>
	/// 座標変換行列データの初期化
	/// </summary>
	void CreateTransformationMatrixData();

	/// <summary>
	/// 平行光源の初期化
	/// </summary>
	void CreateDirectionalLightData();


	/// <summary>
	/// .mtlファイルの読み取り
	/// </summary>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// .objファイルの読み取り
	/// </summary>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);



private:
	//Object3dCommonの初期化
	Object3dCommon* object3dCommon_ = nullptr;

	//モデル読み込み
	ModelData modelData;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;  // 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;  //// マテリアル用の定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;  //TransformMatrix用Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderResource;  //平行光源用
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	DirectionalLight* directionalLightData = nullptr;
	// バッファリソースの使い道を補完するビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};

