#include "ImGuiManager.h"
#include <externals/imgui/imgui_impl_win32.h>

void ImGuiManager::Initialize(WinApp* winApp)
{
	// ImGuiのコンテキストを生成
	ImGui::CreateContext();
	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	// Win32用初期化
	ImGui_ImplWin32_Init(winApp->GetHwnd());
}
