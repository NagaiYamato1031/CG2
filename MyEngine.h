#pragma once

class MyEngine
{
public:
	/// <summary>
	/// システム全体の初期化
	/// </summary>
	/// <param name="title">タイトルバー</param>
	/// <param name="width">ウィンドウ(クライアント領域)の幅</param>
	/// <param name="height">ウィンドウ(クライアント領域)の高さ</param>
	static void Initialize(const char* title, int width = 1280, int height = 720);

	/// <summary>
	/// システム全体の終了
	/// </summary>
	static void Finalize();

	/// <summary>
	/// 画像ファイルを読み込む
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>テクスチャのハンドル</returns>
	//static int LoadTexture(const char* fileName);

	/// <summary>
	/// 画像ファイルの読み込みを解除する
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	//static void UnloadTexture(int textureHandle);

	/// <summary>
	/// スプライトを描画する
	/// </summary>
	/// <param name="x">描画するスプライトの左上座標 X</param>
	/// <param name="y">描画するスプライトの左上座標 Y</param>
	/// <param name="textureHandle">テクスチャのハンドル</param>
	/// <param name="scale">描画するスプライトの倍率 X</param>
	/// <param name="scale">描画するスプライトの倍率 Y</param>
	/// <param name="angle">描画するスプライトの回転角</param>
	/// <param name="color">描画するスプライトの色</param>
	/*static void DrawSprite(
		int x, int y, int textureHandle, float scaleX, float scaleY, float angle,
		unsigned int color);*/

	/// <summary>
	/// Windowsのメッセージを処理する
	/// </summary>
	/// <returns>1: ゲーム終了 0: ゲーム継続</returns>
	static int ProcessMessage();

	/// <summary>
	/// フレーム開始処理。ブレンドモードはkBlendNormalに設定されます。
	/// </summary>
	static void BeginFrame();

	/// <summary>
	/// フレーム終了処理
	/// </summary>
	static void EndFrame();

};

