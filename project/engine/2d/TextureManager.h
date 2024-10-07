#pragma once
#include "DirectXCommon.h"
#include <string>

class TextureManager
{
private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

public:
	//シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	//終了
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void LoadTexture(const std::string& filePath);

	/// <summary>
	/// SRVインデックスの開始番号
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	/// <summary>
	/// テクスチャ番号からGPUハンドルを取得
	/// </summary>
	/// <param name="textureIndex"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	/// <summary>
	/// メタデータを取得
	/// </summary>
	/// <param name="textureIndex"></param>
	/// <returns></returns>
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

private:
	//テクスチャ1枚分のデータ
	struct TextureData {
		std::string filePath;  //画像のファイルパス
		DirectX::TexMetadata metadata;  //画像の幅、高さなどの情報
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;  //テクスチャリソース
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;  //SRV作成時に必要なCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;  //描画コマンドに必要なGPUハンドル
	};

	//テクスチャデータ
	std::vector<TextureData> textureDatas;

	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

	DirectXCommon* dxCommon_ = nullptr;



};

