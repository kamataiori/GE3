#include "AABBCollider.h"
#include "CollisionFunctions.h"
#include "SphereCollider.h"
#include "OBBCollider.h"
#include "CapsuleCollider.h"

AABBCollider::AABBCollider(const AABB& box) : aabb(box) {
    aabb.min = { -1.8f, 2.2f, 3.0f }; // AABB の最小点を少し下げる
    aabb.max = { 1.8f, 1.1f, 0.5f };  // AABB の最大点を少し上げる
    aabb.color = static_cast<int>(Color::WHITE); // AABBの色を赤に設定
}

void AABBCollider::Dispatch(Collider* other) {
    other->Action(this);
}

void AABBCollider::Draw()
{
    DrawLine::GetInstance()->DrawAABB(aabb);
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
