#include "FollowCamera.h"

FollowCamera::FollowCamera(CharacterBase* target, float followDistance)
    : target(target), followDistance(followDistance)
{
}

void FollowCamera::Update()
{
    if (target) // ターゲットが存在する場合のみ追従
    {
        const Vector3& targetPosition = target->GetTransform().translate; // ターゲットの位置を取得
        Vector3 direction = Normalize(transform.translate - targetPosition);
        transform.translate = targetPosition + direction * followDistance;
    }

    // 基底クラスの Update を呼び出す
    Camera::Update();
}
