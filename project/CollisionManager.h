#pragma once
#include "Struct.h"
#include <list>
#include <algorithm>

class Collider;

class CollisionManager {
public:
    void RegisterCollider(Collider* collider);
    void UnregisterCollider(Collider* collider);
    void Reset();
    void CheckAllCollisions();
private:
    std::list<Collider*> colliders;
};


