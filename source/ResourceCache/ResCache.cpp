#include "ResCache.h"

#include <algorithm>
#include <cctype>

#include "../Utilities/String.h"

/***Resource***/
Resource::Resource(const std::string& name)
	:m_name(name)
{
	std::transform(m_name.begin(), m_name.end(), m_name.begin(), (int(*)(int)) std::tolower);
}

std::string Resource::GetName() const {
	return m_name;
}


/***ResourceZipFile***/
ResourceZipFile::~ResourceZipFile()
{
	if (m_pZipFile) {
		delete m_pZipFile;
		m_pZipFile = nullptr;
	}
}

bool ResourceZipFile::VOpen()
{
	m_pZipFile = new ZipFile;
	if (m_pZipFile)
		return m_pZipFile->Init(m_resFileName.c_str());

	return false;
}

int ResourceZipFile::VGetRawResourceSize(const Resource& r)
{
	int resourceNum = m_pZipFile->Find(r.GetName().c_str());
	if (resourceNum == -1)
		return -1;

	return m_pZipFile->GetFileLen(resourceNum);
}

int ResourceZipFile::VGetRawResource(const Resource& r, char* buffer)
{
	int size = 0;
	int resourceNum = m_pZipFile->Find(r.GetName().c_str());
	if (resourceNum != -1)
	{
		size = m_pZipFile->GetFileLen(resourceNum);
		m_pZipFile->ReadFile(resourceNum, buffer);
	}
	return size;
}

int ResourceZipFile::VGetNumResources() const
{
	return (m_pZipFile == NULL) ? 0 : m_pZipFile->GetNumFiles();
}

std::string ResourceZipFile::VGetResourceName(int num) const
{
	std::string resName = "";
	if (m_pZipFile != NULL && num >= 0 && num < m_pZipFile->GetNumFiles())
	{
		resName = m_pZipFile->GetFilename(num);
	}
	return resName;
}


/***ResHandle***/
ResHandle::ResHandle(Resource& resource, char* buffer, unsigned int size, ResCache* pResCache)
	: m_resource(resource)
	, m_buffer(buffer)
	, m_size(size)
	, m_extra(nullptr)
	, m_pResCache(pResCache)
{
}

ResHandle::~ResHandle()
{
	if (m_buffer) {
		delete[] m_buffer;
		m_buffer = nullptr;
	}

	m_pResCache->MemoryHasBeenFreed(m_size);
}

ResCache::ResCache(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> resFile)
	: m_cacheSize(sizeInMb * 1024 * 1024)				// total memory size
	, m_allocated(0)									// total memory allocated
	, m_file(resFile)
{
}

ResCache::~ResCache()
{
	while (!m_lru.empty())
	{
		FreeOneResource();
	}
}

bool ResCache::Init()
{
	bool retValue = false;
	if (m_file->VOpen())
	{
		RegisterLoader(std::shared_ptr<IResourceLoader>(new DefaultResourceLoader()));
		retValue = true;
	}
	return retValue;
}

void ResCache::RegisterLoader(std::shared_ptr<IResourceLoader> loader)
{
	m_resourceLoaders.push_front(loader);
}

std::shared_ptr<ResHandle> ResCache::GetHandle(Resource* r)
{
	std::shared_ptr<ResHandle> handle(Find(r));
	if (handle == nullptr)
	{
		handle = Load(r);
		assert(handle);
	}
	else
	{
		Update(handle);
	}
	return handle;
}

std::shared_ptr<ResHandle> ResCache::Load(Resource* r)
{
	// Create a new resource and add it to the lru list and map
	std::shared_ptr<IResourceLoader> loader;
	std::shared_ptr<ResHandle> handle;

	for (ResourceLoaders::iterator it = m_resourceLoaders.begin(); it != m_resourceLoaders.end(); ++it)
	{
		std::shared_ptr<IResourceLoader> testLoader = *it;

		if (WildcardMatch(testLoader->VGetPattern().c_str(), r->GetName().c_str()))
		{
			loader = testLoader;
			break;
		}
	}

	if (!loader)
	{
		assert(loader && "Default resource loader not found!");
		return handle;		// Resource not loaded!
	}

	int rawSize = m_file->VGetRawResourceSize(*r);
	if (rawSize < 0)
	{
		assert(rawSize > 0 && "Resource size returned -1 - Resource not found");
		return std::shared_ptr<ResHandle>();
	}

	int allocSize = rawSize + ((loader->VAddNullZero()) ? (1) : (0));
	char* rawBuffer = loader->VUseRawFile() ? Allocate(allocSize) : new char[allocSize]();

	if (rawBuffer == nullptr || m_file->VGetRawResource(*r, rawBuffer) == 0)
	{
		// resource cache out of memory
		return std::shared_ptr<ResHandle>();
	}

	char* buffer = nullptr;
	unsigned int size = 0;

	if (loader->VUseRawFile())
	{
		buffer = rawBuffer;
		handle = std::shared_ptr<ResHandle>(new ResHandle(*r, buffer, rawSize, this));
	}
	else
	{
		size = loader->VGetLoadedResourceSize(rawBuffer, rawSize);
		buffer = Allocate(size);
		if (rawBuffer == nullptr || buffer == nullptr)
		{
			// resource cache out of memory
			return std::shared_ptr<ResHandle>();
		}
		handle = std::shared_ptr<ResHandle>(new ResHandle(*r, buffer, size, this));
		bool success = loader->VLoadResource(rawBuffer, rawSize, handle);

		if (loader->VDiscardRawBufferAfterLoad())
		{
			if (rawBuffer) {
				delete[] rawBuffer;
				rawBuffer = nullptr;
			}
		}

		if (!success)
		{
			// resource cache out of memory
			return std::shared_ptr<ResHandle>();
		}
	}

	if (handle)
	{
		m_lru.push_front(handle);
		m_resources[r->GetName()] = handle;
	}

	assert(loader && "Default resource loader not found!");
	return handle;		// ResCache is out of memory!
}

std::shared_ptr<ResHandle> ResCache::Find(Resource* r)
{
	auto itr = m_resources.find(r->GetName());
	if (itr == m_resources.end())
		return std::shared_ptr<ResHandle>();

	return itr->second;
}

void ResCache::Update(std::shared_ptr<ResHandle> handle)
{
	m_lru.remove(handle);
	m_lru.push_front(handle);
}

char* ResCache::Allocate(unsigned int size)
{
	if (!MakeRoom(size))
		return nullptr;

	char* mem = new char[size];
	if (mem)
		m_allocated += size;

	return mem;
}

void ResCache::FreeOneResource()
{
	m_resources.erase(m_lru.back()->m_resource.GetName());
	m_lru.pop_back();
	// Note - you can't change the resource cache size yet - the resource bits could still actually be
	// used by some sybsystem holding onto the ResHandle. Only when it goes out of scope can the memory
	// be actually free again.
}

bool ResCache::MakeRoom(unsigned int size)
{
	if (size > m_cacheSize)
		return false;

	// return null if there's no possible way to allocate the memory
	while (size > (m_cacheSize - m_allocated))
	{
		// The cache is empty, and there's still not enough room.
		if (m_lru.empty())
			return false;

		FreeOneResource();
	}

	return true;
}

void ResCache::Free(std::shared_ptr<ResHandle> gonner)
{
	m_lru.remove(gonner);
	m_resources.erase(gonner->m_resource.GetName());
	// Note - the resource might still be in use by something,
	// so the cache can't actually count the memory freed until the
	// ResHandle pointing to it is destroyed.
}

//This is called whenever the memory associated with a resource is actually freed
void ResCache::MemoryHasBeenFreed(unsigned int size)
{
	m_allocated -= size;
}