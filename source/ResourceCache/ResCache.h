#pragma once

#include <memory>
#include <list>
#include <vector>
#include <cassert>

#include "ZipFile.h"

class ResHandle;
class ResCache;

class Resource
{
public:
	Resource(const std::string& name);

	std::string GetName() const;

private:
	std::string m_name;
};

class IResourceLoader
{
public:
	virtual std::string VGetPattern() = 0;
	virtual bool VUseRawFile() = 0;
	virtual bool VDiscardRawBufferAfterLoad() = 0;
	virtual bool VAddNullZero() { return false; }
	virtual unsigned int VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) = 0;
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> handle) = 0;
};


class IResourceFile
{
public:
	virtual bool VOpen() = 0;
	virtual int VGetRawResourceSize(const Resource& r) = 0;
	virtual int VGetRawResource(const Resource& r, char* buffer) = 0;
	virtual int VGetNumResources() const = 0;
	virtual std::string VGetResourceName(int num) const = 0;
	virtual ~IResourceFile() { }
};


class IResourceExtraData
{
public:
	virtual std::string VToString() = 0;
};

class ResourceZipFile : public IResourceFile
{
public:
	ResourceZipFile(const std::wstring resFileName) :m_pZipFile(nullptr), m_resFileName(resFileName) { }
	virtual ~ResourceZipFile();

	virtual bool VOpen();
	virtual int VGetRawResourceSize(const Resource& r);
	virtual int VGetRawResource(const Resource& r, char* buffer);
	virtual int VGetNumResources() const;
	virtual std::string VGetResourceName(int num) const;

private:
	ZipFile* m_pZipFile;
	std::wstring m_resFileName;
};


class ResHandle
{
	friend class ResCache;

public:
	ResHandle(Resource& resource, char* buffer, unsigned int size, ResCache* pResCache);
	virtual ~ResHandle();

	const std::string GetName() { return m_resource.GetName(); }
	unsigned int Size() const { return m_size; }
	char* Buffer() const { return m_buffer; }
	char* WritableBuffer() { return m_buffer; }

	std::shared_ptr<IResourceExtraData> GetExtra() { return m_extra; }
	void SetExtra(std::shared_ptr<IResourceExtraData> extra) { m_extra = extra; }

protected:
	Resource m_resource;
	char* m_buffer;
	unsigned int m_size;
	std::shared_ptr<IResourceExtraData> m_extra;
	ResCache* m_pResCache;
};


class DefaultResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return true; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return rawSize; }
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> handle) { return true; }
	virtual std::string VGetPattern() { return "*"; }

};


typedef std::list<std::shared_ptr<ResHandle>> ResHandleList;
typedef std::map<std::string, std::shared_ptr<ResHandle>> ResHandleMap;	
typedef std::list<std::shared_ptr<IResourceLoader>> ResourceLoaders;

class ResCache
{
	friend class ResHandle;
public:
	ResCache(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> file);
	virtual ~ResCache();

	bool Init();
	void RegisterLoader(std::shared_ptr<IResourceLoader> loader);

	std::shared_ptr<ResHandle> GetHandle(Resource* r);

protected:
	bool MakeRoom(unsigned int size);
	char* Allocate(unsigned int size);
	void Free(std::shared_ptr<ResHandle> gonner);

	std::shared_ptr<ResHandle> Load(Resource* r);
	std::shared_ptr<ResHandle> Find(Resource* r);
	void Update(std::shared_ptr<ResHandle> handle);

	void FreeOneResource();
	void MemoryHasBeenFreed(unsigned int size);

private:
	ResHandleList m_lru;
	ResHandleMap m_resources;
	ResourceLoaders m_resourceLoaders;

	std::shared_ptr<IResourceFile> m_file;

	unsigned int			m_cacheSize;			// total memory size
	unsigned int			m_allocated;			// total memory allocated
};