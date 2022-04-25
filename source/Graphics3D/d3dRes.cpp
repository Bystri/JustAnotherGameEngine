#include "d3dRes.h"

#include "DDSTextureLoader.h"
#include "d3dUtil.h"

unsigned int TextureResourceLoader::VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) {
	//We keep away our ResCache from allocating the room for texture. DirectX has to do it on his own.
	return 0;
}

bool TextureResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> handle)
{
	std::shared_ptr<d3dTextureResourceExtraData> extra = std::make_shared<d3dTextureResourceExtraData>();

	ThrowIfFailed(DirectX::CreateDDSTextureFromMemory12(m_d3dDevice.Get(),
		m_CommandList.Get(), (uint8_t*)rawBuffer, rawSize,
		extra->Resource, extra->UploadHeap));


	handle->SetExtra(extra);
	return true;
}