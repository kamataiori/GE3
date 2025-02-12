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
