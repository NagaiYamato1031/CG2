#include "./Engine/base/MyConst.h"
#include "./Engine/math/Mymath.h"

#include "./Engine/base/MyEngine.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	MyEngine::Initialize("CG2", 1280, 720);


	Triangle triangle;
	triangle.Initialize();

	triangle.vertices[0] = { -0.5f,-0.5f,0.0f };
	triangle.vertices[1] = { 0.0f,0.5f,0.0f };
	triangle.vertices[2] = { 0.5f,-0.5f,0.0f };


	while (MyEngine::ProcessMessage())
	{
		MyEngine::BeginFrame();


		MyEngine::DrawTriangle(
			{ -0.5f,-0.5f,0.0f },
			{ 0.0f,0.5f,0.0f },
			{ 0.5f,-0.5f,0.0f },
			0xFFFFFFFF
		);

		MyEngine::EndFrame();
	}

	MyEngine::Finalize();

	return 0;
}