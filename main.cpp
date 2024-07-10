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

#include "Transform.h"
#include "MathFunctions.h"
#include "externals/DirectXTex/DirectXTex.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

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

//出力ウィンドウに文字を出す
void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

//ConvertString
//string->wstring
std::wstring ConvertString(const std::string& str);
//wstring->string
std::string ConvertString(const std::wstring& str);

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

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

//MaterialData構造体
struct MaterialData {
	std::string textureFilePath;
};

//ModelData構造体
struct ModelData {
	std::vector<VertexData>vertices;
	MaterialData material;
};

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


//CompileShader関数
IDxcBlob* CompileShader(
	//CompileするShaderファイルへのパス
	const std::wstring& filePath,
	//Compileに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	////=========1.hlslファイルを読む=========////

	//これからシェーダーをコンパイルする旨をログにだす
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSorceBuffer;
	shaderSorceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSorceBuffer.Size = shaderSource->GetBufferSize();
	shaderSorceBuffer.Encoding = DXC_CP_UTF8;    //UTB8の文字コードであることを通知

	////=========2.Compileする=========////

	/*LPCUWSTR*/const wchar_t* arguments[] = {
		filePath.c_str(),    //コンパイル対象のhisiファイル名
		L"-E",L"main",    //エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile,    //ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",    //デバッグ用の情報を埋め込む
		L"-Od",    //最適化を外しておく
		L"-Zpr",    //メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSorceBuffer,    //読み込んだファイル
		arguments,    //コンパイルオプション
		_countof(arguments),    //コンパイルオプションの数
		includeHandler,    //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)    //コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	////=========3.警告・エラーが出ていないか確認する=========////

	//警告・エラーが出てたらログにして止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
		//return nullptr;
	}

	////=========4.Compile結果を受け取って返す=========////

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

////=========Resource作成の関数化=========////
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes)
{
	////=========VertexResourceを生成する=========////

	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;    //uploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;    //リソースのサイズ。今回はVector4を3頂点分
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれをする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

////=========DescriptorHeapの作成関数=========////
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
	Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

////=========Textureを読む=========////

//1.Textureデータを読む
DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミニマップの生成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミニマップ付きのデータを返す
	return mipImages;
}

//2.DirectX12のTextureResourceを作る
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
{
	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resorceDesc{};
	resorceDesc.Width = UINT(metadata.width);  //Textureの幅
	resorceDesc.Height = UINT(metadata.height);  //TExtureの高さ
	resorceDesc.MipLevels = UINT16(metadata.mipLevels);  //mipmapの数
	resorceDesc.DepthOrArraySize = UINT16(metadata.arraySize);  //奥行き　or 配列Textureの配列数
	resorceDesc.Format = metadata.format;  //TextureのFormat
	resorceDesc.SampleDesc.Count = 1;  //サンプリングカウント。1固定。
	resorceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);  //Textureの次元数。普段使っているのは2次元

	//2.利用するHeapの設定
	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;  //細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;  //WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;  //プロセッサの近くに配置

	//3.Resourceを生成する
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,  //heapの設定
		D3D12_HEAP_FLAG_NONE,  //Heapの特殊な設定。特になし。
		&resorceDesc,  //Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,  //初回のResourceState。Textureは基本読むだけ
		nullptr,  //Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));  //作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

//3.TextureResourceにデータを転送する
void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		//MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,  //全領域へコピー
			img->pixels,  //元データアドレス
			UINT(img->rowPitch),  //1ラインサイズ
			UINT(img->slicePitch)  //1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}

}


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

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	//1,中で必要となる変数の宣言
	ModelData modelData;  //構築するModelData
	std::vector<Vector4> positions;  //位置
	std::vector<Vector3> normals;  //法線
	std::vector<Vector2> texcords;  //テクスチャ座標
	std::string line;  //ファイルから読んだ1行を格納するもの

	//2,ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);  //ファイルを開く
	assert(file.is_open());  //とりあえず開けなかったら止める

	//3,実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;  //先頭の識別子を読む

		//identifireに応じた処理
		//頂点情報を読む
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			position.y *= -1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt")
		{
			Vector2 texcord;
			s >> texcord.x >> texcord.y;
			//texcord.y = 1.0f - texcord.y;
			texcord.x = 1.0f - texcord.x;
			texcords.push_back(texcord);
		}
		else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			//normal.x *= -1.0f;
			normals.push_back(normal);
		}
		//三角形を作る
		else if (identifier == "f")
		{
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは[位置/uv/法線]で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');  //区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcord = texcords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				/*VertexData vertex = { position,texcord,normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position,texcord,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib")
		{
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	//4,ModelDataを返す
	return modelData;
}



////////=========リソースリーク=========////

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker()
	{
		//リソースチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};


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


	//	//デバックレイヤー
	//#ifdef _DEBUG
	//
	//	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
	//		//デバックレイヤーを有効化する
	//		debugController->EnableDebugLayer();
	//		//さらにGPU側でもチェックを行うようにする
	//		debugController->SetEnableGPUBasedValidation(TRUE);
	//	}
	//
	//#endif

		////ウィンドウを表示する
		//ShowWindow(hwnd, SW_SHOW);

		//出力ウィンドウへの文字出力
		//OutputDebugStringA("Hello,DirectX!\n");

		////DXGI1ファクトリーの生成
		//Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
		////HRESULはWindows系のエラーコードであり、
		////関数が成功したかどうかをSUCCEEDEDマクロで判定できる
		//HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
		////初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
		//assert(SUCCEEDED(hr));

		////使用するアダプタ用の変数。最初にnullptrを入れておく
		//Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
		////良い順にアダプタを頼む
		//for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		//	//アダプターの情報を取得する
		//	DXGI_ADAPTER_DESC3 adapterDesc{};
		//	hr = useAdapter->GetDesc3(&adapterDesc);
		//	assert(SUCCEEDED(hr));  //取得できないのは一大事
		//	//ソフトウェアアダプタでなければ採用!
		//	if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
		//		//採用したアダプタの情報をログに出力。wstrigの方なので注意
		//		Log(ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
		//		break;
		//	}
		//	useAdapter = nullptr; //ソフトウェアアダプタの場合は見なかったことにする
		//}

		////適切なアダプタが見つからなかったので起動できない
		//assert(useAdapter != nullptr);

		//Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
		////機能レベルとログ出力用の文字列
		//D3D_FEATURE_LEVEL featureLevels[] = {
		//	D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
		//};

		//const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
		////高い順に生成できるか試していく
		//for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//	//採用したアダプターでデバイスを生成
		//	hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		//	//指定した機能レベルでデバイスが生成できたかを確認
		//	if (SUCCEEDED(hr)) {
		//		//生成できたのでログ出力を行ってループを抜ける
		//		Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
		//		break;
		//	}
		//}
		////デバイスの生成がうまくいかなかったので起動できない
		//assert(device != nullptr);
		//Log("Complete create D3D12Device!!!\n");  //初期化完了のログを出す

		/*MSG msg{};*/

	//#ifdef _DEBUG
	//
	//	ID3D12InfoQueue* infoQueue = nullptr;
	//	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
	//		//ヤバイエラー時に止まる
	//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	//		//エラー時に止まる
	//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	//		//警告時に止まる
	//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	//
	//		//抑制するメッセージのID
	//		D3D12_MESSAGE_ID denyIds[] = {
	//			//Windows11でのDXGIデバックレイヤーとDX12デバックレイヤーの相互作用バグによるエラーメッセージ
	//			//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
	//			//D3D12_MESSAGE_ID_RESOUCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	//			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	//		};
	//		//抑制するレベル
	//		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	//		D3D12_INFO_QUEUE_FILTER filter{};
	//		filter.DenyList.NumIDs = _countof(denyIds);
	//		filter.DenyList.pIDList = denyIds;
	//		filter.DenyList.NumSeverities = _countof(severities);
	//		filter.DenyList.pSeverityList = severities;
	//		//指定したメッセージの表示を抑制する
	//		infoQueue->PushStorageFilter(&filter);
	//
	//		//解放
	//		infoQueue->Release();
	//	}
	//
	//#endif

		////コマンドキューを生成する
		//Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
		//D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
		//hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
		////コマンドキューの生成がうまくいかなかったので起動できない
		//assert(SUCCEEDED(hr));

		////コマンドアロケータを生成する
		//Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
		//hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
		////コマンドアロケータの生成がうまくいかなかったので起動できない
		//assert(SUCCEEDED(hr));

		////コマンドリストを生成する
		//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
		//hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
		////コマンドリストの生成がうまくいかなかったので起動できない
		//assert(SUCCEEDED(hr));

		////スワップチェーンを生成する
		//Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
		//DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		//swapChainDesc.Width = WinApp::kClientWidth;      //画面の幅。ウィンドウのクライアント領域を同じものにしておく
		//swapChainDesc.Height = WinApp::kClientHeight;    //画面の高さ。。ウィンドウのクライアント領域を同じものにしておく
		//swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    //色の形式
		//swapChainDesc.SampleDesc.Count = 1;    //マルチサンプルしない
		//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    //描画のターゲットとして利用する
		//swapChainDesc.BufferCount = 2;    //ダブルバッファ
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    //モニタにうつしたら、中身を破棄
		////コマンドキュー、ウィンドウハンドル、設定を渡して生成する
		//hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), winApp->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
		//assert(SUCCEEDED(hr));

		//ディスクリプタヒープの生成

		////RTV用のヒープディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

		////SRV用のヒープディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

		////DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

		////SwapChainからResourceを引っ張ってくる
		//Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
		//hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
		////うまく取得できなければ起動できない
		//assert(SUCCEEDED(hr));
		//hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
		//assert(SUCCEEDED(hr));





	//ポインタ
	DirectXCommon* dxCommon = nullptr;

	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);









	//////=========VertexResourceを生成する=========////

	int  SphreVertex = 32 * 32 * 6;

	//モデル読み込み
	ModelData modelData = LoadObjFile("Resources", "plane.obj");
	//ModelData modelData = LoadObjFile("Resources", "axis.obj");

	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());

	//球
	//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * SphreVertex);

	//三角形
	//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6);


	//Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResorce(device, WinApp::kClientWidth, WinApp::kClientHeight);

	//Sprite用の頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResorceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);


	//////=========RTVの設定=========////

	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;    //出力結果をSRGBに変換して書き込む
	//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;    //2dテクスチャとして書き込む
	////ディスクリプタの先頭を取得する
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	////RTVを2つ作るのでディスクリプタを2つ用意
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	////まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	//rtvHandles[0] = rtvStarHandle;
	//device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	////2つ目のディスクリプタハンドルを得る(自力で)
	//rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	////2つ目を作る
	//device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

	//////=========DSVの設定=========////

	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;  //Format。基本的にはResourceに合わせる
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;  //2dTexture
	////DSVHeapの先頭にDSVをつくる
	//device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	////初期値0でFenceを作る
	//Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	//uint64_t fenceValue = 0;
	//hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	//assert(SUCCEEDED(hr));

	////FenceのSignalを持つためのイベントを生成する
	//HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//assert(fenceEvent != nullptr);

	////dxcCompilerを初期化
	//IDxcUtils* dxcUtils = nullptr;
	//IDxcCompiler3* dxcCompiler = nullptr;
	//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	//assert(SUCCEEDED(hr));
	//hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	//assert(SUCCEEDED(hr));

	////現時点でincludeはしないが、includeに対応するための設定を行っておく
	//IDxcIncludeHandler* includeHandler = nullptr;
	//hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	//assert(SUCCEEDED(hr));


	////=========DescriptorRange=========////

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;  //0から始まる
	descriptorRange[0].NumDescriptors = 1;  //数は1
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;  //SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;  //Offsetを自動計算


	////=========RootSignatureを生成する=========////

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootSignature作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;    //レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;    //VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;    //レジスタ番号0を使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;  //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	////=========平行光源をShaderで使う=========////
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  //PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;  //レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;    //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);    //配列の長さ




	////=========Samplerの設定=========////

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;  //バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;  //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;  //ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;  //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  //PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	////シリアライズしてバイナリにする
	//Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	//Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	//hr = D3D12SerializeRootSignature(&descriptionRootSignature,
	//	D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//if (FAILED(hr)) {
	//	Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//	assert(false);
	//}
	////バイナリを元に生成
	//Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
	//	signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//assert(SUCCEEDED(hr));

	////=========InputLayoutの設定を行う=========////

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	////=========BlendStateの設定を行う=========////

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//α値のブレンド設定で基本的には使わない
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	////=========RasterizerStateの設定を行う=========////

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	////=========ShaderをCompileする=========////

	////Shaderをコンパイルする
	//Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resources/shaders/Object3d.VS.hlsl",
	//	L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(vertexShaderBlob != nullptr);

	//Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/Object3d.PS.hlsl",
	//	L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(pixelShaderBlob != nullptr);


	////=========DepthStencilStateの設定を行う=========////

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	////=========PSOを生成する=========////

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();    //RootSignature
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;    //InputLayout
	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	//vertexShaderBlob->GetBufferSize() };    //VertexShader
	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	//pixelShaderBlob->GetBufferSize() };    //PixelShader
	//graphicsPipelineStateDesc.BlendState = blendDesc;    //BlendState
	//graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;    //RasterizerState
	////書き込むRTVの情報
	//graphicsPipelineStateDesc.NumRenderTargets = 1;
	//graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	////利用するトポロジ(形状)のタイプ。三角形
	//graphicsPipelineStateDesc.PrimitiveTopologyType =
	//	D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	////どのように画面に色を打ち込むかの設定(気にしなくて良い)
	//graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	////DepthStencilの設定
	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	////実際に生成
	//Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState = nullptr;
	//hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
	//	IID_PPV_ARGS(&graphicsPipelineState));
	//assert(SUCCEEDED(hr));



	//////=========Material用のResourceを作る=========////

	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(device, sizeof(Material));
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

	////WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	////データを書き込む
	//TransformationMatrix* wvpData = nullptr;
	////書き込むためのアドレスを取得
	//wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	////単位行列を書き込んでおく
	//wvpData->WVP = MakeIdentity4x4();
	//wvpData->World = MakeIdentity4x4();


	////////=========Material用のResourceを作る=========////

	////Sprite用のマテリアルリソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	////...Mapしてデータを書き込む。色は白を設定しておくとよい...
	//Material* materialDataSprite = nullptr;
	////書き込むためのアドレスを取得
	//materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	////SpriteはLightingしないのでfalseを設定する
	//materialDataSprite->enableLighting = false;
	//materialDataSprite->uvTransform = MakeIdentity4x4();


	//////=========平行光源用のResourceを作成=========////

	////平行光源用のリソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> shaderResource = CreateBufferResource(device, sizeof(DirectionalLight));
	////データを書き込む
	//DirectionalLight* directionalLightData = nullptr;
	////書き込むためのアドレスを取得
	//shaderResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	////デフォルト値はとりあえず以下のようにしておく
	//directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	//directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	//directionalLightData->intensity = 1.0f;


	//////=========Index用のあれやこれやを作成する=========////

	////Resourceを作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

	////Viewを作成する
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	////リソースの先頭アドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	////インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;


	////Resourceを作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> startResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 32 * 32 * 6);

	////Viewを作成する
	//D3D12_INDEX_BUFFER_VIEW startBufferViewSprite{};
	////リソースの先頭アドレスから使う
	//startBufferViewSprite.BufferLocation = startResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックスつ分のサイズ
	//startBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 32 * 32 * 6;
	////インデックスはuint32_tとする
	//startBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;



	////////=========WorldViewProjectionMatrixを作る=========////

	////WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	////三角形
	////ID3D12Resource* WorldViewProjectionMatrixResource = CreateBufferResource(device, sizeof(Matrix4x4));
	////球
	//Microsoft::WRL::ComPtr<ID3D12Resource> WorldViewProjectionMatrixResource = CreateBufferResource(device, sizeof(Matrix4x4) * SphreVertex);
	////データを書き込む
	//Matrix4x4* WorldViewProjectionMatrixData = nullptr;
	////書き込むためのアドレスを取得
	//WorldViewProjectionMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&WorldViewProjectionMatrixData));
	////単位行列を書き込んでおく
	//*WorldViewProjectionMatrixData = MakeIdentity4x4();


	//////=========VertexBufferViewを作成する=========////

	////モデル
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点のサイズ
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	////1頂点あたりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);


	//////=========頂点バッファビューを作成する=========////

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


	////////=========IndexResourceにデータを書き込む=========////

	////インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0;
	//indexDataSprite[1] = 1;
	//indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1;
	//indexDataSprite[4] = 3;
	//indexDataSprite[5] = 2;

	////////=========ResourceSpriteにデータを書き込む=========////

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


	//////=========ViewportとScissor(シザー)=========////

	//D3D12_VIEWPORT viewport{};
	////クライアント領域のサイズと一緒にして画面全体に表示
	//viewport.Width = WinApp::kClientWidth;
	//viewport.Height = WinApp::kClientHeight;
	//viewport.TopLeftX = 0;
	//viewport.TopLeftY = 0;
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;

	////シザー矩形
	//D3D12_RECT scissorRect{};
	////基本的にビューポートと同じ矩形が構成されるようにする
	//scissorRect.left = 0;
	//scissorRect.right = WinApp::kClientWidth;
	//scissorRect.top = 0;
	//scissorRect.bottom = WinApp::kClientHeight;

	MSG msg{};


	////=========Transform周りを作る=========////

	//Sprite用のTransformMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResorceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//書き込むためのアドレスを取得
	//transformationMatrixResorceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//単位行列を書き込んでおく
	//transformationMatrixDataSprite->World = MakeIdentity4x4();


	//////=========Transformを使ってCBufferを更新する=========////

	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	//CPUで動かす用のTransformを作る
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//UVTransform用の変数を用意
	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};


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



	////////=========組み合わせて使う=========////

	////Textureを読んで転送する
	//DirectX::ScratchImage mipImages = LoadTexture("./Resources/uvChecker.png");
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device, metadata);
	//UploadTextureData(textureResource.Get(), mipImages);

	//// Textureを読んで転送する2
	////DirectX::ScratchImage mipImages2 = LoadTexture("./Resources/monsterBall.png");
	//DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = CreateTextureResource(device, metadata2);
	//UploadTextureData(textureResource2.Get(), mipImages2);

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
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 0);//srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 0);//srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	////SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 2);

	////先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	////SRVの生成
	//device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	//device->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	//bool useMonsterBall = true;
	//Vector4 color = { 1,1,1,1 };


	//メインループ
	//ウィンドウの×ボタンが押されるまでループ
	while (true) {

		//Windowsにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMesage())
		{
			//ゲームループを抜ける
			break;
		}

		//////=========Imguiに、ここからフレームが始まる旨を告げる=========////

		//ImGui_ImplDX12_NewFrame();
		//ImGui_ImplWin32_NewFrame();
		//ImGui::NewFrame();

		////////=========入力の更新=========////

		//input->Update();


		////ゲームの処理

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

		//Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
		//uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
		//uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));

		//materialDataSprite->uvTransform = uvTransformMatrix;

		////Sprite用のWorldViewProjectionMatrixを作る
		//Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		//Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
		//Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

		//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;



		////////=========Imguiを使う=========////

		////開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		////ImGui::ShowDemoWindow();

		//ImGui::Begin("Window");
		//ImGui::DragFloat3("translate", &transform.scale.x, 0.01f);
		//ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
		//ImGui::DragFloat3("CameraTranslate", &cameraTransform.scale.x, 0.01f);
		//ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 0.01f);
		//ImGui::DragFloat3("CameraTransform", &cameraTransform.translate.x, 0.01f);
		//ImGui::DragFloat3("directionalLight", &directionalLightData->direction.x, 0.01f);
		//directionalLightData->direction = Normalize(directionalLightData->direction);
		////ImGui::ColorEdit4("color", &materialData->color.x, 0.01f);
		////ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//ImGui::End();

		//ImGui::Begin("BlendMode");
		//const char* items[] = { "kBlendModeNone", "kBlendModeNormal", "kBlendModeAdd", "kBlendModeSubtract", "kBlendModeMultiply", "kBlendModeScreen", "kCountOfBlendMode" };
		//static int current_item = kBlendModeNormal; // 初期値をkBlendModeNormalに設定

		//if (ImGui::Combo("##combo", &current_item, items, IM_ARRAYSIZE(items))) {
		//	// current_itemは選択された項目のインデックスになります
		//	// これをBlendMode型の値にキャストします
		//	BlendMode mode = static_cast<BlendMode>(current_item);
		//}

		//ImGui::ColorEdit4("color", &materialData->color.x, 0.01f);
		//ImGui::End();



		////ImGui::Begin("uv");
		////ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		////ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		////ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		////ImGui::End();

		///*directionalLightData->direction = Normalize(directionalLightData->direction);*/

		////////ImGuiの内部コマンドを生成する
		//ImGui::Render();
		////ImGui::DragFloat3("transform.rotate.y", &transform.rotate.y,0.01f);

		////これから書き込むバックバッファのインデックスを取得
		//UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

		////TransitionBarrierの設定
		//D3D12_RESOURCE_BARRIER barrier{};
		////今回のバリアはTransition
		//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		////Noneにしておく
		//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		////バリアを張る対象のリソース。現在のバックアップバッファに対して行う
		//barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
		////遷移前（現在）のResourceState
		//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		////遷移後のResourceState
		//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		////TransitionBarrierを張る
		//commandList->ResourceBarrier(1, &barrier);

		////////=========DSVを設定する=========////

		////描画先のRTVとDSVを設定する
		//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		//commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

		////指定した深度で画面全体をクリアする
		//commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		////描画先のRTVを設定する
		////commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
		////指定した色で画面全体をクリアする
		//float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };    //青っぽい色。RGBAの順  例{ 0.1f,0.25f,0.5f,1.0f }  灰色{ 0.22f, 0.22f, 0.22f, 1.0f}
		//commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

		////commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		////////=========描画用のDescriptorHeapの設定=========////

		//ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
		//commandList->SetDescriptorHeaps(1, descriptorHeaps);


		////////=========コマンドを積む=========////

		////一般的には2dは最後。3dを描画した後、ImGuiの前に描画する

		//commandList->RSSetViewports(1, &viewport);    //Viewportを設定
		//commandList->RSSetScissorRects(1, &scissorRect);    //Scirssorを設定
		////RootSignatureを設定。PSOに設定しているけど別途設定が必要
		//commandList->SetGraphicsRootSignature(rootSignature.Get());
		//commandList->SetPipelineState(graphicsPipelineState.Get());    //PSOを設定
		//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);    //VBVを設定
		//commandList->IASetIndexBuffer(&startBufferViewSprite);
		////形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		////////=========CBVを設定する=========////

		////マテリアルCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		////wvp用のCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());


		//////=========DescriptorTableを設定する=========////

		//commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);


		//commandList->SetGraphicsRootConstantBufferView(3, shaderResource->GetGPUVirtualAddress());


		////モデル
		//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


		////////=========Spriteの描画コマンドを積む=========////

		////Spriteの描画。変更が必要なものだけ変更する
		//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
		////マテリアルCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
		////TransformwtionMatrixCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResorceSprite->GetGPUVirtualAddress());
		//commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
		//commandList->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
		////描画！（DrawCall/ドローコール）
		////commandList->DrawInstanced(6, 1, 0, 0);
		////commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);



		////////=========実際のcommandListのImGuiの描画コマンドを積む=========////

		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());


		////画面に描く処理はすべて終わり、画面に映すので、状態を遷移
		////今回はRenderTargetからPresentにする
		//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		////TransitionBarrierを張る
		//commandList->ResourceBarrier(1, &barrier);

		////コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
		//hr = commandList->Close();
		//assert(SUCCEEDED(hr));

		////GPUにコマンドリストの実行を行わせる
		//ID3D12CommandList* commandLists[] = { commandList.Get() };
		//commandQueue->ExecuteCommandLists(1, commandLists);

		////GPUとOSに画面の交換を行うように通知する
		//swapChain->Present(1, 0);

		////Fenceの値を更新
		//fenceValue++;
		////GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		//commandQueue->Signal(fence.Get(), fenceValue);

		////Fenceの値が指定したSignal値にたどり着いてるか確認する
		////GetCompletedValueの初期値はFence作成時に渡した初期値
		//if (fence->GetCompletedValue() < fenceValue)
		//{
		//	//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		//	fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//	//イベント待つ
		//	WaitForSingleObject(fenceEvent, INFINITE);
		//}

		////次のフレーム用のコマンドリストを準備
		//hr = commandAllocator->Reset();
		//assert(SUCCEEDED(hr));
		//hr = commandList->Reset(commandAllocator.Get(), nullptr);
		//assert(SUCCEEDED(hr));




        //描画前処理
		dxCommon->PreDraw();

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

	//WindowsAPI解放
	delete winApp;
	//DirectX解放
	delete dxCommon;

	return 0;

}