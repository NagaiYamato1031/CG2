#pragma once

namespace NATTO {
	namespace Base {
		class WinApp;
		class DirectXCommon;
	}
	namespace Object {
		class Transform;
		class Triangle;
	}
	namespace Math {
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix3x3;
		class Matrix4x4;
	}

	class Engine {
	public:

		static void Run();

	private:


		static void Initialize(const char* title, int widht, int height);

		static bool ProcessMessage();


		static void BeginFrame();

		static void EndFrame();


		static void Finalize();

	private:
		// ウィンドウ
		static Base::WinApp* winApp_;
		// DirectX
		static Base::DirectXCommon* dxCommon_;

	};

}

