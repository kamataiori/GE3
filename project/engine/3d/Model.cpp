#include "Model.h"
#include "MathFunctions.h"
#include "TextureManager.h"
#include <Object3d.h>

void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename)
{
	//引数で受け取ってメンバ変数に記録する
	this->modelCommon_ = modelCommon;

	//モデル読み込み
	modelData = LoadModelFile(directorypath, filename);

	// 頂点データを作成
	CreateVertexData();

	// マテリアルデータの初期化
	CreateMaterialData();

	//.objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	//読み込んだテクスチャの番号を取得
	modelData.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);
}

void Model::Draw()
{
	//VertexBufferViewを設定
	modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	//マテリアルCBufferの場所を設定
	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定
	//modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, modelCommon_->GetDxCommon()->GetGPUDescriptorHandle(SrvManager::GetInstance()->GetSrvDescriptorHeap().Get(), SrvManager::GetInstance()->GetDescriptorSizeSRV(), 1));

	// SRVのDescriptorTableを設定,テクスチャを指定
	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(2, modelData.material.textureIndex);

	//描画!（DrawCall/ドローコール）
	modelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

void Model::CreateVertexData()
{
	// 頂点リソースを作成
	vertexResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * modelData.vertices.size();
	//頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//頂点データをリソースにコピー
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

void Model::CreateMaterialData()
{
	// マテリアル用のリソースを作成
	materialResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));

	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//SpriteはLightingしないfalseを設定する
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->shininess = 50.0f;
}

//.mtlファイル読み取り
Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	//1,中で必要となる変数の宣言
	MaterialData materialData;  //構築するMaterialData
	std::string line;  //ファイルから読んだ1行を格納するもの

	//2,ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);  //ファイルを開く
	assert(file.is_open());  //とりあえず開けなかったら止める

	//3,実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;  //先頭の識別子を読む

		//identifireに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	//4,MaterialDataを返す
	return materialData;
}

Model::Node Model::ReadNode(aiNode* node)
{
	Node result;

	aiMatrix4x4 aiLocalMatrix = node->mTransformation;  // nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();  // 列ベクトル形式を行ベクトル形式に転置
	result.localMatrix.m[0][0] = aiLocalMatrix[0][0];
	result.localMatrix.m[0][1] = aiLocalMatrix[0][1];
	result.localMatrix.m[0][2] = aiLocalMatrix[0][2];
	result.localMatrix.m[0][3] = aiLocalMatrix[0][3];

	result.localMatrix.m[1][0] = aiLocalMatrix[1][0];
	result.localMatrix.m[1][1] = aiLocalMatrix[1][1];
	result.localMatrix.m[1][2] = aiLocalMatrix[1][2];
	result.localMatrix.m[1][3] = aiLocalMatrix[1][3];

	result.localMatrix.m[2][0] = aiLocalMatrix[2][0];
	result.localMatrix.m[2][1] = aiLocalMatrix[2][1];
	result.localMatrix.m[2][2] = aiLocalMatrix[2][2];
	result.localMatrix.m[2][3] = aiLocalMatrix[2][3];

	result.localMatrix.m[3][0] = aiLocalMatrix[3][0];
	result.localMatrix.m[3][1] = aiLocalMatrix[3][1];
	result.localMatrix.m[3][2] = aiLocalMatrix[3][2];
	result.localMatrix.m[3][3] = aiLocalMatrix[3][3];

	result.name = node->mName.C_Str();  // Node名を格納
	result.children.resize(node->mNumChildren);  // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

//.objファイル読み取り
Model::ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	// Assimpのインポーターを作成
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	// シーンを読み込む
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);
	assert(scene && scene->HasMeshes()); // シーンが有効でメッシュがあるか確認

	ModelData modelData;

	// Meshを解析する
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないMeshは非対応
		assert(mesh->HasTextureCoords(0)); // テクスチャ座標がないMeshは非対応

		// Faceを解析する
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); // 三角形のみサポート

			// Vertexを解析する
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];

				aiVector3D position = mesh->mVertices[vertexIndex];
				aiVector3D normal = mesh->mNormals[vertexIndex];
				aiVector3D texcoord = mesh->mTextureCoords[0][vertexIndex];

				VertexData vertex;
				vertex.position = { position.x, position.y, position.z, 1.0f };
				vertex.normal = { normal.x, normal.y, normal.z };
				vertex.texcoord = { texcoord.x, texcoord.y };

				// 右手系 -> 左手系変換
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;

				modelData.vertices.push_back(vertex);
			}
		}
	}

	// Materialを解析する
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	// ルートノードを解析してモデルデータに設定
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

bool Model::GetEnableLighting() const
{
	assert(materialData); // materialData が null でないことを確認
	return materialData->enableLighting != 0; // enableLighting が 0 でなければ true を返す
}
