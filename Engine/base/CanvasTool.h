#pragma once
#include "./DirectXCommon.h"
#include "./MyConst.h"

#include <memory>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <vector>


class CanvasTool {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static CanvasTool* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* DirectXCommon);

	/// <summary>
	/// 描画数のリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// VertexResourceを作成
	/// </summary>
	ID3D12Resource* CreateBufferResource(size_t);

#pragma region 描画関数

	/// <summary>
	/// 三角形を描画する
	/// </summary>
	void DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color);
	void DrawTriangle(Triangle& triangle);


#pragma endregion


private:

	// DirectXCommon管理
	DirectXCommon* dxCommon_ = nullptr;

#pragma region 構造体宣言

	//*** DirectXシェーダコンパイラ ***//
	// HLSLコードをバイナリ形式のGPUシェーダーに変換する
	struct DXC {
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;				// dxcの汎用オブジェクト
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;			// dxcのコンパイラオブジェクト
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;	// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
	};
	struct PipelineSet {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;			// リソースとシェーダーのバインディングを定義
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;	// グラフィックパイプラインの状態を定義
	};

	struct CBufferData
	{
		Matrix4x4* wvpData_;
		Vector4* colorData_;
	};

	struct CBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;	// 定数バッファ
		Matrix4x4* wvpData_;									// 定数リソース
		Vector4* colorData_;
	};

	struct VertexTriangle {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;	// GPU上の頂点データの格納場所
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};			// BufferLocationは頂点データ格納場所のアドレス
		Triangle::VectorPosColor* triangleData_ = nullptr;		// 三角形の頂点と色リソース
		uint32_t triangleCount_ = 0;							// 三角形の描画数
	};

	// 構造体宣言
#pragma endregion

#pragma region 定数宣言

	// 三角形の最大数
	static const int32_t kMaxTriangleCount_ = 256;
	// 三角形の頂点数
	static const UINT kVertexCountTriangle_ = 3;

	// 定数宣言
#pragma endregion

#pragma region 変数宣言

	// 描画関連
	std::unique_ptr<DXC> dxc_;
	std::unique_ptr<PipelineSet> pipelineSet_;
	std::unique_ptr<CBuffer> cBuffer_;
	// 三角形
	std::unique_ptr<VertexTriangle> vertexTriangle_;

	// 変数宣言
#pragma endregion


private: // 非公開のメンバ関数
	CanvasTool() = default;
	~CanvasTool() = default;
	CanvasTool(const CanvasTool&) = delete;
	const CanvasTool& operator=(const CanvasTool&) = delete;

	/// <summary>
	/// DXC初期化
	/// </summary>
	void InitializeDXC();

#pragma region 生成関数

#pragma region PipelineSet

#pragma region PSO 生成関連

	void CreateRootSignature();
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	D3D12_BLEND_DESC CreateBlendState();
	D3D12_RASTERIZER_DESC CreateRasterizerState();
	IDxcBlob* CreateVertexShader();
	IDxcBlob* CreatePixelShader();

	// PSO 生成関連
#pragma endregion

	/// <summary>
	/// グラフィックスパイプラインを作成
	/// </summary>
	void CreateGraphicsPipeLineState();

	// PipelineSet
#pragma endregion

	/// <summary>
	/// 定数バッファを作成
	/// </summary>
	void CreateConstantBuffer();

	/// <summary>
	/// 頂点バッファビューを作成
	/// </summary>
	void CreateVertexTriangleBufferView();


	/// <summary>
	/// 三角形の頂点バッファを作成
	/// </summary>
	void CreateVertexTriangle();


	// 生成関数
#pragma endregion


	/// <summary>
	/// シェーダーのコンパイル関数
	/// </summary>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	/// <summary>
	/// unsinged int型の色情報をVector4に変換する
	/// </summary>
	/// <returns>Vector4型の色情報</returns>
	Vector4 HexColorToVector4(unsigned int);

};
