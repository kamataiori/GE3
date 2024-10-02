#pragma once
class Framework
{
public:
	//------メンバ関数------//

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 終了チェック
	/// </summary>
	virtual bool IsEndRequest() { /*return endRequest_;*/ }

public:
	//------メンバ変数------//

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~Framework() = default;

};

