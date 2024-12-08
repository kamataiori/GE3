#include "GamePlayScene.h"
#include <Input.h>
#include "SceneManager.h"

void GamePlayScene::Initialize()
{
	// 3Dカメラの初期化
	cameraManager = std::make_unique<CameraManager>();
	camera1->SetTranslate({ 0.0f, 0.0f, -20.0f });
	cameraManager->AddCamera(camera1.get());

	camera2->SetTranslate({ 5.0f, 1.0f, -40.0f });
	cameraManager->AddCamera(camera2.get());

	// カメラのセット
	particle->SetCameraManager(cameraManager.get());

	// ライト
	// Lightクラスのデータを初期化
	BaseScene::GetLight()->Initialize();
	BaseScene::GetLight()->GetCameraLight();
	BaseScene::GetLight()->GetDirectionalLight();
	BaseScene::GetLight()->SetDirectionalLightIntensity({ 1.0f });
	BaseScene::GetLight()->SetDirectionalLightColor({ 1.0f,1.0f,1.0f,1.0f });
	//BaseScene::GetLight()->SetDirectionalLightDirection(Normalize({ 1.0f,1.0f }));

	particle->Initialize();
	particle->CreateParticleGroup("particle", "Resources/particleTest.png",ParticleManager::BlendMode::kBlendModeAdd);
	//particle->CreateParticleGroup("particle2", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{32.0f,32.0f});
	// ParticleEmitterの初期化
    auto emitter = std::make_unique<ParticleEmitter>(particle.get(), "particle", Transform{ {0.0f, 0.0f, 0.0f},{},{-4.0f,1.0f,0.0f} }, 10, 0.5f, true);
	emitters.push_back(std::move(emitter));
}

void GamePlayScene::Finalize()
{
	
}

void GamePlayScene::Update()
{
    // カメラの更新
    camera1->Update();
    Vector3 cameraRotate = camera2->GetRotate();
    cameraRotate.x = 0.0f;
    cameraRotate.y += 0.1f;
    cameraRotate.z = 0.0f;
    camera2->SetRotate(cameraRotate);
    camera2->Update();

    // エミッター設定ウィンドウ
    ImVec2 emitterWindowPos = ImVec2(815, 185); // 固定位置
    static ImVec2 emitterWindowSize = ImVec2(400, 200); // 初期サイズ
    ImGui::SetNextWindowPos(emitterWindowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(emitterWindowSize, ImGuiCond_FirstUseEver); // サイズ変更を可能に

    ImGui::Begin("Emitter Settings");

    // エミッターが存在する場合に設定UIを表示
    if (!emitters.empty()) {
        auto& emitter = emitters[0]; // 現在は1つ目のエミッターを対象

        // 位置設定
        ImGui::Text("Transform");
        ImGui::DragFloat3("Position", &emitter->transform_.translate.x, 0.1f);
        ImGui::DragFloat3("Rotation", &emitter->transform_.rotate.x, 0.1f);
        ImGui::DragFloat3("Scale", &emitter->transform_.scale.x, 0.1f, 0.1f, 10.0f);

        // 発生数
        ImGui::Text("Emitter Settings");
        ImGui::DragInt("Particle Count", (int*)&emitter->count_, 1, 1, 1000);

        // 発生頻度
        ImGui::DragFloat("Frequency", &emitter->frequency_, 0.01f, 0.01f, 10.0f);

        // 繰り返し設定
        bool repeat = emitter->repeat_;
        if (ImGui::Checkbox("Repeat", &repeat)) {
            emitter->SetRepeat(repeat);
        }
    }
    else {
        ImGui::Text("No emitter available.");
    }

    ImGui::End();


    // パーティクル設定ウィンドウ (位置固定)
    ImVec2 particleWindowPos = ImVec2(815, 0); // 固定位置
    ImGui::SetNextWindowPos(particleWindowPos, ImGuiCond_Always);
    ImGui::Begin("Particle Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    // テクスチャ選択肢
    const char* textureOptions[] = {
        "uvChecker.png",
        "particleTest.png",
        "monsterBall.png",
        "fire.png",
        "circle.png",
        "water.png",
        "smoke.png"
    };
    static int selectedTextureIndex = 1; // 初期選択 (particleTest.png)
    static int previousTextureIndex = selectedTextureIndex; // 前回選択値を保存
    if (ImGui::Combo("Texture File", &selectedTextureIndex, textureOptions, IM_ARRAYSIZE(textureOptions))) {
        if (selectedTextureIndex != previousTextureIndex) {
            previousTextureIndex = selectedTextureIndex;
            textureFilePath = "Resources/";
            textureFilePath += textureOptions[selectedTextureIndex]; // ファイルパスを更新

            // 既存のパーティクルグループを削除して新規作成
            particle->RemoveParticleGroup("particle");
            particle->CreateParticleGroup("particle", textureFilePath, blendMode, { customSizeX, customSizeY });
        }
    }

    // 表示される現在のテクスチャパス
    ImGui::Text("Selected Texture: %s", textureFilePath.c_str());

    // ブレンドモード選択
    const char* blendModeNames[] = {
        "None", "Normal", "Add", "Subtract", "Multiply", "Screen"
    };
    int blendModeIndex = static_cast<int>(blendMode);
    static int previousBlendModeIndex = blendModeIndex; // 前回選択値を保存
    if (ImGui::Combo("Blend Mode", &blendModeIndex, blendModeNames, ParticleManager::BlendMode::kCountOfBlendMode)) {
        if (blendModeIndex != previousBlendModeIndex) {
            previousBlendModeIndex = blendModeIndex;
            blendMode = static_cast<ParticleManager::BlendMode>(blendModeIndex); // UI側の選択を更新
            particle->UpdateBlendMode("particle", blendMode); // 実行中の設定を反映
        }
    }

    // カスタムサイズ変更
    static float customSizeX = 32.0f; // 初期値
    static float customSizeY = 32.0f; // 初期値
    if (ImGui::DragFloat("Custom Size X", &customSizeX, 1.0f, 0.0f, 1000.0f)) {
        particle->RemoveParticleGroup("particle");
        particle->CreateParticleGroup("particle", textureFilePath, blendMode, { customSizeX, customSizeY });
    }
    if (ImGui::DragFloat("Custom Size Y", &customSizeY, 1.0f, 0.0f, 1000.0f)) {
        particle->RemoveParticleGroup("particle");
        particle->CreateParticleGroup("particle", textureFilePath, blendMode, { customSizeX, customSizeY });
    }

    ImGui::End();

    // 乱数範囲設定ウィンドウ (固定)
    ImGui::SetNextWindowPos(ImVec2(815, 390), ImGuiCond_Always); // 固定位置
    ImGui::SetNextWindowSize(ImVec2(480, 240), ImGuiCond_Always); // 固定サイズ
    ImGui::Begin("Random Range Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::Text("Translation Range");
    if (ImGui::DragFloat("Translate Min", &particle->translateRange_.min, 0.1f, -10.0f, 10.0f)) {
        if (particle->translateRange_.min >= particle->translateRange_.max) {
            particle->translateRange_.min = particle->translateRange_.max - 0.1f; // 調整
        }
    }
    if (ImGui::DragFloat("Translate Max", &particle->translateRange_.max, 0.1f, -10.0f, 10.0f)) {
        if (particle->translateRange_.max <= particle->translateRange_.min) {
            particle->translateRange_.max = particle->translateRange_.min + 0.1f; // 調整
        }
    }

    ImGui::Text("Color Range");
    if (ImGui::DragFloat("Color Min", &particle->colorRange_.min, 0.01f, 0.0f, 1.0f)) {
        if (particle->colorRange_.min >= particle->colorRange_.max) {
            particle->colorRange_.min = particle->colorRange_.max - 0.01f; // 調整
        }
    }
    if (ImGui::DragFloat("Color Max", &particle->colorRange_.max, 0.01f, 0.0f, 1.0f)) {
        if (particle->colorRange_.max <= particle->colorRange_.min) {
            particle->colorRange_.max = particle->colorRange_.min + 0.01f; // 調整
        }
    }

    ImGui::Text("Lifetime Range");
    if (ImGui::DragFloat("Lifetime Min", &particle->lifetimeRange_.min, 0.1f, 0.1f, 10.0f)) {
        if (particle->lifetimeRange_.min >= particle->lifetimeRange_.max) {
            particle->lifetimeRange_.min = particle->lifetimeRange_.max - 0.1f; // 調整
        }
    }
    if (ImGui::DragFloat("Lifetime Max", &particle->lifetimeRange_.max, 0.1f, 0.1f, 10.0f)) {
        if (particle->lifetimeRange_.max <= particle->lifetimeRange_.min) {
            particle->lifetimeRange_.max = particle->lifetimeRange_.min + 0.1f; // 調整
        }
    }

    ImGui::End();

    // カメラコントロールウィンドウ (位置固定)
    ImVec2 cameraWindowPos = ImVec2(815, 630); // 固定位置
    ImGui::SetNextWindowPos(cameraWindowPos, ImGuiCond_Always);
    ImGui::Begin("Camera Control", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    if (ImGui::Checkbox("Use Second Camera", &cameraFlag)) {
        cameraManager->SetCurrentCamera(cameraFlag ? 1 : 0);
    }
    ImGui::End();

    // その他の更新
    for (auto& emitter : emitters) {
        emitter->Update();
    }
    particle->Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
        // シーン切り替え
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}



void GamePlayScene::BackGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の背景描画
	// ================================================

	// ================================================
	// ここまでSprite個々の背景描画
	// ================================================
}

void GamePlayScene::Draw()
{
	// 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
	Object3dCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここから3Dオブジェクト個々の描画
	// ================================================

	
	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================
}

void GamePlayScene::ForeGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の前景描画(UIなど)
	// ================================================

	particle->Draw();

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}
