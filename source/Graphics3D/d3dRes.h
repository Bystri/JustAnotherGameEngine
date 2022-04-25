#pragma once

#include "../ResourceCache/ResCache.h"

#include <d3d12.h>
#include <wrl.h>

class TextureResourceLoader : public IResourceLoader
{
public:
	TextureResourceLoader(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList)
		: m_d3dDevice(d3dDevice), m_CommandList(CommandList)
	{}

	virtual std::string VGetPattern() { return "*.dds"; };
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> handle);

private:
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
};

class d3dTextureResourceExtraData : public IResourceExtraData
{
	friend class TextureResourceLoader;
public:
	virtual std::string VToString() { return "d3dTextureResourceExtraData"; }

	Microsoft::WRL::ComPtr<ID3D12Resource> GetD3DResource() { return Resource; }
	Microsoft::WRL::ComPtr<ID3D12Resource> GetD3DUploadHeap() { return UploadHeap; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};