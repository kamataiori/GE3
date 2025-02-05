#pragma once
#include "CollisionFunctions.h"
#include "DrawLine.h"

class SphereCollider;
class AABBCollider;
class OBBCollider;
class CapsuleCollider;

class Collider {
public:
    
    virtual bool Dispatch(Collider* other) = 0;

    virtual void Draw() = 0;

    virtual void OnCollision();

public:
    virtual bool Action(SphereCollider* other) = 0;
    virtual bool Action(AABBCollider* other) = 0;
    virtual bool Action(OBBCollider* other) = 0;
    virtual bool Action(CapsuleCollider* other) = 0;
};