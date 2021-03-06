#pragma once

#include "../../Graphics3D/d3dApp.h"
#include "../../Graphics3D/MathHelper.h"
#include "../../Graphics3D/UploadBuffer.h"
#include "../../Graphics3D/FrameResource.h"
#include "../../Graphics3D/GeometryGenerator.h"
#include "../../Graphics3D/Camera.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem;

enum class RenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	Count
};


class GameEngine : public D3DApp
{
public:
	GameEngine(HINSTANCE hInstance);
	GameEngine(const GameEngine& rhs) = delete;
	GameEngine& operator=(const GameEngine& rhs) = delete;
	~GameEngine();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMaterialBuffer(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);

	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
private:
	std::vector<std::unique_ptr<FrameResource>> m_FrameResources;
	FrameResource* m_CurrFrameResource = nullptr;
	int m_CurrFrameResourceIndex = 0;

	UINT m_CbvSrvDescriptorSize = 0;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_Geometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> m_Materials;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_Shaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> m_AllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> m_RitemLayer[(int)RenderLayer::Count];

	bool m_FrustumCullingEnabled = true;

	DirectX::BoundingFrustum m_CamFrustum;

	PassConstants m_MainPassCB;

	Camera m_Camera;

	POINT m_LastMousePos;
};