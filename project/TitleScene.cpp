#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"

void TitleScene::Initialize()
{
	
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// ゲームプレイシーン (次シーン) を生成
		BaseScene* scene = new GamePlayScene();
		// シーン切り替え依頼
		SceneManager::GetInstance()->SetNextScene(scene);
	}
}

void TitleScene::Draw()
{
}
