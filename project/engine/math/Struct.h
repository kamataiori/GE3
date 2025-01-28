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
	Vector3 normal;  // 法線ベクトル
	float distance;  // 原点から平面までの距離
	float size;      // 平面のサイズ (正方形の半径)
	int divisions;   // グリッドの分割数
};

struct Capsule {
	Vector3 start;   // カプセルの開始点
	Vector3 end;     // カプセルの終了点
	float radius;    // カプセルの半径
	int color;       // カプセルの色 (Color 型に対応)
	int segments;    // カプセルの円周を構成する分割数
	int rings;       // 球部分を構成する分割数
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

struct OBB {
	Vector3 center;    // 中心点
	Vector3 orientations[3];    // 座標軸。正規化 ・ 直交必須
	Vector3 size;    // 座標軸方向の長さの半分。中心から面までの距離
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
