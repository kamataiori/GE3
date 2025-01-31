#pragma once
#include "CollisionFunctions.h"
#include "DrawLine.h"

class SphereCollider;
class AABBCollider;
class OBBCollider;
class CapsuleCollider;

class Collider {
public:
    
    virtual void Dispatch(Collider* other) = 0;

    virtual void Draw() = 0;

public:
    virtual void Action(SphereCollider* other) = 0;
    virtual void Action(AABBCollider* other) = 0;
    virtual void Action(OBBCollider* other) = 0;
    virtual void Action(CapsuleCollider* other) = 0;
};