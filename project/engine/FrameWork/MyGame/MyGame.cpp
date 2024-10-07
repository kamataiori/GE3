#include "MyGame.h"

void MyGame::Initialize()
{
    // 基底クラスの初期化処理
    Framework::Initialize();

    // ゲームシーンの初期化
    scene_ = std::make_unique<GamePlayScene>();
    scene_->Initialize();
}


void MyGame::Finalize()
{
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // ゲームシーンの終了
    scene_->Finalize();
    
    // 基底クラスの終了処理
    Framework::Finalize();
}

void MyGame::Update()
{

    // ImGuiのフレーム開始を宣言
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 基底クラスの更新処理
    Framework::Update();
   
    // ゲームシーンの更新
    scene_->Update();
    

    // ImGuiの内部コマンドを生成する
    ImGui::Render();
}


void MyGame::Draw()
{
    // DirectXの描画前処理。全ての描画に共通のグラフィックスコマンドを積む
    dxCommon->PreDraw();

    // ゲームシーンの描画
    scene_->Draw();

    // 実際にcommandListのImGuiの描画コマンドを積む
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

    // 描画後処理
    dxCommon->PostDraw();
}

