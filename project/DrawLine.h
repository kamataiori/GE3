#pragma once
#include "DirectXCommon.h"
#include "DrawLineCommon.h"
#include "Color.h"
#include <vector>

class DrawLine
{
public:

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	static DrawLine* GetInstance();


	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// ラインの追加
	/// </summary>
	void AddLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
		Color startColor, Color endColor);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ラインの数を取得
	/// </summary>
	size_t GetLineCount() const;


	/// <summary>
    /// 頂点データとインデックスデータをリセットする
    /// </summary>
	void ResetData();


public:
	// コンストラクタをプライベートに
	DrawLine() = default;
	~DrawLine() = default;

	// コピーコンストラクタと代入演算子を削除
	DrawLine(const DrawLine&) = delete;
	DrawLine& operator=(const DrawLine&) = delete;

public:

	static DrawLine* instance;

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
