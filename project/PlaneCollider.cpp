#include "PlaneCollider.h"
#include <iostream>
#include <SphereCollider.h>
#include <AABBCollider.h>
#include <OBBCollider.h>
#include <CapsuleCollider.h>
#include <ConeCollider.h>
#include <MathFunctions.h>

PlaneCollider::PlaneCollider(CollisionManager& manager, const Vector3& normal, float distance)
    : Collider(manager), plane{ Normalize(normal), distance } {
}

bool PlaneCollider::CheckCollision(Collider* other) {
    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return collisionManager.CheckSphereVsPlane(static_cast<SphereCollider*>(other)->GetCollisionShape(), this->plane);
    case ColliderType::AABB:
        return collisionManager.CheckPlaneVsAABB(this->plane, static_cast<AABBCollider*>(other)->GetCollisionShape());
    case ColliderType::OBB:
        return collisionManager.CheckPlaneVsOBB(this->plane, static_cast<OBBCollider*>(other)->GetCollisionShape());
    case ColliderType::Capsule:
        return collisionManager.CheckPlaneVsCapsule(this->plane, static_cast<CapsuleCollider*>(other)->GetCollisionShape());
    case ColliderType::Cone:
        return collisionManager.CheckPlaneVsCone(this->plane, static_cast<ConeCollider*>(other)->GetCollisionShape());
    case ColliderType::Plane:
        return false; // 平面同士の衝突は定義しない
    default:
        return false;
    }
}

void PlaneCollider::OnCollision(Collider* other) {
    std::cout << "PlaneCollider collided with another object!" << std::endl;
}
