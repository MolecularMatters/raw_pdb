// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

typedef void* HANDLE;


namespace MemoryMappedFile
{
	struct Handle
	{
		HANDLE file;
		HANDLE fileMapping;
		void* baseAddress;
	};

	PDB_NO_DISCARD Handle Open(const wchar_t* path) PDB_NO_EXCEPT;
	void Close(Handle& handle) PDB_NO_EXCEPT;
}
