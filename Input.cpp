#include "Input.h"

void Input::Initialize()
{
	////DirectInputのインスタンス生成
	//Comptr<IDirectInput8> directInput = nullptr;
	//result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	//assert(SUCCEEDED(result));
	////キーボードデバイス生成
	//Comptr<IDirectInputDevice8> keyboard;
	//result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	//assert(SUCCEEDED(result));
	////入力データ形式のセット
	//result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	//assert(SUCCEEDED(result));
	////排他制御レベルのセット

}

void Input::Update()
{
}
