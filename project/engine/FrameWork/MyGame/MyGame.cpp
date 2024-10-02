#include "MyGame.h"

void MyGame::Initialize()
{
    //基底クラスの初期化処理
    Framework::Initialize();

    // テクスチャマネージャーの初期化
    TextureManager::GetInstance()->Initialize(dxCommon.get());

    ////-----Spriteの初期化-----////

    for (uint32_t i = 0; i < 6; ++i)
    {
        // スプライトをunique_ptrで初期化
        auto sprite = std::make_unique<Sprite>();

        if (i % 2 == 0) {
            // 0, 2, 4 は "Resources/uvChecker.png"
            sprite->Initialize(spriteCommon.get(), "Resources/uvChecker.png");
        }
        else {
            // 1, 3, 5 は "Resources/monsterBall.png"
            sprite->Initialize(spriteCommon.get(), "Resources/monsterBall.png");
        }

        sprites.push_back(std::move(sprite));
    }

    // モデルマネージャーの初期化
    ModelManager::GetInstance()->Initialize(dxCommon.get());

    // 3Dオブジェクトの初期化
    plane->Initialize(object3dCommon.get());
    axis->Initialize(object3dCommon.get());

    // モデル読み込み
    ModelManager::GetInstance()->LoadModel("plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");
    plane->SetModel("plane.obj");
    axis->SetModel("axis.obj");

    // モデルにSRTを設定
    plane->SetScale({ 1.0f, 1.0f, 1.0f });
    plane->SetRotate({ 0.0f, 45.0f, 0.0f });
    plane->SetTranslate({ -2.0f, 0.0f, 0.0f });

    axis->SetScale({ 1.0f, 1.0f, 1.0f });
    axis->SetRotate({ 0.0f, 45.0f, 0.0f });
    axis->SetTranslate({ 2.0f, 0.0f, 0.0f });

    // 3Dカメラの初期化
    cameraManager = std::make_unique<CameraManager>();
    camera1->SetTranslate({ 0.0f, 0.0f, -10.0f });
    cameraManager->AddCamera(camera1.get());

    camera2->SetTranslate({ 5.0f, 1.0f, -40.0f });
    cameraManager->AddCamera(camera2.get());

    // カメラのセット
    plane->SetCameraManager(cameraManager.get());
    axis->SetCameraManager(cameraManager.get());
}


void MyGame::Finalize()
{
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // テクスチャマネージャーの終了処理
    TextureManager::GetInstance()->Finalize();
    // モデルマネージャーの終了処理
    ModelManager::GetInstance()->Finalize();

    // 3Dオブジェクトの解放
    plane.reset();
    axis.reset();

    // Sprite の解放
    sprites.clear();

    // 各クラスの解放
    cameraManager.reset();

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
   

    // 各3Dオブジェクトの更新
    plane->Update();
    axis->Update();

    // ImGuiでオブジェクトの情報を表示
    plane->ImGuiUpdate("plane");
    axis->ImGuiUpdate("axis");

    // カメラの更新
    camera1->Update();
    camera2->Update();

    // カメラコントロール用のウィンドウを作成
    ImGui::Begin("Camera Control");

    // カメラの切り替え
    if (ImGui::Checkbox("Use Second Camera", &cameraFlag)) {
        cameraManager->SetCurrentCamera(cameraFlag ? 1 : 0);
    }

    ImGui::End();

    // 各スプライトの更新処理
    for (size_t i = 0; i < sprites.size(); ++i) {
        auto& sprite = sprites[i];

        // X座標を設定
        Vector2 position = sprite->GetPosition();
        position.x = initialX + i * offsetX;  // X座標をずらす
        position.y = 100.0f;
        sprite->SetPosition(position);

        // 角度を変化させる
        float rotation = sprite->GetRotation();
        rotation += 0.08f;
        sprite->SetRotation(rotation);

        // 色を変化させる
        Vector4 color = sprite->GetColor();
        color.x += 0.01f;
        if (color.x > 1.0f) {
            color.x -= 1.0f;
        }
        sprite->SetColor(color);

        // スプライトのサイズを設定
        sprite->SetSize(initialSize);
        sprite->SetAnchorPoint({ 0.5f, 0.5f });
        sprite->SetFlipX(false);
        sprite->SetFlipY(false);

        // テクスチャ範囲指定
        if (i % 2 == 0) {
            sprite->SetTextureLeftTop({ 0.0f, 0.0f });
            sprite->SetTextureSize({ 64.0f, 64.0f });
        }
        else {
            sprite->SetTextureLeftTop({ 600.0f, 280.0f });
            sprite->SetTextureSize({ 200.0f, 200.0f });
        }

        // 各スプライトを更新
        sprite->Update();
    }

    // ImGuiの内部コマンドを生成する
    ImGui::Render();
}


void MyGame::Draw()
{

    // DirectXの描画前処理。全ての描画に共通のグラフィックスコマンドを積む
    dxCommon->PreDraw();

    // 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
    object3dCommon->CommonSetting();

    // ================================================
    // ここから3Dオブジェクト個々の描画
    // ================================================

    // 各オブジェクトの描画
    plane->Draw();
    axis->Draw();

    // ================================================
    // ここまで3Dオブジェクト個々の描画
    // ================================================

    // Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
    spriteCommon->CommonSetting();

    // ================================================
    // ここからSprite個々の描画
    // ================================================

    // スプライトを描画する
    for (const auto& sprite : sprites) {
        sprite->Draw();
    }

    // ================================================
    // ここまでSprite個々の描画
    // ================================================

    // 実際にcommandListのImGuiの描画コマンドを積む
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

    // 描画後処理
    dxCommon->PostDraw();
}

