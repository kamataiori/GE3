#pragma once
#include "Struct.h"
#include <vector>
#include <algorithm>
#include "Collider.h"

class CollisionManager
{
public:

	// コンストラクタ
	CollisionManager() = default;
	// デストラクタ
	~CollisionManager() = default;

	/// <summary>
	/// コライダーを登録
	/// </summary>
	void RegisterCollider(Collider* collider);

	/// <summary>
	/// コライダーを登録解除
	/// </summary>
	void UnregisterCollider(Collider* collider);

	/// <summary>
	/// すべてのコライダーの衝突をチェック
	/// </summary>
	void CheckAllCollisions();

private:

	// 登録されたコライダーのリスト
	std::vector<Collider*> colliders;

public:

	// Sphere
	bool CheckSphereVsSphere(const Sphere& s1, const Sphere& s2);
	bool CheckSphereVsPlane(const Sphere& sphere, const Plane& plane);
	bool CheckSphereVsAABB(const Sphere& sphere, const AABB& aabb);
	bool CheckSphereVsOBB(const Sphere& sphere, const OBB& obb);
	bool CheckSphereVsCapsule(const Sphere& sphere, const Capsule& capsule);
	bool CheckSphereVsCone(const Sphere& sphere, const Cone& cone);

	// Plane
	bool CheckPlaneVsAABB(const Plane& plane, const AABB& aabb);
	bool CheckPlaneVsOBB(const Plane& plane, const OBB& obb);
	bool CheckPlaneVsCapsule(const Plane& plane, const Capsule& capsule);
	bool CheckPlaneVsCone(const Plane& plane, const Cone& cone);

	// AABB
	bool CheckAABBVsAABB(const AABB& aabb1, const AABB& aabb2);
	bool CheckAABBVsOBB(const AABB& aabb, const OBB& obb);
	bool CheckAABBVsCapsule(const AABB& aabb, const Capsule& capsule);
	bool CheckAABBVsCone(const AABB& aabb, const Cone& cone);

	// OBB
	bool CheckOBBVsOBB(const OBB& obb1, const OBB& obb2);
	bool CheckOBBVsCapsule(const OBB& obb, const Capsule& capsule);
	bool CheckOBBVsCone(const OBB& obb, const Cone& cone);

	// Capsule
	bool CheckCapsuleVsCapsule(const Capsule& capsule1, const Capsule& capsule2);
	bool CheckCapsuleVsCone(const Capsule& capsule, const Cone& cone);

	// Cone
	bool CheckConeVsCone(const Cone& cone1, const Cone& cone2);

private:

	void ClosestPointSegmentToSegment(
		const Vector3& p1, const Vector3& q1, // 線分Aの両端
		const Vector3& p2, const Vector3& q2, // 線分Bの両端
		Vector3& c1, Vector3& c2);

};

