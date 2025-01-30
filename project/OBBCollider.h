#pragma once
#include "Collider.h"

class OBBCollider : public Collider {
public:
    OBB obb;

    OBBCollider(const OBB& box);

    void Dispatch(Collider* other) override;

protected:
    void Action(SphereCollider* other) override;
    void Action(AABBCollider* other) override;
    void Action(OBBCollider* other) override;
    void Action(CapsuleCollider* other) override;
};
