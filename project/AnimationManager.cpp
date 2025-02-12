#include "AnimationManager.h"
#include <assimp/anim.h>
#include <assimp/Importer.hpp>
#include <iostream>
#include <cassert>
#include <assimp/scene.h>

AnimationData AnimationManager::LoadAnimationFile(const std::string& directoryPath, const std::string& fileName)
{
	AnimationData animation; // 今回作るアニメーション

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	// ファイルの読み込みが成功したか確認
	if (!scene) {
		std::cerr << "Error loading animation file: " << importer.GetErrorString() << std::endl;
		assert(scene && "Failed to load animation file.");
		return animation; // もしくは適切なエラー処理を行う
	}

	// アニメーションが含まれているか確認
	assert(scene->mNumAnimations != 0 && "No animations found in the file.");

	aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメーションだけ採用。もちろん複数対応するに越したことはない

	// 時間の単位を秒に変換
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	// assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報をとってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.NodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		// 各PositionKeysをKeyframeVector3としてNodeAnimationに追加
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここでも秒に変換
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手→左手
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		// 各RotationKeysをKeyframeQuaternionとしてNodeAnimationに追加
		if (nodeAnimationAssimp->mNumRotationKeys > 0)
		{
			// RotationKeysが存在するかチェック
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyframeQuaternion keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 右手→左手
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}
		}

		// 各ScalingKeysをKeyframeVector3としてNodeAnimationに追加
		if (nodeAnimationAssimp->mNumScalingKeys > 0)
		{
			// ScalingKeysが存在するかチェック
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}

	}

	// 解析完了
	return animation;
}
