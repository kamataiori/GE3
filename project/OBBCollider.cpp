#include "OBBCollider.h"
#include <iostream>
#include <AABBCollider.h>
#include <SphereCollider.h>
#include <CapsuleCollider.h>
#include <ConeCollider.h>
#include <PlaneCollider.h>

OBBCollider::OBBCollider(CollisionManager& manager, const Vector3& center, const Vector3& size, const Vector3 orientations[3])
    : Collider(manager), obb{ center, {orientations[0], orientations[1], orientations[2]}, size } {
}

bool OBBCollider::CheckCollision(Collider* other) {
    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return collisionManager.CheckSphereVsOBB(static_cast<SphereCollider*>(other)->GetCollisionShape(), this->obb);
    case ColliderType::AABB:
        return collisionManager.CheckAABBVsOBB(static_cast<AABBCollider*>(other)->GetCollisionShape(), this->obb);
    case ColliderType::OBB:
        return collisionManager.CheckOBBVsOBB(this->obb, static_cast<OBBCollider*>(other)->GetCollisionShape());
    case ColliderType::Capsule:
        return collisionManager.CheckOBBVsCapsule(this->obb, static_cast<CapsuleCollider*>(other)->GetCollisionShape());
    case ColliderType::Cone:
        return collisionManager.CheckOBBVsCone(this->obb, static_cast<ConeCollider*>(other)->GetCollisionShape());
    case ColliderType::Plane:
        return collisionManager.CheckPlaneVsOBB(static_cast<PlaneCollider*>(other)->GetCollisionShape(), this->obb);
    default:
        return false;
    }
}

void OBBCollider::OnCollision(Collider* other) {
    std::cout << "OBBCollider collided with another object!" << std::endl;
}
