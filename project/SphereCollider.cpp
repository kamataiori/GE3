#include "SphereCollider.h"
#include "CollisionFunctions.h"
#include "AABBCollider.h"
#include "OBBCollider.h"
#include "CapsuleCollider.h"
#include <DrawLine.h>

SphereCollider::SphereCollider(const Sphere& s) : sphere(s) {
    sphere = { {-4.0f, -1.2f, 0.0f}, 1.0f, static_cast<int>(Color::WHITE) };
}

void SphereCollider::Dispatch(Collider* other) {
    other->Action(this);
}

void SphereCollider::Draw()
{
    DrawLine::GetInstance()->DrawSphere(sphere, 16, 16);
}

Vector3 SphereCollider::GetCenter() const
{
    return sphere.center;
}

void SphereCollider::SetPosition(const Vector3& position)
{
    sphere.center = position;
}

float SphereCollider::GetRadius() const
{
    return sphere.radius;
}

void SphereCollider::SetRadius(float radius)
{
    sphere.radius = radius;
}

int SphereCollider::GetColor() const
{
    return sphere.color;
}

void SphereCollider::SetColor(int color)
{
    sphere.color = color;
}

void SphereCollider::Action(SphereCollider* other) {
    CheckSphereVsSphere(sphere, other->sphere);
}

void SphereCollider::Action(AABBCollider* other) {
    CheckSphereVsAABB(sphere, other->aabb);
}

void SphereCollider::Action(OBBCollider* other) {
    CheckSphereVsOBB(sphere, other->obb);
}

void SphereCollider::Action(CapsuleCollider* other) {
    CheckSphereVsCapsule(sphere, other->capsule);
}
