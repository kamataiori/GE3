#include "DrawLine.h"

void DrawLine::Initialize(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
    Color startColor, Color endColor)
{
    // DirectXCommon のインスタンスを取得
    dxCommon_ = DirectXCommon::GetInstance();

    // DrawLineCommon のインスタンスを取得
    drawLineCommon_ = DrawLineCommon::GetInstance();

    // 始点・終点・色を設定
    startPoint_ = startPoint;
    endPoint_ = endPoint;
    startColor_ = GetColorValue(startColor);
    endColor_ = GetColorValue(endColor);

    // 頂点データとインデックスデータを作成
    CreateVertexData(startPoint, endPoint, GetColorValue(startColor), GetColorValue(endColor));
    CreateIndexData();
}

void DrawLine::Update()
{
    //// 始点・終点・色を更新
    //startPoint_ = startPoint;
    //endPoint_ = endPoint;
    //startColor_ = GetColorValue(startColor);
    //endColor_ = GetColorValue(endColor);

    //// 頂点データを更新
    //vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    //vertexData_[0] = { {startPoint_.x, startPoint_.y, startPoint_.z},
    //                   {startColor_.x, startColor_.y, startColor_.z, startColor_.w} }; // 始点
    //vertexData_[1] = { {endPoint_.x, endPoint_.y, endPoint_.z},
    //                   {endColor_.x, endColor_.y, endColor_.z, endColor_.w} }; // 終点
    //vertexResource_->Unmap(0, nullptr);
}

void DrawLine::Draw()
{
    // トポロジの設定
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    // 頂点バッファの設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

    // インデックスバッファの設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

    // 描画コマンドの発行
    dxCommon_->GetCommandList()->DrawIndexedInstanced(2, 1, 0, 0, 0);
}

void DrawLine::CreateVertexData(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint, const DirectX::XMFLOAT4& startColor, const DirectX::XMFLOAT4& endColor)
{
    // 頂点バッファサイズの計算（2つの頂点を保持する）
    const size_t vertexBufferSize = sizeof(DrawLineCommon::Vertex) * 2;

    // 頂点リソースの作成
    vertexResource_ = dxCommon_->CreateBufferResource(vertexBufferSize);

    // 頂点バッファビューの設定
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast<UINT>(vertexBufferSize);
    vertexBufferView_.StrideInBytes = sizeof(DrawLineCommon::Vertex);

    // 頂点データを初期化（Mapして直接書き込み）
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    vertexData_[0] = { {startPoint.x, startPoint.y, startPoint.z},
                       {startColor.x, startColor.y, startColor.z, startColor.w} }; // 始点
    vertexData_[1] = { {endPoint.x, endPoint.y, endPoint.z},
                       {endColor.x, endColor.y, endColor.z, endColor.w} }; // 終点

    vertexResource_->Unmap(0, nullptr);
}

void DrawLine::CreateIndexData()
{
    const size_t indexBufferSize = sizeof(uint32_t) * 2;

    // インデックスリソースの作成
    indexResource_ = dxCommon_->CreateBufferResource(indexBufferSize);

    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = static_cast<UINT>(indexBufferSize);
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    // インデックスデータを設定
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
    indexData_[0] = 0; // 始点
    indexData_[1] = 1; // 終点
    indexResource_->Unmap(0, nullptr);
}
