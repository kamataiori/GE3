#include "ConeCollider.h"
#include <iostream>
#include <SphereCollider.h>
#include <AABBCollider.h>
#include <OBBCollider.h>
#include <CapsuleCollider.h>

ConeCollider::ConeCollider(CollisionManager& manager, const Vector3& baseCenter, const Vector3& tip, float radius)
    : Collider(manager), cone{ baseCenter, tip, radius } {
}

bool ConeCollider::CheckCollision(Collider* other) {
    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return collisionManager.CheckSphereVsCone(static_cast<SphereCollider*>(other)->GetCollisionShape(), this->cone);
    case ColliderType::AABB:
        return collisionManager.CheckAABBVsCone(static_cast<AABBCollider*>(other)->GetCollisionShape(), this->cone);
    case ColliderType::OBB:
        return collisionManager.CheckOBBVsCone(static_cast<OBBCollider*>(other)->GetCollisionShape(), this->cone);
    case ColliderType::Capsule:
        return collisionManager.CheckCapsuleVsCone(static_cast<CapsuleCollider*>(other)->GetCollisionShape(), this->cone);
    case ColliderType::Cone:
        return collisionManager.CheckConeVsCone(this->cone, static_cast<ConeCollider*>(other)->GetCollisionShape());
    case ColliderType::Plane:
        return collisionManager.CheckPlaneVsCone(static_cast<PlaneCollider*>(other)->GetCollisionShape(), this->cone);
    default:
        return false;
    }
}

void ConeCollider::OnCollision(Collider* other) {
    std::cout << "ConeCollider collided with another object!" << std::endl;
}
