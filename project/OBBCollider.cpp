#include "OBBCollider.h"
#include "CollisionFunctions.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "CapsuleCollider.h"

OBBCollider::OBBCollider(const OBB& box) : obb(box) {}

void OBBCollider::Dispatch(Collider* other) {
    other->Action(this);
}

void OBBCollider::Action(SphereCollider* other) {
    CheckSphereVsOBB(other->sphere, obb);
}

void OBBCollider::Action(AABBCollider* other) {
    CheckAABBVsOBB(other->aabb, obb);
}

void OBBCollider::Action(OBBCollider* other) {
    CheckOBBVsOBB(obb, other->obb);
}

void OBBCollider::Action(CapsuleCollider* other) {
    CheckOBBVsCapsule(obb, other->capsule);
}
