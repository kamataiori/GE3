#pragma once
#include "Transform.h"

class CharacterBase
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    virtual void Initialize() = 0;

    /// <summary>
    /// 更新
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// 描画
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// Transformを取得
    /// </summary>
    const Transform& GetTransform() const { return transform; }

protected:
    Transform transform; // キャラクターの基本Transform
};
