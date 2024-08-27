// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleMemoryMappedFile.h"


MemoryMappedFile::Handle MemoryMappedFile::Open(const char* path)
{
#ifdef _WIN32
	void* file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);

	if (file == INVALID_HANDLE_VALUE)
	{
		return Handle { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, nullptr, 0 };
	}

	void* fileMapping = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

	if (fileMapping == nullptr)
	{
		CloseHandle(file);

		return Handle { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, nullptr, 0 };
	}

	void* baseAddress = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);

	if (baseAddress == nullptr)
	{
		CloseHandle(fileMapping);
		CloseHandle(file);

		return Handle { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, nullptr, 0 };
	}

	BY_HANDLE_FILE_INFORMATION fileInformation;
	const bool getInformationResult = GetFileInformationByHandle(file, &fileInformation);
	if (!getInformationResult)
	{
		UnmapViewOfFile(baseAddress);
		CloseHandle(fileMapping);
		CloseHandle(file);

		return Handle { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, nullptr, 0 };
	}

	const size_t fileSizeHighBytes = static_cast<size_t>(fileInformation.nFileSizeHigh) << 32;
	const size_t fileSizeLowBytes = fileInformation.nFileSizeLow;
	const size_t fileSize = fileSizeHighBytes | fileSizeLowBytes;
	return Handle { file, fileMapping, baseAddress, fileSize };
#else
	struct stat fileSb;

	int file = open(path, O_RDONLY);

	if (file == INVALID_HANDLE_VALUE)
	{
		return Handle { INVALID_HANDLE_VALUE, nullptr, 0 };
	}

	if (fstat(file, &fileSb) == -1)
	{
		close(file);

		return Handle { INVALID_HANDLE_VALUE, nullptr, 0 };
	}

	void* baseAddress = mmap(nullptr, fileSb.st_size, PROT_READ, MAP_PRIVATE, file, 0);

	if (baseAddress == MAP_FAILED)
	{
		close(file);

		return Handle { INVALID_HANDLE_VALUE, nullptr, 0 };
	}

	return Handle { file, baseAddress, static_cast<size_t>(fileSb.st_size) };
#endif
}


void MemoryMappedFile::Close(Handle& handle)
{
#ifdef _WIN32
	UnmapViewOfFile(handle.baseAddress);
	CloseHandle(handle.fileMapping);
	CloseHandle(handle.file);

	handle.file = nullptr;
	handle.fileMapping = nullptr;
#else
	munmap(handle.baseAddress, handle.len);
	close(handle.file);

	handle.file = 0;
#endif

	handle.baseAddress = nullptr;
}
