#include "CollisionManager.h"
#include "Collider.h"

void CollisionManager::RegisterCollider(Collider* collider) {
    colliders.push_back(collider);
}

void CollisionManager::UnregisterCollider(Collider* collider) {
    colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
}

void CollisionManager::Reset() {
    colliders.clear();
}

void CollisionManager::CheckAllCollisions() {
    for (auto it1 = colliders.begin(); it1 != colliders.end(); ++it1) {
        auto it2 = it1;
        ++it2;
        for (; it2 != colliders.end(); ++it2) {
            if ((*it1)->Dispatch(*it2)) {
                (*it1)->OnCollision();
                (*it2)->OnCollision();
            }
        }
    }
}
