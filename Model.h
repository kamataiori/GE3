#pragma once
#include <fstream>

#include "ModelCommon.h"
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"

//---前方宣言---//
class ModelCommon;

class Model
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


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelCommon* modelCommon,const std::string& directorypath,const std::string& filename);

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
	/// .objファイルの読み取り
	/// </summary>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

private:
	//ModelCommonの初期化
	ModelCommon* modelCommon_ = nullptr;

	//モデル読み込み
	ModelData modelData;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;  // 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;  //// マテリアル用の定数バッファ
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;
	// バッファリソースの使い道を補完するビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;


};

