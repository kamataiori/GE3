#include "OBBCollider.h"
#include "CollisionFunctions.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "CapsuleCollider.h"

OBBCollider::OBBCollider(const OBB& box) : obb(box) {
    obb.center = { -1.9f, -0.3f, 0.0f };
    obb.orientations[0] = { 1.0f, 0.0f, 0.0f }; // X軸
    obb.orientations[1] = { 0.0f, 1.0f, 0.0f }; // Y軸
    obb.orientations[2] = { 0.0f, 0.0f, 1.0f }; // Z軸
    obb.size = { 1.0f, 1.0f, 0.5f }; // 各軸方向の半サイズ
    obb.color = static_cast<int>(Color::WHITE); // 色の初期値
}

void OBBCollider::Dispatch(Collider* other) {
    other->Action(this);
}

void OBBCollider::Draw()
{
    DrawLine::GetInstance()->DrawOBB(obb);
}

Vector3 OBBCollider::GetCenter() const
{
    return obb.center;
}

void OBBCollider::SetPosition(const Vector3& position)
{
    obb.center = position;
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
