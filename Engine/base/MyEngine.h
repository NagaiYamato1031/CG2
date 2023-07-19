#pragma once

#include "./WinApp.h"
#include "./DirectXCommon.h"
#include "./CanvasTool.h"
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

#pragma region 描画関数

	/// <summary>
	/// 三角形を描画する
	/// </summary>
	/// <param name="pos1">座標１</param>
	/// <param name="pos2">座標２</param>
	/// <param name="pos3">座標３</param>
	/// <param name="color">カラー</param>
	static void DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color);

#pragma endregion


	/// <summary>
	/// Windowsのメッセージを処理する
	/// </summary>
	/// <returns>1: ゲーム終了 0: ゲーム継続</returns>
	static bool ProcessMessage();

	/// <summary>
	/// フレーム開始処理。ブレンドモードはkBlendNormalに設定されます。
	/// </summary>
	static void BeginFrame();

	/// <summary>
	/// フレーム終了処理
	/// </summary>
	static void EndFrame();

private: // メンバ関数

	// ウィンドウ
	static WinApp* winApp_;
	// DirectX
	static DirectXCommon* dxCommon_;
	// 描画システム
	static CanvasTool* canvasTool_;

};

