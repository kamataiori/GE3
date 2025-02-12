#pragma once
#include "StructAnimation.h"

class AnimationManager
{
public:

	/// <summary>
	/// Animation解析の関数
	/// </summary>
	AnimationData LoadAnimationFile(const std::string& directoryPath, const std::string& fileName);

	template <typename T>
	T CalculateValue(const std::vector<Keyframe<T>>& keyframes, float time)
	{
		assert(!keyframes.empty());  // キーがないものは返す値がわからないのでダメ
		if (keyframes.size() == 1 || time <= keyframes[0].time)
		{
			// キーが1つか、時刻がキーフレーム前なら最初の値とする
			return keyframes[0].value;
		}

		// 補間のためのループ
		for (size_t index = 0; index < keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
			if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
				// 範囲内なら補間する
				float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
				if constexpr (std::is_same<T, Vector3>::value) {
					return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
				}
				else if constexpr (std::is_same<T, Quaternion>::value) {
					return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
				}
			}
		}

		// ここまできた場合は一番後の時刻よりも後なので最後の値を返す
		return keyframes.back().value;
	}

private:


};

