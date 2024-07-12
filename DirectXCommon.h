#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "WinApp.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


class DirectXCommon
{
public:

	///===========================
	// メンバ関数
	///===========================

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void Device();

	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void Command();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void SwapChain();

	/// <summary>
	/// 深度バッファの生成
	/// </summary>
	void DepthBuffer();

	/// <summary>
	/// 各種ディスクリプタヒープの生成
	/// </summary>
	void DescriptorHeap();

	/// <summary>
	/// レンダーターゲットビューの初期化
	/// </summary>
	void RenderTargetView();

	/// <summary>
	/// 深度ステンシルビューの初期化
	/// </summary>
	void DepthStencilView();

	/// <summary>
	/// フェンスの生成
	/// </summary>
	void Fence();

	/// <summary>
	/// ビューポート矩形の初期化
	/// </summary>
	void ViewportRectangle();

	/// <summary>
	/// シザリング矩形の初期化
	/// </summary>
	void ScissoringRectangle();

	/// <summary>
	/// DXCコンパイラの生成
	/// </summary>
	void DXCCompiler();

	/// <summary>
	/// ImGuiの初期化
	/// </summary>
	void ImGui();



	/// <summary>
	/// ディスクリプタヒープの生成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// 指定番号のCPUディスクリプタハンドルを取得する
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize,uint32_t index);

	/// <summary>
	/// 指定番号のGPUディスクリプタハンドルを取得する
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);


	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResorce(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	////===============描画系===============////

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();



private:
	
	//DXGI1ファクトリーの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	//コマンドアロケータの生成
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	//コマンドリストの生成
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	//コマンドキューの生成
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

	//WindowsAPI
	WinApp* winApp = nullptr;

	//スワップチェーンを生成する
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};


	//DiscriptorSizeの取得
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;







	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];







	//RTV用のヒープディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;

	//SRV用のヒープディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	//DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;


	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle;

	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };




	//RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;



	//初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	//初期値0でFenceを作る
	uint64_t fenceValue = 0;
	//FenceのSignalを持つためのイベントを生成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);


	//ビューポート矩形
	D3D12_VIEWPORT viewport{};
	//シザー矩形
	D3D12_RECT scissorRect{};


	//dxcCompiler
	IDxcUtils* dxcUtils;
	IDxcCompiler3* dxcCompiler;
	IDxcIncludeHandler* includeHandler;





	//リソースバリアで書き込み可能に変更
	D3D12_RESOURCE_BARRIER barrier{};

};

