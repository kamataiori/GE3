#include "MyGame.h"

void MyGame::Initialize()
{
    // 基底クラスの初期化処理
    Framework::Initialize();

    // シーンファクトリーを生成し、マネージャーにセット
    sceneFactory_ = new SceneFactory();
    SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

    SceneManager::GetInstance()->ChangeScene("TITLE");

    // ImGuiManagerの初期化
    imGuiManager_ = std::make_unique<ImGuiManager>();
    imGuiManager_->Initialize(winApp.get(), DirectXCommon::GetInstance());
}


void MyGame::Finalize()
{
    // ImGuiの終了処理
    imGuiManager_->Finalize();
    
    // 基底クラスの終了処理
    Framework::Finalize();
}

void MyGame::Update()
{
    // ImGuiのフレーム開始を宣言
    imGuiManager_->Update();

    // 基底クラスの更新処理
    Framework::Update();

    // ImGuiの内部コマンドを生成する
    ImGui::Render();
}


//void MyGame::Draw()
//{
//    // DirectXの描画前処理。全ての描画に共通のグラフィックスコマンドを積む
//    dxCommon->PreDraw();
//    SrvManager::GetInstance()->PreDraw();
//
//    // ゲームシーンの描画
//    SceneManager::GetInstance()->Draw();
//
//    // 実際にcommandListのImGuiの描画コマンドを積む
//    imGuiManager_->Draw();
//
//    // 描画後処理
//    dxCommon->PostDraw();
//}

void MyGame::Draw()
{
    // RenderTextureへの描画前処理
    dxCommon->PreDrawForRenderTexture();
    SrvManager::GetInstance()->PreDraw();

    // ゲームシーンの描画 (RenderTextureに対して)
    SceneManager::GetInstance()->Draw();

    // RenderTextureの描画後処理
    //dxCommon->PostDrawForRenderTexture();

    // スワップチェーンへの描画前処理
    dxCommon->PreDraw();
    SrvManager::GetInstance()->PreDraw();

    // ImGuiの描画 (スワップチェーンに対して)
    imGuiManager_->Draw();

    // スワップチェーンの描画後処理
    dxCommon->PostDraw();
}

