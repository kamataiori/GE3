#include "CollisionManager.h"

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
    for (size_t i = 0; i < colliders.size(); ++i) {
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            if (colliders[i]->Dispatch(colliders[j])) {
                colliders[i]->OnCollision();
                colliders[j]->OnCollision();
            }
        }
    }
}
