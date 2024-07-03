#pragma once
#define DIRECTINPUT_VERSION    0x0800 //DirectInputのバージョン指定
#include <wrl.h>
#include <cassert>
#include <dinput.h>
#include <Windows.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//入力
class Input
{
public:

	//namespace省略
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;
	
	///========================
	// メンバ関数
	///========================

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	
	///========================
	// メンバ変数
	///========================

	/// <summary>
	/// キーボードのデバイス
	/// </summary>
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard;

};

