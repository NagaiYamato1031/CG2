#include "./MyEngine.h"
#include "./MyUtility.h"
#include "../math/Mymath.h"
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")


void MyEngine::Initialize(const char* title, int width, int height) {
	
	winApp_ = WinApp::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	canvasTool_ = CanvasTool::GetInstance();

	winApp_->Initialize(title, width, height);
	dxCommon_->Initialize(winApp_, width, height);
	canvasTool_->Initialize(dxCommon_);

}

void MyEngine::Finalize() {

	CloseWindow(winApp_->GetHWND());

}
bool MyEngine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}


void MyEngine::BeginFrame() {
	dxCommon_->PreDraw();
}

void MyEngine::EndFrame() {
	dxCommon_->PostDraw();
	// 描画数リセット
	canvasTool_->Reset();
}

#pragma region 描画関数

void MyEngine::DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color) {
	canvasTool_->DrawTriangle(pos1, pos2, pos3, color);
}


#pragma endregion

#pragma region メンバ変数

WinApp* MyEngine::winApp_ = nullptr;
DirectXCommon* MyEngine::dxCommon_ = nullptr;
CanvasTool* MyEngine::canvasTool_ = nullptr;

#pragma endregion