#pragma once

#include <map>
#include <string>
#include <fstream>

typedef std::map<std::string, int> ZipContentsMap;

class ZipFile {
public:
	ZipFile() : m_nEntries(0), m_InputFile(), m_pDirData(nullptr), m_papDir(nullptr) {}
	explicit ZipFile(const std::wstring& resFileName) : m_nEntries(0), m_InputFile(), m_pDirData(nullptr), m_papDir(nullptr) {}
	~ZipFile() { End(); m_InputFile.close(); }

	ZipFile(ZipFile&& orig) noexcept;
	ZipFile& operator=(ZipFile&& rhs) noexcept;

	bool Init(const std::wstring& resFileName);
	void End();

	int GetNumFiles() const { return m_nEntries; }
	std::string GetFilename(int i) const;
	int GetFileLen(int i) const;
	bool ReadFile(int i, void* pBuf);

	bool ReadLargeFile(int i, void* pBuf, void (*progressCallback)(int, bool&));

	int Find(const std::string& path) const;

	ZipContentsMap m_ZipContentsMap;

private:
	struct TZipDirHeader;
	struct TZipDirFileHeader;
	struct TZipLocalHeader;

	std::wstring m_ResFileName;
	std::ifstream m_InputFile;
	char *m_pDirData;
	int m_nEntries;

	const TZipDirFileHeader **m_papDir;
};