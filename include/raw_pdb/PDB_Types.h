// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_DisableWarningsPush.h"
#include <cstdint>
#include "Foundation/PDB_DisableWarningsPop.h"

namespace PDB
{
    PDB_PUSH_WARNING_MSVC										
    PDB_PUSH_WARNING_CLANG										
    PDB_DISABLE_WARNING_MSVC(4200)								
    PDB_DISABLE_WARNING_CLANG("-Wc99-extensions")				
    PDB_DISABLE_WARNING_CLANG("-Wmicrosoft-flexible-array")		
	PDB_DISABLE_WARNING_CLANG("-Wflexible-array-extensions")

	// emulating std::byte from C++17 to make the intention clear that we're dealing with untyped data in certain cases, without actually requiring C++17
	enum class Byte : unsigned char {};

	// this matches the definition in guiddef.h, but we don't want to pull that in
	struct GUID
	{
		uint32_t Data1;
		uint16_t Data2;
		uint16_t Data3;
		uint8_t  Data4[8];
	};

	static_assert(sizeof(GUID) == 16u, "Size mismatch.");

	// this matches the definition in winnt.h, but we don't want to pull that in
	struct IMAGE_SECTION_HEADER
	{
		unsigned char Name[8];
		union
		{
			uint32_t PhysicalAddress;
			uint32_t VirtualSize;
		} Misc;
		uint32_t VirtualAddress;
		uint32_t SizeOfRawData;
		uint32_t PointerToRawData;
		uint32_t PointerToRelocations;
		uint32_t PointerToLinenumbers;
		uint16_t NumberOfRelocations;
		uint16_t NumberOfLinenumbers;
		uint32_t Characteristics;
	};

	static_assert(sizeof(IMAGE_SECTION_HEADER) == 40u, "Size mismatch.");

	// https://llvm.org/docs/PDB/MsfFile.html#msf-superblock
	struct PDB_NO_DISCARD SuperBlock
	{
		static const char MAGIC[30u];

		char fileMagic[30u];
		char padding[2u];
		uint32_t blockSize;
		uint32_t freeBlockMapIndex;										// index of the free block map
		uint32_t blockCount;											// number of blocks in the file
		uint32_t directorySize;											// size of the stream directory in bytes
		uint32_t unknown;
		uint32_t directoryBlockIndices[];								// indices of the blocks that make up the directory indices
	};

	// https://llvm.org/docs/PDB/PdbStream.html#stream-header
	struct Header
	{
		enum class PDB_NO_DISCARD Version : uint32_t
		{
			VC2 = 19941610u,
			VC4 = 19950623u,
			VC41 = 19950814u,
			VC50 = 19960307u,
			VC98 = 19970604u,
			VC70Dep = 19990604u,
			VC70 = 20000404u,
			VC80 = 20030901u,
			VC110 = 20091201u,
			VC140 = 20140508u
		};

		Version version;
		uint32_t signature;
		uint32_t age;
		GUID guid;
	};

	// https://llvm.org/docs/PDB/PdbStream.html
	struct NamedStreamMap
	{
		uint32_t length;
		char stringTable[];

		struct HashTableEntry
		{
			uint32_t stringTableOffset;
			uint32_t streamIndex;
		};
	};

	// https://llvm.org/docs/PDB/HashTable.html
	struct SerializedHashTable
	{
		struct Header
		{
			uint32_t size;
			uint32_t capacity;
		};

		struct BitVector
		{
			uint32_t wordCount;
			uint32_t words[];
		};
	};

	// https://llvm.org/docs/PDB/PdbStream.html#pdb-feature-codes
	enum class PDB_NO_DISCARD FeatureCode : uint32_t
	{
		VC110 = 20091201,
		VC140 = 20140508,

		// https://github.com/microsoft/microsoft-pdb/blob/master/PDB/include/pdbcommon.h#L23
		NoTypeMerge = 0x4D544F4E,				// "NOTM"
		MinimalDebugInfo = 0x494E494D			// "MINI", i.e. executable was linked with /DEBUG:FASTLINK
	};

	// header of the public stream, based on PSGSIHDR defined here:
	// https://github.com/Microsoft/microsoft-pdb/blob/master/PDB/dbi/gsi.h#L240
	struct PublicStreamHeader
	{
		uint32_t symHash;
		uint32_t addrMap;
		uint32_t thunkCount;
		uint32_t sizeOfThunk;
		uint16_t isectThunkTable;
		uint16_t padding;
		uint32_t offsetThunkTable;
		uint16_t sectionCount;
		uint16_t padding2;
	};

	// header of the hash tables used by the public and global symbol stream, based on GSIHashHdr defined here:
	// https://github.com/Microsoft/microsoft-pdb/blob/master/PDB/dbi/gsi.h#L62
	struct HashTableHeader
	{
		static const uint32_t Signature;
		static const uint32_t Version;

		uint32_t signature;
		uint32_t version;
		uint32_t size;
		uint32_t bucketCount;
	};

	// hash record, based on HRFile defined here:
	// https://github.com/Microsoft/microsoft-pdb/blob/master/PDB/dbi/gsi.h#L8
	struct HashRecord
	{
		uint32_t offset;		// offset into the symbol record stream
		uint32_t cref;
	};

    PDB_POP_WARNING_MSVC
    PDB_POP_WARNING_CLANG
}
