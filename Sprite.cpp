#include "Sprite.h"
#include "MathFunctions.h"

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	//引数で受け取ってメンバ変数に記録する
	this->spriteCommon = spriteCommon;

	// 頂点データを作成
	CreateVertexData();

	// インデックスデータを作成
	CreateIndexData();

	// マテリアルデータの初期化
	CreateMaterialData();

	// 座標変換行列データの初期化
	CreateTransformationMatrixData();

	////////=========組み合わせて使う=========////

	////Textureを読んで転送する
	//DirectX::ScratchImage mipImages = spriteCommon->GetDxCommon()->LoadTexture("./Resources/uvChecker.png");
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = spriteCommon->GetDxCommon()->CreateTextureResource(metadata);
	//spriteCommon->GetDxCommon()->UploadTextureData(textureResource, mipImages);

	//// Textureを読んで転送する2
	//DirectX::ScratchImage mipImages2 = spriteCommon->GetDxCommon()->LoadTexture("./Resources/monsterBall.png");
	////DirectX::ScratchImage mipImages2 = spriteCommon->GetDxCommon()->LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = spriteCommon->GetDxCommon()->CreateTextureResource(metadata2);
	//spriteCommon->GetDxCommon()->UploadTextureData(textureResource2, mipImages2);

	////metaDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = metadata.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  //2Dテクスチャ
	//srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	////metaDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata2.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  //2Dテクスチャ
	//srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//////SRVを作成するDescriptorHeapの場所を決める
	//textureSrvHandleCPU = GetCPUDescriptorHandle(spriteCommon->GetDxCommon()->GetSrvDescriptorHeap().Get(), spriteCommon->GetDxCommon()->GetDescriptorSizeSRV(), 1);//srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//textureSrvHandleGPU = GetGPUDescriptorHandle(spriteCommon->GetDxCommon()->GetSrvDescriptorHeap().Get(), spriteCommon->GetDxCommon()->GetDescriptorSizeSRV(), 1);//srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	////SRVを作成するDescriptorHeapの場所を決める
	//textureSrvHandleCPU2 = GetCPUDescriptorHandle(spriteCommon->GetDxCommon()->GetSrvDescriptorHeap().Get(), spriteCommon->GetDxCommon()->GetDescriptorSizeSRV(), 2);
	//textureSrvHandleGPU2 = GetGPUDescriptorHandle(spriteCommon->GetDxCommon()->GetSrvDescriptorHeap().Get(), spriteCommon->GetDxCommon()->GetDescriptorSizeSRV(), 2);


	//////SRVの生成
	//spriteCommon->GetDxCommon()->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	//spriteCommon->GetDxCommon()->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
}

void Sprite::Update()
{
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));

	materialDataSprite->uvTransform = uvTransformMatrix;

	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataSprite->World = worldMatrixSprite;
}

void Sprite::Draw()
{
	//Spriteの描画。変更が必要なものだけ変更する
	spriteCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	spriteCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定

	//マテリアルCBufferの場所を設定
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, spriteCommon->GetDxCommon()->GetGPUDescriptorHandle(spriteCommon->GetDxCommon()->GetSrvDescriptorHeap().Get(), spriteCommon->GetDxCommon()->GetDescriptorSizeSRV(), 2));
	//描画！（DrawCall/ドローコール）
	////dxCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);
	spriteCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateVertexData()
{
	// 頂点リソースを作成
	vertexResourceSprite = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//////=========ResourceSpriteにデータを書き込む=========////

	//VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//１枚目の三角形
	//左下
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//左上
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f,0.0f };
	//右下
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

	//２枚目の三角形
	//左上
	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f,0.0f };

	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
}

void Sprite::CreateIndexData()
{
	//Resourceを作成
	indexResourceSprite = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);

	//リソースの先頭アドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	//////=========IndexResourceにデータを書き込む=========////

	//インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
}

void Sprite::CreateMaterialData()
{
	// マテリアル用のリソースを作成
	materialResourceSprite = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));

	//書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//SpriteはLightingしないのfalseを設定する
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform = MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrixData()
{
	// 座標変換行列用のリソースを作成
	transformationMatrixResourceSprite = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//単位行列を書き込んでおく
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();

}
