#pragma once
#include <Vector3.h>
#include <Quaternion.h>
#include <vector>
#include <map>
#include <string>

// Keyframe構造体
template <typename tValue>
struct Keyframe {
	float time;
	tValue value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

// NodeのAnimation構造体
template<typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

// Animationを表現する

struct AnimationData {
	float duration;  // アニメーション全体の尺
	// NodeAnimationの集合。Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> NodeAnimations;
};

//=====EulerTransform=========

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;  // Eulerでの回転
	Vector3 translate;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct Joint {
	QuaternionTransform transform;  // Transform情報
	Matrix4x4 localMatrix;  // localMatrix
	Matrix4x4 skeletonSpaceMatrix;  // skeletonSpaceでの変換行列
	std::string name;  // 名前
	std::vector<int32_t> children;  // 子JointのIndexのリスト。いなければ空き
	int32_t index; // 自身のIndex
	std::optional<int32_t> parent;  // 親JointのIndex。いなければnull
};

struct Skeleton {
	int32_t root;  // RootJointのIndex
	std::map<std::string, int32_t> jointMap;  // Joint名とIndexとの辞書
	std::vector<Joint> joints;  // 所属しているジョイント
};
