/*	手を付け加える場所
* 
* ・基底図形クラスを作る
* ・スクリーン座標上を指定できる Draw 関数を作る
* 
*/


#include "./Engine/base/MyConst.h"
#include "./Engine/math/Mymath.h"

#include "./Engine/base/MyEngine.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	MyEngine::Initialize("CG2", 1280, 720);


	Triangle triangle;
	triangle.Initialize();

	triangle.vertices[0] = { .position{-0.5f,-0.5f,0.0f},.color{1.0f,1.0f,1.0f,1.0f} };
	triangle.vertices[1] = { .position{0.0f,0.5f,0.0f},.color{1.0f,1.0f,1.0f,1.0f} };
	triangle.vertices[2] = { .position{0.5f,-0.5f,0.0f},.color{1.0f,1.0f,1.0f,1.0f} };


	while (MyEngine::ProcessMessage())
	{
		MyEngine::BeginFrame();

		triangle.transform_.rotate_.y += 0.01f;
		triangle.transform_.rotate_.y = triangle.transform_.rotate_.y > 3.14f * 2.0f ? 0 : triangle.transform_.rotate_.y;

		//MyEngine::DrawTriangle(
		//	{ -0.5f,-0.5f,0.0f },
		//	{ 0.0f,0.5f,0.0f },
		//	{ 0.5f,-0.5f,0.0f },
		//	0xFFFFFFFF
		//);


		MyEngine::DrawTriangle(triangle);

		MyEngine::EndFrame();
	}

	MyEngine::Finalize();

	return 0;
}