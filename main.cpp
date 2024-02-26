#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#include <numbers>
#include <memory>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include "MyConst.h"
#include "Mymath.h"

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material
{
	Vector4 color;
	int32_t enableLighting;
	float shininess;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight
{
	Vector4 color;		// ライトの色
	Vector3 direction;	// ライトの向き
	float intensity;	// 輝度
};

struct CameraForGPU
{
	Vector3 worldPosition;
};

void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

std::wstring ConvertString(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0)
	{
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str)
{
	if (str.empty())
	{
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0)
	{
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;

}


// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}

IDxcBlob* CompileShader(
	// Compile する Shader ファイルへのパス
	const std::wstring& filePath,
	// Compiler に使用する Profile
	const wchar_t* profile,
	// 初期化で生成したものを 3 つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
#pragma region hlsl ファイルを読む

	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlsl ファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	// UTF8 の文字コードであることを通知
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

#pragma endregion

#pragma region Compile する

	LPCWSTR arguments[] = {
		filePath.c_str(),			// コンパイル対象の hlsl ファイル名
		L"-E",L"main",				// エントリーポイントの指定。基本的に main 以外にはしない
		L"-T",profile,				// ShaderProfile の設定
		L"-Zi", L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",						// 最適化を外しておく
		L"-Zpr",					// メモリレイアウトは行優先
	};
	// 実際に Shader をコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,		// 読み込んだファイル
		arguments,					// コンパイルオプション
		_countof(arguments),		// コンパイルオプションの数
		includeHandler,				// include が含まれた諸々
		IID_PPV_ARGS(&shaderResult)	// コンパイル結果
	);
	// コンパイルエラーではなく dxc が起動できないなどの致命的な状況
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 警告・エラーが出ていないか確認する

	// 警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		// 警告・エラーダメ絶対
		assert(false);
	}

#pragma endregion

#pragma region Compile 結果を受け取って返す

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;

#pragma endregion

}


ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	// UploadHeap を使う
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソースの設定
	D3D12_RESOURCE_DESC bufferResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// リソースのサイズ。
	bufferResourceDesc.Width = sizeInBytes;
	// バッファの場合はこれらは 1 にする決まり
	bufferResourceDesc.Height = 1;
	bufferResourceDesc.DepthOrArraySize = 1;
	bufferResourceDesc.MipLevels = 1;
	bufferResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ID3D12Resource* bufferResource = nullptr;
	device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(bufferResource != nullptr);
	return bufferResource;
}

ID3D12DescriptorHeap* CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescrioptors, bool shaderVisible)
{
	// ディスクリプタヒープの生成
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescrioptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	hr;
	return descriptorHeap;
}

DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;
}

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{

#pragma region metadata を基に Resource の設定

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);								// Texture の幅
	resourceDesc.Height = UINT(metadata.height);							// Texture の高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);					// mipmap の数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);				// 奥行き or 配列 Texture の配列数
	resourceDesc.Format = metadata.format;									// Texture の Format
	resourceDesc.SampleDesc.Count = 1;										// サンプリングカウント。1 固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	// Texture の次元数。普段は 2 次元

#pragma endregion

#pragma region 利用する Heap の設定

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;							// 細かい設定を行う
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// WriteBack ポリシーで CPU アクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;				// プロセッサの近くに配置

#pragma endregion

#pragma region Resource を生成する

	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heap の設定
		D3D12_HEAP_FLAG_NONE,				// Heap の特殊な設定。特になし。
		&resourceDesc,						// Resource の設定
		D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
		nullptr,							// Clear 最適値。使わないので nullptr
		IID_PPV_ARGS(&resource)				// 作成する Resource ポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	hr;
	return resource;

#pragma endregion

}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
	// 生成する Resource の設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;										// Texture の幅
	resourceDesc.Height = height;									// Texture の高さ
	resourceDesc.MipLevels = 1;										// mipmap の数
	resourceDesc.DepthOrArraySize = 1;								// 奥行き or 配列 Texture の配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// DepthStencil として利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;								// サンプリングカウント。1 固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2 次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// DepthStencil として使う通知

	// 利用する Heap の設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM 上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;				// 1.0f (最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// フォーマット。Resource と合わせる

	// Resource の生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heap の設定
		D3D12_HEAP_FLAG_NONE,				// Haap の特殊な設定。特になし。
		&resourceDesc,						// Resource の設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値を書き込む状態にしておく
		&depthClearValue,					// Clear 最適値
		IID_PPV_ARGS(&resource)				// 作成する Resource ポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	hr;
	return resource;
}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	// Meta 情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全 MipMap について
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		// MipLevel を指定して各 Image を取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Texture に転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,				// 全領域へコピー
			img->pixels,			// 元データアドレス
			UINT(img->rowPitch),	// 1 ラインサイズ
			UINT(img->slicePitch)	// 1 枚サイズ
		);
		assert(SUCCEEDED(hr));
		hr;
	}
}

[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	// Texture への転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DEST から D3D12_RESOURCE_STATE_GENERIC_READ へ ResourceState を変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

void CreateSphere(VertexData* const vertexData, const uint32_t& kSubdivision)
{
	const float& pi = static_cast<float>(std::numbers::pi);
	// 分割数
	//const uint32_t kSubdivision = 16;
	// 経度分割 1 つ分の角度
	const float kLonEvery = 2 * pi / float(kSubdivision);
	// 緯度分割 1 つ分の角度
	const float kLatEvery = pi / float(kSubdivision);

	// 頂点の数
	//uint32_t vertexSize = kSubdivision * kSubdivision * 6;
	// 頂点データ
	//VertexData vertexData[6];

	const float dTheta = pi / kSubdivision;
	const float dPhi = 2 * pi / kSubdivision;
	// 緯度の方向に分割 -π / 2 ～ π / 2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++)
	{
		// 現在の緯度
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		// 経度の方向に分割 0 ～ 2 π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++)
		{
			// 現在の経度
			float lon = lonIndex * kLonEvery;
			// world 座標系での a, b, c を求める
			Vector4 pA, pB, pC, pD;
			// texcoord
			Vector2 tA, tB, tC, tD;
			tA.x = float(lonIndex) / float(kSubdivision);
			tA.y = 1.0f - float(latIndex) / float(kSubdivision);
			tB.x = float(lonIndex) / float(kSubdivision);
			tB.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			tC.x = float(lonIndex + 1) / float(kSubdivision);
			tC.y = 1.0f - float(latIndex) / float(kSubdivision);
			tD.x = float(lonIndex + 1) / float(kSubdivision);
			tD.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			// a, b, c を Screen 座標系まで変換
			pA = { cosf(lat) * cosf(lon),sinf(lat),cosf(lat) * sinf(lon),1.0f };
			pB = { cosf(lat + dTheta) * cosf(lon),sinf(lat + dTheta),cosf(lat + dTheta) * sinf(lon) ,1.0f };
			pC = { cosf(lat) * cosf(lon + dPhi),sinf(lat),cosf(lat) * sinf(lon + dPhi) ,1.0f };
			pD = { cosf(lat + dTheta) * cosf(lon + dPhi),sinf(lat + dTheta),cosf(lat + dTheta) * sinf(lon + dPhi) ,1.0f };

			uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 6;

			vertexData[startIndex].position = pA;
			vertexData[startIndex].texcoord = tA;
			vertexData[startIndex].normal.x = vertexData[startIndex].position.x;
			vertexData[startIndex].normal.y = vertexData[startIndex].position.y;
			vertexData[startIndex].normal.z = vertexData[startIndex].position.z;

			vertexData[startIndex + 1].position = pB;
			vertexData[startIndex + 1].texcoord = tB;
			vertexData[startIndex + 1].normal.x = vertexData[startIndex + 1].position.x;
			vertexData[startIndex + 1].normal.y = vertexData[startIndex + 1].position.y;
			vertexData[startIndex + 1].normal.z = vertexData[startIndex + 1].position.z;

			vertexData[startIndex + 2].position = pC;
			vertexData[startIndex + 2].texcoord = tC;
			vertexData[startIndex + 2].normal.x = vertexData[startIndex + 2].position.x;
			vertexData[startIndex + 2].normal.y = vertexData[startIndex + 2].position.y;
			vertexData[startIndex + 2].normal.z = vertexData[startIndex + 2].position.z;

			vertexData[startIndex + 3].position = pC;
			vertexData[startIndex + 3].texcoord = tC;
			vertexData[startIndex + 3].normal.x = vertexData[startIndex + 3].position.x;
			vertexData[startIndex + 3].normal.y = vertexData[startIndex + 3].position.y;
			vertexData[startIndex + 3].normal.z = vertexData[startIndex + 3].position.z;

			vertexData[startIndex + 4].position = pB;
			vertexData[startIndex + 4].texcoord = tB;
			vertexData[startIndex + 4].normal.x = vertexData[startIndex + 4].position.x;
			vertexData[startIndex + 4].normal.y = vertexData[startIndex + 4].position.y;
			vertexData[startIndex + 4].normal.z = vertexData[startIndex + 4].position.z;

			vertexData[startIndex + 5].position = pD;
			vertexData[startIndex + 5].texcoord = tD;
			vertexData[startIndex + 5].normal.x = vertexData[startIndex + 5].position.x;
			vertexData[startIndex + 5].normal.y = vertexData[startIndex + 5].position.y;
			vertexData[startIndex + 5].normal.z = vertexData[startIndex + 5].position.z;

		}
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{

#pragma region 初期化

#pragma region 一次初期化

#pragma region COM の初期化

	CoInitializeEx(0, COINIT_MULTITHREADED);

#pragma endregion

#pragma region ウィンドウプロシージャの設定

	WNDCLASS wc{ };
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名(なんでもいい)
	wc.lpszClassName = L"CG2WindowClass";
	// インスタントハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc);

#pragma endregion

#pragma region ウィンドウサイズを設定

	// クライアントの領域サイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	// クライアント領域を元に実際のサイズに wrc を変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

#pragma endregion

#pragma region ウィンドウを生成

	//ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,		// 利用するクラス名
		L"CG2",					// タイトルバーの文字(なんでもいい)
		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
		CW_USEDEFAULT,			// 表示 X 座標(Windowsに任せる)
		CW_USEDEFAULT,			// 表示 Y 座標(WindowsOSに任せる)
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		wc.hInstance,			// インスタンスハンドル
		nullptr					// オプション
	);

	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

#pragma endregion

#pragma region DebugLayer

#ifdef _DEBUG

	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらに GPU 側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}

#endif // _DEBUG


#pragma endregion

#pragma region DirectX12 の初期化

#pragma region DXGIFactory の生成

	// DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	// HRESULT は Windows 系のエラーであり、
	// 関数が成功したかどうかを SUCCEEDED マクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	// 初期化の根本的なエラーが出た場合はプログラムが間違っているか、
	// どうにも出来ない場合が多いので assert にしておく
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 使用するアダプタ(GPU)を決定する

	// 使用するアダプタ用の変数、最初に nullptr を入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	// いい順にアダプタを選ぶ
	for (UINT i = 0
		; dxgiFactory->EnumAdapterByGpuPreference(
			i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)
		) != DXGI_ERROR_NOT_FOUND
		; ++i)
	{
		// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		// 取得できないのは一大事
		assert(SUCCEEDED(hr));
		// ソフトウェアアダプタ出なければ採用!!
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			// 採用したアダプタの情報をログに出力。 wstring	の方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		// ソフトウェアアダプタの場合は見なかったことにする
		useAdapter = nullptr;
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

#pragma endregion

#pragma region D3D12Deviceの生成

	ID3D12Device* device = nullptr;
	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelsStrings[] = { "12.2","12.1","12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		// 採用したアダプタでデバイス生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel  : {}\n", featureLevelsStrings[i]));
			break;
		}
	}
	// デバイスの生成が上手くいかなかったので生成できない
	assert(device != nullptr);
	// 初期化完了のログを出す
	Log("Complete create D3D12Device!!!\n");

#pragma endregion

#pragma region エラー・警告がある場合に停止させる

#ifdef _DEBUG

	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

#pragma region エラーと警告の抑制

		/// 抑制するメッセージの ID
		D3D12_MESSAGE_ID denyIDs[] = {
			// Windows11 での DXGI デバッグレイヤート DX12 デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIDs);
		filter.DenyList.pIDList = denyIDs;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

#pragma endregion


		// 解放
		infoQueue->Release();
	}

#endif // _DEBUG


#pragma endregion



	// DirectX12 の初期化の終わり
#pragma endregion

#pragma region DescriptorSize の取得

	const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	descriptorSizeDSV;

#pragma endregion


	// 一次初期化の終わり
#pragma endregion

#pragma region 二次初期化


#pragma region CommandQueue を生成する

	// コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	// コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region CommandList を生成する

	// コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	// コマンドアロケータの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	// コマンドリストの生成がうまいので起動できない
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region SwapChain を生成する

	// スワップチェインを生成する
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;								// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = kClientHeight;							// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc.SampleDesc.Count = 1;								// マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;									// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタに移したら、中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain)
	);
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region DescriptorHeap を生成する

	// ディスクリプタヒープの生成
	// RTV 用のヒープでディスクリプタの数は 2 。RTV は Shader 内で触るものではないので、ShaderVisible は false
	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	// SRV 用のヒープでディスクリプタの数は 128	。SRV は Shader 内で触るものなので、ShaderVisible は true
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

#pragma endregion

#pragma region SwapChain から Resource を引っ張ってくる

	// SwapChain から Resource を引っ張ってくる
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	// 上手く取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region RTV(Render_Target_View) を作る

	// RTV の設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;		// 出力結果を SRGB に変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// 2D テクスチャとして書き込む
	// ディスクリプタの先頭を取得する
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 0);
	// RTV を 2 つ作るのでディスクリプタを 2 つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// まず 1 つ目を作る。 1 つ目は最初の所に作る。作る場所をこちらで用意してあげる必要がある
	rtvHandles[0] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 0);
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	// 2 つ目のディスクリプタハンドルを作る(自力で)
	rtvHandles[1] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 1);
	// 2 つ目を作る
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

#pragma endregion


	// 二次初期化の終わり
#pragma endregion

#pragma region Fence と Event を生成する

	// 初期値 0 で Fence を作る
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	// Fence の Signal を待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

#pragma endregion

#pragma region DXC の初期化

	// dxcompiler を初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	// 現時点で include はしないが、include するための設定を行う
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

#pragma endregion


#pragma region PSO の設定 : (PipelineStateObject)

#pragma region RootSignature を生成する

	// RootSignature 作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter 作成。PixelShader の Material と VertexShader の Tranform
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBV を使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShader で使う
	rootParameters[0].Descriptor.ShaderRegister = 0;					// レジスタ番号 0 とバインド
	// TransformMatrix
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBV を使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShader で使う
	rootParameters[1].Descriptor.ShaderRegister = 0;					// レジスタ番号 0 とバインド

#pragma region DescriptorRange の生成

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;						// 0 から始まる
	descriptorRange[0].NumDescriptors = 1;							// 数は 1 つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRV を使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;						// Offset を自動計算

#pragma endregion

#pragma region DescriptorTable の生成
	// テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTabel を使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShader で使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Table の中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Table で利用する数

#pragma endregion

#pragma region ConstantBuffer 用の情報
	// 平行光源
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBV を使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShader で使う
	rootParameters[3].Descriptor.ShaderRegister = 1;					// レジスタ番号 1 を使う

	// カメラ位置
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBV を使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShader で使う
	rootParameters[4].Descriptor.ShaderRegister = 2;					// レジスタ番号 1 を使う

#pragma endregion

	descriptionRootSignature.pParameters = rootParameters;				// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);	// 配列の長さ

#pragma region Sampler の設定

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;			// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		// 0 ～ 1 の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;		// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;						// ありったけの MitMap を使う
	staticSamplers[0].ShaderRegister = 0;								// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShader で使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

#pragma endregion

	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(
		&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));


	// RootSignature を生成する の終わり
#pragma endregion



#pragma region InputLayout の設定を行う

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion

#pragma region BlendState の設定を行う

	// BlendState の設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

#pragma endregion

#pragma region RasterizerState の設定を行う

	// Rasterizerstate の設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region Shader を Compile する

	// Shader をコンパイルする
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	IDxcBlob* pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

#pragma endregion

#pragma region DepthStencilState の設定

	// DepthStencilState の設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depth の機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みする
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数は LessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion


#pragma region PSO を生成する

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込む RTV の情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくていい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencil の設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

#pragma endregion

	// PSO の終わり
#pragma endregion

#pragma region VertexResource を生成する


#pragma region CreateBufferResource関数内にまとめた処理

	//// 頂点リソース用のヒープの設定
	//D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	//// UploadHeap を使う
	//uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//// 頂点リソースの設定
	//D3D12_RESOURCE_DESC vertexResourceDesc{};
	//// バッファリソース。テクスチャの場合はまた別の設定をする
	//vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//// リソースのサイズ。今回は Vector4 を 3 頂点分
	//vertexResourceDesc.Width = sizeof(VertexData) * 3;
	//// バッファの場合はこれらは 1 にする決まり
	//vertexResourceDesc.Height = 1;
	//vertexResourceDesc.DepthOrArraySize = 1;
	//vertexResourceDesc.MipLevels = 1;
	//vertexResourceDesc.SampleDesc.Count = 1;
	//// バッファの場合はこれにする決まり
	//vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//// 実際に頂点リソースを作る
	//ID3D12Resource* vertexResource = nullptr;
	//hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
	//	&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	//assert(SUCCEEDED(hr));

#pragma endregion

	// DepthStencilTexture をウィンドウサイズで作成
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	//depthStencilResource;


	const uint32_t kSubdivision = 16;

	const uint32_t kMaxVertexData = kSubdivision * kSubdivision * 6;

	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * kMaxVertexData);

	// Sprite 用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

#pragma endregion

#pragma region VertexBufferView を作成する

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点 3 つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * kMaxVertexData;
	// 1 頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Sprite 用の頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点 6 つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	// 1 頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);


#pragma endregion

#pragma region Resource にデータを書き込む

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// ここで処理ができる
	CreateSphere(vertexData, kSubdivision);


	// Sprite 用の頂点データを設定する
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	// 1 枚目の三角形
	// 左下
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };
	// 左下
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };
	// 左下
	vertexDataSprite[2].position = { 360.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };
	// 2 枚目の三角形
	// 左下
	vertexDataSprite[3].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { 0.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };
	// 左下
	vertexDataSprite[4].position = { 360.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[4].texcoord = { 1.0f,0.0f };
	vertexDataSprite[4].normal = { 0.0f,0.0f,-1.0f };
	// 左下
	vertexDataSprite[5].position = { 360.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[5].texcoord = { 1.0f,1.0f };
	vertexDataSprite[5].normal = { 0.0f,0.0f,-1.0f };


#pragma region Material 用の Resource を作る

	// マテリアルリソースを作る。
	ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Material));
	// マテリアルにデータを書き込む
	//Vector4* materialData = nullptr;
	Material* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は白を書き込んでみる
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->shininess = 40.0f;

	// Sprite 用のマテリアルリソースを作る。
	ID3D12Resource* materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	// マテリアルにデータを書き込む
	//Vector4* materialData = nullptr;
	Material* materialDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	// 今回は白を書き込んでみる
	materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataSprite->enableLighting = false;
	materialDataSprite->shininess = 0.0f;

	//// 三角形用のマテリアルリソース
	//ID3D12Resource* materialResourceTriangle = CreateBufferResource(device, sizeof(Material));
	//// データを書き込む
	//Material* materialDataTriangle = nullptr;
	//materialResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&materialDataTriangle));
	//materialDataTriangle->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialDataTriangle->enableLighting = false;

#pragma endregion

#pragma region DirectionalLight 用の Resource を作成

	ID3D12Resource* directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	// デフォルト値はとりあえずで設定
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;

#pragma endregion

#pragma region TransformationMatrix 用の Resource を作る

	// WVP 用のリソースを作る。Matrix4x4 1 つ分のサイズを用意する
	ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込んでおく
	wvpData->WVP = Mymath::MakeIdentity4x4();
	wvpData->World = Mymath::MakeIdentity4x4();

	// Spirte 用の Transform 周りを作る
	ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	// 単位行列を書き込んでおく
	transformationMatrixDataSprite->WVP = Mymath::MakeIdentity4x4();
	transformationMatrixDataSprite->World = Mymath::MakeIdentity4x4();

	// 三角形用の WVP
	ID3D12Resource* transformResourceTriangle = CreateBufferResource(device, sizeof(TransformationMatrix));
	TransformationMatrix* transformDataTriangle = nullptr;
	transformResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&transformDataTriangle));
	// 単位行列を書き込む
	transformDataTriangle->WVP = Mymath::MakeIdentity4x4();
	transformDataTriangle->World = Mymath::MakeIdentity4x4();


#pragma endregion


	// カメラ用のリソースを作成
	ID3D12Resource* cameraResource = CreateBufferResource(device, sizeof(CameraForGPU));


	CameraForGPU* camera = nullptr;
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&camera));

	camera->worldPosition = { 0.0f,0.0f,-10.0f };

	// Resource にデータを書き込む の終わり
#pragma endregion

#pragma region Viewport と Scissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;

#pragma endregion

#pragma region ImGui の初期化

	// ImGui の初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

#pragma endregion

#pragma region Texture を読んで転送する

	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
	//UploadTextureData(textureResource, mipImages);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, device, commandList);

	// モンスターボールを読み込む
	DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
	ID3D12Resource* intermediateResource2 = UploadTextureData(textureResource2, mipImages2, device, commandList);


	// commandList を Close する
	hr = commandList->Close();
	assert(SUCCEEDED(hr));

#pragma region コマンドをキックする

	// GPU にコマンドリストの実行を行わせる
	ID3D12CommandList* intermediateCommandList[] = { commandList };
	commandQueue->ExecuteCommandLists(1, intermediateCommandList);

#pragma region GPU に Signal(シグナル)を送る

	// Fence の値を更新
	fenceValue++;
	// GPU がここまで辿り着いたときに、Fence の値を指定した値に代入するように Signal を送る
	commandQueue->Signal(fence, fenceValue);

#pragma endregion

#pragma region Fence の値を確認して GPU を待つ

	// Fence の値が指定した Signal 値に辿り着いているか確認する
	// GetCompletedValue の初期値は Fence 作成時に渡した初期値
	if (fence->GetCompletedValue() < fenceValue)
	{
		// 指定した Signal に辿り着いていないので、辿り着くまで待つようにイベントを設定する
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		// イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

#pragma endregion

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));

#pragma endregion

	intermediateResource->Release();
	intermediateResource2->Release();

	// Texture を読み込んで転送する の終わり
#pragma endregion

#pragma region View 作成

#pragma region ShaderResourceView を作る

	// metadata	を基に SRV の設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D テクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRV を作成する DescriptorHeap の場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	// 先頭は ImGui が使っているのでその次を使う
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRV の生成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

	// 2 枚目の SRV を作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D テクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	// SRV の生成
	device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);

#pragma endregion

#pragma region DepthStencilView を作る

	// DSV 用のヒープでディスクリプタの数は 1 。DSV は Shader 内で触るものではないので、ShaderVisible は flase。
	ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	// Heap 上に DSV を構築する
	// DSV の設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// Format。基本的には Resource に合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// 2dTexture
	// DSVHeap の先頭に DSV を作る
	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

#pragma endregion


	// View 作成 の終わり
#pragma endregion


	// 初期化の終わり
#pragma endregion

#pragma region メインループ

	// Transform 変数を作る
	Transform transformSphere{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform transformSprite{ {1.0f,1.0f,1.0f} ,{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform transformTriangle{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-3.0f} };

	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},camera->worldPosition };

	bool useMonsterBall = true;

	MSG msg{};
	// ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		// Window にメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			// ImGui にフレームが始まることを伝える
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// 開発用 UI の処理。実際の開発用の UI を出す場合はここをゲーム固有の処理に置き換える
#pragma region ImGui

			//ImGui::ShowDemoWindow();
			ImGui::Begin("Settings");

			if (ImGui::TreeNode("Camera"))
			{
				ImGui::DragFloat3("CameraTranslate", &camera->worldPosition.x, 0.01f);
				ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 0.01f);

				ImGui::TreePop();
			}
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);

			if (ImGui::TreeNode("Triangle"))
			{
				ImGui::DragFloat3("translate", &transformTriangle.translate.x, 0.01f);
				ImGui::DragFloat3("rotate", &transformTriangle.rotate.x, 0.01f);
				//ImGui::ColorEdit4("Color", &materialDataTriangle->color.x);

				//ImGui::SliderInt("enableLighitng", &materialDataTriangle->enableLighting, 0, 1);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Sphere"))
			{
				ImGui::DragFloat3("translate", &transformSphere.translate.x, 0.01f);
				ImGui::DragFloat3("rotate", &transformSphere.rotate.x, 0.01f);
				ImGui::ColorEdit4("Color", &materialData->color.x);
				ImGui::DragFloat("Shininess", &materialData->shininess, 0.1f);

				ImGui::SliderInt("enableLighitng", &materialData->enableLighting, 0, 1);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Sprite"))
			{

				ImGui::DragFloat3("translate", &transformSprite.translate.x, 0.01f);
				ImGui::DragFloat3("rotate", &transformSprite.rotate.x, 0.01f);
				ImGui::ColorEdit4("Color", &materialDataSprite->color.x);

				ImGui::SliderInt("enableLighitng", &materialDataSprite->enableLighting, 0, 1);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("DirectionalLight"))
			{
				ImGui::DragFloat3("Direction", &directionalLightData->direction.x, 0.01f);
				directionalLightData->direction = Mymath::Normalize(directionalLightData->direction);
				ImGui::ColorEdit4("Color", &directionalLightData->color.x);

				ImGui::SliderFloat("Intensity", &directionalLightData->intensity, 0, 10.0f);
		
				ImGui::TreePop();
			}


			ImGui::End();




#pragma endregion

			// ゲームの処理
			transformSphere.rotate.y += 0.01f;
			Matrix4x4 worldMatrix = Mymath::MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
			Matrix4x4 cameraMatrix = Mymath::MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, camera->worldPosition);
			Matrix4x4 viewMatrix = Mymath::Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = Mymath::MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Mymath::Multiply(Mymath::Multiply(worldMatrix, viewMatrix), projectionMatrix);
			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldMatrix;

			// Sprite 用の WorldViewProjectionMatrix を作る
			Matrix4x4 worldMatrixSprite = Mymath::MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = Mymath::MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = Mymath::MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Mymath::Multiply(Mymath::Multiply(worldMatrixSprite, viewMatrixSprite), projectionMatrixSprite);
			transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
			transformationMatrixDataSprite->World = worldMatrixSprite;

			//// 三角形用の WVPを作る
			//Matrix4x4 matWorldTriangle = Mymath::MakeAffineMatrix(transformTriangle.scale, transformTriangle.rotate, transformTriangle.translate);
			//Matrix4x4 matWVPTriangle = Mymath::Multiply(Mymath::Multiply(matWorldTriangle, viewMatrix), projectionMatrix);
			//transformDataTriangle->WVP = matWVPTriangle;
			//transformDataTriangle->World = matWorldTriangle;


			// ImGui の内部コマンドを生成する
			ImGui::Render();


#pragma region 画面の色を変える

#pragma region コマンドを積み込んで確定させる

			// これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

#pragma region TrandsitionBarrier を張る

			// TransitionBarrier の設定
			D3D12_RESOURCE_BARRIER barrier{};
			// 今回のバリアは Transition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			// None にしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// バリアを張る対象のリソース。現在のバックバッファに対して行う
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			// 遷移前（現在）の ResourcesState
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			// 遷移後の ResourceState
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			// TransitionBarrier を張る
			commandList->ResourceBarrier(1, &barrier);

#pragma endregion

#pragma region DSV を設定する

			// 描画先の RTV を設定する
			//commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);

			// 描画先の RTV と DSV を設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

#pragma endregion

			// 指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	// 青っぽい色。RGBA の順
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

			// フレームの最初に DSV をクリアする
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

#pragma region ImGui でも利用する DescriptorHeap の設定

			// 描画用の DescriptorHeap の設定
			ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
			commandList->SetDescriptorHeaps(1, descriptorHeaps);

#pragma endregion


#pragma region 描画用のコマンドを積む

			// Viewport を設定
			commandList->RSSetViewports(1, &viewport);
			// Scisoor を設定
			commandList->RSSetScissorRects(1, &scissorRect);
			// RootSignature を設定。PSO に設定しているけど別途設定が必要
			commandList->SetGraphicsRootSignature(rootSignature);
			// PSO を設定
			commandList->SetPipelineState(graphicsPipelineState);

			// VBV を設定
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			// 形状を設定。PSO に設定しているものとはまた別。同じものを設定すると考える
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			// マテリアル CBuffer の場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			// wvp 用の CBuffer の場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

			// DirectinalLight の場所を設定
			commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

			// カメラのリソースを引き渡し
			commandList->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());

			// SRV の DescriptorTable の先頭を設定。2 は rootParameter[2] である。
			commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);

			// 描画！(DrawCall / ドローコール)。3 頂点で 1 つのインスタンス。
			commandList->DrawInstanced(kMaxVertexData, 1, 0, 0);

			//// 三角形を描画
			//// VBV
			//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewTriangle);
			//commandList->SetGraphicsRootConstantBufferView(0, materialResourceTriangle->GetGPUVirtualAddress());
			//commandList->SetGraphicsRootConstantBufferView(1, transformResourceTriangle->GetGPUVirtualAddress());

			//// SRV
			//commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);

			//// 描画
			//commandList->DrawInstanced(3, 1, 0, 0);


			// Sprite 用の描画コマンド
			// 変更が必要なものだけ変更する
			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);	// VBV を設定
			// マテリアル CBuffer の場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			// TransformationMatrixCBuffer の場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			// 描画！(DrawCall / ドローコール)
			commandList->DrawInstanced(6, 1, 0, 0);

#pragma endregion

#pragma region ImGui の描画コマンドを積む

			// 実際の commandList の ImGui の描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#pragma endregion


#pragma region Resourceの状態を RENDER_TARGET から PRESENT にする

			// 画面に描く処理はすべて終わり、画面に移すので状態を遷移
			// 今回は RenderTarget から Present にする
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			// TransitionBarrier を張る
			commandList->ResourceBarrier(1, &barrier);

#pragma endregion

			// コマンドリストの内容を確定させる。すべてのコマンドを詰んでから Close すること
			hr = commandList->Close();
			assert(SUCCEEDED(hr));

			// コマンドを積み込んで確定させるの終わり
#pragma endregion

#pragma region コマンドをキックする

			// GPU にコマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, commandLists);
			// GPU と OS に画面の交換を行うように通知する
			swapChain->Present(1, 0);

#pragma region GPU に Signal(シグナル)を送る

			// Fence の値を更新
			fenceValue++;
			// GPU がここまで辿り着いたときに、Fence の値を指定した値に代入するように Signal を送る
			commandQueue->Signal(fence, fenceValue);

#pragma endregion

#pragma region Fence の値を確認して GPU を待つ

			// Fence の値が指定した Signal 値に辿り着いているか確認する
			// GetCompletedValue の初期値は Fence 作成時に渡した初期値
			if (fence->GetCompletedValue() < fenceValue)
			{
				// 指定した Signal に辿り着いていないので、辿り着くまで待つようにイベントを設定する
				fence->SetEventOnCompletion(fenceValue, fenceEvent);
				// イベントを待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}

#pragma endregion

			// 次のフレーム用のコマンドリストを準備
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator, nullptr);
			assert(SUCCEEDED(hr));

#pragma endregion

			// 画面の色を変えるの終わり
#pragma endregion


		}
	}

#pragma endregion

#pragma region 解放処理

	dsvDescriptorHeap->Release();
	//intermediateResource->Release();
	mipImages2.Release();
	textureResource2->Release();
	mipImages.Release();
	textureResource->Release();
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	transformResourceTriangle->Release();
	transformationMatrixResourceSprite->Release();
	wvpResource->Release();
	cameraResource->Release();
	directionalLightResource->Release();
	//materialResourceTriangle->Release();
	materialResourceSprite->Release();
	materialResource->Release();
	//vertexResourceTriangle->Release();
	vertexResourceSprite->Release();
	vertexResource->Release();
	depthStencilResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

	CloseHandle(fenceEvent);
	fence->Release();
	srvDescriptorHeap->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(hwnd);
	CoUninitialize();


#pragma endregion

#pragma region ReportLiveObjects オブジェクトの解放チェック

	// リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

#pragma endregion


	return 0;
}


