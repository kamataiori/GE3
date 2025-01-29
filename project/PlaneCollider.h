#pragma once
#include "Collider.h"
#include "Struct.h"

class PlaneCollider : public Collider {
public:
    PlaneCollider(CollisionManager& manager, const Vector3& normal, float distance);
    ~PlaneCollider() override = default;

    bool CheckCollision(Collider* other) override;
    void OnCollision(Collider* other) override;
    ColliderType GetColliderType() const override { return ColliderType::Plane; }

    Plane GetCollisionShape() const { return plane; }

private:
    Plane plane;
};
