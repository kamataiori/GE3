#pragma once
#include "DirectXCommon.h"
#include "DrawLineCommon.h"
#include "Color.h"

class DrawLine
{
public:
    void Initialize(); // 初期化（リソース作成のみ）
    void Update(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
        Color startColor, Color endColor); // データの更新
    void Draw(); // 描画

private:
    void CreateVertexData();
    void CreateIndexData();

    DirectXCommon* dxCommon_ = nullptr;
    DrawLineCommon* drawLineCommon_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;

    DrawLineCommon::Vertex* vertexData_ = nullptr;
    uint32_t* indexData_ = nullptr;

    DirectX::XMFLOAT3 startPoint_ = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 endPoint_ = { 1.0f, 1.0f, 0.0f };
    DirectX::XMFLOAT4 startColor_ = GetColorValue(Color::WHITE);
    DirectX::XMFLOAT4 endColor_ = GetColorValue(Color::WHITE);
};
