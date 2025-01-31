#include "CapsuleCollider.h"
#include "CollisionFunctions.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"

CapsuleCollider::CapsuleCollider(const Capsule& cap) : capsule(cap) {
    capsule.start = { 1.6f, 0.0f, 0.0f };
    capsule.end = { 1.6f, -1.5f, 0.0f };
    capsule.radius = 0.5f;
    capsule.color = static_cast<int>(Color::WHITE);
    capsule.segments = 16; // 円周を構成する分割数
    capsule.rings = 8;     // 球部分を構成する分割数
}

void CapsuleCollider::Dispatch(Collider* other) {
    other->Action(this);
}

void CapsuleCollider::Draw()
{
    DrawLine::GetInstance()->DrawCapsule(capsule);
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
