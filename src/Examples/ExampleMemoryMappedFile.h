// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define INVALID_HANDLE_VALUE ((long)-1)
#endif

namespace MemoryMappedFile
{
	struct Handle
	{
#ifdef _WIN32
		void* file;
		void* fileMapping;
#else
		int   file;
#endif
		void* baseAddress;
#ifndef _WIN32
		long len;
#endif
	};

	Handle Open(const char* path);
	void Close(Handle& handle);
}
