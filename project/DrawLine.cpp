#include "DrawLine.h"

DrawLine* DrawLine::instance = nullptr;

DrawLine* DrawLine::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new DrawLine;
    }
    return instance;
}

void DrawLine::Finalize()
{
    delete instance;
    instance = nullptr;
}

void DrawLine::Initialize()
{
    // DirectXCommon のインスタンスを取得
    dxCommon_ = DirectXCommon::GetInstance();

    // DrawLineCommon のインスタンスを取得
    drawLineCommon_ = DrawLineCommon::GetInstance();
}

void DrawLine::AddLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
    Color startColor, Color endColor) {
    // 現在の頂点数を基準にインデックスを設定
    uint32_t baseIndex = static_cast<uint32_t>(vertices_.size());

    // 色を取得してfloat配列に変換
    DirectX::XMFLOAT4 startColorValue = GetColorValue(startColor);
    DirectX::XMFLOAT4 endColorValue = GetColorValue(endColor);

    // 頂点データを追加
    vertices_.push_back({
        {startPoint.x, startPoint.y, startPoint.z}, // 座標
        {startColorValue.x, startColorValue.y, startColorValue.z, startColorValue.w} // 色
        });

    vertices_.push_back({
        {endPoint.x, endPoint.y, endPoint.z}, // 座標
        {endColorValue.x, endColorValue.y, endColorValue.z, endColorValue.w} // 色
        });

    // インデックスデータを追加
    indices_.push_back(baseIndex);
    indices_.push_back(baseIndex + 1);
}

void DrawLine::Update() {
    
}

void DrawLine::Draw() {

    if (vertices_.size() > 0)
    {
        const size_t vertexBufferSize = vertices_.size() * sizeof(DrawLineCommon::Vertex);
        const size_t indexBufferSize = indices_.size() * sizeof(uint32_t);

        // 頂点バッファの更新
        vertexResource_ = dxCommon_->CreateBufferResource(vertexBufferSize);
        vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
        memcpy(vertexData_, vertices_.data(), vertexBufferSize);
        vertexResource_->Unmap(0, nullptr);

        // インデックスバッファの更新
        indexResource_ = dxCommon_->CreateBufferResource(indexBufferSize);
        indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
        memcpy(indexData_, indices_.data(), indexBufferSize);
        indexResource_->Unmap(0, nullptr);

        // バッファビューを更新
        vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
        vertexBufferView_.SizeInBytes = static_cast<UINT>(vertexBufferSize);
        vertexBufferView_.StrideInBytes = sizeof(DrawLineCommon::Vertex);

        indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
        indexBufferView_.SizeInBytes = static_cast<UINT>(indexBufferSize);
        indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

        // トポロジの設定
        dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

        // 頂点バッファの設定
        dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

        // インデックスバッファの設定
        dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

        // 描画コマンドの発行
        dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
    }

}

size_t DrawLine::GetLineCount() const
{
    return vertices_.size() / 2;
}

// 頂点データとインデックスデータをリセットする
void DrawLine::ResetData() {
    vertices_.clear();
    indices_.clear();
    vertexData_ = nullptr;
    indexData_ = nullptr;
}

