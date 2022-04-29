// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)


namespace MemoryMappedFile
{
	struct Handle
	{
		HANDLE file;
		HANDLE fileMapping;
		void* baseAddress;
	};

	Handle Open(const wchar_t* path);
	void Close(Handle& handle);
}
