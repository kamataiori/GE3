#include "TextureManager.h"
using namespace StringUtility;

TextureManager* TextureManager::instance = nullptr;
//ImGuiで0番目を使用するため、1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	srvManager_ = SrvManager::GetInstance();
	//SRVの数と同数
	textureDatas.reserve(SrvManager::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		// 読み込み済みなら早期return
		return;
	}

	//テクスチャ枚数上限チェック
	assert(SrvManager::GetInstance()->IsBelowMaxCount());

	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	////テクスチャデータを追加
	//textureDatas.resize(textureDatas.size() + 1);
	// テクスチャデータを追加して書き込む
	TextureData& textureData = textureDatas[filePath];

	//テクスチャデータ書き込み
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	// UploadTextureData を呼び出し、intermediateResource を取得
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCommon_->UploadTextureData(
		textureData.resource.Get(), mipImages, dxCommon_->GetDevice().Get(), dxCommon_->GetCommandList().Get()
	);

	// commandListをCloseし、commandQueue->ExecuteCommandListsを使いキックする
	dxCommon_->GetCommandList()->Close();
	ID3D12CommandList* commandLists[] = { dxCommon_->GetCommandList().Get() };
	dxCommon_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	// 実行を待つ
	dxCommon_->SetFenceValue(dxCommon_->GetFenceValue() + 1);
	dxCommon_->GetCommandQueue()->Signal(dxCommon_->GetFence().Get(), dxCommon_->GetFenceValue());
	if (dxCommon_->GetFence()->GetCompletedValue() < dxCommon_->GetFenceValue()) {
		dxCommon_->GetFence()->SetEventOnCompletion(dxCommon_->GetFenceValue(), dxCommon_->GetFenceEvent());
		WaitForSingleObject(dxCommon_->GetFenceEvent(), INFINITE);
	}

	// 実行が完了したので、allocatorとcommandListをResetして次のコマンドを積めるようにする
	dxCommon_->GetCommandAllocator()->Reset();
	dxCommon_->GetCommandList()->Reset(dxCommon_->GetCommandAllocator().Get(), nullptr);

	// intermediateResource はこの時点で解放可能
	intermediateResource.Reset();


	//テクスチャデータの要素数番号をSRVのインデックスを計算する
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
	textureData.srvIndex = srvIndex;
	srvManager_->Allocate();

	textureData.srvHandleCPU = dxCommon_->GetCPUDescriptorHandle(srvManager_->GetSrvDescriptorHeap().Get(), srvManager_->GetDescriptorSizeSRV(), srvIndex);
	textureData.srvHandleGPU = textureData.srvHandleGPU = dxCommon_->GetGPUDescriptorHandle(srvManager_->GetSrvDescriptorHeap().Get(), srvManager_->GetDescriptorSizeSRV(), srvIndex);

	//SRVの設定を行なう
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

	//SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	// テクスチャが存在するかチェック
	auto it = textureDatas.find(filePath);
	assert(it != textureDatas.end() && "テクスチャが存在しません");

	// 読み込み済みならSRVインデックスを返す
	return it->second.srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	// テクスチャが存在するかチェック
	assert(textureDatas.contains(filePath) && "テクスチャが存在しません");

	// テクスチャデータを取得
	TextureData& textureData = textureDatas[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	// テクスチャが存在するかチェック
	assert(textureDatas.contains(filePath) && "テクスチャが存在しません");

	// テクスチャデータを取得
	TextureData& textureData = textureDatas[filePath];
	return textureData.metadata;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath)
{
	// テクスチャが存在するかチェック
	assert(textureDatas.contains(filePath) && "テクスチャが存在しません");

	// テクスチャデータを取得
	TextureData& textureData = textureDatas[filePath];
	return textureData.srvIndex;
}
