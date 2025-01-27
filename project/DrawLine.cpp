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

    // TransformationMatrix 用のバッファを作成
    transformationMatrixResource_ = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
    *transformationMatrixData_ = { MakeIdentity4x4(), MakeIdentity4x4() }; // 初期値は単位行列
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

void DrawLine::DrawAABB(const AABB& aabb)
{
    // 8つの頂点を計算
    Vector3 vertices[8] = {
        {aabb.min.x, aabb.min.y, aabb.min.z}, // 0: min
        {aabb.max.x, aabb.min.y, aabb.min.z}, // 1: +x
        {aabb.min.x, aabb.max.y, aabb.min.z}, // 2: +y
        {aabb.max.x, aabb.max.y, aabb.min.z}, // 3: +x, +y
        {aabb.min.x, aabb.min.y, aabb.max.z}, // 4: +z
        {aabb.max.x, aabb.min.y, aabb.max.z}, // 5: +x, +z
        {aabb.min.x, aabb.max.y, aabb.max.z}, // 6: +y, +z
        {aabb.max.x, aabb.max.y, aabb.max.z}  // 7: +x, +y, +z
    };

    // 色を取得
    Color color = static_cast<Color>(aabb.color);

    // 各エッジを描画
    AddLine(ToXMFLOAT3(vertices[0]), ToXMFLOAT3(vertices[1]), color, color); // Edge 0-1
    AddLine(ToXMFLOAT3(vertices[1]), ToXMFLOAT3(vertices[3]), color, color); // Edge 1-3
    AddLine(ToXMFLOAT3(vertices[3]), ToXMFLOAT3(vertices[2]), color, color); // Edge 3-2
    AddLine(ToXMFLOAT3(vertices[2]), ToXMFLOAT3(vertices[0]), color, color); // Edge 2-0

    AddLine(ToXMFLOAT3(vertices[4]), ToXMFLOAT3(vertices[5]), color, color); // Edge 4-5
    AddLine(ToXMFLOAT3(vertices[5]), ToXMFLOAT3(vertices[7]), color, color); // Edge 5-7
    AddLine(ToXMFLOAT3(vertices[7]), ToXMFLOAT3(vertices[6]), color, color); // Edge 7-6
    AddLine(ToXMFLOAT3(vertices[6]), ToXMFLOAT3(vertices[4]), color, color); // Edge 6-4

    AddLine(ToXMFLOAT3(vertices[0]), ToXMFLOAT3(vertices[4]), color, color); // Edge 0-4
    AddLine(ToXMFLOAT3(vertices[1]), ToXMFLOAT3(vertices[5]), color, color); // Edge 1-5
    AddLine(ToXMFLOAT3(vertices[2]), ToXMFLOAT3(vertices[6]), color, color); // Edge 2-6
    AddLine(ToXMFLOAT3(vertices[3]), ToXMFLOAT3(vertices[7]), color, color); // Edge 3-7
}

void DrawLine::Update() {
    if (!camera_) return;

    // World-View-Projection 行列を計算
    transformationMatrixData_->World = MakeIdentity4x4(); // 必要に応じて変更
    transformationMatrixData_->WVP = Multiply(transformationMatrixData_->World, camera_->GetViewProjectionMatrix());
}

void DrawLine::Draw() {

    if (vertices_.empty() || !camera_) return;

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

        // TransformationMatrix をシェーダーに設定
        dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, transformationMatrixResource_->GetGPUVirtualAddress());

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

