#pragma once
#include "Camera.h"
#include "CharacterBase.h"

class FollowCamera : public Camera
{
public:
    FollowCamera(CharacterBase* target, float followDistance);

    /// <summary>
    /// 更新（追従処理）
    /// </summary>
    void Update() override;

private:
    CharacterBase* target; // キャラクター（PlayerやEnemyなど）
    float followDistance;  // 対象との距離
};
