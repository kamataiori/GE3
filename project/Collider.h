#pragma once
#include "CollisionManager.h"

enum class ColliderType {
    Sphere,
    AABB,
    OBB,
    Capsule,
    Cone,
    Plane
};

class Collider
{
public:
    // コンストラクタ
    Collider(CollisionManager& manager) : collisionManager(manager) {}
    // デストラクタ
    virtual ~Collider() = default;

    /// <summary>
    /// 衝突時に呼ばれる
    /// </summary>
    /// <param name="other"></param>
    virtual void OnCollision(Collider* other) = 0;

    /// <summary>
    /// 衝突形状を取得
    /// </summary>
    virtual bool CheckCollision(Collider* other) = 0;

    /// <summary>
    /// コライダーの種類を取得
    /// </summary>
    virtual ColliderType GetColliderType() const = 0;

protected:

    CollisionManager& collisionManager; // 衝突判定を呼び出すための参照

};

