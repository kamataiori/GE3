#pragma once
#include "Collider.h"
#include "CollisionManager.h"
#include "Struct.h"

class SphereCollider : public Collider
{
public:

    SphereCollider(CollisionManager& manager, const Vector3& center, float radius);
    ~SphereCollider() override = default;

    bool CheckCollision(Collider* other) override;
    void OnCollision(Collider* other) override;
    ColliderType GetColliderType() const override { return ColliderType::Sphere; }

    Sphere GetCollisionShape() const { return sphere; }

private:
    Sphere sphere;
};

