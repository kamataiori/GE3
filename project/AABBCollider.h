#pragma once
#include "Collider.h"

class AABBCollider : public Collider {
public:
    AABB aabb;

    AABBCollider(const AABB& box);

    void Dispatch(Collider* other) override;

protected:
    void Action(SphereCollider* other) override;
    void Action(AABBCollider* other) override;
    void Action(OBBCollider* other) override;
    void Action(CapsuleCollider* other) override;
};
