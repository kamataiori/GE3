#include "CapsuleCollider.h"
#include <iostream>
#include <SphereCollider.h>
#include <AABBCollider.h>
#include <OBBCollider.h>
#include <ConeCollider.h>
#include <PlaneCollider.h>

CapsuleCollider::CapsuleCollider(CollisionManager& manager, const Vector3& start, const Vector3& end, float radius)
    : Collider(manager), capsule{ start, end, radius } {
}

bool CapsuleCollider::CheckCollision(Collider* other) {
    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return collisionManager.CheckSphereVsCapsule(static_cast<SphereCollider*>(other)->GetCollisionShape(), this->capsule);
    case ColliderType::AABB:
        return collisionManager.CheckAABBVsCapsule(static_cast<AABBCollider*>(other)->GetCollisionShape(), this->capsule);
    case ColliderType::OBB:
        return collisionManager.CheckOBBVsCapsule(static_cast<OBBCollider*>(other)->GetCollisionShape(), this->capsule);
    case ColliderType::Capsule:
        return collisionManager.CheckCapsuleVsCapsule(this->capsule, static_cast<CapsuleCollider*>(other)->GetCollisionShape());
    case ColliderType::Cone:
        return collisionManager.CheckCapsuleVsCone(this->capsule, static_cast<ConeCollider*>(other)->GetCollisionShape());
    case ColliderType::Plane:
        return collisionManager.CheckPlaneVsCapsule(static_cast<PlaneCollider*>(other)->GetCollisionShape(), this->capsule);
    default:
        return false;
    }
}

void CapsuleCollider::OnCollision(Collider* other) {
    std::cout << "CapsuleCollider collided with another object!" << std::endl;
}
