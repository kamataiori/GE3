#pragma once
#include "Collider.h"
#include "Struct.h"

class CapsuleCollider : public Collider {
public:
    CapsuleCollider(CollisionManager& manager, const Vector3& start, const Vector3& end, float radius);
    ~CapsuleCollider() override = default;

    bool CheckCollision(Collider* other) override;
    void OnCollision(Collider* other) override;
    ColliderType GetColliderType() const override { return ColliderType::Capsule; }

    Capsule GetCollisionShape() const { return capsule; }

private:
    Capsule capsule;
};
