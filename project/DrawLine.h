#pragma once
#include "DirectXCommon.h"
#include "DrawLineCommon.h"
#include "Color.h"

class DrawLine
{
public:
    // ラインの初期化（始点・終点・色を指定可能）
    void Initialize(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
        Color startColor, Color endColor);

    // ラインの更新（動的に変更可能）
    void Update();

    void Draw();

private:
    void CreateVertexData(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
        const DirectX::XMFLOAT4& startColor, const DirectX::XMFLOAT4& endColor);
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

