#include "SphereCollider.h"
#include <iostream>
#include <AABBCollider.h>
#include <OBBCollider.h>
#include <CapsuleCollider.h>
#include <ConeCollider.h>
#include <PlaneCollider.h>

SphereCollider::SphereCollider(CollisionManager& manager, const Vector3& center, float radius)
    : Collider(manager), sphere{ center, radius } {
}

bool SphereCollider::CheckCollision(Collider* other)
{
    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return collisionManager.CheckSphereVsSphere(this->sphere,
            static_cast<SphereCollider*>(other)->GetCollisionShape());
    case ColliderType::AABB:
        return collisionManager.CheckSphereVsAABB(this->sphere,
            static_cast<AABBCollider*>(other)->GetCollisionShape());
    case ColliderType::OBB:
        return collisionManager.CheckSphereVsOBB(this->sphere,
            static_cast<OBBCollider*>(other)->GetCollisionShape());
    case ColliderType::Capsule:
        return collisionManager.CheckSphereVsCapsule(this->sphere,
            static_cast<CapsuleCollider*>(other)->GetCollisionShape());
    case ColliderType::Cone:
        return collisionManager.CheckSphereVsCone(this->sphere,
            static_cast<ConeCollider*>(other)->GetCollisionShape());
    case ColliderType::Plane:
        return collisionManager.CheckSphereVsPlane(this->sphere,
            static_cast<PlaneCollider*>(other)->GetCollisionShape());
    default:
        return false;
    }
}

void SphereCollider::OnCollision(Collider* other)
{
	std::cout << "SphereCollider collided with another object!" << std::endl;
}
