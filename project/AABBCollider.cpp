#include "AABBCollider.h"
#include "CollisionFunctions.h"
#include "SphereCollider.h"
#include "OBBCollider.h"
#include "CapsuleCollider.h"

AABBCollider::AABBCollider(const AABB& box) : aabb(box) {}

void AABBCollider::Dispatch(Collider* other) {
    other->Action(this);
}

void AABBCollider::Action(SphereCollider* other) {
    CheckSphereVsAABB(other->sphere, aabb);
}

void AABBCollider::Action(AABBCollider* other) {
    CheckAABBVsAABB(aabb, other->aabb);
}

void AABBCollider::Action(OBBCollider* other) {
    CheckAABBVsOBB(aabb, other->obb);
}

void AABBCollider::Action(CapsuleCollider* other) {
    CheckAABBVsCapsule(aabb, other->capsule);
}
