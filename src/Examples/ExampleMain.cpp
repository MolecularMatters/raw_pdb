// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleMemoryMappedFile.h"
#include "PDB.h"
#include "PDB_RawFile.h"
#include "PDB_InfoStream.h"
#include "PDB_DBIStream.h"
#include "PDB_TPIStream.h"
#include "PDB_IPIStream.h"
#include "PDB_NamesStream.h"

namespace
{
	PDB_NO_DISCARD static bool IsError(PDB::ErrorCode errorCode)
	{
		switch (errorCode)
		{
			case PDB::ErrorCode::Success:
				return false;

			case PDB::ErrorCode::InvalidSuperBlock:
				printf("Invalid Superblock\n");
				return true;

			case PDB::ErrorCode::InvalidFreeBlockMap:
				printf("Invalid free block map\n");
				return true;

			case PDB::ErrorCode::InvalidStream:
				printf("Invalid stream\n");
				return true;

			case PDB::ErrorCode::InvalidSignature:
				printf("Invalid stream signature\n");
				return true;

			case PDB::ErrorCode::InvalidStreamIndex:
				printf("Invalid stream index\n");
				return true;

			case PDB::ErrorCode::InvalidDataSize:
				printf("Invalid data size\n");
				return true;

			case PDB::ErrorCode::UnknownVersion:
				printf("Unknown version\n");
				return true;
		}

		// only ErrorCode::Success means there wasn't an error, so all other paths have to assume there was an error
		return true;
	}

	PDB_NO_DISCARD static bool HasValidDBIStreams(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream)
	{
		// check whether the DBI stream offers all sub-streams we need
		if (IsError(dbiStream.HasValidSymbolRecordStream(rawPdbFile)))
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

		if (IsError(dbiStream.HasValidImageSectionStream(rawPdbFile)))
		{
			return false;
		}

		return true;
	}
}


// declare all examples
extern void ExamplePDBSize(const PDB::RawFile&, const PDB::DBIStream&);
extern void ExampleTPISize(const PDB::TPIStream& tpiStream, const char* outPath);
extern void ExampleContributions(const PDB::RawFile&, const PDB::DBIStream&);
extern void ExampleSymbols(const PDB::RawFile&, const PDB::DBIStream&);
extern void ExampleFunctionSymbols(const PDB::RawFile&, const PDB::DBIStream&);
extern void ExampleFunctionVariables(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::TPIStream&);
extern void ExampleLines(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::InfoStream& infoStream);
extern void ExampleTypes(const PDB::TPIStream&);
extern void ExampleIPI(const PDB::RawFile& rawPdbFile, const PDB::InfoStream& infoStream, const PDB::TPIStream& tpiStream, const PDB::IPIStream& ipiStream);

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Usage: Examples <PDB path>\nError: Incorrect usage\n");

		return 1;
	}

	printf("Opening PDB file %s\n", argv[1]);

	// try to open the PDB file and check whether all the data we need is available
	MemoryMappedFile::Handle pdbFile = MemoryMappedFile::Open(argv[1]);
	if (!pdbFile.baseAddress)
	{
		printf("Cannot memory-map file %s\n", argv[1]);

		return 1;
	}

	if (IsError(PDB::ValidateFile(pdbFile.baseAddress, pdbFile.len)))
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

	const PDB::InfoStream infoStream(rawPdbFile);
	if (infoStream.UsesDebugFastLink())
	{
		printf("PDB was linked using unsupported option /DEBUG:FASTLINK\n");

		MemoryMappedFile::Close(pdbFile);

		return 4;
	}

	const auto h = infoStream.GetHeader();
	printf("Version %u, signature %u, age %u, GUID %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\n",
		static_cast<uint32_t>(h->version), h->signature, h->age,
		h->guid.Data1, h->guid.Data2, h->guid.Data3,
		h->guid.Data4[0], h->guid.Data4[1], h->guid.Data4[2], h->guid.Data4[3], h->guid.Data4[4], h->guid.Data4[5], h->guid.Data4[6], h->guid.Data4[7]);

	const PDB::DBIStream dbiStream = PDB::CreateDBIStream(rawPdbFile);
	if (!HasValidDBIStreams(rawPdbFile, dbiStream))
	{
		MemoryMappedFile::Close(pdbFile);

		return 5;
	}

	if (IsError(PDB::HasValidTPIStream(rawPdbFile)))
	{
		MemoryMappedFile::Close(pdbFile);

		return 5;
	}
	const PDB::TPIStream tpiStream = PDB::CreateTPIStream(rawPdbFile);

	PDB::IPIStream ipiStream;

	// It's perfectly possible that an old PDB does not have an IPI stream.
	if(infoStream.HasIPIStream())
	{
		PDB::ErrorCode error = PDB::HasValidIPIStream(rawPdbFile);

		if (error != PDB::ErrorCode::InvalidStream && IsError(error))
		{
			MemoryMappedFile::Close(pdbFile);

			return 5;
		}

		ipiStream = PDB::CreateIPIStream(rawPdbFile);
	}	


	// run all examples
	ExamplePDBSize(rawPdbFile, dbiStream);
	ExampleContributions(rawPdbFile, dbiStream);
	ExampleSymbols(rawPdbFile, dbiStream);
	ExampleFunctionSymbols(rawPdbFile, dbiStream);
	ExampleFunctionVariables(rawPdbFile, dbiStream, tpiStream);
	ExampleLines(rawPdbFile, dbiStream, infoStream);
	ExampleTypes(tpiStream);
	ExampleIPI(rawPdbFile, infoStream, tpiStream, ipiStream);
	// uncomment to dump type sizes to a CSV
	// ExampleTPISize(tpiStream, "output.csv");

	MemoryMappedFile::Close(pdbFile);

	return 0;
}
