// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleMemoryMappedFile.h"

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#ifdef _WIN32
MemoryMappedFile::Handle MemoryMappedFile::Open(const wchar_t* path)
{
	HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
	if (file == INVALID_HANDLE_VALUE)
	{
		return Handle { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, nullptr };
	}

	HANDLE fileMapping = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (fileMapping == nullptr)
	{
		CloseHandle(file);

		return Handle { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, nullptr };
	}

	void* baseAddress = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
	if (baseAddress == nullptr)
	{
		CloseHandle(fileMapping);
		CloseHandle(file);

		return Handle { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, nullptr };
	}

	return Handle { file, fileMapping, baseAddress };
}


void MemoryMappedFile::Close(Handle& handle)
{
	UnmapViewOfFile(handle.baseAddress);
	CloseHandle(handle.fileMapping);
	CloseHandle(handle.file);

	handle.file = nullptr;
	handle.fileMapping = nullptr;
	handle.baseAddress = nullptr;
}
#else
MemoryMappedFile::Handle MemoryMappedFile::Open(const wchar_t* path)
{
	return {};
}

void MemoryMappedFile::Close(Handle& handle)
{
}
#endif