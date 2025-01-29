#pragma once
#include "Collider.h"
#include "Struct.h"

class ConeCollider : public Collider {
public:
    ConeCollider(CollisionManager& manager, const Vector3& baseCenter, const Vector3& tip, float radius);
    ~ConeCollider() override = default;

    bool CheckCollision(Collider* other) override;
    void OnCollision(Collider* other) override;
    ColliderType GetColliderType() const override { return ColliderType::Cone; }

    Cone GetCollisionShape() const { return cone; }

private:
    Cone cone;
};
