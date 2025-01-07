#include "Input.h"

Input* Input::instance = nullptr;

Input* Input::GetInstance()
{
	if (!instance) {
		instance = new Input();
	}
	return instance;
}

void Input::Finalize()
{
	delete instance;
	instance = nullptr;
}

void Input::Initialize(WinApp* winApp)
{
	//借りてきたWinAppのインスタンスを記録
	this->winApp_ = winApp;

	//DirectInputのインスタンス生成
	//Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	HRESULT result = DirectInput8Create(winApp->GetInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//キーボードデバイス生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// マウスデバイス生成
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = mouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = mouse->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));

}

void Input::Update()
{
	HRESULT result;

	//前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));

	//キーボード情報の取得開始
	result = keyboard->Acquire();
	////全キーの入力情報を取得する
	//BYTE key[256] = {};
	result = keyboard->GetDeviceState(sizeof(key), key);

	// 前回のマウス状態を保存
	mouseStatePre = mouseState;

	// マウス情報の取得開始
	result = mouse->Acquire();
	if (SUCCEEDED(result)) {
		result = mouse->GetDeviceState(sizeof(mouseState), &mouseState);
	}
}

bool Input::PushKey(BYTE keyNumber)
{
	//指定キーを押していればtrueを返す
	if (key[keyNumber])
	{
		return true;
	}

	//そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (keyPre[keyNumber])
	{
		return true;
	}

	return false;
}

void Input::GetMouseDelta(int& deltaX, int& deltaY)
{
	deltaX = mouseState.lX; // X方向の移動量
	deltaY = mouseState.lY; // Y方向の移動量
}

bool Input::PushMouseButton(int button)
{
	if (mouseState.rgbButtons[button] & 0x80) {
		return true;
	}
	return false;
}

bool Input::TriggerMouseButton(int button)
{
	if ((mouseState.rgbButtons[button] & 0x80) && !(mouseStatePre.rgbButtons[button] & 0x80)) {
		return true;
	}
	return false;
}
