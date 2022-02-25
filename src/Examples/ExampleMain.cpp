// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleMemoryMappedFile.h"
#include "PDB.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"


namespace
{
	PDB_NO_DISCARD static bool IsError(PDB::ErrorCode errorCode)
	{
		switch (errorCode)
		{
			case PDB::ErrorCode::Success:
				return false;

			case PDB::ErrorCode::InvalidSuperBlock:
				PDB_LOG_ERROR("Invalid Superblock");
				return true;

			case PDB::ErrorCode::InvalidFreeBlockMap:
				PDB_LOG_ERROR("Invalid free block map");
				return true;

			case PDB::ErrorCode::UnhandledDirectorySize:
				PDB_LOG_ERROR("Directory is too large");
				return true;

			case PDB::ErrorCode::InvalidSignature:
				PDB_LOG_ERROR("Invalid stream signature");
				return true;

			case PDB::ErrorCode::InvalidStreamIndex:
				PDB_LOG_ERROR("Invalid stream index");
				return true;

			case PDB::ErrorCode::UnknownVersion:
				PDB_LOG_ERROR("Unknown version");
				return true;
		}

		// only ErrorCode::Success means there wasn't an error, so all other paths have to assume there was an error
		return true;
	}

	PDB_NO_DISCARD static bool HasValidDBIStreams(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream)
	{
		// check whether the DBI stream offers all sub-streams we need
		if (IsError(dbiStream.HasValidImageSectionStream(rawPdbFile)))
		{
			return false;
		}
		
		if (IsError(dbiStream.HasValidPublicSymbolStream(rawPdbFile)))
		{
			return false;
		}

		if (IsError(dbiStream.HasValidGlobalSymbolStream(rawPdbFile)))
		{
			return false;
		}

		if (IsError(dbiStream.HasValidSectionContributionStream(rawPdbFile)))
		{
			return false;
		}

		return true;
	}
}


// declare all examples
extern void ExampleSymbols(const PDB::RawFile&, const PDB::DBIStream&);
extern void ExampleContributions(const PDB::RawFile&, const PDB::DBIStream&);
extern void ExampleFunctionSymbols(const PDB::RawFile&, const PDB::DBIStream&);


int main(void)
{
#ifdef _DEBUG
	const wchar_t* const pdbPath = LR"(..\bin\x64\Debug\Examples.pdb)";
#else
	const wchar_t* const pdbPath = LR"(..\bin\x64\Release\Examples.pdb)";
#endif

	printf("Opening PDB file %ls\n", pdbPath);

	// try to open the PDB file and check whether all the data we need is available
	MemoryMappedFile::Handle pdbFile = MemoryMappedFile::Open(pdbPath);
	if (!pdbFile.baseAddress)
	{
		return 1;
	}

	if (IsError(PDB::ValidateFile(pdbFile.baseAddress)))
	{
		MemoryMappedFile::Close(pdbFile);

		return 2;
	}

	const PDB::RawFile rawPdbFile = PDB::CreateRawFile(pdbFile.baseAddress);
	if (IsError(PDB::HasValidDBIStream(rawPdbFile)))
	{
		MemoryMappedFile::Close(pdbFile);

		return 3;
	}

	const PDB::DBIStream dbiStream = PDB::CreateDBIStream(rawPdbFile);
	if (!HasValidDBIStreams(rawPdbFile, dbiStream))
	{
		MemoryMappedFile::Close(pdbFile);

		return 4;
	}

	// run all examples
	ExampleContributions(rawPdbFile, dbiStream);
	ExampleSymbols(rawPdbFile, dbiStream);
	ExampleFunctionSymbols(rawPdbFile, dbiStream);

	MemoryMappedFile::Close(pdbFile);

	return 0;
}
