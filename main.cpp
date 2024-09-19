#include <Windows.h>

#include <cstdint>
#include <string>
#include <format>
#include <cassert>
#define _USE_MATH_DEFINES
#include "math.h"
#include <fstream>
#include <sstream>
#include "wrl.h"

#include "ResourceObject.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "Transform.h"
#include "MathFunctions.h"
#include "externals/DirectXTex/DirectXTex.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//ImguiのWindowProcの改造
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

////頂点データの拡張
//struct VertexData {
//	Vector4 position;
//	Vector2 texcoord;
//	Vector3 normal;
//};

////マテリアルを拡張する
//struct Material {
//	Vector4 color;
//	int32_t enableLighting;
//	float padding[3];
//	Matrix4x4 uvTransform;
//};

//平行光源
struct DirectionalLight {
	Vector4 color;  //!<ライトの色
	Vector3 direction;  //!<ライトの向き
	float intensity;  //!<輝度
};

//MaterialData構造体
struct MaterialData {
	std::string textureFilePath;
};

//ModelData構造体
//struct ModelData {
//	std::vector<VertexData>vertices;
//	MaterialData material;
//};

//BlendMode
enum BlendMode {
	//!< ブレンドなし
	kBlendModeNone,
	//!< 通常αブレンド。デフォルト。Src * SrcA + Dest * (1 - SrcA)
	kBlendModeNormal,
	//!< 加算。Src * SrcA + Dest * 1
	kBlendModeAdd,
	//!< 減算。Dest * 1 - Src * SrcA
	kBlendModeSubtract,
	//!< 乗算。Src * 0 + Dest * Src
	kBlendModeMultiply,
	//!< スクリーン。Src * (1 - Dest) + Dest * 1
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

BlendMode mode = kBlendModeNormal;

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;


////////=========DepthStencilTextureを作る=========////

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResorce(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height)
{
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;  //Textureの幅
	resourceDesc.Height = height;  //Textureの高さ
	resourceDesc.MipLevels = 1;  //mipmapの数
	resourceDesc.DepthOrArraySize = 1;  //奥行き　or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;  //DepthStenciとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;  //サンプリングカウント、1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  //2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;  //DepthStenciとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;  //VRAM上に作る

	//深度値のクリア最適化設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;  //1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;  //フォーマット。Resorceと合わせる

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,  //Heapの設定
		D3D12_HEAP_FLAG_NONE,  //Heapの特殊な設定。特になし。
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,  //深度値を書き込む状態にしておく
		&depthClearValue,  //Clear最適値
		IID_PPV_ARGS(&resource)  //作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}


////////=========DescriptorHandleの取得の関数化=========////

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}


////////=========mtlファイルを読む関数=========////

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
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


////////=========Objファイルを読む関数=========////

//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
//{
//	//1,中で必要となる変数の宣言
//	ModelData modelData;  //構築するModelData
//	std::vector<Vector4> positions;  //位置
//	std::vector<Vector3> normals;  //法線
//	std::vector<Vector2> texcords;  //テクスチャ座標
//	std::string line;  //ファイルから読んだ1行を格納するもの
//
//	//2,ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename);  //ファイルを開く
//	assert(file.is_open());  //とりあえず開けなかったら止める
//
//	//3,実際にファイルを読み、ModelDataを構築していく
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;  //先頭の識別子を読む
//
//		//identifireに応じた処理
//		//頂点情報を読む
//		if (identifier == "v")
//		{
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			position.x *= -1.0f;
//			position.y *= -1.0f;
//			positions.push_back(position);
//		}
//		else if (identifier == "vt")
//		{
//			Vector2 texcord;
//			s >> texcord.x >> texcord.y;
//			//texcord.y = 1.0f - texcord.y;
//			texcord.x = 1.0f - texcord.x;
//			texcords.push_back(texcord);
//		}
//		else if (identifier == "vn")
//		{
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			//normal.x *= -1.0f;
//			normals.push_back(normal);
//		}
//		//三角形を作る
//		else if (identifier == "f")
//		{
//			VertexData triangle[3];
//			//面は三角形限定。その他は未対応
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
//			{
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				//頂点の要素へのIndexは[位置/uv/法線]で格納されているので、分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; ++element)
//				{
//					std::string index;
//					std::getline(v, index, '/');  //区切りでインデックスを読んでいく
//					elementIndices[element] = std::stoi(index);
//				}
//				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcord = texcords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				/*VertexData vertex = { position,texcord,normal };
//				modelData.vertices.push_back(vertex);*/
//				triangle[faceVertex] = { position,texcord,normal };
//			}
//			//頂点を逆順で登録することで、回り順を逆にする
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);
//		}
//		else if (identifier == "mtllib")
//		{
//			//materialTemplateLibraryファイルの名前を取得する
//			std::string materialFilename;
//			s >> materialFilename;
//			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
//			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//		}
//	}
//
//	//4,ModelDataを返す
//	return modelData;
//}



//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;


	//////=========COMの初期化=========////

	CoInitializeEx(0, COINITBASE_MULTITHREADED);

	//ポインタ
	WinApp* winApp = nullptr;

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	////ウィンドウを表示する
	//ShowWindow(hwnd, SW_SHOW);

	//出力ウィンドウへの文字出力
	//OutputDebugStringA("Hello,DirectX!\n");


	/*MSG msg{};*/

//ポインタ
	DirectXCommon* dxCommon = nullptr;

	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//Sprite共通部の初期化
	SpriteCommon* spriteCommon = nullptr;
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

	//Spriteの初期化
	/*Sprite* sprite = nullptr;
	sprite = new Sprite();
	sprite->Initialize(spriteCommon);*/

	////-----Spriteの更新処理と描画処理-----////
	float offsetX = 100.0f;  // 各スプライトのX座標をずらすオフセット値
	float initialX = 100.0f; // 初期X座標
	// 小さく描画するための初期スケールを設定
	Vector2 initialSize(80.0f, 80.0f);  // 例として50x50のサイズに設定
	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 6; ++i)
	{
		Sprite* sprite = new Sprite();
		if (i % 2 == 0) {
			// 0, 2, 4 は "Resources/uvChecker.png"
			sprite->Initialize(spriteCommon, "Resources/uvChecker.png");
		}
		else {
			// 1, 3, 5 は "Resources/monsterBall.png"
			sprite->Initialize(spriteCommon, "Resources/monsterBall.png");
		}
		sprites.push_back(sprite);
	}
	
	//3Dオブジェクト共通部の初期化
	Object3dCommon* object3dCommon = nullptr;
	object3dCommon = new Object3dCommon();
	object3dCommon->Initialize();

	//3Dオブジェクトの初期化
	Object3d* object3d = new Object3d();
	object3d->Initialize();



	//////=========VertexResourceを生成する=========////

	int  SphereVertex = 32 * 32 * 6;

	//モデル読み込み
	//ModelData modelData = LoadObjFile("Resources", "plane.obj");
	//ModelData modelData = LoadObjFile("Resources", "axis.obj");

	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(/*device,*/ sizeof(VertexData) * modelData.vertices.size());

	//球
	//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * SphreVertex);

	//三角形
	//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6);


	//Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResorce(device, WinApp::kClientWidth, WinApp::kClientHeight);

	//Sprite用の頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResorceSprite = dxCommon->CreateBufferResource(/*device,*/ sizeof(VertexData) * 6);

	//////=========Material用のResourceを作る=========////

	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(Material));
	////ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Matrix4x4));
	////マテリアルにデータを描き込む
	//Material* materialData = nullptr;
	////Matrix4x4* materialData = nullptr;
	////書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	////今回は赤を書き込んでみる
	///**materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);*/
	//materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialData->enableLighting = true;
	//materialData->uvTransform = MakeIdentity4x4();


	////////=========Material用のResourceを作る=========////

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(Sprite::TransformationMatrix));
	//データを書き込む
	Sprite::TransformationMatrix* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();


	//////=========Material用のResourceを作る=========////

	////Sprite用のマテリアルリソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(Material));
	//////...Mapしてデータを書き込む。色は白を設定しておくとよい...
	////Material* materialDataSprite = nullptr;
	////書き込むためのアドレスを取得
	//materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	////SpriteはLightingしないのでfalseを設定する
	//materialDataSprite->enableLighting = false;
	//materialDataSprite->uvTransform = MakeIdentity4x4();


	////=========平行光源用のResourceを作成=========////

	//平行光源用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderResource = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(DirectionalLight));
	//データを書き込む
	DirectionalLight* directionalLightData = nullptr;
	//書き込むためのアドレスを取得
	shaderResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;


	////=========Index用のあれやこれやを作成する=========////

	////Resourceを作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(uint32_t) * 6);

	////Viewを作成する
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	////リソースの先頭アドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	////インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;


	//Resourceを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> startResourceSprite = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(uint32_t) * 32 * 32 * 6);

	//Viewを作成する
	D3D12_INDEX_BUFFER_VIEW startBufferViewSprite{};
	//リソースの先頭アドレスから使う
	startBufferViewSprite.BufferLocation = startResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスつ分のサイズ
	startBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 32 * 32 * 6;
	//インデックスはuint32_tとする
	startBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;



	//////=========WorldViewProjectionMatrixを作る=========////

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//三角形
	//ID3D12Resource* WorldViewProjectionMatrixResource = CreateBufferResource(device, sizeof(Matrix4x4));
	//球
	Microsoft::WRL::ComPtr<ID3D12Resource> WorldViewProjectionMatrixResource = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(Matrix4x4) * SphereVertex);
	//データを書き込む
	Matrix4x4* WorldViewProjectionMatrixData = nullptr;
	//書き込むためのアドレスを取得
	WorldViewProjectionMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&WorldViewProjectionMatrixData));
	//単位行列を書き込んでおく
	*WorldViewProjectionMatrixData = MakeIdentity4x4();


	//////=========VertexBufferViewを作成する=========////

	////モデル
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点のサイズ
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	////1頂点あたりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);


	////=========頂点バッファビューを作成する=========////

	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	////リソースの先頭のアドレスから使う
	//vertexBufferViewSprite.BufferLocation = vertexResorceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点6つ分のサイズ
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	////頂点あたりのサイズ
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);


	//////=========Resourceにデータを書き込む=========////

	////モデル
	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	////頂点データをリソースにコピー
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	//////=========IndexResourceにデータを書き込む=========////

	////インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0;
	//indexDataSprite[1] = 1;
	//indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1;
	//indexDataSprite[4] = 3;
	//indexDataSprite[5] = 2;

	//////=========ResourceSpriteにデータを書き込む=========////

	//VertexData* vertexDataSprite = nullptr;
	//vertexResorceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	////１枚目の三角形
	////左下
	//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };
	//vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	////左上
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };
	//vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	////右下
	//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };
	//vertexDataSprite[2].texcoord = { 1.0f,1.0f };

	////２枚目の三角形
	////左上
	//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };
	//vertexDataSprite[3].texcoord = { 1.0f,0.0f };




	//vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };


	////=========ViewportとScissor(シザー)=========////

	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kClientWidth;
	viewport.Height = WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kClientHeight;

	MSG msg{};


	//=========Transform周りを作る=========////

	////Sprite用のTransformMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResorceSprite = dxCommon->CreateBufferResource(/*dxCommon->GetDevice().Get(),*/ sizeof(TransformationMatrix));
	////データを書き込む
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	////書き込むためのアドレスを取得
	//transformationMatrixResorceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	////単位行列を書き込んでおく
	//transformationMatrixDataSprite->World = MakeIdentity4x4();


	//////=========Transformを使ってCBufferを更新する=========////

	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	////CPUで動かす用のTransformを作る
	//Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	////UVTransform用の変数を用意
	//Transform uvTransformSprite{
	//	{1.0f,1.0f,1.0f},
	//	{0.0f,0.0f,0.0f},
	//	{0.0f,0.0f,0.0f},
	//};


	//////=========入力の初期化=========////

	//ポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(winApp);


	//////=========Imguiの初期化=========////

	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(device.Get(),
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());*/


		////////=========DescriptorSizeを取得しておく=========////

		/*const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	*/



	//////=========組み合わせて使う=========////

	////Textureを読んで転送する
	//DirectX::ScratchImage mipImages = dxCommon->LoadTexture("./Resources/uvChecker.png");
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata);
	//dxCommon->UploadTextureData(textureResource.Get(), mipImages);

	//// Textureを読んで転送する2
	//DirectX::ScratchImage mipImages2 = dxCommon->LoadTexture("./Resources/monsterBall.png");
	////DirectX::ScratchImage mipImages2 = dxCommon->LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = dxCommon->CreateTextureResource(metadata2);
	//dxCommon->UploadTextureData(textureResource2.Get(), mipImages2);

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

	////SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(dxCommon->GetSrvDescriptorHeap().Get(), dxCommon->GetDescriptorSizeSRV(), 1);//srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(dxCommon->GetSrvDescriptorHeap().Get(), dxCommon->GetDescriptorSizeSRV(), 1);//srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	////SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(dxCommon->GetSrvDescriptorHeap().Get(), dxCommon->GetDescriptorSizeSRV(), 2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(dxCommon->GetSrvDescriptorHeap().Get(), dxCommon->GetDescriptorSizeSRV(), 2);

	////先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleCPU2.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU2.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//////SRVの生成
	//dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	//dxCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	bool useMonsterBall = true;
	Vector4 color = { 1,1,1,1 };




	//////-----Spriteの更新処理と描画処理-----////
	//float offsetX = 100.0f;  // 各スプライトのX座標をずらすオフセット値
	//float initialX = 100.0f; // 初期X座標
	//// 小さく描画するための初期スケールを設定
	//Vector2 initialSize(80.0f, 80.0f);  // 例として50x50のサイズに設定



	//メインループ
	//ウィンドウの×ボタンが押されるまでループ
	while (true) {

		//Windowsにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMesage())
		{
			//ゲームループを抜ける
			break;
		}

		////=========Imguiに、ここからフレームが始まる旨を告げる=========////

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//////=========入力の更新=========////

		input->Update();


		//ゲームの処理

		////transform.rotate.y += 0.02f;
		//Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		////*wvpData = worldMatrix;
		////wvpData->WVP = worldMatrix;
		//wvpData->World = worldMatrix;

		//Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		////WVPMatrixを作る
		//Matrix4x4 worldviewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		//*WorldViewProjectionMatrixData = worldviewProjectionMatrix;
		//wvpData->WVP = worldviewProjectionMatrix;


		////-----Spriteの更新処理-----////
		//sprite->Update();

		////現在の座標を変数で受け取る
		//Vector2 position = sprite->GetPosition();
		////座標を変更
		//position.x += 1.1f;
		//position.y += 1.1f;
		////変更を反映する
		//sprite->SetPosition(position);

		////角度を変化させる
		//float rotation = sprite->GetRotation();
		//rotation += 0.008f;
		//sprite->SetRotation(rotation);

		////色を変化させる
		//Vector4 color = sprite->GetColor();
		//color.x += 0.01f;
		//if (color.x > 1.0f)
		//{
		//	color.x -= 1.0f;
		//}
		//sprite->SetColor(color);

		////スケールを変化させる
		//Vector2 size = sprite->GetSize();
		//size.x += 0.6f;
		//size.y += 0.6f;
		//sprite->SetSize(size);


		for (size_t i = 0; i < sprites.size(); ++i) {
			Sprite* sprite = sprites[i];

			// 現在の座標を変数で受け取る
			Vector2 position = sprite->GetPosition();
			position.x = initialX + i * offsetX;  // X座標をずらす

			//// 座標を変更
			//position.x += 1.1f;
			//position.y += 1.1f;
			position.y = 100.0f;

			//// 変更を反映する
			sprite->SetPosition(position);


			////角度を変化させる
			float rotation = sprite->GetRotation();
			rotation += 0.08f;
			sprite->SetRotation(rotation);

			////色を変化させる
			Vector4 color = sprite->GetColor();
			color.x += 0.01f;
			if (color.x > 1.0f)
			{
				color.x -= 1.0f;
			}
			sprite->SetColor(color);

			//スケールを変化させる
			/*Vector2 size = sprite->GetSize();
			size.x -= 0.6f;
			size.y -= 0.6f;
			sprite->SetSize(size);*/
			//各スプライトを初期サイズに設定
			sprite->SetSize(initialSize);

			//アンカーポイント
			Vector2 currentAnchor = sprite->GetAnchorPoint();
			sprite->SetAnchorPoint({ 0.5f, 0.5f });

			//フリップ
			bool flipX = sprite->GetFlipX();
			sprite->SetFlipX(false);
			bool flipY = sprite->GetFlipY();
			sprite->SetFlipY(false);

			//テクスチャ範囲指定
			Vector2 leftTop = sprite->GetTextureLeftTop();
			Vector2 size = sprite->GetTextureSize();
			if (i % 2 == 0)
			{
				sprite->SetTextureLeftTop({ 0.0f, 0.0f });
				sprite->SetTextureSize({ 64.0f, 64.0f });
			}
			else
			{
				sprite->SetTextureLeftTop({ 600.0f, 280.0f });
				sprite->SetTextureSize({ 200.0f, 200.0f });
			}
			


			// 各スプライトを更新
			sprite->Update();

		}


		/*Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
		uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
		uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));

		materialDataSprite->uvTransform = uvTransformMatrix;*/

		////Sprite用のWorldViewProjectionMatrixを作る
		//Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		//Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
		//Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

		//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;



		//////=========Imguiを使う=========////

		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		//ImGui::ShowDemoWindow();

		//ImGui::Begin("Window");
		///*ImGui::DragFloat3("translate", &transform.scale.x, 0.01f);
		//ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
		//ImGui::DragFloat3("CameraTranslate", &cameraTransform.scale.x, 0.01f);
		//ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 0.01f);
		//ImGui::DragFloat3("CameraTransform", &cameraTransform.translate.x, 0.01f);
		//ImGui::DragFloat3("directionalLight", &directionalLightData->direction.x, 0.01f);
		//directionalLightData->direction = Normalize(directionalLightData->direction);*/
		////ImGui::ColorEdit4("color", &materialData->color.x, 0.01f);
		//ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//ImGui::End();

		//ImGui::Begin("BlendMode");
		//const char* items[] = { "kBlendModeNone", "kBlendModeNormal", "kBlendModeAdd", "kBlendModeSubtract", "kBlendModeMultiply", "kBlendModeScreen", "kCountOfBlendMode" };
		//static int current_item = kBlendModeNormal; // 初期値をkBlendModeNormalに設定

		////if (ImGui::Combo("##combo", &current_item, items, IM_ARRAYSIZE(items))) {
		////	// current_itemは選択された項目のインデックスになります
		////	// これをBlendMode型の値にキャストします
		////	BlendMode mode = static_cast<BlendMode>(current_item);
		////}

		////ImGui::ColorEdit4("color", &materialData->color.x, 0.01f);
		//ImGui::End();



		//ImGui::Begin("uv");
		//ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		//ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		//ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		//ImGui::End();

		/*directionalLightData->direction = Normalize(directionalLightData->direction);*/

		//////ImGuiの内部コマンドを生成する
		ImGui::Render();
		//ImGui::DragFloat3("transform.rotate.y", &transform.rotate.y,0.01f);




		//DirectXの描画前処理。全ての描画に共通のグラフィックスコマンドを積む
		dxCommon->PreDraw();


		//Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
		spriteCommon->CommonSetting();



		////////=========コマンドを積む=========////

		////一般的には2dは最後。3dを描画した後、ImGuiの前に描画する

		dxCommon->GetCommandList()->RSSetViewports(1, &viewport);    //Viewportを設定
		dxCommon->GetCommandList()->RSSetScissorRects(1, &scissorRect);    //Scissorを設定
		//RootSignatureを設定。PSOに設定しているけど別途設定が必要
		/*dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());*/
		//dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());    //PSOを設定
		/*for (Sprite* sprite : sprites)*/
		//{
		//	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, GetVertexBufferView());    //VBVを設定
		//}
		dxCommon->GetCommandList()->IASetIndexBuffer(&startBufferViewSprite);
		//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		//////=========CBVを設定する=========////

		//マテリアルCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite->GetMaterialResource()->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());


		////=========DescriptorTableを設定する=========////

		//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? sprite->GetTexture2SrvHandleGPU() : sprite->GetTextureSrvHandleGPU());


		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, shaderResource->GetGPUVirtualAddress());


		//モデル
		//dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


		////=========Spriteの描画コマンドを積む=========////

		////Spriteの描画。変更が必要なものだけ変更する
		//dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
		////マテリアルCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
		////TransformwtionMatrixCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResorceSprite->GetGPUVirtualAddress());
		//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
		//dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
		////描画！（DrawCall/ドローコール）
		//////dxCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);
		//dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);


		//sprite->Draw();
		for (Sprite* sprite : sprites) {

			// スプライトを描画する
			sprite->Draw();
		}


		////////=========実際commandListのImGuiの描画コマンドを積む=========////

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

		//描画後処理
		dxCommon->PostDraw();



	}

	////=========解放処理=========////

	//解放処理

	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	delete input;

	//CloseHandle(fenceEvent);


	////=========COMの終了処理=========////

	//WinAppの終了処理
	winApp->Finalize();
	//テクスチャマネージャーの終了処理
	TextureManager::GetInstance()->Finalize();

	//WindowsAPI解放
	delete winApp;
	//DirectX解放
	delete dxCommon;
	//Sprite解放
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	delete spriteCommon;
	//3Dオブジェクト解放
	delete object3dCommon;
	delete object3d;

	return 0;

}