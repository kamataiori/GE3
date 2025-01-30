#include "CapsuleCollider.h"
#include "CollisionFunctions.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"

CapsuleCollider::CapsuleCollider(const Capsule& cap) : capsule(cap) {}

void CapsuleCollider::Dispatch(Collider* other) {
    other->Action(this);
}

void CapsuleCollider::Action(SphereCollider* other) {
    CheckSphereVsCapsule(other->sphere, capsule);
}

void CapsuleCollider::Action(AABBCollider* other) {
    CheckAABBVsCapsule(other->aabb, capsule);
}

void CapsuleCollider::Action(OBBCollider* other) {
    CheckOBBVsCapsule(other->obb, capsule);
}

void CapsuleCollider::Action(CapsuleCollider* other) {
    CheckCapsuleVsCapsule(capsule, other->capsule);
}
