#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

struct Sphere {
	Vector3 center;
	float radius;
	int color;
};

struct Plane {
	Vector3 normal;
	float distance;
};

struct Line {
	Vector3 origin; //start point
	Vector3 diff; //difference between start and end
};

struct Ray {
	Vector3 origin; //start point
	Vector3 diff; //difference between start and end
};

struct Segment {
	Vector3 origin; //start point
	Vector3 diff; //difference between start and end
	int color;
};

struct Triangle {
	Vector3 vertices[3];
	int color;
};

struct AABB {
	Vector3 min;
	Vector3 max;
	int color;
};

struct Spring {
	Vector3 anchor;
	float naturalLength;
	float stiffness;
	float dampingCoefficient;
};

struct Ball {
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

struct Pendulum {
	Vector3 anchor;
	float length;
	float angle;
	float angularVelocity;
	float angularAcceleration;
};

struct ConicalPendulum {
	Vector3 anchor;
	float length;
	float halfApexAngle;
	float angle;
	float angularVelocity;
};
