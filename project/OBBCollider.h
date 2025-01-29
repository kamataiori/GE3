#pragma once
#include "Collider.h"
#include "Struct.h"

class OBBCollider : public Collider
{
public:
    OBBCollider(CollisionManager& manager, const Vector3& center, const Vector3& size, const Vector3 orientations[3]);
    ~OBBCollider() override = default;

    bool CheckCollision(Collider* other) override;
    void OnCollision(Collider* other) override;
    ColliderType GetColliderType() const override { return ColliderType::OBB; }

    OBB GetCollisionShape() const { return obb; }

private:
    OBB obb;
};

