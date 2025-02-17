#pragma once
#include <fstream>
#include "ModelCommon.h"
#include "SrvManager.h"
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "StructAnimation.h"

//---前方宣言---//
class ModelCommon;

class Model
{
public:
	//--------構造体--------//

	// 頂点データの拡張
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	// マテリアルを拡張する
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};

	// MaterialData構造体
	struct MaterialData {
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	// Node構造体
	struct Node {
		Matrix4x4 localMatrix;  // NodeのTransform
		std::string name;  // Nodeの名前
		std::vector<Node> children;  // 子供のNode
	};

	// ModelData構造体
	struct ModelData {
		std::vector<VertexData>vertices;
		MaterialData material;
		Node rootNode;
		bool isAnimation;
	};


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 頂点データを作成
	/// </summary>
	void CreateVertexData();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void  CreateMaterialData();

	/// <summary>
	/// .mtlファイルの読み取り
	/// </summary>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// Nodeでの階層構造
	/// </summary>
	static Node ReadNode(aiNode* node);

	/// <summary>
	/// .objファイルの読み取り
	/// </summary>
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);


	/// <summary>
    /// Animation解析の関数
    /// </summary>
	AnimationData LoadAnimationFile(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// ModelDataのGetter
	/// </summary>
	/// <returns></returns>
	const ModelData& GetModelData() const { return modelData; }

	// materialData->colorのゲッター
	const Vector4& GetMaterialColor() const { return materialData->color; }

	// materialData->colorのセッター
	void SetMaterialColor(const Vector4& color) { materialData->color = color; }

	// materialData->enableLightingのゲッター
	bool GetEnableLighting() const;

	// materialDataのゲッター
	Model::Material* GetMaterial() const { return materialData; }

private:
	// ModelCommonの初期化
	ModelCommon* modelCommon_ = nullptr;

	// モデル読み込み
	ModelData modelData;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;  // 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;  // マテリアル用の定数バッファ
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;
	// バッファリソースの使い道を補完するビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

};

