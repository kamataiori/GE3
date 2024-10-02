#include <MyGame.h>

//BlendMode
enum BlendMode {
	//!< ブレンドなし
	kBlendModeNone,
	//!< 通常αブレンド。デフォルト。Src * SrcA + Dest * (1 - SrcA)
	kBlendModeNormal,
	//!< 加算。Src * SrcA + Dest * 1
	kBlendModeAdd,
	//!< 減算。Dest * 1 - Src * SrcA
	kBlendModeSubtract,
	//!< 乗算。Src * 0 + Dest * Src
	kBlendModeMultiply,
	//!< スクリーン。Src * (1 - Dest) + Dest * 1
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

BlendMode mode = kBlendModeNormal;

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	
	//リークチェッカー
	D3DResourceLeakChecker leakCheck;

	//MyGameのインスタンス作成
	MyGame game;

	// ゲームの初期化
	game.Initialize();

	//メインループ
	//ウィンドウの×ボタンが押されるまでループ
	while (true) {

		//毎フレーム更新
		game.Update();

		//終了リクエストが来たら抜ける
		if (game.IsEndRequest())
		{
			//ゲームループを抜ける
			break;
		}

		//描画
		game.Draw();
	}
	//ゲームの終了
	game.Finalize();

	return 0;
}