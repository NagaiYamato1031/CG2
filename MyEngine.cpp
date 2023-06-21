#include "MyEngine.h"
#include "Mymath.h"

#include <Windows.h>
#include <string>
#include <cassert>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "SafeDelete.h"

namespace {


	std::wstring ConvertString(const std::string& str) {
		if (str.empty()) {
			return std::wstring();
		}

		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}

	std::string ConvertString(const std::wstring& str) {
		if (str.empty()) {
			return std::string();
		}

		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
		if (sizeNeeded == 0) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
		return result;

	}

	class MyEngineSystem {
		friend class MyEngine;

	public:
		MyEngineSystem() = default;
		~MyEngineSystem() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// リセット
		/// </summary>
		void Reset();

	private:

#pragma region 定数

#pragma endregion

#pragma region 構造体

#pragma endregion

		MyEngineSystem(const MyEngineSystem&) = delete;
		MyEngineSystem& operator=(const MyEngineSystem&) = delete;

#pragma region 内部関数

		int ProcessMessage();
		void BegineFrame();
		void EndFrame();

		HWND GetWindowHandle();

#pragma endregion


#pragma region 内部変数

		// WindowsAPI
		WinApp* winApp_ = nullptr;
		// DirectX
		DirectXCommon* dxCommon_ = nullptr;


		// 定数バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;

#pragma endregion

	};

#pragma region MyEngineSystem関数


	void MyEngineSystem::Initialize() {

		dxCommon_ = DirectXCommon::GetInstance();
		winApp_ = WinApp::GetInstance();

	}

	void MyEngineSystem::Reset() {

	}

	int MyEngineSystem::ProcessMessage() { return winApp_->ProcessMessage() ? 1 : 0; }

	void MyEngineSystem::BegineFrame() {
		dxCommon_->PreDraw();
	}

	void MyEngineSystem::EndFrame() {

		//ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

		// DirectX描画終了
		dxCommon_->PostDraw();

		Reset();
	}


	HWND MyEngineSystem::GetWindowHandle() { return winApp_->GetHwnd(); }

#pragma endregion

	WinApp* sWinApp = nullptr;
	DirectXCommon* sDxCommon = nullptr;

	MyEngineSystem* sMyEngineSystem = nullptr;

} // namespace

void MyEngine::Initialize(const char* title, int width, int height) {
	// 既に初期化済み
	assert(!sWinApp);
	assert(!sDxCommon);

	assert(!sMyEngineSystem);

	// ゲームウィンドウの作成
	auto&& titleString = ConvertString(title);
	sWinApp = WinApp::GetInstance();
	sWinApp->CreateGameWindow(
		titleString.c_str(), WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
		width, height);
	sWinApp->SetSizeChangeMode(WinApp::SizeChangeMode::kNone);

	// DirectX 初期化
	sDxCommon = DirectXCommon::GetInstance();
	sDxCommon->Initialize(sWinApp, width, height);


	sMyEngineSystem = new MyEngineSystem();
	sMyEngineSystem->Initialize();

}

void MyEngine::Finalize() {
	// 各種解放
	SafeDelete(sMyEngineSystem);
	
	// ゲームウィンドウの破棄
	sWinApp->TerminateGameWindow();

}

int MyEngine::ProcessMessage() { return sMyEngineSystem->ProcessMessage(); }

void MyEngine::BeginFrame() { sMyEngineSystem->BegineFrame(); }

void MyEngine::EndFrame() { sMyEngineSystem->EndFrame(); }