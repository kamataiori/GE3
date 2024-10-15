#pragma once
#include "WinApp.h"
#include <memory>

class ImGuiManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

private:
	// WinAppの初期化
	std::unique_ptr<WinApp> winApp = nullptr;
};

