#pragma once
#include "Collider.h"
#include "Struct.h"

class AABBCollider : public Collider
{
public:
    AABBCollider(CollisionManager& manager, const Vector3& min, const Vector3& max);
    ~AABBCollider() override = default;

    bool CheckCollision(Collider* other) override;
    void OnCollision(Collider* other) override;
    ColliderType GetColliderType() const override { return ColliderType::AABB; }

    AABB GetCollisionShape() const { return aabb; }

private:
    AABB aabb;
};

