#pragma once

#include "DirectXCommon.h"
#include "DrawLineCommon.h"
#include "Color.h"
#include <vector>

class DrawLine
{
public:
    void Initialize(); // 初期化（リソース作成のみ）
    void AddLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
        Color startColor, Color endColor); // ラインの追加
    void Update(); // バッファの更新
    void Draw(); // 描画

private:
    DirectXCommon* dxCommon_ = nullptr;
    DrawLineCommon* drawLineCommon_ = nullptr;

    // 頂点データとインデックスデータを保持
    std::vector<DrawLineCommon::Vertex> vertices_;
    std::vector<uint32_t> indices_;

    // GPUリソースとビュー
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;

    DrawLineCommon::Vertex* vertexData_ = nullptr;
    uint32_t* indexData_ = nullptr;
};
