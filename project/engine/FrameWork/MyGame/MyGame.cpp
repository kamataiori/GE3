#include "MyGame.h"

void MyGame::Initialize()
{
    // 基底クラスの初期化処理
    Framework::Initialize();

    // シーンファクトリーを生成し、マネージャーにセット
    sceneFactory_ = new SceneFactory();
    SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

    SceneManager::GetInstance()->ChangeScene("TITLE");
}


void MyGame::Finalize()
{
    // ImGuiの終了処理
  /*  ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();*/
    
    // 基底クラスの終了処理
    Framework::Finalize();
}

void MyGame::Update()
{

    // ImGuiのフレーム開始を宣言
   /* ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();*/

    // 基底クラスの更新処理
    Framework::Update();

    // ImGuiの内部コマンドを生成する
    //ImGui::Render();
}


void MyGame::Draw()
{
    // DirectXの描画前処理。全ての描画に共通のグラフィックスコマンドを積む
    dxCommon->PreDraw();
    SrvManager::GetInstance()->PreDraw();

    // ゲームシーンの描画
    SceneManager::GetInstance()->Draw();

    // 実際にcommandListのImGuiの描画コマンドを積む
   // ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

    // 描画後処理
    dxCommon->PostDraw();
}

