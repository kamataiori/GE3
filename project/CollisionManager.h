#pragma once
#include "Struct.h"
#include <vector>
#include <algorithm>

class Collider;

#pragma once
#include "Collider.h"
#include <vector>

class CollisionManager {
public:
    void RegisterCollider(Collider* collider);
    void UnregisterCollider(Collider* collider);
    void Reset();
    void CheckAllCollisions();
private:
    std::vector<Collider*> colliders;
};


