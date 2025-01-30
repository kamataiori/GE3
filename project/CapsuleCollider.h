#pragma once
#include "Collider.h"

class CapsuleCollider : public Collider {
public:
    Capsule capsule;

    CapsuleCollider(const Capsule& cap);

    void Dispatch(Collider* other) override;

protected:
    void Action(SphereCollider* other) override;
    void Action(AABBCollider* other) override;
    void Action(OBBCollider* other) override;
    void Action(CapsuleCollider* other) override;
};
