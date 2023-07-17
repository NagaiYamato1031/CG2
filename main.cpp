#include "base/MyConst.h"
#include "base/Mymath.h"

#include "base/MyEngine.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	MyEngine::Initialize("CG2", 1280, 720);

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