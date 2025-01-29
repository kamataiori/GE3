#include "AABBCollider.h"
#include <SphereCollider.h>
#include <iostream>
#include <OBBCollider.h>
#include <CapsuleCollider.h>
#include <ConeCollider.h>
#include <PlaneCollider.h>

AABBCollider::AABBCollider(CollisionManager& manager, const Vector3& min, const Vector3& max)
    : Collider(manager)  // Collider の親クラスを初期化
{
    aabb = AABB(min, max);
}


bool AABBCollider::CheckCollision(Collider* other)
{
    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return collisionManager.CheckSphereVsAABB(static_cast<SphereCollider*>(other)->GetCollisionShape(), this->aabb);
    case ColliderType::AABB:
        return collisionManager.CheckAABBVsAABB(this->aabb, static_cast<AABBCollider*>(other)->GetCollisionShape());
    case ColliderType::OBB:
        return collisionManager.CheckAABBVsOBB(this->aabb, static_cast<OBBCollider*>(other)->GetCollisionShape());
    case ColliderType::Capsule:
        return collisionManager.CheckAABBVsCapsule(this->aabb, static_cast<CapsuleCollider*>(other)->GetCollisionShape());
    case ColliderType::Cone:
        return collisionManager.CheckAABBVsCone(this->aabb, static_cast<ConeCollider*>(other)->GetCollisionShape());
    case ColliderType::Plane:
        return collisionManager.CheckPlaneVsAABB(static_cast<PlaneCollider*>(other)->GetCollisionShape(), this->aabb);
    default:
        return false;
    }
}

void AABBCollider::OnCollision(Collider* other)
{
    std::cout << "AABBCollider collided with another object!" << std::endl;
}
